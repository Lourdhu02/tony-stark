# Chapter 07 · Forward dynamics: the Articulated Body Algorithm

> Lab: `dynamics.aba` · Tests: `test_aba_*`, `test_energy_is_conserved_in_simulation` · Prerequisites: chapters 05–06

**The one idea:** a simulator must answer *"given the torques, how does the robot accelerate?"* The obvious answer, solving $M\ddot q = \tau - h$, costs $O(n^3)$. Featherstone's insight is that each subtree, **with its joints free to move**, responds to a push like a single body with an *articulated* inertia. You can compute that inertia inward in $O(n)$ and then solve for every acceleration outward in $O(n)$.

---

## 1. The naive route and its cost

```math
\ddot q = M(q)^{-1}\big(\tau - h(q,\dot q)\big),\qquad h = \mathrm{RNEA}(q,\dot q,0).
```

That's CRBA at $O(n^2)$, a Cholesky factorization at $O(n^3)$, and an RNEA at $O(n)$. For $n = 7$ it's trivial. For a 30-DOF humanoid inside a 4000-environment RL simulation (Mark XIII) it isn't. `test_aba_matches_mass_matrix_solve` uses this route as the **oracle** for ABA.

## 2. The articulated-body inertia

Consider the subtree hanging from link $i$ (link $i$ plus everything outboard), with **known joint torques** on all of its joints. Push on link $i$ with some wrench. How does link $i$ accelerate?

- If the subtree's joints were **locked**, it would respond with the composite inertia $\mathcal I^c_i$ (chapter 06).
- With the joints **free**, the children swing away under the push, and link $i$ *feels lighter* in some directions.

**Claim.** For any subtree, the relationship between the acceleration of its root and the wrench applied to that root is **affine**:

```math
\boxed{\;\mathcal F_i = \mathcal I^A_i\,\dot{\mathcal V}_i + p^A_i\;}
```

Here $\mathcal I^A_i$ (6×6, symmetric, PSD) is the **articulated-body inertia**, and $p^A_i$ is the **articulated bias wrench**, which collects the velocity-dependent forces and the known joint torques of the subtree. We prove the claim by induction from the tips inward.

## 3. The inductive step (the heart of ABA)

**Base case.** A leaf link $n$ is a single rigid body. By Newton–Euler (chapter 04), $\mathcal F_n = \mathcal G_n\dot{\mathcal V}_n - [\mathrm{ad}_{\mathcal V_n}]^\top\mathcal G_n\mathcal V_n$. So $\mathcal I^A_n = \mathcal G_n$ and $p^A_n = -[\mathrm{ad}_{\mathcal V_n}]^\top\mathcal G_n\mathcal V_n$. ✓

**Inductive step.** Suppose child $i$ already satisfies $\mathcal F_i = \mathcal I^A_i\dot{\mathcal V}_i + p^A_i$. Its parent's acceleration $\dot{\mathcal V}_{i-1}$ is unknown. Write $X = [\mathrm{Ad}_{T_{i,i-1}}]$ and use the acceleration recursion from chapter 03:

```math
\dot{\mathcal V}_i = X\dot{\mathcal V}_{i-1} + c_i + \mathcal A_i\ddot q_i .
```

Joint $i$'s torque is known: $\tau_i = \mathcal A_i^\top\mathcal F_i$. Substitute:

```math
\tau_i = \mathcal A_i^\top\Big(\mathcal I^A_i\big(X\dot{\mathcal V}_{i-1} + c_i + \mathcal A_i\ddot q_i\big) + p^A_i\Big).
```

Define

```math
U_i = \mathcal I^A_i\mathcal A_i,\qquad D_i = \mathcal A_i^\top U_i\ (\text{scalar} > 0),\qquad u_i = \tau_i - \mathcal A_i^\top p^A_i ,
```

and solve for the joint acceleration (**this is the formula the third pass uses**):

```math
\boxed{\;\ddot q_i = \frac{u_i - U_i^\top\big(X\dot{\mathcal V}_{i-1} + c_i\big)}{D_i}\;}
```

Substitute back into $\mathcal F_i$ and collect the terms in $X\dot{\mathcal V}_{i-1}$:

```math
\mathcal F_i = \underbrace{\Big(\mathcal I^A_i - \frac{U_iU_i^\top}{D_i}\Big)}_{\mathcal I^a_i}\big(X\dot{\mathcal V}_{i-1}\big)
+ \underbrace{p^A_i + \mathcal I^a_i\,c_i + \frac{U_i\,u_i}{D_i}}_{p^a_i}.
```

That's still affine. The parent receives the child's wrench transformed into its own frame ($X^\top$), so the parent's articulated quantities accumulate:

```math
\boxed{\;\mathcal I^A_{i-1} \mathrel{+}= X^\top\mathcal I^a_iX, \qquad p^A_{i-1} \mathrel{+}= X^\top p^a_i\;}
```

