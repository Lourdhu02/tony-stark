```text
 ███╗   ███╗██╗  ██╗    ██╗██╗██╗
 ████╗ ████║██║ ██╔╝    ██║██║██║    THE KERNEL
 ██╔████╔██║█████╔╝     ██║██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ██║██║██║    kernels · concurrency · memory ordering · i/o
 ██║ ╚═╝ ██║██║  ██╗    ██║██║██║    weeks 15–22 · 4 systems
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝╚═╝╚═╝
```

> *"Sometimes you gotta run before you can walk."* In a kernel, that means interrupts before `main()`.

### `> cat mission.txt`

Write a kernel that boots on a RISC-V machine (QEMU), takes timer interrupts and preemptively schedules tasks. Around it, master the concurrency toolbox: locks, condition variables, atomics, memory ordering and lock-free structures, plus event-driven I/O that holds 10,000 connections.

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| Interrupts, traps and context switches | Mark IV's RTOS *is* a tiny kernel; you'll read FreeRTOS source and recognize every line |
| Scheduling and priorities | Control loops miss deadlines because of scheduling. Priority inversion nearly killed the Mars Pathfinder mission (1997). |
| Memory ordering and lock-free queues | Sensor → estimator → controller pipelines pass data between threads at kHz rates without locks (VII, VIII) |
| Event-driven I/O (`epoll`) | ROS 2 executors, telemetry servers and JARVIS's audio/LLM/tool fan-out are all event loops |
| Virtual memory and page tables | Why `fork` is cheap (copy-on-write), why GPUs have unified memory, and what a segfault *is* |

### `> ./prereqs --check`

- [ ] Mark I (C, processes, the allocator) and Mark II (RISC-V, Rust)
- [ ] *Rust Book* ch. 15–16 (smart pointers, fearless concurrency)
- [ ] `qemu-system-riscv64` installed; the `riscv64gc-unknown-none-elf` Rust target added

### `> cat concept_map`

```text
KERNEL
├── boot: firmware (OpenSBI) → _start (asm) → stack → Rust kmain
├── privilege: M-mode (firmware) · S-mode (kernel) · U-mode (apps)
├── traps: stvec → save registers → scause says why → handle → sret
│   ├── interrupts: timer (SBI set_timer), external (PLIC)
│   └── exceptions: ecall (syscalls), page faults, illegal instruction
├── scheduling: task = {registers, stack} · round-robin · time slice
├── memory: Sv39 page tables · satp · TLB · copy-on-write
└── devices: 16550 UART at 0x1000_0000 on QEMU virt

CONCURRENCY
├── problems: race conditions · data races · deadlock · priority inversion · starvation
├── blocking: Mutex · Condvar · Semaphore · RwLock
├── atomics: load/store/CAS · orderings: Relaxed · Acquire · Release · AcqRel · SeqCst
├── lock-free: SPSC ring · MPSC queue · ABA problem · false sharing
└── verification: loom (exhaustive interleavings) · ThreadSanitizer

I/O MODELS
└── blocking → thread-per-connection → non-blocking + epoll (level/edge) → io_uring
```

---

### `> cat syllabus`

| Week | Theory | Build | Checkpoint |
|---|---|---|---|
| **15** | OSTEP ch. 26–31 (threads, locks, condition variables, semaphores) · Rust Book ch. 16 | `01-sync`: a bounded blocking queue (Mutex + Condvar) | producer/consumer stress test passes under TSan |
| **16** | *Rust Atomics and Locks* ch. 1–3 | `01-sync`: a spinlock from `AtomicBool`; model it with `loom` | loom finds the bug when you weaken an ordering |
| **17** | *RA&L* ch. 4–6 · manual: false sharing | `01-sync`: an SPSC ring buffer + a work-stealing thread pool | SPSC ≥ 3× `Mutex<VecDeque>` throughput |
| **18** | xv6 book ch. 1–2 · MIT 6.1810 lecture videos | `02-xv6-labs`: *util*, *syscall* | `trace` syscall works in xv6 |
| **19** | xv6 book ch. 3–4 (page tables, traps) | `02-xv6-labs`: *pgtbl*, *traps* | `backtrace()` prints in xv6 |
| **20** | RISC-V privileged spec (S-mode CSRs) · Oppermann's "Freestanding Rust binary" (adapt to RISC-V) | `03-stark-os`: boot, UART driver, `println!`, panic handler | `hello from stark-os` in QEMU |
| **21** | xv6 `swtch.S` + `trampoline.S`, read line by line | `03-stark-os`: trap vector, 100 Hz timer, context switch, round-robin | two tasks interleave output with preemption |
| **22** | Kerrisk, *TLPI* ch. 63 (epoll) · the C10K problem | `04-epoll-server` (C): edge-triggered echo server + a load generator | 10k concurrent connections, p99 latency reported |

### `> ls systems/`

| # | System | Exit criteria |
|---|---|---|
| 01 | `sync` (Rust): bounded queue, spinlock, SPSC ring, thread pool | all loom models pass; SPSC ≥ **3×** `Mutex<VecDeque>` at 1 producer + 1 consumer; no TSan reports |
| 02 | `xv6-labs` (C): util, syscall, pgtbl, traps | the official `make grade` passes for each lab |
| 03 | `stark-os` (Rust, `no_std`, RV64 on QEMU virt) | boots; timer interrupts at 100 Hz; ≥ 3 tasks preemptively scheduled; a panic prints `scause`/`sepc`/`stval` |
| 04 | `epoll-server` (C) | **10,000** concurrent echo connections; p50/p99 latency and throughput measured; no fd leaks after 1M connects |

