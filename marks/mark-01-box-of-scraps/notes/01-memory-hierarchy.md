# Field manual 01 · The memory hierarchy

> Used by: `01-linalg` (`mat_mul_fast`) · Later: Mark VIII (CUDA tiling is this same idea)

**The one idea:** the CPU is fast, and memory is slow. Almost every performance trick is a way to keep the data you need **close to the CPU, in the order you'll use it.**

## 1. The pyramid

```text
             ┌──────────┐   ~0.3 ns    registers        ~1 KB
             │   REG    │
            ┌┴──────────┴┐  ~1 ns      L1 cache         32–48 KB / core
            │     L1     │
          ┌─┴────────────┴─┐ ~4 ns     L2 cache         0.5–2 MB / core
          │       L2       │
       ┌──┴────────────────┴──┐ ~15 ns  L3 cache        8–64 MB shared
       │          L3          │
   ┌───┴──────────────────────┴───┐ ~80 ns   DRAM       GBs
   │             DRAM             │
   └──────────────────────────────┘
```

These numbers are orders of magnitude, not specs; measure your own machine. The gap between L1 and DRAM is about **100×**. A loop that misses cache on every access runs at DRAM speed no matter how clever its arithmetic is.

## 2. Cache lines: you never load one number

Memory moves in **cache lines** of 64 bytes, which is **8 doubles**. Touch `data[0]` and you get `data[0..7]` for free. That gives two kinds of locality:

- **Spatial:** use the neighbours you already paid for (walk arrays with stride 1).
- **Temporal:** reuse data while it's still in cache (do all the work on a tile before moving on).

## 3. Why the textbook matmul is slow

`Mat` is **row-major**: row `i` is contiguous and a column is strided by `cols`.

```text
A (row-major)                      element (i,j) at data[i*cols + j]
┌───┬───┬───┬───┐
│ 0 │ 1 │ 2 │ 3 │ ◀─ row 0: one cache line, stride-1 walk is cheap
├───┼───┼───┼───┤
│ 4 │ 5 │ 6 │ 7 │
├───┼───┼───┼───┤
│ 8 │ 9 │10 │11 │
└───┴───┴───┴───┘
  ▲
  └── column 0 = 0, 4, 8: every step jumps a whole row → a new cache line
```

The naive `C[i][j] += A[i][k] * B[k][j]` has `k` innermost:

| Loop order | Inner loop walks | Misses per inner iteration (8 doubles/line, n large) |
|---|---|---|
| `i-j-k` (naive) | A along a row ✓, **B down a column ✗** | 1/8 + 1 = **1.125** |
| `j-k-i` | A down a column ✗, C down a column ✗ | 1 + 1 = **2.0** |
| `i-k-j` | B along a row ✓, C along a row ✓ | 1/8 + 1/8 = **0.25** |

Same arithmetic, same result, and **4.5× fewer misses** just from reordering the loops (the analysis is from CS:APP §6.6.2). This is the first speedup you'll see in `make bench`.

## 4. Blocking (tiling): making reuse happen

Even `i-k-j` streams all of B through the cache once per row of A. For large n, B doesn't fit, so every row of A re-reads B from DRAM.

Instead, compute C in **b×b tiles**, so that the three tiles you're working on (one each of A, B and C) all fit in cache:

```text
3 tiles × b² doubles × 8 bytes ≤ cache size

L1 = 32 KB  →  b ≤ √(32768 / 24) ≈ 36   → try b = 32
L2 = 1 MB   →  b ≤ √(1048576 / 24) ≈ 209 → try b = 128–192
```

Inside a tile, keep the `i-k-j` order. A simple blocked version reached about **7×** at n=512 on the machine this repo was built on. Your target is 3×.

## 5. Arithmetic intensity (roofline thinking)

**Arithmetic intensity** = FLOPs ÷ bytes moved from memory.

- Matmul does `2n³` FLOPs on `3n²` numbers, so its ideal intensity grows with n. It *can* be compute-bound.
- Vector add does `n` FLOPs on `3n` numbers, so its intensity is constant and low. It will *always* be memory-bound.

When the naive matmul reaches about 1 GFLOP/s on a core that can do more than 30, it isn't doing math; it's waiting on DRAM. Blocking raises the *effective* intensity by reusing each loaded byte many times. **This is the whole story of GPU kernel optimization in Mark VIII**, with shared memory in the role of the L1 tile.

## 6. Measure it, don't guess

```sh
make -C 01-linalg bench                                 # GFLOP/s, naive vs fast
perf stat -e cache-references,cache-misses ./build/bench 512
valgrind --tool=cachegrind ./build/bench 256             # per-line miss counts
```

## Exercises

1. Predict, then measure, the GFLOP/s of all six loop orders at n = 512. Which pair ties, and why?
2. Sweep the tile size b ∈ {8, 16, 32, 64, 128, 256} and plot GFLOP/s. Where's the knee? Does it match your L1/L2 sizes (`lscpu`)?
3. Why does the naive version get *slower per FLOP* as n grows from 128 to 512 (see the bench table)?
4. Transposing B first makes the naive loop stride-1 as well. What does the transpose itself cost, and when is it worth it?
