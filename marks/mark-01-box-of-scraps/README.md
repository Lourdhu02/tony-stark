```text
 ███╗   ███╗██╗  ██╗    ██╗
 ████╗ ████║██║ ██╔╝    ██║     BOX OF SCRAPS
 ██╔████╔██║█████╔╝     ██║     ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ██║     c · memory · numerics · processes
 ██║ ╚═╝ ██║██║  ██╗    ██║     weeks 1–6 · 4 systems · 79 tests
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝
```

> *"Tony Stark was able to build this in a cave! With a box of scraps!"*

### `> cat mission.txt`

Build the four things everything else sits on, from nothing but C, `man` pages and a compiler:
- a matrix library
- a physics integrator
- `malloc`
- a Unix shell

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| How memory *really* behaves (caches, layout, the heap) | Mark VIII's CUDA kernels are the same tiling idea at 1000× scale; Mark IV's MCUs have 128 KB of RAM and no `malloc` safety net |
| Numerical stability and pivoting | LQR (V), Kalman filters (VII) and IK (VI) all solve linear systems every cycle, and they fail silently when you get this wrong |
| Integrators and energy | Your quadrotor sim (V) and every physics engine (VI, X) are this, scaled up |
| Processes, pipes and signals | ROS 2 (VI–VII) is processes talking; JARVIS (IX) supervises tool processes; Mark III has you implement `fork` itself |

For an ML engineer, this Mark closes the gap between "I call `torch.matmul`" and "I know why it's fast".

### `> ./prereqs --check`

- [ ] Comfortable reading C: pointers, structs, `malloc`/`free`. If you're rusty, spend a weekend on K&R ch. 5–6 first.
- [ ] Can use `gdb` to set a breakpoint and print a variable.
- [ ] Linux or WSL2 with `gcc`, `make` and `python3`.

---

### `> cat syllabus`

| Week | Theory (2 × 2 h) | Build (2 × 3 h) | Checkpoint |
|---|---|---|---|
| **1** | CS:APP §6.1–6.6 · Strang 18.06 lectures 2–4 · [manual 01](notes/01-memory-hierarchy.md) + [02](notes/02-lu-and-floating-point.md) · Goldberg (skim) | [`01-linalg`](01-linalg) | 14/14 and `make bench` ≥ 3× · derive a 3×3 LU with pivoting **by hand** |
| **2** | [manual 03](notes/03-numerical-integration.md) · Hairer–Lubich–Wanner §1 | [`02-sim`](02-sim) | 13/13 · phase-portrait plot of all 3 methods in your lab note |
| **3** | CS:APP §9.1–9.9 · OSTEP ch. 13–17 · [manual 04](notes/04-allocators.md) | [`03-malloc`](03-malloc): layout, malloc, free, splitting, coalescing | first 8 unit tests · `sm_check()` written |
| **4** | Re-read CS:APP §9.9 (dynamic memory allocation) | `03-malloc`: calloc, realloc, memalign, stress, preload | 19/19 · `python3` runs on your heap |
| **5** | CS:APP §8.1–8.4 and ch. 10 · OSTEP ch. 5 · [manual 05](notes/05-processes-pipes-signals.md) | [`04-shell`](04-shell): tokenizer, parser, single commands, builtins | 10/10 parser tests · `cd /tmp` then `pwd` works |
| **6** | CS:APP §8.5 (signals) · Beej's IPC guide (pipes) | `04-shell`: redirection, pipes, background jobs, signals | 33/33 · the **boss fight** below |

### `> ls systems/`

| # | System | You build | Tests | Brief |
|---|---|---|---|---|
| 01 | `linalg` | Matrices, LU with pivoting, solve, inverse, determinant, cache-aware matmul | 14 | [README](01-linalg/README.md) |
| 02 | `sim` | Euler, RK4 and Verlet integrators; pendulum and spring | 13 | [README](02-sim/README.md) |
| 03 | `malloc` | A real allocator: `ls`, `sort`, `awk`, `bash` and `python3` run on it | 19 | [README](03-malloc/README.md) |
| 04 | `shell` | `stark-shell`: pipes, redirection, quoting, jobs, signals | 33 | [README](04-shell/README.md) |

Each brief has the spec, the order of attack, a **3-level hint ladder** (nudge → approach → algorithm, never code), debugging tips and stretch goals.

### `> cat boot_sequence`

```sh
make test                  # dashboard for all four systems
make -C 01-linalg test     # one system, every test listed
make -C 01-linalg bench    # naive vs your fast matmul, in GFLOP/s
make -C 02-sim demo        # CSVs: euler / rk4 / verlet energy drift
make -C 04-shell run       # drop into your own shell
make SAN=1 test            # the same, under AddressSanitizer + UBSan
```

**How the scaffolding works:**
- Headers in `include/` are the contract.
- Files marked `given` (the REPL, the private heap, the `LD_PRELOAD` shim) are infrastructure, so you can focus on the hard part.
- Everything else is a `TODO` stub.
- Every test runs in its own forked process with a timeout, so a segfault costs you one test, not the whole run.

---

### `> cat boss_fight`

The Mark I finale uses **everything you built, at once**. From inside *your* shell, run *your* simulator on *your* allocator through a pipe:

```text
$ make all && make -C 04-shell run
stark> env LD_PRELOAD=../03-malloc/build/libstarkmalloc.so ../02-sim/build/pendulum --method verlet --dt 0.1 --T 10000 --every 1000 | tail -n 3 > boss.txt
stark> cat boss.txt
stark> exit
```

