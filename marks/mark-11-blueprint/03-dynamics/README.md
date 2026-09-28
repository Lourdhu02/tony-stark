# 03 · dynamics

> Weeks 95–100 · 14 tests · Chapters: [04](../chapters/04-newton-euler.md) · [05](../chapters/05-rnea.md) · [06](../chapters/06-mass-matrix.md) · [07](../chapters/07-aba.md) · Code: [`lab/blueprint/dynamics.py`](../lab/blueprint/dynamics.py)

The three canonical rigid-body dynamics algorithms, which every simulator, controller and trajectory optimizer calls millions of times.

### `> cat spec`

| Function | Complexity | Oracle in the tests |
|---|---|---|
| `rnea(chain, q, qd, qdd, gravity=True)` | O(n) | the double pendulum's closed-form Lagrangian |
| `mass_matrix(chain, q)` (CRBA) | O(n²) | columns of RNEA with q̇ = 0, gravity off |
| `aba(chain, q, qd, tau)` | O(n) | $M^{-1}(\tau - h)$, and RNEA's inverse |
| `energy(chain, q, qd)` | O(n) | conservation over a 1 s RK4 simulation |

The strictest test is `test_energy_is_conserved_in_simulation`. Any inconsistency anywhere shows up as energy created or destroyed.

### `> cat order_of_attack`

```text
energy → rnea (pendulum → double pendulum) → mass_matrix → passivity test → aba → energy conservation
```

### `> ./hints`

<details><summary><b>rnea</b> · hint 1</summary>

Store the three per-link lists from the outward pass: the adjoints $X_i$, the $\mathcal V_i$ and the $\dot{\mathcal V}_i$. The inward pass needs $X_{i+1}^\top$, the adjoint belonging to the **child**, not to link $i$.
</details>

<details><summary><b>rnea</b> · hint 2</summary>

With `gravity=False`, set $\dot{\mathcal V}_0 = 0$; otherwise $(0, -g)$ where `g = chain.g = (0, 0, -9.81)`. So the base's linear acceleration is $(0, 0, +9.81)$: *upward*.
</details>

<details><summary><b>mass_matrix</b> · hint</summary>

Build the composite inertias inward first. Then for each column $i$: $\mathcal F = \mathcal I^c_i\mathcal A_i$, $M_{ii} = \mathcal A_i^\top\mathcal F$; then for $j = i-1 \dots 1$: $\mathcal F \leftarrow X_{j+1}^\top\mathcal F$, $M_{ji} = M_{ij} = \mathcal A_j^\top\mathcal F$.
</details>

<details><summary><b>aba</b> · hint</summary>

Write the three passes exactly as in chapter 07 §5, storing $U_i$, $D_i$, $u_i$ and $c_i$ in lists. The two classic bugs are forgetting $\mathcal I^a_i c_i$ in the bias, and using $\mathcal I^A_i$ instead of $\mathcal I^a_i$ when accumulating into the parent.
</details>

<details><summary><b>energy</b> · hint</summary>

Kinetic: $\sum\tfrac12\mathcal V_i^\top\mathcal G_i\mathcal V_i$ using `link_twists`. Potential: $\sum m_i\cdot(-g)\cdot p_i$, with $p_i$ from `fk` (the frames are at the CoMs).
</details>

### `> cat stretch.txt`

- `coriolis_matrix` via Christoffel symbols, and a skew-symmetry test (chapter 06, exercise 3).
- Tree versions of all three algorithms.
- A benchmark: `aba` vs `solve(mass_matrix)` for n = 5…80. Where's the crossover in Python?
- Cross-check against Pinocchio on 100 random chains.
