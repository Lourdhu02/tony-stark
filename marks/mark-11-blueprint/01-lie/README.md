# 01 · lie

> Weeks 91–93 · 20 tests · Chapters: [01 rotations](../chapters/01-rotations.md), [02 rigid motions](../chapters/02-rigid-motions.md) · Code: [`lab/blueprint/so3.py`](../lab/blueprint/so3.py), [`se3.py`](../lab/blueprint/se3.py)

The Lie-group toolkit that every later system (and Marks XII–XIII) is built on.

### `> cat spec`

| Function | Contract |
|---|---|
| `so3.hat/vee`, `se3.hat/vee` | the isomorphisms $\mathbb R^3 \leftrightarrow \mathfrak{so}(3)$ and $\mathbb R^6 \leftrightarrow \mathfrak{se}(3)$ |
| `so3.exp/log` | Rodrigues; the log is correct in all 3 regimes (θ ≈ 0, generic, θ ≈ π) |
| `so3.left_jacobian(_inv)` | closed forms; finite at θ = 0 |
| `so3.quat_from_matrix / matrix_from_quat` | Hamilton $(w,x,y,z)$, $w \ge 0$, Shepperd-safe |
| `se3.exp/log/inverse` | closed forms, reusing `so3` |
| `se3.adjoint`, `se3.ad` | $[\mathrm{Ad}_T]$ and $[\mathrm{ad}_\xi]$, twist-first ordering |

### `> make test`

```sh
cd marks/mark-11-blueprint/01-lie && make test     # or `make test` in the Mark root for the dashboard
```

### `> ./hints`

<details><summary><b>so3.log</b> · hint 1</summary>

Compute θ from the trace first and **clip** the arccos argument to [−1, 1]. Then branch on θ, not on `sin θ`.
</details>

<details><summary><b>so3.log</b> · hint 2 (near π)</summary>

$(R + \mathbb 1)/2 = \hat\omega\hat\omega^\top$ at θ = π. Take the column with the largest diagonal entry, normalize it, and multiply by θ. If `exp` of your answer doesn't reproduce R to 1e-6, flip the sign. (Near but not exactly at π, the sign matters.)
</details>

<details><summary><b>left_jacobian</b> · hint</summary>

It has the same odd/even collapse as Rodrigues, with coefficients $(1-\cos\theta)/\theta^2$ and $(\theta-\sin\theta)/\theta^3$. For small θ use $\mathbb 1 + \tfrac12[\omega] + \tfrac16[\omega]^2$.
</details>

<details><summary><b>se3.exp</b> · hint</summary>

It's two lines once `so3` works: rotation `so3.exp(w)`, translation `so3.left_jacobian(w) @ v`. If you're writing trig here, stop.
</details>

<details><summary><b>se3.adjoint</b> · hint</summary>

The lower-left block is `so3.hat(p) @ R`, not `R @ so3.hat(p)`. The conjugation test catches the difference immediately.
</details>

### `> cat stretch.txt`

- `interpolate(T0, T1, t)` on SE(3), and on SO(3) × R³ for comparison. Plot both paths.
- The right Jacobian and its inverse; verify $J_r(\omega) = J_l(-\omega)$.
- Cross-check `adjoint` against Pinocchio's `SE3.action` (mind the $(v, \omega)$ ordering).
