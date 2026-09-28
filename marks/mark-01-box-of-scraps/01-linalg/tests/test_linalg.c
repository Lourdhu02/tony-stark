#include "linalg.h"
#include "test.h"

#include <stdint.h>

/* Deterministic pseudo-random numbers in [-1, 1]. */
static uint64_t rng_state = 42;
static double rnd(void)
{
    rng_state = rng_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return (double)(rng_state >> 11) / (double)(1ULL << 53) * 2.0 - 1.0;
}

static Mat *random_mat(size_t r, size_t c)
{
    Mat *m = mat_new(r, c);
    REQUIRE(m != NULL);
    for (size_t k = 0; k < r * c; k++)
        m->data[k] = rnd();
    return m;
}

static Mat *from(size_t r, size_t c, const double *vals)
{
    Mat *m = mat_new(r, c);
    REQUIRE(m != NULL);
    for (size_t k = 0; k < r * c; k++)
        m->data[k] = vals[k];
    return m;
}

TEST(new_is_zeroed)
{
    Mat *m = mat_new(3, 4);
    REQUIRE(m != NULL);
    CHECK(m->rows == 3 && m->cols == 4);
    for (size_t k = 0; k < 12; k++)
        CHECK(m->data[k] == 0.0);
    mat_free(m);
    mat_free(NULL);
}

TEST(new_rejects_zero_size)
{
    Mat *ok = mat_new(1, 1);
    REQUIRE(ok != NULL);
    mat_free(ok);
    CHECK(mat_new(0, 3) == NULL);
    CHECK(mat_new(3, 0) == NULL);
}

TEST(identity)
{
    Mat *I = mat_identity(4);
    REQUIRE(I != NULL);
    for (size_t i = 0; i < 4; i++)
        for (size_t j = 0; j < 4; j++)
            CHECK(mat_at(I, i, j) == (i == j ? 1.0 : 0.0));
    mat_free(I);
}

TEST(copy_is_deep)
{
    Mat *a = random_mat(3, 3);
    Mat *b = mat_copy(a);
    REQUIRE(b != NULL);
    CHECK(b->data != a->data);
    CHECK(mat_equal(a, b, 0.0));
    mat_set(b, 0, 0, 123.0);
    CHECK(mat_at(a, 0, 0) != 123.0);
    mat_free(a);
    mat_free(b);
}

TEST(add_and_scale)
{
    Mat *a = from(2, 2, (double[]){1, 2, 3, 4});
    Mat *b = from(2, 2, (double[]){10, 20, 30, 40});
    Mat *want_sum = from(2, 2, (double[]){11, 22, 33, 44});
    Mat *want_scaled = from(2, 2, (double[]){-2, -4, -6, -8});

    Mat *sum = mat_add(a, b);
    Mat *scaled = mat_scale(a, -2.0);
    CHECK(mat_equal(sum, want_sum, 0.0));
    CHECK(mat_equal(scaled, want_scaled, 0.0));

    Mat *c = mat_new(3, 2);
    CHECK(mat_add(a, c) == NULL); /* shape mismatch */

    mat_free(a); mat_free(b); mat_free(c);
    mat_free(sum); mat_free(scaled);
    mat_free(want_sum); mat_free(want_scaled);
}

TEST(transpose_non_square)
{
    Mat *a = from(2, 3, (double[]){1, 2, 3, 4, 5, 6});
    Mat *want = from(3, 2, (double[]){1, 4, 2, 5, 3, 6});
    Mat *t = mat_transpose(a);
    CHECK(mat_equal(t, want, 0.0));
    mat_free(a); mat_free(want); mat_free(t);
}

TEST(mul_known_values)
{
    Mat *a = from(2, 3, (double[]){1, 2, 3, 4, 5, 6});
    Mat *b = from(3, 2, (double[]){7, 8, 9, 10, 11, 12});
    Mat *want = from(2, 2, (double[]){58, 64, 139, 154});
    Mat *c = mat_mul(a, b);
    CHECK(mat_equal(c, want, 1e-12));
    CHECK(mat_mul(a, a) == NULL); /* (2x3)(2x3) is undefined */
    mat_free(a); mat_free(b); mat_free(want); mat_free(c);
}

TEST(mul_fast_matches_naive)
{
    /* Odd sizes on purpose: blocked code must handle ragged edges. */
    Mat *a = random_mat(97, 131);
    Mat *b = random_mat(131, 53);
    Mat *slow = mat_mul(a, b);
    Mat *fast = mat_mul_fast(a, b);
    REQUIRE(slow && fast);
    CHECK(mat_equal(slow, fast, 1e-9));
    CHECK(mat_mul_fast(a, a) == NULL);
    mat_free(a); mat_free(b); mat_free(slow); mat_free(fast);
}

