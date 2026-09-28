# Chapter 03 · Kinematic chains: recursive kinematics

> Lab: `blueprint/kinematics.py` · Tests: `02-chains/tests/test_kinematics.py` · Prerequisites: chapter 02, Mark VI (PoE)

**The one idea:** a robot is a chain of rigid bodies. Every quantity we need (pose, velocity, acceleration, and in chapter 05 force) can be computed by **walking the chain once**, passing a 6-vector from each link to the next. That's the whole trick behind O(n) dynamics.

---

## 1. Two ways to write forward kinematics

Mark VI used the **space product of exponentials** with every screw axis $\mathcal S_i$ expressed in $\{0\}$:

```math
T_{0n}(q) = e^{[\mathcal S_1]q_1}\,e^{[\mathcal S_2]q_2}\cdots e^{[\mathcal S_n]q_n}\,M_{0n}.
```

Dynamics algorithms prefer a **link-local** form, with one frame per link and each joint axis in its own link's frame (our `Chain`):

```math
T_{i-1,i}(q_i) = M_{i-1,i}\,e^{[\mathcal A_i]q_i}, \qquad T_{0n} = T_{01}T_{12}\cdots T_{n-1,n}.
```

**They're equivalent.** Using $M\,e^{[\mathcal A]q}\,M^{-1} = e^{[\mathrm{Ad}_M\mathcal A]q}$ (chapter 02, §5) to push each $M$ to the right:

```math
\mathcal S_i = [\mathrm{Ad}_{M_{0i}}]\,\mathcal A_i, \qquad M_{0i} = M_{01}M_{12}\cdots M_{i-1,i}.
```

Local form wins for dynamics because each link's inertia is constant in its own frame. In the space frame, $\mathcal G_i$ would change with every configuration.

## 2. Velocity propagation

The body twist of link $i$ is $[\mathcal V_i] = T_{0i}^{-1}\dot T_{0i}$. Write $T_{0i} = T_{0,i-1}\,M_{i-1,i}\,e^{[\mathcal A_i]q_i}$ and differentiate by the product rule (the constant $M$ contributes nothing):

```math
\dot T_{0i} = \dot T_{0,i-1}\,M_{i-1,i}\,e^{[\mathcal A_i]q_i} \;+\; T_{0,i-1}\,M_{i-1,i}\,e^{[\mathcal A_i]q_i}\,[\mathcal A_i]\dot q_i .
```

Left-multiply by $T_{0i}^{-1} = e^{-[\mathcal A_i]q_i}\,M_{i-1,i}^{-1}\,T_{0,i-1}^{-1}$:

```math
[\mathcal V_i] = \underbrace{\big(e^{-[\mathcal A_i]q_i}M_{i,i-1}\big)}_{T_{i,i-1}}\,[\mathcal V_{i-1}]\,\underbrace{\big(M_{i-1,i}\,e^{[\mathcal A_i]q_i}\big)}_{T_{i-1,i}} \;+\; [\mathcal A_i]\dot q_i .
```

The first term is a conjugation, which is an adjoint:

```math
\boxed{\;\mathcal V_i = [\mathrm{Ad}_{T_{i,i-1}}]\,\mathcal V_{i-1} + \mathcal A_i\,\dot q_i, \qquad \mathcal V_0 = 0\;}
```

In words: *my twist is my parent's twist, seen from my frame, plus what my own joint adds.*

## 3. Acceleration propagation

Differentiate the boxed equation. $\mathcal A_i$ is constant, but $[\mathrm{Ad}_{T_{i,i-1}}]$ is not, because $T_{i,i-1}$ depends on $q_i$:

```math
\dot T_{i,i-1} = \tfrac{d}{dt}\big(e^{-[\mathcal A_i]q_i}\big)M_{i,i-1} = -[\mathcal A_i]\dot q_i\,T_{i,i-1}
\quad\Longrightarrow\quad
\tfrac{d}{dt}[\mathrm{Ad}_{T_{i,i-1}}] = -[\mathrm{ad}_{\mathcal A_i\dot q_i}]\,[\mathrm{Ad}_{T_{i,i-1}}].
```

So

```math
\dot{\mathcal V}_i = [\mathrm{Ad}_{T_{i,i-1}}]\dot{\mathcal V}_{i-1} - [\mathrm{ad}_{\mathcal A_i\dot q_i}]\,\underbrace{[\mathrm{Ad}_{T_{i,i-1}}]\mathcal V_{i-1}}_{\mathcal V_i - \mathcal A_i\dot q_i} + \mathcal A_i\ddot q_i .
```

