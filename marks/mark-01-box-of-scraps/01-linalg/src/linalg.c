/*
 * linalg.c: your implementation.
 *
 * Suggested order: new/free, identity, copy, add, scale, transpose, mul,
 * then mat_lu, and build solve/inverse/det on top of it.
 * Run `make test` after each function and watch the bar fill up.
 */
#include "linalg.h"

#include <stdlib.h>
#include <string.h>

Mat *mat_new(size_t rows, size_t cols)
{
    /* TODO: allocate the struct and a zeroed data buffer (hint: calloc). */
    (void)rows;
    (void)cols;
    return NULL;
}

void mat_free(Mat *m)
{
    /* TODO */
    (void)m;
}

Mat *mat_identity(size_t n)
{
    /* TODO */
    (void)n;
    return NULL;
}

Mat *mat_copy(const Mat *m)
{
    /* TODO */
    (void)m;
    return NULL;
}

Mat *mat_add(const Mat *a, const Mat *b)
{
    /* TODO: NULL on shape mismatch. */
    (void)a;
    (void)b;
    return NULL;
}

Mat *mat_scale(const Mat *a, double s)
{
    /* TODO */
    (void)a;
    (void)s;
    return NULL;
}

Mat *mat_transpose(const Mat *a)
{
    /* TODO */
    (void)a;
    return NULL;
}

Mat *mat_mul(const Mat *a, const Mat *b)
{
    /* TODO: the classic i-j-k loop. Keep it naive; it's your baseline. */
    (void)a;
    (void)b;
    return NULL;
}

Mat *mat_mul_fast(const Mat *a, const Mat *b)
{
    /*
     * TODO: beat mat_mul by at least 3x at n=512 (`make bench`).
     * Ideas, roughly in order of payoff:
     *   1. loop order i-k-j, so the innermost loop walks B and C row-wise
     *   2. hoist a(i,k) into a register
     *   3. cache blocking (tile sizes around 32–64)
     */
    (void)a;
    (void)b;
    return NULL;
}

int mat_lu(const Mat *a, Mat **L, Mat **U, size_t *perm)
{
    /*
     * TODO: Doolittle elimination with partial pivoting.
     * For each column k: pick the row with the largest |value| at or below
     * the diagonal, swap it into place (and record it in perm), then
     * eliminate everything below the pivot.
     * Treat |pivot| < 1e-12 as singular.
     */
    (void)a;
    (void)L;
    (void)U;
    (void)perm;
    return -1;
}

Mat *mat_solve(const Mat *a, const Mat *b)
{
    /* TODO: LU once, then forward-substitute (L) and back-substitute (U) per column of B. */
    (void)a;
    (void)b;
    return NULL;
}

Mat *mat_inverse(const Mat *a)
{
    /* TODO: solve A·X = I. */
    (void)a;
    return NULL;
}

double mat_det(const Mat *a)
{
    /* TODO: the product of U's diagonal, times the sign of the permutation. 0.0 if singular. */
    (void)a;
    return NAN;
}