When `boss.txt` shows the last three samples of a 10,000-second pendulum whose energy error stays around 2%, **Mark I is done.** Record it: take a screenshot for the lab note, write the blog post, and flip the status in the main README.

---

### `> ./quiz`

Answer from memory, then check. Revisit these in your Sunday spaced-review sessions.

<details><summary>1. Why is the i-k-j loop order faster than i-j-k for row-major matmul?</summary>

In i-j-k, the innermost loop walks **down a column of B**, so every step jumps a full row and touches a new cache line (about 1.125 misses per iteration). In i-k-j, the inner loop walks **along rows** of B and C with stride 1, so one line serves 8 doubles (about 0.25 misses per iteration). The arithmetic is identical; only the memory traffic changes.
</details>

<details><summary>2. What does partial pivoting guarantee, and why does it matter?</summary>

Every multiplier satisfies |lᵢₖ| ≤ 1, because you divide by the largest available pivot. Without it, a tiny pivot creates enormous multipliers that swamp the other entries in rounding, like the ε = 10⁻²⁰ example where x₀ comes out as 0 instead of 1.
</details>

<details><summary>3. Why should you never compute A⁻¹ to solve Ax = b?</summary>

It costs about 3× the FLOPs of an LU solve (2n³ vs ⅔n³) and is less accurate. Factor once, then do two triangular solves per right-hand side at O(n²) each.
</details>

<details><summary>4. You halve dt and the error drops by 16×. What's the method's order?</summary>

4, because 2⁴ = 16. For RK4 that's expected; for anything else, suspect a lucky test.
</details>

<details><summary>5. Why does explicit Euler gain energy on an undamped oscillator, for any step size?</summary>

For y' = λy it multiplies by (1 + hλ) each step. An oscillator has λ = ±iω, so |1 + ihω| = √(1 + h²ω²) > 1 for every h > 0. Every step amplifies the oscillation.
</details>

<details><summary>6. Verlet is 2nd order and RK4 is 4th. Why does Verlet win over 10⁵ steps?</summary>

Verlet is **symplectic**: it exactly conserves a nearby "shadow" energy, so the true energy error oscillates within a bound. RK4 isn't symplectic, so its small per-step errors accumulate into a secular drift (−65% in the repo's measurement). Preserving the right structure beats a higher order over long horizons.
</details>

<details><summary>7. Why does a boundary-tag allocator store a footer as well as a header?</summary>

So `free` can find the **previous** block's size, and whether it's free, in O(1): the previous block's footer sits immediately before this block's header. That makes backward coalescing constant-time.
</details>

<details><summary>8. What's the difference between internal and external fragmentation?</summary>

Internal fragmentation is waste inside allocated blocks: headers, padding, minimum sizes. External fragmentation is free memory split into pieces too small for the request, even though the total would be enough.
</details>

<details><summary>9. <code>ls | wc -l</code> prints nothing and hangs. What's the most likely bug?</summary>

A write end of the pipe is still open somewhere, in the shell or in the `wc` child itself. `wc` only sees EOF when *every* write end is closed. Fix: close both pipe ends in the parent after forking, and close the unused ends in every child.
</details>

<details><summary>10. Why must a child reset SIGINT to SIG_DFL before <code>exec</code>, if the shell ignores it?</summary>

Across `exec`, *handled* signals reset to default, but *ignored* signals stay ignored. If the shell uses SIG_IGN and the child doesn't restore SIG_DFL, the exec'd program can't be interrupted with Ctrl-C.
</details>

<details><summary>11. Why is <code>cd</code> a builtin?</summary>

The working directory is per-process state. A forked child running `cd` would change its own directory and then exit, leaving the shell's directory unchanged.
</details>

<details><summary>12. What is a zombie process, and how does a shell avoid leaving them?</summary>

A child that has exited but whose parent hasn't collected its status with `wait`/`waitpid`. The shell reaps background jobs by looping `waitpid(-1, &st, WNOHANG)` before each prompt (or in a SIGCHLD handler).
</details>

### `> cat interview.txt`

After Mark I you can answer these properly, down to the syscalls and cache lines:

- *"What happens, step by step, when you type `ls | wc -l` into a shell?"*
- *"How would you implement `malloc`? What are the trade-offs between free-list policies?"*
- *"Why might two loops with identical FLOP counts differ 5× in speed?"*
- *"What is numerical stability? Give an example of an unstable algorithm for a well-conditioned problem."*
- *"Explain `fork` and `exec`. Why are they separate calls?"*
- *"How do signals interact with `exec`?"*
- *"How does `LD_PRELOAD` work, and what can go wrong?"*

### `> cat after_action.txt`

When a system goes 100%:
1. Add its folder name (for example `01-linalg`) to [`COMPLETED`](COMPLETED). CI will fail if it ever regresses.
2. Write that week's [lab note](../../lab-notes/TEMPLATE.md).

When all four are ONLINE and the boss fight passes:
1. Re-score yourself on the [self-assessment](../../docs/self-assessment.md). Expect *C and memory* and *Numerics* to reach 3.
2. Write the Mark I post, and set Mark I to `✓ ONLINE` in the [main README](../../README.md).
3. Open [Mark II · Silicon](../mark-02-silicon).

> Stuck for more than 45 minutes? Follow [the method's unstuck ladder](../../docs/method.md#getting-unstuck-in-this-order). Look up concepts, never code.
