/*
 * stark_malloc.h: your own general-purpose memory allocator.
 *
 * You implement the sm_* functions in src/stark_malloc.c on top of a
 * private, contiguous heap provided by src/heap_os.c (given). Then
 * src/preload.c (given) exports them as malloc/free/... so that
 *
 *     LD_PRELOAD=./build/libstarkmalloc.so ls -l
 *
 * runs a real program on your allocator.
 */
#ifndef STARK_MALLOC_H
#define STARK_MALLOC_H

#include <stddef.h>

#define SM_ALIGN 16 /* every pointer you return must be 16-byte aligned */

/* ── yours (src/stark_malloc.c) ─────────────────────────────────────── */

void  *sm_malloc(size_t size);             /* size 0 → NULL or a unique freeable pointer */
void   sm_free(void *ptr);                 /* NULL-safe */
void  *sm_calloc(size_t n, size_t size);   /* zeroed; NULL if n·size overflows */
void  *sm_realloc(void *ptr, size_t size); /* NULL ptr → malloc; size 0 → free + NULL */

/* Aligned allocation. alignment is a power of two ≥ SM_ALIGN. */
void  *sm_memalign(size_t alignment, size_t size);

/* Usable bytes in the block (≥ the size requested). */
size_t sm_usable_size(void *ptr);

/* ── given (src/heap_os.c) ──────────────────────────────────────────── */

/*
 * A private sbrk(): grows the heap by `increment` bytes and returns the
 * old break, i.e. the start of the new region. Returns NULL when out of
 * space. Memory comes from one big reserved mmap region, so successive
 * calls are always contiguous, which is what makes coalescing possible.
 */
void  *heap_grow(size_t increment);
void  *heap_lo(void);       /* first byte of the heap */
void  *heap_hi(void);       /* one past the last byte (the current break) */
size_t sm_heap_size(void);  /* heap_hi() - heap_lo() */

#endif /* STARK_MALLOC_H */
