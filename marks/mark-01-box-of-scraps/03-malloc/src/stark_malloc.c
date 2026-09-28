/*
 * stark_malloc.c: your allocator.
 *
 * Suggested design (the classic one from CS:APP §9.9):
 *
 *   block = [ header | payload ............ | footer ]
 *   header/footer = block size | allocated bit   (sizes are multiples of 16,
 *                                                  so the low 4 bits are free)
 *
 *   - An explicit, doubly-linked free list threaded through free payloads
 *   - First-fit search; split a block when the remainder is big enough
 *   - Boundary-tag coalescing with both neighbours on free
 *   - Grow with heap_grow() only when nothing fits (and only by what you
 *     need, at most max(request, 64 KiB), or the tests will notice)
 *
 * Rules:
 *   - Do NOT call malloc/free/printf in here: under LD_PRELOAD they are
 *     *you*, and you'll recurse forever. Debug with write(2, ...) if needed.
 *   - Every returned pointer must be 16-byte aligned.
 */
#include "stark_malloc.h"

#include <stdint.h>
#include <string.h>

void *sm_malloc(size_t size)
{
    /* TODO */
    (void)size;
    return NULL;
}

void sm_free(void *ptr)
{
    /* TODO */
    (void)ptr;
}

void *sm_calloc(size_t n, size_t size)
{
    /* TODO: check n·size for overflow before you multiply. */
    (void)n;
    (void)size;
    return NULL;
}

void *sm_realloc(void *ptr, size_t size)
{
    /* TODO: shrink in place; grow in place if the next block is free; else move. */
    (void)ptr;
    (void)size;
    return NULL;
}

void *sm_memalign(size_t alignment, size_t size)
{
    /*
     * TODO: over-allocate, then find an aligned address inside the block.
     * sm_free() must still work on the pointer you return, so either split
     * off the leading slack as its own free block, or leave a small tag
     * just before the aligned pointer that leads back to the real header.
     */
    (void)alignment;
    (void)size;
    return NULL;
}

size_t sm_usable_size(void *ptr)
{
    /* TODO */
    (void)ptr;
    return 0;
}
