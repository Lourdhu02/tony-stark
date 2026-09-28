```text
 ███╗   ███╗██╗  ██╗    ██╗██╗
 ████╗ ████║██║ ██╔╝    ██║██║    SILICON
 ██╔████╔██║█████╔╝     ██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ██║██║    nand → cpu · isa · emulators · rust
 ██║ ╚═╝ ██║██║  ██╗    ██║██║    weeks 7–14 · 3 systems
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝╚═╝
```

> *"I am Iron Man. The suit and I are one."* To be one with the machine, you have to know what the machine is.

### `> cat mission.txt`

Build a computer from NAND gates, then build one in software. Finish by running C code you compiled yourself, including your own Mark I code, on a RISC-V CPU **you wrote**.

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| How a CPU executes instructions (fetch, decode, execute, pipelines, hazards) | Mark III's kernel manipulates this machine directly: traps, privilege modes, page tables |
| An ISA at bit level (RISC-V) | The ESP32-C3 and many new MCUs are RISC-V; Mark IV firmware debugging means reading disassembly |
| Interpreter and emulator design | The mental model behind every VM, JIT and simulator, and behind PyTorch's dispatcher |
| Rust | Marks III, IV (Embassy, optional) and IX use it; ownership is a memory model you now understand from Mark I |
| Why hardware is parallel and pipelined | Mark VIII's GPU performance model starts here |

### `> ./prereqs --check`

- [ ] Mark I complete (C, memory, bit manipulation)
- [ ] *The Rust Programming Language* ch. 1–6 read, and rustlings started
- [ ] Comfortable with binary and hex, and two's complement on paper

### `> cat concept_map`

```text
NAND ─▶ NOT/AND/OR/XOR ─▶ MUX/DMUX ─▶ half/full adder ─▶ ALU ─────────────┐
  │                                                                       │
  └▶ DFF (clocked state) ─▶ bit ─▶ register ─▶ RAM ─▶ PC ─────────────────┤
                                                                          ▼
                                    CPU = ALU + registers + PC + control logic
                                                                          │
      ISA (the contract) ◀────────────────────────────────────────────────┘
        │   RV32I: 32 registers (x0 ≡ 0), 32-bit instructions,
        │   formats R / I / S / B / U / J, load-store architecture
        ▼
   machine code ─▶ EMULATOR: loop { fetch(pc) → decode → execute → pc' }
        ▲
   compiler (riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32) + ELF loader
```

---

### `> cat syllabus`