Each child contributes the same way, and the parent's own body adds its $\mathcal G$ and gyroscopic bias. So the claim holds for the parent's subtree. ∎

## 4. The meaning of $\mathcal I^a_i$

Apply it to the joint axis:

```math
\mathcal I^a_i\,\mathcal A_i = \mathcal I^A_i\mathcal A_i - \frac{U_i\,(U_i^\top\mathcal A_i)}{D_i} = U_i - U_i\frac{D_i}{D_i} = 0 .
```

**The articulated inertia has zero stiffness along the joint's free direction.** Of course: you can't transmit inertial force through a joint along the axis it moves freely on. That force goes into accelerating the joint instead. $\mathcal I^a_i$ is $\mathcal I^A_i$ with that direction projected out, which is exactly the "feels lighter" intuition, made precise.

The $c_i$ term inside $p^a_i$ is subtle, and dropping it is a classic bug. The mutation test "ABA drops $\mathcal I^a c$" fails five tests, including energy conservation.

## 5. The algorithm

```text
ABA(q, q̇, τ):
  # pass 1: outward, kinematics
  for i = 1..n:
      X_i  = Ad(T_{i,i-1}(q_i))
      V_i  = X_i V_{i-1} + A_i q̇_i
      c_i  = ad(V_i) A_i q̇_i
      I^A_i = G_i
      p^A_i = −ad(V_i)ᵀ G_i V_i

  # pass 2: inward, articulated inertias
  for i = n..1:
      U_i = I^A_i A_i ;  D_i = A_iᵀ U_i ;  u_i = τ_i − A_iᵀ p^A_i
      if i > 1:
          I^a = I^A_i − U_i U_iᵀ / D_i
          p^a = p^A_i + I^a c_i + U_i u_i / D_i
          I^A_{i-1} += X_iᵀ I^a X_i
          p^A_{i-1} += X_iᵀ p^a

  # pass 3: outward, accelerations
  V̇_0 = (0, −g)
  for i = 1..n:
      a    = X_i V̇_{i-1} + c_i
      q̈_i = (u_i − U_iᵀ a) / D_i
      V̇_i  = a + A_i q̈_i
  return q̈
```

Three sweeps with constant work per link: **O(n)**. $M$ is never formed.

## 6. Numerics and edge cases

- **$D_i > 0$** as long as every subtree has mass along its joint's motion. Point masses (zero rotational inertia) are fine; the double pendulum in the tests uses them. A *massless* subtree makes $D_i = 0$, which is physically meaningless for dynamics.
- **Stiffness.** ABA computes the same $\ddot q$ as $M^{-1}(\tau - h)$ (the oracle test checks this to $10^{-9}$). The equations are equally stiff either way; the integrator (Mark I, manual 03) decides stability.
- **Crossover.** In optimized C++, ABA's O(n) advantage shows once chains get long. For 6–7-DOF arms both routes are sub-microsecond; for humanoids and long chains ABA generally wins. The exact crossover depends on the implementation and on branching, so measure rather than assume. In **Python**, per-link overhead dominates. Exercise 4 has you measure it.
- **What engines do.** Pinocchio provides both ABA and CRBA. MuJoCo builds $M$ with a composite-rigid-body pass and factorizes it sparsely, because its contact solver needs $M$ explicitly (chapter 08).

## 7. Energy as the final exam

`test_energy_is_conserved_in_simulation` integrates a random 6-DOF chain (with a prismatic joint) for 1 s with RK4 at $\Delta t = 10^{-3}$, using your ABA, and requires relative energy drift below $10^{-6}$. It's the strictest test in the Mark, because *any* inconsistency (a wrong sign in $c_i$, a missing $\mathrm{ad}^\top$, a transposed $X$) shows up as energy injected or removed. Physics is the oracle.

## Exercises

1. ★ Redo the §3 derivation for a **multi-DOF joint** (a spherical joint: $\mathcal A_i$ is 6×3, $D_i$ is 3×3). Which scalar divisions become matrix solves?
2. Show that $\mathcal I^A_i \preceq \mathcal I^c_i$ in the Loewner order. What does that say physically?
3. ★ Implement `aba` for **trees** with a `parent[]` array. Test it on a "humanoid-ish" tree with two 6-DOF arms on a 3-DOF torso.
4. Time `aba` against `solve(mass_matrix, …)` for $n = 5, 10, 20, 40, 80$ on random chains. Plot both, and explain the Python crossover you observe.
5. Add a joint friction model $\tau_f = -b\dot q$ to the simulation. Energy should now decrease monotonically. Verify it.

## Read deeper

- Featherstone, *Rigid Body Dynamics Algorithms*, ch. 7: the definitive treatment (and his 1983 paper introducing ABA).
- Lynch & Park, ch. 8.5 (forward dynamics via $M^{-1}$).
- Carpentier et al., "The Pinocchio C++ library" (2019), for implementation details and benchmarks of RNEA, CRBA and ABA.
