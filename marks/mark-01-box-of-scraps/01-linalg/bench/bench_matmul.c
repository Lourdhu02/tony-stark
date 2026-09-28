/*
 * bench_matmul: mat_mul vs mat_mul_fast.
 *
 *   make bench             sizes 128 256 512
 *   ./build/bench 1024     a custom size
 */
#include "linalg.h"

#include <stdlib.h>
#include <time.h>

static double now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static double time_it(Mat *(*mul)(const Mat *, const Mat *), const Mat *a, const Mat *b)
{
    double best = 1e30;
    for (int rep = 0; rep < 3; rep++) {
        double t0 = now();
        Mat *c = mul(a, b);
        double dt = now() - t0;
        if (!c) return -1.0;
        mat_free(c);
        if (dt < best) best = dt;
    }
    return best;
}

static void bench(size_t n)
{
    Mat *a = mat_new(n, n), *b = mat_new(n, n);
    if (!a || !b) {
        printf("  %6zu  mat_new() not implemented yet\n", n);
        return;
    }
    for (size_t k = 0; k < n * n; k++) {
        a->data[k] = rand() / (double)RAND_MAX;
        b->data[k] = rand() / (double)RAND_MAX;
    }

    double flops = 2.0 * n * n * n;
    double slow = time_it(mat_mul, a, b);
    double fast = time_it(mat_mul_fast, a, b);

    if (slow < 0 || fast < 0)
        printf("  %6zu  mat_mul / mat_mul_fast not implemented yet\n", n);
    else
        printf("  %6zu  %12.2f  %12.2f  %7.1fx\n", n,
               flops / slow * 1e-9, flops / fast * 1e-9, slow / fast);

    mat_free(a);
    mat_free(b);
}

int main(int argc, char **argv)
{
    printf("  %6s  %12s  %12s  %8s\n", "n", "naive GF/s", "fast GF/s", "speedup");
    printf("  ──────  ────────────  ────────────  ────────\n");
    if (argc > 1) {
        for (int i = 1; i < argc; i++)
            bench((size_t)strtoul(argv[i], NULL, 10));
    } else {
        bench(128);
        bench(256);
        bench(512);
    }
    return 0;
}
