# 01 · linalg

> Week 1 · 14 tests · Field manuals: [01 memory hierarchy](../notes/01-memory-hierarchy.md), [02 LU and floating point](../notes/02-lu-and-floating-point.md)

A dense matrix library in plain C: the foundation for LQR (Mark V), Kalman filters (Mark VII) and everything else that says "solve".

### `> cat spec`

| Function | Contract | Tests |
|---|---|---|
| `mat_new`, `mat_free` | zeroed `rows×cols`; `NULL` if either is 0; `free` is NULL-safe | `new_is_zeroed`, `new_rejects_zero_size` |
| `mat_identity`, `mat_copy` | deep copy: the new matrix owns its buffer | `identity`, `copy_is_deep` |
| `mat_add`, `mat_scale`, `mat_transpose` | `NULL` on shape mismatch | `add_and_scale`, `transpose_non_square` |
| `mat_mul` | the naive i-j-k loop, your baseline | `mul_known_values` |
| `mat_mul_fast` | same result as `mat_mul` within 1e-9, **3× faster or more** at n=512 | `mul_fast_matches_naive` + `make bench` |
| `mat_lu` | P·A = L·U with partial pivoting; −1 if singular or not square | `lu_reconstructs_pa`, `lu_requires_pivoting`, `lu_detects_singular` |
| `mat_solve`, `mat_inverse` | built on LU; `NULL` if singular | `solve_residual`, `inverse` |
| `mat_det` | (−1)^swaps · Π uᵢᵢ; `NAN` if not square | `determinant` |

### `> cat order_of_attack`

```text
new/free → identity → copy → add/scale → transpose → mul          (day 1: C warm-up)
mat_lu (on paper first!) → solve → inverse → det                  (day 2–3)
mul_fast → bench → tile-size sweep                                (day 4)
```

### `> ./hints`

<details><summary><b>mat_lu</b> · hint 1 (nudge)</summary>

Work on a *copy* of A that becomes U. Start L as the identity. Every time you swap rows k and p in U, you must also swap the *already computed* part of L (columns 0…k−1) and the entries of `perm`.
</details>

<details><summary><b>mat_lu</b> · hint 2 (approach)</summary>

For each column k: find p = argmax over i ≥ k of |U[i][k]|. If that value is below 1e-12, free what you allocated and return −1. Swap as described in hint 1. Then for each row i > k: compute l = U[i][k] / U[k][k], store it in L[i][k], and subtract l × row k from row i of U (columns k…n−1).
</details>

<details><summary><b>mat_lu</b> · hint 3 (algorithm check)</summary>

After the loop, U[i][k] for i > k should be *exactly* 0. Set it explicitly, because rounding leaves values like 1e-17 and the test checks for exact zeros. `perm[i]` starts as `i` and is swapped together with the rows.
</details>

<details><summary><b>mat_solve</b> · hint</summary>

Per column c of B, do a forward solve on L·y = P·b (read b in `perm` order), then a back solve on U·x = y. L has ones on its diagonal, so the forward solve divides by nothing.
</details>

<details><summary><b>mat_mul_fast</b> · hints</summary>

1. Change the loop order to i-k-j before anything else. That alone is usually 3–5× at n=512.
2. Hoist `a = A[i][k]` into a local variable and use raw row pointers (`double *crow = C->data + i*cols`).
3. Block all three loops with a tile size around 32–64, using `k < K && k < kk + T` style bounds so ragged edges work (the test deliberately uses 97×131 · 131×53).
</details>

### `> cat debugging.txt`

- **`lu_reconstructs_pa` fails but `solve` passes?** Your L swap is probably touching columns ≥ k. Only swap the first k columns of L.
- **Numbers are NaN?** Division by a zero pivot. Is the pivot search starting at row k, not row 0?
- **Crashes in random tests?** Run `make SAN=1 test`. An off-by-one in `i * cols + j` shows up as heap-buffer-overflow.

### `> cat stretch.txt`

- AVX2 intrinsics (`_mm256_fmadd_pd`) in the inner loop. Where does GFLOP/s top out?
- Parallelize the outer tile loop with pthreads and measure scaling with 1, 2, 4 and 8 threads.
- Cholesky factorization for symmetric positive-definite matrices: about half the cost of LU, and what Kalman filters use.
