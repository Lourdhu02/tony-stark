# Chapter 08 · Constraints and contact

> Lab: stretch exercises (no graded tests) · Prerequisites: chapters 05–07 · Leads to: Mark XII (legged robots), Mark XIII (sim-to-real)

**The one idea:** a robot touching the world is a robot with **constraints**. Bilateral constraints (a closed loop, a welded foot) turn dynamics into a linear system with Lagrange multipliers. Unilateral constraints (a foot that can lift off, friction that can slip) turn it into a **complementarity problem**, and the way a simulator solves that problem is the biggest single source of the sim-to-real gap.

---

## 1. Bilateral constraints

A holonomic constraint $\phi(q) = 0$ (for example, the foot's position is fixed) with Jacobian $J_c = \partial\phi/\partial q$ implies, by differentiating twice:

```math
J_c\,\dot q = 0, \qquad J_c\,\ddot q + \dot J_c\,\dot q = 0 .
```

The environment enforces it with a **constraint force** $\lambda$, which enters the dynamics through $J_c^\top$ (the same duality as $J^\top\mathcal F$ in chapter 05, exercise 1):

```math
M\ddot q + h = \tau + J_c^\top\lambda .
```

Stack the two equations into the **KKT system**:

```math
\begin{bmatrix} M & -J_c^\top \\ J_c & 0\end{bmatrix}
\begin{bmatrix}\ddot q \\ \lambda\end{bmatrix}
= \begin{bmatrix}\tau - h \\ -\dot J_c\dot q\end{bmatrix}.
```

**Solving it.** Eliminate $\ddot q = M^{-1}(\tau - h + J_c^\top\lambda)$ and substitute into the constraint:

```math
\underbrace{J_cM^{-1}J_c^\top}_{\Lambda^{-1}}\,\lambda = -\dot J_c\dot q - J_cM^{-1}(\tau - h).
```

$\Lambda = (J_cM^{-1}J_c^\top)^{-1}$ is the **operational-space inertia**: the inertia the robot presents at the contact point. It reappears as the core object of task-space control in Mark XII.

## 2. Gauss's principle of least constraint

The KKT solution has an elegant variational form. Among all accelerations consistent with the constraint, nature chooses the one **closest to the unconstrained acceleration** $\ddot q_{\text{free}} = M^{-1}(\tau - h)$, in the metric $M$:

```math
\ddot q^\star = \arg\min_{\ddot q}\ \tfrac12(\ddot q - \ddot q_{\text{free}})^\top M(\ddot q - \ddot q_{\text{free}})
\quad\text{s.t.}\quad J_c\ddot q = -\dot J_c\dot q .
```

**Proof.** The Lagrangian of this QP is $\tfrac12\|\ddot q - \ddot q_{\text{free}}\|^2_M - \lambda^\top(J_c\ddot q + \dot J_c\dot q)$. Its stationarity condition is $M(\ddot q - \ddot q_{\text{free}}) = J_c^\top\lambda$, which is exactly the first KKT row. ∎

This is not just a curiosity: **MuJoCo's contact model is built on Gauss's principle**, generalized to soft, inequality-constrained contact (§5).

## 3. Drift and Baumgarte stabilization

Integrating $J_c\ddot q = -\dot J_c\dot q$ enforces the constraint only at the acceleration level. Numerical error accumulates, and $\phi(q)$ drifts away from zero. **Baumgarte stabilization** replaces the constraint with a stable second-order system:

```math
\ddot\phi + 2\zeta\omega\,\dot\phi + \omega^2\phi = 0
\quad\Longleftrightarrow\quad
J_c\ddot q = -\dot J_c\dot q - 2\zeta\omega\,J_c\dot q - \omega^2\phi(q).
```

Too small an $\omega$ and drift persists; too large and the system becomes stiff. MuJoCo's constraint parameters (`solref`: a time constant and damping ratio) are the modern descendant of this idea.

## 4. Unilateral contact and friction

Real contacts can **push but not pull**, and they can **stick or slip**.

**Normal direction.** With gap $\phi_n \ge 0$ and normal force $\lambda_n \ge 0$, at most one can be nonzero:

```math
0 \le \phi_n \;\perp\; \lambda_n \ge 0 \qquad (\text{complementarity}).
```

**Tangential direction (Coulomb friction).** The friction force lies in the **cone** $\|\lambda_t\| \le \mu\lambda_n$. If the contact slides, the friction force is on the cone's boundary, opposing the sliding velocity (the **maximum dissipation principle**).

Put together, one time step of contact dynamics is a **nonlinear complementarity problem (NCP)**. Linearizing the friction cone into a polyhedron gives a **linear complementarity problem (LCP)**, which is the classic Stewart–Trinkle / Anitescu–Potra time-stepping formulation.

**Two famous pathologies:**

| Pathology | What happens |
|---|---|
| **Painlevé paradox** | with rigid bodies and Coulomb friction, some configurations have no solution, or several. A pencil dragged across a table chatters. |
| **Indeterminacy** | a table on four legs: the four normal forces aren't unique (the system is statically indeterminate). Rigid models can't choose; compliance picks one. |

## 5. How real simulators choose

| Engine | Contact model (in brief) | Consequence |
|---|---|---|
| **MuJoCo** | soft, convex: Gauss's principle with a constraint-violation cost; solved exactly by Newton or CG | smooth, differentiable-ish, fast. Contacts are slightly penetrating and "spongy" by design. |
| **Bullet, PhysX** | iterative impulse solvers (projected Gauss–Seidel and variants) | real-time friendly; accuracy depends on iteration count; stacking and jitter artifacts |
| **Drake** | compliant hydroelastic contact, plus convex time-stepping formulations | physically grounded pressure fields; aims at accuracy for manipulation |
| **Isaac / massively parallel sims** | GPU-parallel iterative solvers | thousands of environments at once; the workhorse of RL for legged robots (Mark XIII) |

**Why this matters to you:**
- A legged controller tuned in one simulator can fail in another purely because of contact modelling.
- Domain randomization in Mark XIII randomizes friction, restitution and contact softness **because** nobody knows the true values, and every engine's approximation is different.
- Differentiable simulation, which is useful for trajectory optimization and learning, lives or dies on how the complementarity problem is smoothed.

## 6. Impacts

When a foot strikes the ground, velocities jump in zero time. Integrating the dynamics over an infinitesimal impact gives

```math
M\,(\dot q^+ - \dot q^-) = J_c^\top\Lambda_{\text{impulse}}, \qquad J_c\dot q^+ = -e\,J_c\dot q^- ,
```

with coefficient of restitution $e \in [0, 1]$ ($e = 0$ for a plastic foot strike). The same Schur-complement solve as §1 gives the impulse. Walking robots (Mark XII) are **hybrid systems**: continuous dynamics between impacts, and discrete resets at impact.

## Exercises (stretch, ungraded)

1. ★ **Pendulum as a constrained particle.** Model the pendulum as a free point mass in the plane (2 DOF) with the constraint $\|p\| = l$. Implement the KKT solve with Baumgarte stabilization and compare $\theta(t)$ with your ABA pendulum over 10 s.
2. Verify Gauss's principle numerically: solve the KKT system *and* the QP (with `scipy.optimize` or a closed form), and confirm the answers match.
3. Implement a 1-D bouncing ball with a complementarity-based time step (the normal force as an LCP of size 1). Compare against a penalty-spring model with a very stiff spring. Which is more robust at a large dt?
4. ★ Read Todorov's "Convex and analytically-invertible dynamics with contacts and constraints" (ICRA 2014). Summarize, in one page, how MuJoCo turns an NCP into a convex problem, and what physical assumption it gives up in exchange.

## Read deeper

- Featherstone, RBDA, ch. 8 (closed loops) and ch. 11 (contact and impact).
- Stewart, "Rigid-body dynamics with friction and impact", SIAM Review (2000): the complementarity viewpoint.
- Todorov, "Convex and analytically-invertible dynamics with contacts and constraints" (ICRA 2014): the MuJoCo contact model.
- Tedrake, *Underactuated Robotics*, the chapters on simulation with contact and on walking (hybrid systems).