*Code scaffolding arrives when you start this Mark.*

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| Trap handler clobbers a register | random corruption after the first interrupt | save **all 31** general-purpose registers (not just callee-saved) on trap entry |
| Re-enabling interrupts too early | nested trap on the same stack, then a crash | keep `sstatus.SIE` off until the handler finishes, or give each task its own kernel stack |
| `Relaxed` where you needed `Release`/`Acquire` | works on x86, fails on ARM (and in loom) | publish data with `Release`, consume with `Acquire`, then let loom prove it |
| False sharing | "lock-free" is slower than a mutex | pad `head` and `tail` onto separate cache lines (`#[repr(align(128))]`) |
| Edge-triggered epoll without draining | connections stall randomly | with `EPOLLET`, read and accept in a loop until `EAGAIN` |
| `ulimit -n` of 1024 | the load test dies at about 1000 connections | raise the fd limit and tune `net.core.somaxconn` |

### `> ./quiz`

<details><summary>1. What's the difference between a race condition and a data race?</summary>

A **data race** is two unsynchronized accesses to the same memory location, at least one a write. It's undefined behaviour in C/C++, and prevented in safe Rust. A **race condition** is a logic bug where correctness depends on timing, and it can happen even with perfectly synchronized operations (check-then-act across two locked calls, for example).
</details>

<details><summary>2. What do Release and Acquire actually guarantee?</summary>

If thread A does a `Release` store to X, and thread B does an `Acquire` load of X that sees that value, then **everything A wrote before the store is visible to B after the load.** It's a one-way "happens-before" edge, and it's exactly what publishing data through a flag or an index needs.
</details>

<details><summary>3. What went wrong on Mars Pathfinder in 1997?</summary>

**Priority inversion.** A low-priority task held a mutex that a high-priority task needed, while a medium-priority task preempted the low one. The high-priority task missed its deadline, and a watchdog kept resetting the system. The fix, uploaded remotely, was to enable **priority inheritance** on that mutex.
</details>

<details><summary>4. Walk through what your kernel does on a timer interrupt.</summary>

The CPU jumps to `stvec` → the trap entry saves all registers to the current task's trap frame → `scause` says "supervisor timer interrupt" → the handler schedules the next timer via SBI → the scheduler picks the next task → the handler restores that task's registers → `sret` resumes it in its own context.
</details>

<details><summary>5. What are the four conditions for deadlock?</summary>

These are the Coffman conditions: **mutual exclusion, hold-and-wait, no preemption, circular wait.** Breaking any one prevents deadlock. The most practical is a global lock ordering, which breaks circular wait.
</details>

<details><summary>6. Level-triggered vs edge-triggered epoll?</summary>

**Level-triggered:** notifies as long as the fd is ready, so you can read partially and get told again. **Edge-triggered:** notifies only on a *change* to ready, so you must drain until `EAGAIN` or risk never being woken again. ET means fewer wakeups, and it's easier to get wrong.
</details>

<details><summary>7. What does a page table do, and what's a TLB?</summary>

The page table maps virtual pages to physical frames, with permissions, per address space. The **TLB** is a hardware cache of recent translations. A miss triggers a (hardware) page-table walk, which is why switching address spaces, which flushes or tags the TLB, has a cost.
</details>

<details><summary>8. Why is <code>fork()</code> cheap, even for a process with 4 GB of memory?</summary>

**Copy-on-write.** The child initially shares all physical pages, marked read-only in both page tables. A page is copied only when either process writes to it, and the page-fault handler does that copy lazily.
</details>

### `> cat interview.txt`

- *"Explain memory ordering. When would you use Relaxed?"*
- *"Design a lock-free single-producer single-consumer queue."*
- *"What happens during a context switch?"*
- *"How does epoll scale better than select/poll?"*
- *"Walk me through a page fault."*
- *"You have a deadlock in production. How do you find it?"*

### `> cat boss_fight`

**"Three tasks, one core, no cooperation."** `stark-os` boots in QEMU and runs three tasks under a 100 Hz preemptive scheduler:
- Task 1 computes primes.
- Task 2 draws a scrolling ASCII matrix rain on the UART.
- Task 3 runs in **U-mode** and prints only through an `ecall` syscall your kernel implements.

None of them ever yields voluntarily. Kill the scheduler's timer and the rain freezes: that's your proof of preemption.

### `> cat library`

| Source | Use |
|---|---|
| OSTEP: concurrency part (ch. 26–34) | Weeks 15–16 |
| Mara Bos, *Rust Atomics and Locks* (free online) | Weeks 16–17: build every chapter's primitive |
| Cox, Kaashoek & Morris, *xv6 book* + MIT 6.1810 labs | Weeks 18–19 |
| *The RISC-V Instruction Set Manual, Vol. II: Privileged* | Weeks 20–21 |
| Oppermann, *Writing an OS in Rust* | Ideas for `no_std`, panic handling and allocators (it targets x86-64) |
| Kerrisk, *The Linux Programming Interface* | Ch. 63 (alternative I/O models) |
| `loom` crate docs | Model checking your atomics |

**Hardware:** none (QEMU).
