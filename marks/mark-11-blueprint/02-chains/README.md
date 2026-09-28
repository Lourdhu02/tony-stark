# 02 · chains

> Week 94 · 8 tests · Chapter: [03 kinematic chains](../chapters/03-kinematic-chains.md) · Code: [`lab/blueprint/kinematics.py`](../lab/blueprint/kinematics.py)

Recursive forward kinematics and velocity propagation: the outward half of every dynamics algorithm.

### `> cat spec`

| Function | Contract |
|---|---|
| `fk(chain, q)` | list of $T_{0,i}$, $i = 1..n$ |
| `link_twists(chain, q, qd)` | list of body twists $\mathcal V_i$ in frame $\{i\}$ |
| `body_jacobian(chain, q)` | 6×n $J_b$ with $\mathcal V_n = J_b\dot q$ |

The model format is documented at the top of [`chain.py`](../lab/blueprint/chain.py). Read it before writing any code.

### `> ./hints`

<details><summary><b>fk</b> · hint</summary>

One accumulator: `T = T @ chain.M[i] @ se3.exp(chain.A[i] * q[i])`, appended every iteration.
</details>

<details><summary><b>link_twists</b> · hint</summary>

You need $T_{i,i-1} = e^{-[\mathcal A_i]q_i}\,M_{i-1,i}^{-1}$, not $T_{i-1,i}$. Using the wrong one leaves the `fk` tests green but fails the finite-difference twist tests, because its adjoint maps twists the wrong way.
</details>

<details><summary><b>body_jacobian</b> · hint</summary>

Sweep **inward** from the tip, keeping $T_{n,i}$ in an accumulator: column $i$ is `adjoint(T_n_i) @ A[i]`, then update `T_n_i = T_n_i @ T_{i,i-1}`. The last column is simply $\mathcal A_n$, and there's a test for that.
</details>

### `> cat stretch.txt`

- `space_jacobian` and the identity $J_s = [\mathrm{Ad}_{T_{0n}}]J_b$.
- $\dot J_b\dot q$ from the velocity-product terms (chapter 03, exercise 3).
- Trees: replace `i-1` with `parent[i]` everywhere.