| Week | Theory | Build | Checkpoint |
|---|---|---|---|
| **7** | nand2tetris ch. 1–2 (Boolean logic, arithmetic) · Rust Book ch. 1–6 | nand2tetris projects 1–2 in the official hardware simulator | ALU passes all tests |
| **8** | nand2tetris ch. 3–4 (sequential logic, machine language) · Rust Book ch. 7–10 | projects 3–4: registers, RAM, Hack assembly | `Mult.asm` and `Fill.asm` pass |
| **9** | nand2tetris ch. 5 (computer architecture) | project 5: the Hack CPU and computer | `ComputerMax.tst` passes: **you built a CPU** |
| **10** | CHIP-8 technical reference (Cowgod's) | `02-chip8` in Rust: 35 opcodes, a 64×32 display rendered as **ASCII in the terminal** | passes the Timendus `chip8-test-suite` core tests |
| **11** | Harris & Harris ch. 6 · RISC-V spec vol. I, RV32I chapter | `03-rv32i`: instruction decoder with unit tests for every format | decoder round-trips all 6 formats |
| **12** | Harris & Harris ch. 7.1–7.4 (single-cycle, multicycle) | execute loop, memory, ELF32 loader | runs a hand-assembled loop |
| **13** | `riscv-tests` README (the `tohost` protocol) · libgloss syscall ABI | pass `rv32ui-p-*`; implement `write`/`exit`/`brk` ecalls | all rv32ui tests pass · `printf("hello")` from C |
| **14** | Harris & Harris ch. 7.5 (pipelining) | boss fight, then write-up · *optional:* a 5-stage pipelined core in Verilog + Verilator | Mark II post published |

### `> ls systems/`

| # | System | Spec | Exit criteria |
|---|---|---|---|
| 01 | `hack-cpu` | nand2tetris projects 1–5, in HDL | every official `.tst` passes |
| 02 | `chip8` | a Rust CHIP-8 interpreter with an ASCII terminal renderer, a 60 Hz timer and a keypad | Timendus test suite: `corax+`, `flags` and `quirks` pass; plays a public-domain ROM |
| 03 | `rv32i` | a Rust RV32I emulator: ELF loader, all 40-ish base instructions, ecall syscalls, instruction tracing (`--trace`) | **all `rv32ui-p-*` tests pass**; runs C compiled with newlib; ≥ 20 MIPS on a laptop core |

*Code scaffolding (Cargo workspace, test harness, CI) arrives when you start this Mark. The dossier comes first so you can prepare.*

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| Immediates aren't sign-extended | negative branch offsets jump forward into garbage | every RISC-V immediate is sign-extended from **bit 31** of the instruction |
| B- and J-format bit scrambling | branches land a few bytes off | reassemble `imm[12|10:5]` / `imm[4:1|11]` exactly as the spec's figure shows; unit-test each field |
| Writing to x0 | `x0` stops reading 0 | ignore writes to register 0, or re-zero it after every instruction |
| Shift amounts | `sll` by 33 behaves strangely | RV32 uses only the low 5 bits of the shift amount |
| `SRA` vs `SRL`, `SLT` vs `SLTU` | sign bugs in C code | cast to `i32` for arithmetic shifts and signed compares, `u32` for the others |
| Endianness | loaded words look byte-swapped | RISC-V is little-endian: use `u32::from_le_bytes` |

### `> ./quiz`

<details><summary>1. Why can every Boolean function be built from NAND alone?</summary>

NAND is **functionally complete**: NOT a = NAND(a, a), AND = NOT(NAND), and OR follows from De Morgan: a OR b = NAND(NOT a, NOT b). Any truth table can be written in sum-of-products form using AND, OR and NOT.
</details>

<details><summary>2. Combinational vs sequential logic: what's the difference, and why do we need a clock?</summary>

Combinational outputs depend only on the current inputs. Sequential circuits have **state** (flip-flops) that depends on history. The clock defines *when* state updates, so signals have time to settle through the combinational logic between edges.
</details>

<details><summary>3. Why does RISC-V scramble the immediate bits in B- and J-format instructions?</summary>

To keep the sign bit always at instruction bit 31 and to put each immediate bit in the *same position across formats* as far as possible. That minimizes the multiplexers in hardware decode. The cost is paid by the software that encodes and decodes.
</details>

<details><summary>4. What's the point of a hardwired zero register (x0)?</summary>

Many operations come free without extra opcodes: `mv rd, rs` = `addi rd, rs, 0`; `nop` = `addi x0, x0, 0`; a plain jump = `jal x0, off` (discard the return address); comparisons against zero need no constant load.
</details>

<details><summary>5. How do the official <code>riscv-tests</code> report pass or fail?</summary>

The test writes to a memory location labelled `tohost`. A value of 1 means pass; otherwise the value is `(failing_test_number << 1) | 1`. Your emulator watches that address (found from the ELF symbol table).
</details>

<details><summary>6. What are the three kinds of pipeline hazards?</summary>

**Data** (an instruction needs a result that isn't written back yet), **control** (the next PC isn't known until a branch resolves) and **structural** (two instructions need the same hardware unit in the same cycle). The fixes are forwarding, stalls and branch prediction.
</details>

<details><summary>7. What does "little-endian" mean for <code>lw</code> at address A?</summary>

The least significant byte is at A, the next at A+1, and so on. The word is `mem[A] | mem[A+1] << 8 | mem[A+2] << 16 | mem[A+3] << 24`.
</details>

<details><summary>8. Your interpreter runs at 5 MIPS. What are the first three things you'd profile?</summary>

1. The decode cost per instruction: pre-decode, or cache decoded instructions by PC.
2. Memory access: bounds checks and the address translation layer in the hot path.
3. Dispatch: a `match` on opcode is usually fine in Rust; check that it compiles to a jump table and not a chain of comparisons.

Measure with `perf` before changing anything.
</details>

### `> cat interview.txt`

- *"Walk me through how a CPU executes `add x1, x2, x3`."*
- *"What is pipelining? What breaks it, and how do CPUs cope?"*
- *"Explain two's complement and sign extension."*
- *"How would you build an emulator, and how would you make it fast?"*
- *"What's the difference between an ISA and a microarchitecture?"*

### `> cat boss_fight`

**"Running on a CPU I built."** Cross-compile your Mark I `linalg` library, plus a `main.c` that multiplies two 4×4 matrices and prints the result, with `riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32`. Run it on your emulator. Then run it again with `--trace` and count the instructions `mat_mul` executes. Compare that count with your FLOP estimate: what fraction of instructions are actually doing math?

### `> cat library`

| Source | Use |
|---|---|
| Nisan & Schocken, *The Elements of Computing Systems* (2nd ed.) + nand2tetris.org | Weeks 7–9: every project |
| Harris & Harris, *Digital Design and Computer Architecture: RISC-V Edition* | Ch. 6–7: the ISA and microarchitecture |
| *The RISC-V Instruction Set Manual, Vol. I* (free) | The ground truth for your emulator |
| Klabnik & Nichols, *The Rust Programming Language* (free) | Ch. 1–10 |
| Cowgod's *CHIP-8 Technical Reference* + Timendus `chip8-test-suite` | Week 10 |
| `riscv-software-src/riscv-tests` | Week 13 |
| Ben Eater, *Building an 8-bit breadboard computer* (YouTube) | Optional: the same ideas in physical wires |

**Hardware:** none. *Optional:* an iCE40 or Gowin FPGA board (about $25–70) to run your Verilog core on real silicon.
