```text
 ███╗   ███╗██╗  ██╗    ██╗
 ████╗ ████║██║ ██╔╝    ██║     BOX OF SCRAPS
 ██╔████╔██║█████╔╝     ██║     ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ██║     c · syscalls · memory · numerics
 ██║ ╚═╝ ██║██║  ██╗    ██║     weeks 1–6 · 4 systems · 79 tests
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝
```

> *"Tony Stark was able to build this in a cave! With a box of scraps!"*

No libraries and no frameworks: just C, `man` pages and a compiler.
By the end you'll have written the things everything else sits on: a matrix library,
a physics integrator, `malloc` and a Unix shell.

### `> ls`

| # | System | You build | Tests | Week |
|---|---|---|---|---|
| 01 | [`linalg`](01-linalg) | Dense matrices: multiplication, LU with pivoting, solve, inverse, determinant | 14 | 1 |
| 02 | [`sim`](02-sim) | Euler, RK4 and symplectic Verlet integrators, plus pendulum and spring models | 13 | 2 |
| 03 | [`malloc`](03-malloc) | A real allocator that runs `ls`, `sort` and `python3` via `LD_PRELOAD` | 19 | 3–4 |
| 04 | [`shell`](04-shell) | `stark-shell`: pipes, redirection, quoting, background jobs, signals | 33 | 5–6 |

### `> cat boot_sequence`

```sh
make test                  # dashboard for all four systems
make -C 01-linalg test     # one system, every test listed
make -C 01-linalg bench    # naive vs your fast matmul, in GFLOP/s
make -C 02-sim demo        # CSVs comparing euler / rk4 / verlet energy drift
make -C 04-shell run       # drop into your own shell
make SAN=1 test            # the same, under AddressSanitizer + UBSan
```

**How each system works:**
- The headers in `include/` are the contract. Read them first.
- Files under `src/` that are marked `given` are infrastructure (the REPL loop, the private heap, the `LD_PRELOAD` shim), so you can focus on the hard part.
- Every other function is a `TODO` stub.
- Every test runs in its own forked process with a timeout, so a segfault costs you one test, not the whole run.

---

## 01 · linalg

A row-major `Mat` type and everything from `mat_new` to `mat_det`. The part worth your time is `mat_lu`: partial pivoting is the difference between a textbook algorithm and one that survives real matrices (the `lu_requires_pivoting` test puts a zero on the diagonal).
The other part is `mat_mul_fast`. The naive i-j-k loop strides down columns of B and thrashes the cache. Reorder the loops, block them, and measure.

**Exit criteria:** 14/14 tests, and `make bench` shows `mat_mul_fast` at **3x or more** the naive speed at n=512.
**Read:** *CS:APP* §6 (the memory hierarchy), and Strang's 18.06 lectures 4–5 (LU).
**Stretch:** SIMD intrinsics (AVX2), threading with pthreads, then compare against OpenBLAS.

## 02 · sim

`step_euler`, `step_rk4` and `step_verlet`, plus the pendulum and spring models. The tests verify the **order of convergence** of each method (halve dt: error ÷2, ÷16, ÷4) and then show the real lesson:

```text
dt = 0.1 s over 10,000 s     max |ΔE/E₀|
  euler                      runs away (+11,800%)
  rk4                        slowly bleeds out (−65%)
  verlet (symplectic)        bounded forever (~2%)
```

A 4th-order method loses to a 2nd-order one because Verlet preserves the *geometry* of Hamiltonian mechanics. You'll see this again in robotics and in MD simulation.

**Exit criteria:** 13/13 tests, including RK4 drift below 0.1% over 100 s at dt = 0.05.
**Read:** Hairer, Lubich & Wanner, *Geometric numerical integration illustrated by the Störmer–Verlet method* (Acta Numerica, 2003).
**Stretch:** adaptive RK45 (Dormand–Prince) with error control; a double pendulum; plot the phase portraits.

## 03 · malloc

`sm_malloc`, `sm_free`, `sm_calloc`, `sm_realloc`, `sm_memalign` and `sm_usable_size`, on top of a contiguous private heap (`heap_grow()`, given). The tests check alignment, reuse, **splitting**, **coalescing with both neighbours**, `calloc` overflow, heap growth discipline and heap utilization under a 200k-op random workload.
Then the preload suite runs real programs on your allocator:

```sh
LD_PRELOAD=./build/libstarkmalloc.so python3 -c "print('running on my malloc')"
```

**Exit criteria:** 19/19 tests. `ls`, `sort`, `awk`, `bash` and `python3` all run on your allocator.
**Read:** *CS:APP* §9.9 (dynamic memory allocation), the whole section. *OSTEP* ch. 17 (free-space management).
**Stretch:** segregated free lists, per-thread caches (replace the global lock), and a heap consistency checker.

## 04 · shell

`parse_line` (a tokenizer and parser with quoting), `run_pipeline` (fork/exec, `pipe`, `dup2`, redirection), the `cd`/`pwd`/`exit` builtins, background jobs and signal handling. The end-to-end suite feeds scripts to your shell and diffs stdout. It also sends a real Ctrl-C to check that the child dies and the shell survives.

**Exit criteria:** 33/33 tests, including `ls | grep '\.c$' | wc -l > out.txt`.
**Read:** *CS:APP* §8 (exceptional control flow), *OSTEP* ch. 5 (process API), and Beej's *Guide to Unix IPC*.
**Stretch:** `$VAR` expansion, `&&`/`||`, `jobs`/`fg`/`bg` with process groups and `tcsetpgrp`, and line editing with raw terminal mode.

---

### `> cat after_action.txt`

When a system goes 100%:

1. Add its folder name (e.g. `01-linalg`) to [`COMPLETED`](COMPLETED). CI will now fail if it ever regresses.
2. Write the week's [lab note](../../lab-notes/TEMPLATE.md).
3. When all four are ONLINE, write the Mark I post and flip its row in the [main README](../../README.md) to `✓ ONLINE`.

> Stuck for more than 45 minutes? Re-read the header, run `make SAN=1 test`, and use `gdb`.
> Only then go looking for hints, and never for code.