TEST(lu_reconstructs_pa)
{
    enum { N = 6 };
    Mat *a = random_mat(N, N);
    Mat *L = NULL, *U = NULL;
    size_t perm[N];
    REQUIRE(mat_lu(a, &L, &U, perm) == 0);
    REQUIRE(L && U);

    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++) {
            if (j > i) CHECK(mat_at(L, i, j) == 0.0);
            if (j == i) CHECK_NEAR(mat_at(L, i, j), 1.0, 1e-12);
            if (j < i) CHECK(mat_at(U, i, j) == 0.0);
        }

    Mat *pa = mat_new(N, N);
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            mat_set(pa, i, j, mat_at(a, perm[i], j));
    Mat *lu = mat_mul(L, U);
    CHECK(mat_equal(pa, lu, 1e-10));

    mat_free(a); mat_free(L); mat_free(U); mat_free(pa); mat_free(lu);
}

TEST(lu_requires_pivoting)
{
    /* A zero on the diagonal: LU without pivoting divides by zero here. */
    Mat *a = from(3, 3, (double[]){0, 2, 1, 1, 1, 1, 2, 1, 0});
    Mat *L = NULL, *U = NULL;
    size_t perm[3];
    REQUIRE(mat_lu(a, &L, &U, perm) == 0);
    for (size_t k = 0; k < 9; k++)
        CHECK(isfinite(L->data[k]) && isfinite(U->data[k]));
    mat_free(a); mat_free(L); mat_free(U);
}

TEST(lu_detects_singular)
{
    Mat *a = from(3, 3, (double[]){1, 2, 3, 2, 4, 6, 1, 0, 1}); /* row 2 = 2 * row 1 */
    Mat *L = NULL, *U = NULL;
    size_t perm[3];
    CHECK(mat_lu(a, &L, &U, perm) == -1);
    Mat *rect = mat_new(2, 3);
    CHECK(mat_lu(rect, &L, &U, perm) == -1);
    mat_free(a); mat_free(rect);
}

TEST(solve_residual)
{
    enum { N = 8 };
    Mat *a = random_mat(N, N);
    Mat *b = random_mat(N, 3);
    Mat *x = mat_solve(a, b);
    REQUIRE(x != NULL);
    CHECK(x->rows == N && x->cols == 3);
    Mat *ax = mat_mul(a, x);
    CHECK(mat_equal(ax, b, 1e-9));
    mat_free(a); mat_free(b); mat_free(x); mat_free(ax);
}

TEST(inverse)
{
    enum { N = 5 };
    Mat *a = random_mat(N, N);
    Mat *inv = mat_inverse(a);
    REQUIRE(inv != NULL);
    Mat *prod = mat_mul(a, inv);
    Mat *I = mat_identity(N);
    CHECK(mat_equal(prod, I, 1e-10));

    Mat *sing = from(2, 2, (double[]){1, 2, 2, 4});
    CHECK(mat_inverse(sing) == NULL);
    mat_free(a); mat_free(inv); mat_free(prod); mat_free(I); mat_free(sing);
}

TEST(determinant)
{
    Mat *a = from(3, 3, (double[]){6, 1, 1, 4, -2, 5, 2, 8, 7});
    CHECK_NEAR(mat_det(a), -306.0, 1e-9);

    Mat *swap = from(2, 2, (double[]){0, 1, 1, 0}); /* one row swap: det = -1 */
    CHECK_NEAR(mat_det(swap), -1.0, 1e-12);

    Mat *sing = from(2, 2, (double[]){1, 2, 2, 4});
    CHECK_NEAR(mat_det(sing), 0.0, 1e-12);

    Mat *rect = mat_new(2, 3);
    CHECK(isnan(mat_det(rect)));
    mat_free(a); mat_free(swap); mat_free(sing); mat_free(rect);
}

int main(void)
{
    RUN(new_is_zeroed);
    RUN(new_rejects_zero_size);
    RUN(identity);
    RUN(copy_is_deep);
    RUN(add_and_scale);
    RUN(transpose_non_square);
    RUN(mul_known_values);
    RUN(mul_fast_matches_naive);
    RUN(lu_reconstructs_pa);
    RUN(lu_requires_pivoting);
    RUN(lu_detects_singular);
    RUN(solve_residual);
    RUN(inverse);
    RUN(determinant);
    return TEST_REPORT("linalg");
}
