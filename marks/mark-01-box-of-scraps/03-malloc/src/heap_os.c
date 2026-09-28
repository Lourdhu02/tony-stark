/*
 * heap_os.c: a private, contiguous heap (given code).
 *
 * We deliberately avoid the real sbrk(): glibc's malloc also moves the
 * program break, and two allocators fighting over it corrupts both.
 * Instead we reserve a large virtual range once and hand it out
 * incrementally. The kernel only backs pages with RAM when first touched.
 */
#include "stark_malloc.h"

#include <stdint.h>
#include <sys/mman.h>

#define HEAP_RESERVE (1ULL << 32) /* 4 GiB of address space, not RAM */

static char *lo, *brk_, *hi_limit;

static int heap_init(void)
{
    void *p = mmap(NULL, HEAP_RESERVE, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (p == MAP_FAILED)
        return -1;
    lo = brk_ = p;
    hi_limit = lo + HEAP_RESERVE;
    return 0;
}

void *heap_grow(size_t increment)
{
    if (!lo && heap_init() != 0)
        return NULL;
    if (increment > (size_t)(hi_limit - brk_))
        return NULL;
    char *old = brk_;
    brk_ += increment;
    return old;
}

void *heap_lo(void) { return lo; }
void *heap_hi(void) { return brk_; }
size_t sm_heap_size(void) { return (size_t)(brk_ - lo); }
