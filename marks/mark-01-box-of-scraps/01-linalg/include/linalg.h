/*
 * linalg.h: a dense matrix library in plain C.
 *
 * Every function that returns a Mat * allocates a new matrix that the
 * caller owns and must release with mat_free(). Functions return NULL on
 * invalid input (shape mismatch, zero size, singular matrix, allocation
 * failure) rather than crashing.
 */
#ifndef LINALG_H
#define LINALG_H

#include <math.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
    size_t rows, cols;
    double *data; /* row-major: element (i, j) lives at data[i * cols + j] */
} Mat;

/* ── given ──────────────────────────────────────────────────────────── */

static inline double mat_at(const Mat *m, size_t i, size_t j)
{
    return m->data[i * m->cols + j];
}

static inline void mat_set(Mat *m, size_t i, size_t j, double v)
{
    m->data[i * m->cols + j] = v;
}

/* 1 if a and b have the same shape and every element differs by <= tol. */
static inline int mat_equal(const Mat *a, const Mat *b, double tol)
{
    if (!a || !b || a->rows != b->rows || a->cols != b->cols)
        return 0;
    for (size_t k = 0; k < a->rows * a->cols; k++)
        if (!(fabs(a->data[k] - b->data[k]) <= tol))
            return 0;
    return 1;
}

static inline void mat_print(const Mat *m)
{
    for (size_t i = 0; i < m->rows; i++) {
        for (size_t j = 0; j < m->cols; j++)
            printf("%10.4f ", mat_at(m, i, j));
        putchar('\n');
    }
}

/* ── yours ──────────────────────────────────────────────────────────── */

/* Construction */
Mat *mat_new(size_t rows, size_t cols);  /* zero-filled; NULL if rows or cols is 0 */
void mat_free(Mat *m);                   /* NULL-safe */
Mat *mat_identity(size_t n);
Mat *mat_copy(const Mat *m);             /* deep copy */

/* Element-wise and structural */
Mat *mat_add(const Mat *a, const Mat *b);
Mat *mat_scale(const Mat *a, double s);
Mat *mat_transpose(const Mat *a);

/* Multiplication */
Mat *mat_mul(const Mat *a, const Mat *b);      /* textbook i-j-k triple loop */
Mat *mat_mul_fast(const Mat *a, const Mat *b); /* your cache-friendly version */

/*
 * LU decomposition with partial pivoting: P·A = L·U
 *   L     unit lower-triangular (ones on the diagonal)
 *   U     upper-triangular
 *   perm  array of n row indices: row i of P·A is row perm[i] of A
 * Returns 0 on success, -1 if A is not square or is singular.
 * On success, *L and *U are new matrices owned by the caller.
 */
int mat_lu(const Mat *a, Mat **L, Mat **U, size_t *perm);

Mat *mat_solve(const Mat *a, const Mat *b); /* X such that A·X = B */
Mat *mat_inverse(const Mat *a);
double mat_det(const Mat *a);               /* NAN if A is not square */

#endif /* LINALG_H */
