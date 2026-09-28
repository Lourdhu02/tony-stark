# 03 · malloc

> Weeks 3–4 · 19 tests (14 unit + 5 real programs) · Field manual: [04 allocators](../notes/04-allocators.md)

Write the allocator that real programs run on. When `python3` prints under `LD_PRELOAD=build/libstarkmalloc.so`, every object Python created lives in memory **you** manage.

### `> cat spec`

| Function | Contract |
|---|---|
| `sm_malloc(n)` | 16-byte aligned; `NULL` for impossible sizes; the heap grows by at most `max(request + overhead, 64 KiB)` |
| `sm_free(p)` | NULL-safe; coalesces with **both** neighbours |
| `sm_calloc(n, s)` | zeroed; `NULL` if `n·s` overflows |
| `sm_realloc(p, n)` | preserves data; `NULL` p acts like malloc; size 0 frees and returns `NULL` |
| `sm_memalign(A, n)` | A-aligned (power of two ≥ 16); `sm_free` must accept the pointer |
| `sm_usable_size(p)` | at least the size requested |

**Given (don't modify):** `heap_os.c` (a private, contiguous `sbrk` via `heap_grow`) and `preload.c` (exports `malloc`/`free`/… under a mutex).

### `> cat order_of_attack`

```text
week 3: block layout on paper → malloc via heap_grow only (no reuse) → free + explicit list
        → reuse + splitting → coalescing (all four cases)
week 4: calloc/realloc → memalign + usable_size → stress test → LD_PRELOAD suite
```

Getting "malloc with no free" working first is a legitimate milestone. Six tests pass with a bump allocator.

### `> ./hints`

<details><summary><b>block layout</b> · hint 1</summary>

Use a 16-byte header (size | alloc bit, plus 8 spare bytes) so the payload stays 16-aligned whenever blocks start 16-aligned. `heap_lo()` is page-aligned, so the first block is too.
</details>

<details><summary><b>block layout</b> · hint 2</summary>

Write tiny static helpers first and test them in your head: `size(b)`, `is_alloc(b)`, `mark(b, size, alloc)` (writes the header *and* the footer), `next_block(b)`, `prev_block(b)` (reads the footer just before b), `payload(b)` and `block_of(ptr)`. The rest of the allocator becomes readable.
</details>

<details><summary><b>growing the heap</b> · hint</summary>

When nothing fits, check whether the **last** block in the heap is free (its footer sits just before `heap_hi()`). If it is, you only need to grow by the difference. Make the new region one free block, then coalesce it backwards.
</details>

<details><summary><b>memalign</b> · hint</summary>

The simplest correct approach is to allocate `n + A + min_block`, compute the first A-aligned address at or after the payload, and, if it differs from the payload, store the offset in the spare header word just before the aligned pointer. `sm_free` and `sm_usable_size` check that word. It must be zero for normal blocks, so clear it on every allocation.
</details>

<details><summary><b>LD_PRELOAD crashes immediately</b></summary>

Almost always one of these: you called `printf`/`malloc` inside `sm_*` (infinite recursion), `sm_malloc(0)` returned something `sm_free` can't handle, or `realloc` read past the old block's usable size.
</details>

### `> cat debugging.txt`

- Write `sm_check()` (manual 04, exercise 1) **before** debugging anything. It turns "random crash in test 14" into "footer mismatch at block 0x7f…".
- The unit tests call `sm_*` directly, so `make SAN=1 test` still catches out-of-bounds writes in your *bookkeeping*.
- For preload crashes, run `LD_PRELOAD=build/libstarkmalloc.so gdb --args ls` and use `bt` to see which libc function called you with what.

### `> cat stretch.txt`

- Segregated size classes, and measure `time sort` under preload against glibc.
- Per-thread caches to replace the global lock. Benchmark a multi-threaded Python workload.
- Get stress-test utilization under 2× (address-ordered fits).
