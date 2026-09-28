# Chapter 06 · The mass matrix, CRBA and passivity

> Lab: `dynamics.mass_matrix`, `dynamics.energy` · Prerequisites: chapters 03–05

**The one idea:** $M(q)$ is the kinetic-energy metric of the robot. Every joint velocity has an energy cost, and $M$ is the quadratic form that prices it. From that single fact follow its symmetry, its positive-definiteness, an $O(n^2)$ algorithm to compute it, and the passivity property that makes whole classes of controllers provably stable.

---

## 1. M from kinetic energy

Link $i$'s body twist is linear in $\dot q$: $\mathcal V_i = J_i(q)\dot q$, where $J_i$ is the body Jacobian of link $i$ (its columns $j > i$ are zero). Summing kinetic energies:

```math
K = \sum_i \tfrac12\,\mathcal V_i^\top\mathcal G_i\mathcal V_i = \tfrac12\,\dot q^\top\underbrace{\Big(\sum_i J_i^\top\mathcal G_i J_i\Big)}_{M(q)}\dot q .
```

**Properties, each one line from here:**

| Property | Why |
|---|---|
| $M = M^\top$ | each term $J^\top\mathcal G J$ is symmetric |
| $M \succ 0$ | $\dot q^\top M\dot q = 2K > 0$ for every $\dot q \ne 0$, if every joint moves some mass |
| configuration-dependent, velocity-independent | the $J_i$ depend only on $q$ |
| uniformly bounded: $\lambda_{\min}\mathbb 1 \preceq M(q) \preceq \lambda_{\max}\mathbb 1$ for all $q$ (revolute arms) | $q$ enters only through sines and cosines |

## 2. The Lagrangian, for comparison

With $L = K - P$ and $P(q)$ the potential energy, the Euler–Lagrange equations $\frac{d}{dt}\frac{\partial L}{\partial\dot q} - \frac{\partial L}{\partial q} = \tau$ give

```math
M(q)\ddot q + C(q,\dot q)\dot q + g(q) = \tau,
\qquad
C_{ij} = \sum_k \Gamma_{ijk}\,\dot q_k,
\qquad
\Gamma_{ijk} = \tfrac12\Big(\frac{\partial M_{ij}}{\partial q_k} + \frac{\partial M_{ik}}{\partial q_j} - \frac{\partial M_{jk}}{\partial q_i}\Big).
```

The $\Gamma_{ijk}$ are the **Christoffel symbols** of the metric $M$. The unforced robot moves along geodesics of the Riemannian metric $M(q)$, and the Coriolis and centrifugal terms are that metric's connection terms. This geometric view is the foundation of modern geometric control.

**$C$ is not unique.** Many matrices satisfy $C\dot q = $ (the true Coriolis and centrifugal vector). The Christoffel choice is the special one (§5).

## 3. CRBA: the Composite Rigid Body Algorithm

Set $\dot q = 0$ and turn off gravity. Then $\tau = M\ddot q$, and choosing $\ddot q = e_j$ gives column $j$. What does that motion look like? **Only joint $j$ accelerates.** Every link from $j$ outward moves as **one rigid body** (a *composite* body), and every link before $j$ stays still.

**Composite inertia.** The inertia of the subtree rooted at link $i$, expressed in $\{i\}$, accumulates inward with the congruence transform from chapter 04:

```math
\boxed{\;\mathcal I^c_n = \mathcal G_n, \qquad \mathcal I^c_i = \mathcal G_i + [\mathrm{Ad}_{T_{i+1,i}}]^\top\,\mathcal I^c_{i+1}\,[\mathrm{Ad}_{T_{i+1,i}}]\;}
```

**Columns.** With only joint $j$ accelerating, $\dot{\mathcal V}_j = \mathcal A_j$ and the composite body $j..n$ needs wrench $\mathcal F_j = \mathcal I^c_j\mathcal A_j$. The velocity is zero, so there are no bias terms. Links before $j$ don't accelerate, so they pass that wrench inward unchanged apart from the frame change:

```math
M_{jj} = \mathcal A_j^\top\mathcal I^c_j\mathcal A_j,
\qquad
\mathcal F \leftarrow [\mathrm{Ad}_{T_{k,k-1}}]^\top\mathcal F \ \ (k = j, j-1, \dots),
\qquad
M_{k-1,j} = M_{j,k-1} = \mathcal A_{k-1}^\top\mathcal F .
```

Each column takes $O(n)$ work and there are $n$ columns, so the total is **O(n²)**: $O(nd)$ for trees of depth $d$, which matters for humanoids, whose limbs are short branches. Computing the same thing with $n$ RNEA calls is also $O(n^2)$, but with a much larger constant, because it recomputes kinematics every time.

## 4. The energy identity (holds for *any* valid C)

