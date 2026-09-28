#include "stark_malloc.h"
#include "test.h"

#include <stdint.h>

#define KiB ((size_t)1024)
#define MiB (1024 * KiB)

static int aligned16(const void *p) { return ((uintptr_t)p & (SM_ALIGN - 1)) == 0; }

static uint64_t rng = 0x5eed;
static uint32_t rnd(void)
{
    rng ^= rng << 13;
    rng ^= rng >> 7;
    rng ^= rng << 17;
    return (uint32_t)rng;
}

/* ── basics ─────────────────────────────────────────────────────────── */

TEST(malloc_returns_writable_memory)
{
    char *p = sm_malloc(100);
    REQUIRE(p != NULL);
    memset(p, 'x', 100);
    CHECK(p[0] == 'x' && p[99] == 'x');
    sm_free(p);
}

TEST(pointers_are_16_byte_aligned)
{
    void *ptrs[300];
    for (size_t i = 0; i < 300; i++) {
        ptrs[i] = sm_malloc(i + 1);
        REQUIRE(ptrs[i] != NULL);
        CHECK(aligned16(ptrs[i]));
    }
    for (size_t i = 0; i < 300; i++)
        sm_free(ptrs[i]);
}

TEST(blocks_never_overlap)
{
    enum { N = 500 };
    unsigned char *ptrs[N];
    size_t sizes[N];
    for (size_t i = 0; i < N; i++) {
        sizes[i] = 1 + rnd() % 2000;
        ptrs[i] = sm_malloc(sizes[i]);
        REQUIRE(ptrs[i] != NULL);
        memset(ptrs[i], (int)(i & 0xff), sizes[i]);
    }
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < sizes[i]; j++)
            if (ptrs[i][j] != (i & 0xff)) {
                CHECK(!"block contents were overwritten by another block");
                return;
            }
    for (size_t i = 0; i < N; i += 2) /* free in a scattered order */
        sm_free(ptrs[i]);
    for (size_t i = 1; i < N; i += 2)
        sm_free(ptrs[i]);
}

TEST(free_null_and_malloc_zero)
{
    void *real = sm_malloc(8);
    REQUIRE(real != NULL);
    sm_free(real);
    sm_free(NULL);
    void *p = sm_malloc(0);
    sm_free(p); /* NULL or a unique pointer; both must be freeable */
    CHECK(sm_malloc(SIZE_MAX - 8) == NULL);
}

TEST(heap_grows_modestly)
{
    void *p = sm_malloc(16);
    REQUIRE(p != NULL);
    CHECK(sm_heap_size() <= 68 * KiB);
    void *q = sm_malloc(1 * MiB);
    REQUIRE(q != NULL);
    CHECK(sm_heap_size() <= 1 * MiB + 140 * KiB);
}

/* ── the free list ──────────────────────────────────────────────────── */

TEST(reuses_freed_memory)
{
    void *p = sm_malloc(256 * KiB);
    REQUIRE(p != NULL);
    sm_free(p);
    size_t before = sm_heap_size();
    void *q = sm_malloc(256 * KiB);
    REQUIRE(q != NULL);
    CHECK(sm_heap_size() == before);
}

TEST(splits_large_free_blocks)
{
    void *big = sm_malloc(1 * MiB);
    void *guard = sm_malloc(16);
    REQUIRE(big && guard);
    sm_free(big);
    size_t before = sm_heap_size();
    void *a = sm_malloc(300 * KiB), *b = sm_malloc(300 * KiB), *c = sm_malloc(300 * KiB);
    REQUIRE(a && b && c);
    CHECK(sm_heap_size() == before); /* all three fit in the freed 1 MiB */
}

TEST(coalesces_both_neighbours)
{
    void *a = sm_malloc(256 * KiB), *b = sm_malloc(256 * KiB), *c = sm_malloc(256 * KiB);
    void *guard = sm_malloc(16);
    REQUIRE(a && b && c && guard);
    sm_free(a);
    sm_free(c);
    sm_free(b); /* b must merge with the free blocks on BOTH sides */
    size_t before = sm_heap_size();
    void *big = sm_malloc(700 * KiB);
    REQUIRE(big != NULL);
    CHECK(sm_heap_size() == before);
}

/* ── calloc / realloc / memalign ────────────────────────────────────── */

TEST(calloc_zeroes_recycled_memory)
{
    unsigned char *dirty = sm_malloc(4096);
    REQUIRE(dirty != NULL);
    memset(dirty, 0xAB, 4096);
    sm_free(dirty);

    unsigned char *p = sm_calloc(64, 64);
    REQUIRE(p != NULL);
    for (size_t i = 0; i < 4096; i++)
        if (p[i] != 0) {
            CHECK(!"calloc returned non-zero bytes");
            break;
        }
    CHECK(sm_calloc(SIZE_MAX / 2, 4) == NULL); /* n·size overflows */
}

