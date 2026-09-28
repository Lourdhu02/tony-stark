# Field manual 04 · Memory allocators

> Used by: `03-malloc` · Later: Mark III (kernel page allocator), Mark IV (static allocation on MCUs), Mark VIII (GPU memory pools)

**The one idea:** `malloc` is a data structure (a set of free blocks inside a big array) plus a policy for choosing among them. Everything else is bookkeeping and alignment.

## 1. Where the heap lives

```text
high  ┌──────────────────────┐
      │ stack        ↓ grows │  local variables, return addresses
      ├──────────────────────┤
      │                      │
      │ mmap region          │  shared libs, big allocations, our heap
      │                      │
      ├──────────────────────┤
      │ heap         ↑ grows │  classic brk/sbrk heap
      ├──────────────────────┤
      │ .bss .data           │  globals
      │ .text                │  code
low   └──────────────────────┘
```

A real `malloc` gets memory from the kernel two ways: moving the **program break** (`brk`/`sbrk`) for small blocks, or calling **`mmap`** for big ones (glibc's default threshold is 128 KiB). Here, `heap_grow()` gives you a private `sbrk` over one big reserved `mmap` region, so your heap is contiguous and never collides with glibc's.

## 2. Anatomy of a block

```text
       header (16 B)            payload                 footer (8 B)
   ┌────────────────────┬─────────────────────────┬──────────────┐
   │ size | a │ (spare) │  user data ...           │  size | a    │
   └────────────────────┴─────────────────────────┴──────────────┘
   ▲                    ▲
   block start          pointer returned to the user (16-byte aligned)

   size is a multiple of 16, so its low 4 bits are always 0.
   Steal bit 0 for "allocated" (a).
```

**Why a footer?** When freeing a block you want to merge with the block *before* it. The footer of the previous block sits immediately before your header, so `*(size_t *)((char *)hdr - 8)` gives the previous block's size in O(1). These are **boundary tags**, described by Knuth in *The Art of Computer Programming*, Vol. 1.

**Minimum block size:** header + footer + room for two free-list pointers (a free block stores `next`/`prev` in its payload). That's 16 + 8 + 16 = 40, which rounds up to **48 bytes**.

## 3. Finding a free block

| Structure | Search cost | Notes |
|---|---|---|
| Implicit list: walk every block by size | O(all blocks) | Simple and slow. CS:APP's first version. |
| **Explicit list**: link free blocks only | O(free blocks) | The recommended design here |
| Segregated lists: one list per size class | ≈ O(1) | What real allocators do |

| Policy | Picks | Trade-off |
|---|---|---|
| First fit | the first block that fits | fast; fragments the front of the list |
| Next fit | first fit, resuming where the last search ended | faster, and usually worse fragmentation |
| Best fit | the smallest block that fits | less waste, and slower without a tree |
| LIFO insertion | freed blocks go to the head | O(1) free; the reference gets ~3.1× utilization |
| Address-ordered | the list is kept sorted by address | O(n) free; noticeably better fragmentation |

## 4. Splitting and coalescing

**Splitting:** found a 1 MiB block for a 300 KiB request? Cut it, return the front, and put the remainder back on the free list. Only split when the remainder is at least the minimum block size.

**Coalescing on free:** there are four cases, depending on the neighbours:

```text
case 1:  [ alloc ][ FREEING ][ alloc ]   →  just insert
case 2:  [ alloc ][ FREEING ][ free  ]   →  merge with next
case 3:  [ free  ][ FREEING ][ alloc ]   →  merge with prev
case 4:  [ free  ][ FREEING ][ free  ]   →  merge all three
```

The `coalesces_both_neighbours` test frees `a`, then `c`, then `b`, which is case 4. If you only merge forward, the 700 KiB request won't fit and the heap grows.

**Edge cases:** the first block has no previous neighbour, and the last block has no next. Either check against `heap_lo()`/`heap_hi()`, or place permanently allocated **sentinel** blocks at both ends so the neighbour checks never need a special case.

## 5. Fragmentation

- **Internal:** waste *inside* blocks (headers, alignment padding, a 48-byte minimum for a 1-byte request).
- **External:** plenty of free memory in total, but none of it contiguous enough for the request.

**Utilization** = peak live bytes ÷ heap size. The stress test requires a heap smaller than 4× the peak live bytes. LIFO first-fit lands around 3×; getting under 2× takes address ordering or segregated fits.

## 6. `realloc` and `memalign`

- **realloc:** shrinking is free (split in place). Growing: if the *next* block is free and big enough, absorb it in place. Otherwise malloc, `memcpy` and free.
- **memalign(A, n):** allocate n + A + (minimum block), then find the first A-aligned address inside. Either split off the leading slack as its own free block, or store a small tag just before the aligned pointer so that `sm_free` can find the real header.

## 7. Running under `LD_PRELOAD`

The dynamic linker resolves `malloc` to the **first** definition it finds. `LD_PRELOAD` puts your library first, so `ls`, `sort` and `python3` all call *your* code. This is **symbol interposition**. Consequences:

- **No `printf` inside `sm_*`:** `printf` may call `malloc`, which is you, so you recurse forever. Debug with `write(2, msg, len)`.
- **`preload.c` holds a global mutex:** Python has threads. Real allocators avoid the lock with per-thread caches.
- **Every `malloc`-family symbol must be yours** (`posix_memalign`, `malloc_usable_size`, …). Otherwise glibc's version receives your pointer and crashes, which is why `preload.c` exports all of them.

## 8. How the real ones work

| Allocator | Key ideas |
|---|---|
| glibc `ptmalloc2` | Bins by size, multiple arenas for threads, `mmap` for large blocks |
| `jemalloc` (FreeBSD, Rust's old default) | Size classes, per-thread caches, low fragmentation |
| `tcmalloc` (Google) | Thread-local caches for small objects, central free lists |
| `mimalloc` (Microsoft) | Free-list sharding per page, very fast |

## Exercises

1. Write `sm_check()`, a heap consistency checker that walks every block and verifies that headers match footers, no two free blocks are adjacent, and every free block is on the list. Call it after every operation in a debug build.
2. Switch from LIFO to address-ordered insertion and measure the stress test's utilization before and after.
3. Add 8 segregated size classes (16, 32, 64, …, ≥2048) and measure `sort` under `LD_PRELOAD` with `time`.
4. Why must `sm_calloc` check `n * size` for overflow *before* multiplying? Construct an input that would corrupt the heap if it didn't.
