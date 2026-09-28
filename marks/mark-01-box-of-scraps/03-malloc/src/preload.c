/*
 * preload.c: exports the standard allocator API on top of sm_* (given code).
 *
 *   LD_PRELOAD=./build/libstarkmalloc.so <any program>
 *
 * A single global lock makes your allocator safe for multi-threaded
 * programs. (Making it scale, with per-thread caches, is a stretch goal.)
 */
#include "stark_malloc.h"

#include <errno.h>
#include <pthread.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *malloc(size_t size)
{
    pthread_mutex_lock(&lock);
    void *p = sm_malloc(size);
    pthread_mutex_unlock(&lock);
    return p;
}

void free(void *ptr)
{
    if (!ptr)
        return;
    pthread_mutex_lock(&lock);
    sm_free(ptr);
    pthread_mutex_unlock(&lock);
}

void *calloc(size_t n, size_t size)
{
    pthread_mutex_lock(&lock);
    void *p = sm_calloc(n, size);
    pthread_mutex_unlock(&lock);
    return p;
}

void *realloc(void *ptr, size_t size)
{
    pthread_mutex_lock(&lock);
    void *p = sm_realloc(ptr, size);
    pthread_mutex_unlock(&lock);
    return p;
}

size_t malloc_usable_size(void *ptr)
{
    if (!ptr)
        return 0;
    pthread_mutex_lock(&lock);
    size_t n = sm_usable_size(ptr);
    pthread_mutex_unlock(&lock);
    return n;
}

static void *aligned(size_t alignment, size_t size)
{
    if (alignment < SM_ALIGN)
        alignment = SM_ALIGN;
    pthread_mutex_lock(&lock);
    void *p = sm_memalign(alignment, size);
    pthread_mutex_unlock(&lock);
    return p;
}

int posix_memalign(void **out, size_t alignment, size_t size)
{
    if (alignment == 0 || (alignment & (alignment - 1)) || alignment % sizeof(void *))
        return EINVAL;
    void *p = aligned(alignment, size);
    if (!p && size)
        return ENOMEM;
    *out = p;
    return 0;
}

void *aligned_alloc(size_t alignment, size_t size) { return aligned(alignment, size); }
void *memalign(size_t alignment, size_t size) { return aligned(alignment, size); }
void *valloc(size_t size) { return aligned(4096, size); }
void *pvalloc(size_t size) { return aligned(4096, (size + 4095) & ~(size_t)4095); }