The power into the system equals the rate of change of mechanical energy:

```math
\frac{d}{dt}\big(K + P\big) = \dot q^\top\tau .
```

Expand $\dot K = \dot q^\top M\ddot q + \tfrac12\dot q^\top\dot M\dot q$ and $\dot P = \dot q^\top g$, then substitute $M\ddot q = \tau - C\dot q - g$:

```math
\dot q^\top\tau - \dot q^\top C\dot q + \tfrac12\dot q^\top\dot M\dot q = \dot q^\top\tau
\quad\Longrightarrow\quad
\boxed{\;\dot q^\top\big(\dot M - 2C\big)\dot q = 0\;}
```

`test_passivity_identity` checks exactly this, with $\dot M$ from a finite difference of your CRBA along $\dot q$ and $C\dot q$ from your RNEA. **It cross-validates two algorithms against a law of physics.** If CRBA and RNEA disagree in any way, it fails.

## 5. Skew-symmetry (the Christoffel choice)

With the Christoffel $C$ of §2, the stronger statement holds: $\dot M - 2C$ is **skew-symmetric**, so $x^\top(\dot M - 2C)x = 0$ for **every** $x$, not just $\dot q$.

**Proof.** $\dot M_{ij} = \sum_k \frac{\partial M_{ij}}{\partial q_k}\dot q_k$. Compute

```math
(\dot M - 2C)_{ij} = \sum_k\Big(\frac{\partial M_{ij}}{\partial q_k} - \frac{\partial M_{ij}}{\partial q_k} - \frac{\partial M_{ik}}{\partial q_j} + \frac{\partial M_{jk}}{\partial q_i}\Big)\dot q_k
= \sum_k\Big(\frac{\partial M_{jk}}{\partial q_i} - \frac{\partial M_{ik}}{\partial q_j}\Big)\dot q_k ,
```

and swapping $i \leftrightarrow j$ flips the sign. ∎

## 6. Why controllers care: passivity

Skew-symmetry makes the robot a **passive** system: it can store and dissipate energy, but never create it. The classic payoff:

**PD + gravity compensation is globally stable.** Apply $\tau = g(q) - K_p e - K_d\dot q$, with $e = q - q_d$ and $K_p, K_d \succ 0$. Take the Lyapunov function

```math
V = \tfrac12\dot q^\top M\dot q + \tfrac12 e^\top K_p e .
```

Then

```math
\dot V = \dot q^\top M\ddot q + \tfrac12\dot q^\top\dot M\dot q + \dot q^\top K_p e
= \dot q^\top(\tau - C\dot q - g) + \tfrac12\dot q^\top\dot M\dot q + \dot q^\top K_p e
= -\dot q^\top K_d\dot q \;\le\; 0 ,
```

using the identity from §4. By LaSalle's invariance principle, $q \to q_d$ **from any initial state, with no knowledge of $M$ or $C$**. The same structure underlies Slotine–Li adaptive control and impedance control (Mark XII).

## 7. Conditioning: why simulators struggle

$M$'s eigenvalues span the robot's physical scales. A heavy base with a light wrist can have $\kappa(M) \sim 10^4$–$10^6$. That hurts in three places:
- explicit integrators need tiny time steps: the stiffest mode sets the step size
- inverting $M$ amplifies rounding error (Mark I, manual 02)
- learned policies see badly scaled joint dynamics

Physics engines add **armature** (rotor inertia reflected through the gearbox, added to $M$'s diagonal) partly for realism and partly for this reason.

## Exercises

1. ★ Prove $M = \sum_i J_i^\top\mathcal G_iJ_i$ equals what CRBA computes, by expanding the composite inertia recursion.
2. Compute $M(q)$ for the double pendulum by hand from §1. It should match the closed form in the tests.
3. ★ Implement `coriolis_matrix(chain, q, qd)` using the Christoffel formula with finite-difference $\partial M/\partial q$. Verify skew-symmetry of $\dot M - 2C$ on a random chain. Then show that a *different* valid $C$ (with the same $C\dot q$) can satisfy §4 yet fail §5.
4. Simulate PD + gravity compensation on a random 6-DOF chain using `sim.simulate` with a custom torque function. Plot $V(t)$: it must never increase.
5. Add armature $\operatorname{diag}(a_i)$ to $M$. How do you change RNEA and ABA to match? (One line each.)

## Read deeper

- Lynch & Park, ch. 8.1 (Lagrangian) and 8.4.
- Featherstone, RBDA, ch. 6 (CRBA).
- Slotine & Li, *Applied Nonlinear Control*, ch. 9 (passivity and adaptive robot control).
- Spong, Hutchinson & Vidyasagar, *Robot Modeling and Control*: the dynamics and passivity-based control chapters.