TEST(realloc_semantics)
{
    char *p = sm_realloc(NULL, 64); /* acts like malloc */
    REQUIRE(p != NULL);
    for (int i = 0; i < 64; i++)
        p[i] = (char)i;

    p = sm_realloc(p, 100 * KiB); /* grow: contents preserved */
    REQUIRE(p != NULL);
    CHECK(aligned16(p));
    for (int i = 0; i < 64; i++)
        CHECK(p[i] == (char)i);

    p = sm_realloc(p, 32); /* shrink: prefix preserved */
    REQUIRE(p != NULL);
    for (int i = 0; i < 32; i++)
        CHECK(p[i] == (char)i);

    CHECK(sm_realloc(p, 0) == NULL); /* acts like free */
}

TEST(memalign_honours_alignment)
{
    for (size_t align = 32; align <= 4096; align <<= 1) {
        unsigned char *p = sm_memalign(align, 100);
        REQUIRE(p != NULL);
        CHECK(((uintptr_t)p & (align - 1)) == 0);
        memset(p, 0xCD, 100);
        sm_free(p);
    }
    size_t before = sm_heap_size();
    for (int i = 0; i < 1000; i++)
        sm_free(sm_memalign(4096, 100));
    CHECK(sm_heap_size() - before < 1 * MiB); /* freed aligned blocks get reused */
}

TEST(usable_size_covers_request)
{
    for (size_t n = 1; n < 5000; n += 97) {
        void *p = sm_malloc(n);
        REQUIRE(p != NULL);
        CHECK(sm_usable_size(p) >= n);
        sm_free(p);
    }
}

TEST(large_allocation)
{
    size_t n = 64 * MiB;
    unsigned char *p = sm_malloc(n);
    REQUIRE(p != NULL);
    p[0] = 1;
    p[n - 1] = 2;
    CHECK(p[0] == 1 && p[n - 1] == 2);
    sm_free(p);
}

/* ── the real workout ───────────────────────────────────────────────── */

TEST(stress_random_workload)
{
    enum { SLOTS = 1000, OPS = 200000 };
    static unsigned char *ptr[SLOTS];
    static size_t size[SLOTS];
    size_t live = 0, peak = 0;

    for (int op = 0; op < OPS; op++) {
        size_t i = rnd() % SLOTS;
        if (ptr[i]) {
            for (size_t j = 0; j < size[i]; j += 61)
                if (ptr[i][j] != (unsigned char)(i * 7)) {
                    CHECK(!"heap corruption detected");
                    return;
                }
            if (rnd() % 4 == 0) { /* sometimes realloc instead of free */
                size_t n = 1 + rnd() % 8192;
                unsigned char *q = sm_realloc(ptr[i], n);
                REQUIRE(q != NULL);
                for (size_t j = 0; j < (n < size[i] ? n : size[i]); j += 61)
                    if (q[j] != (unsigned char)(i * 7)) {
                        CHECK(!"realloc lost data");
                        return;
                    }
                memset(q, (int)(i * 7) & 0xff, n);
                live = live - size[i] + n;
                ptr[i] = q;
                size[i] = n;
            } else {
                sm_free(ptr[i]);
                live -= size[i];
                ptr[i] = NULL;
            }
        } else {
            size_t n = (rnd() % 100 == 0) ? 64 * KiB + rnd() % (192 * KiB) : 1 + rnd() % 4096;
            ptr[i] = sm_malloc(n);
            REQUIRE(ptr[i] != NULL);
            CHECK(aligned16(ptr[i]));
            memset(ptr[i], (int)(i * 7) & 0xff, n);
            size[i] = n;
            live += n;
        }
        if (live > peak) peak = live;
    }

    /*
     * Utilization: heap size vs peak live bytes. LIFO first-fit with
     * coalescing lands around 3x on this workload. Address-ordered or
     * segregated free lists do better: get under 2x for bragging rights.
     */
    CHECK(sm_heap_size() < 4 * peak);
}

int main(void)
{
    RUN(malloc_returns_writable_memory);
    RUN(pointers_are_16_byte_aligned);
    RUN(blocks_never_overlap);
    RUN(free_null_and_malloc_zero);
    RUN(heap_grows_modestly);
    RUN(reuses_freed_memory);
    RUN(splits_large_free_blocks);
    RUN(coalesces_both_neighbours);
    RUN(calloc_zeroes_recycled_memory);
    RUN(realloc_semantics);
    RUN(memalign_honours_alignment);
    RUN(usable_size_covers_request);
    RUN(large_allocation);
    RUN(stress_random_workload);
    return TEST_REPORT("malloc");
}