Two facts clean this up: $[\mathrm{ad}_{x}]x = 0$ removes the $\mathcal A_i\dot q_i$ part, and antisymmetry turns $-[\mathrm{ad}_{\mathcal A_i\dot q_i}]\mathcal V_i$ into $[\mathrm{ad}_{\mathcal V_i}]\mathcal A_i\dot q_i$:

```math
\boxed{\;\dot{\mathcal V}_i = [\mathrm{Ad}_{T_{i,i-1}}]\,\dot{\mathcal V}_{i-1} + \underbrace{[\mathrm{ad}_{\mathcal V_i}]\,\mathcal A_i\dot q_i}_{c_i\ \text{(velocity-product term)}} + \mathcal A_i\,\ddot q_i\;}
```

The term $c_i$ is where centripetal and Coriolis accelerations come from. It's nonzero even when every $\ddot q = 0$: a spinning joint drags its child around a curve. The mutation "RNEA drops the velocity-product acceleration" breaks 6 of the dynamics tests. Forgetting $c_i$ is the most common first-implementation bug.

## 4. The body Jacobian

Unroll the velocity recursion from $\mathcal V_0 = 0$:

```math
\mathcal V_n = \sum_{i=1}^n [\mathrm{Ad}_{T_{n,i}}]\,\mathcal A_i\,\dot q_i
\quad\Longrightarrow\quad
J_b(q) = \Big[\,[\mathrm{Ad}_{T_{n,1}}]\mathcal A_1 \;\Big|\; \cdots \;\Big|\; [\mathrm{Ad}_{T_{n,n-1}}]\mathcal A_{n-1} \;\Big|\; \mathcal A_n\,\Big].
```

Column $i$ is joint $i$'s screw axis **as seen from the tip frame**. Build it with a single inward sweep that accumulates $T_{n,i}$. The space Jacobian follows as $J_s = [\mathrm{Ad}_{T_{0n}}]J_b$.

**Checking it without trusting yourself:** `test_link_twists_match_finite_differences` compares $[\mathcal V_i]$ against $T_{0i}^{-1}\,\frac{T_{0i}(q+\epsilon\dot q) - T_{0i}(q-\epsilon\dot q)}{2\epsilon}$. That is the *definition* of the body twist, computed from forward kinematics alone. Whenever you derive a velocity formula, check it against a finite difference of positions.

## 5. Why O(n) matters

| Approach | Cost per evaluation |
|---|---|
| multiply out $T_{0n}$ symbolically, then differentiate | exponential blow-up in expression size |
| build $J$ column by column, each from scratch | $O(n^2)$ |
| **recursive propagation (this chapter)** | $O(n)$: one 6×6 multiply per link |

For a 7-DOF arm the difference is small. For a 30-DOF humanoid running whole-body MPC (Mark XII) at 1 kHz, it's the difference between feasible and not.

## What the tests check

| Test | Property |
|---|---|
| `test_fk_pendulum`, `test_fk_double_pendulum_matches_geometry` | FK against closed-form trigonometry |
| `test_fk_home_is_product_of_M` | $q = 0$ reduces to the $M$ chain |
| `test_link_twists_match_finite_differences` | §2, against the definition itself (random 6-DOF, with a prismatic joint) |
| `test_body_jacobian_*` | §4 |

## Exercises

1. ★ Prove $\mathcal S_i = [\mathrm{Ad}_{M_{0i}}]\mathcal A_i$ by induction on $i$.
2. Write the **space**-frame version of the velocity recursion. Why is it less convenient for dynamics?
3. Derive $\dot J_b\,\dot q$ from §3. (Hint: it's the sum of the $c_i$ terms, carried to the tip.) Operational-space control in Mark XII needs it.
4. ★ Implement `space_jacobian` and verify $J_s = [\mathrm{Ad}_{T_{0n}}]J_b$ on a random chain.
5. Extend `fk` and `link_twists` to **trees**, with a `parent[i]` array instead of `i-1`. What changes? (Almost nothing, and that's the point.)

## Read deeper

- Lynch & Park, ch. 4 (FK) and ch. 5 (Jacobians); ch. 8.3.1 uses exactly this recursion.
- Featherstone, *Rigid Body Dynamics Algorithms*, ch. 4 (kinematic trees) and ch. 5 (inverse dynamics).
