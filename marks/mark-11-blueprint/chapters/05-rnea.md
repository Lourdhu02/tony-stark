# Chapter 05 · Inverse dynamics: the Recursive Newton–Euler Algorithm

> Lab: `dynamics.rnea` · Tests: `03-dynamics/tests/test_dynamics.py` · Prerequisites: chapters 03 and 04

**The one idea:** to find the torques that produce a motion, compute every link's acceleration on the way **out** (chapter 03), then walk back **in**. Each link must receive, from its parent, the wrench that accelerates it *plus* everything it carries. That's two sweeps and O(n) work.

---

## 1. The problem

**Inverse dynamics:** given $(q, \dot q, \ddot q)$, find $\tau$ such that the robot moves that way.

```math
\tau = \mathrm{ID}(q, \dot q, \ddot q) = M(q)\ddot q + C(q,\dot q)\dot q + g(q).
```

It's the workhorse of robot control: computed-torque control (Mark VI), feed-forward terms, and the inner loop of trajectory optimization (Mark XII). The Lagrangian form on the right is *what* it computes. RNEA is *how* to compute it without ever forming $M$ or $C$.

## 2. Outward pass: kinematics

For $i = 1 \dots n$ (chapter 03):

```math
\begin{aligned}
T_{i,i-1} &= e^{-[\mathcal A_i]q_i}M_{i,i-1} \\
\mathcal V_i &= [\mathrm{Ad}_{T_{i,i-1}}]\mathcal V_{i-1} + \mathcal A_i\dot q_i \\
\dot{\mathcal V}_i &= [\mathrm{Ad}_{T_{i,i-1}}]\dot{\mathcal V}_{i-1} + [\mathrm{ad}_{\mathcal V_i}]\mathcal A_i\dot q_i + \mathcal A_i\ddot q_i
\end{aligned}
\qquad\text{with } \mathcal V_0 = 0,\ \dot{\mathcal V}_0 = (0, -g).
```

## 3. Inward pass: forces

Draw the free-body diagram of link $i$. Two wrenches act on it:

```text
          joint i                       joint i+1
 link i-1 ───●═══════════ link i ═══════════●─── link i+1
             │  F_i  (from parent, in {i})   │  −F_{i+1} (reaction from the child)
```

- $\mathcal F_i$, applied **by link $i-1$ through joint $i$**, expressed in $\{i\}$;
- the reaction to what link $i$ applies to its child: $-\mathcal F_{i+1}$. It's expressed in $\{i+1\}$, so it must be moved into $\{i\}$ with the wrench transform from chapter 02: $[\mathrm{Ad}_{T_{i+1,i}}]^\top$.

Newton–Euler (chapter 04) for link $i$:

```math
\mathcal F_i - [\mathrm{Ad}_{T_{i+1,i}}]^\top\mathcal F_{i+1} = \mathcal G_i\dot{\mathcal V}_i - [\mathrm{ad}_{\mathcal V_i}]^\top\mathcal G_i\mathcal V_i
```

so, for $i = n \dots 1$, with $\mathcal F_{n+1} = 0$ (or the tip wrench if the robot pushes on something):

```math
\boxed{\;\mathcal F_i = [\mathrm{Ad}_{T_{i+1,i}}]^\top\mathcal F_{i+1} + \mathcal G_i\dot{\mathcal V}_i - [\mathrm{ad}_{\mathcal V_i}]^\top\mathcal G_i\mathcal V_i\;}
```

## 4. From wrench to joint torque

$\mathcal F_i$ is a full 6-D wrench, but a revolute or prismatic joint can only *actively* supply the component along its own axis. The other five components are **constraint wrenches**, provided by the bearing's structure. Constraint wrenches do no work along allowed motions (the joint can only move along $\mathcal A_i$). Projecting onto the motion subspace isolates the actuated part:

```math
\boxed{\;\tau_i = \mathcal A_i^\top\,\mathcal F_i\;}
```

It's the power pairing again: $\tau_i\dot q_i = \mathcal F_i^\top(\mathcal A_i\dot q_i)$, the power delivered through joint $i$.

## 5. Gravity is a base acceleration

We never add gravity forces to the links. We set $\dot{\mathcal V}_0 = (0, -g)$ instead. **Why is that exact?**

**Step 1.** Gravity acts on link $i$ as an external wrench at its CoM (no moment): $\mathcal W_i = (0,\ m_i R_{0i}^\top g)$. Because $\mathcal G_i = \operatorname{diag}(\mathcal I_c, m_i\mathbb 1)$, that's $\mathcal W_i = \mathcal G_i\,(0,\ R_{0i}^\top g)$.

**Step 2.** Newton–Euler with the extra external wrench:

```math
\mathcal F_i - [\mathrm{Ad}_{T_{i+1,i}}]^\top\mathcal F_{i+1} + \mathcal W_i = \mathcal G_i\dot{\mathcal V}_i - [\mathrm{ad}_{\mathcal V_i}]^\top\mathcal G_i\mathcal V_i
\;\;\Longrightarrow\;\;
\mathcal F_i = [\mathrm{Ad}]^\top\mathcal F_{i+1} + \mathcal G_i\big(\dot{\mathcal V}_i - (0,\ R_{0i}^\top g)\big) - [\mathrm{ad}_{\mathcal V_i}]^\top\mathcal G_i\mathcal V_i .
```

So gravity is equivalent to **replacing every $\dot{\mathcal V}_i$ by $\dot{\mathcal V}_i - (0,\ R_{0i}^\top g)$**.

**Step 3.** The acceleration recursion is linear in $\dot{\mathcal V}_0$, and the adjoint of a pure linear acceleration only rotates it: $[\mathrm{Ad}_T](0, a) = (0, Ra)$. So starting the recursion from $\dot{\mathcal V}_0 = (0, -g)$ adds exactly $(0, -R_{0i}^\top g)$ to every $\dot{\mathcal V}_i$, and the $c_i$ and $\ddot q$ terms are unaffected. That's precisely the substitution from step 2. ∎

This is **Einstein's equivalence principle**: a uniform gravitational field is indistinguishable from an upward-accelerating frame. It saves a gravity term per link, and it makes gravity compensation, $g(q) = \mathrm{RNEA}(q, 0, 0)$, fall out for free.

> Test your intuition: `test_rnea_pendulum_gravity_torque` expects $\tau = mgl\sin q$ at rest. Holding the pendulum out at angle $q$ takes exactly that torque.

## 6. Extracting M, C and g with RNEA

Because RNEA computes $M\ddot q + C\dot q + g$ exactly, you can peel the pieces apart:

| You want | Call |
|---|---|
| $g(q)$ | $\mathrm{RNEA}(q, 0, 0)$ |
| $C(q,\dot q)\dot q$ | $\mathrm{RNEA}(q, \dot q, 0)$ with gravity off |
| column $j$ of $M(q)$ | $\mathrm{RNEA}(q, 0, e_j)$ with gravity off |
| $h(q,\dot q) = C\dot q + g$ | $\mathrm{RNEA}(q, \dot q, 0)$ |

The tests use the last-column trick as an **independent oracle** for your CRBA (`test_crba_equals_rnea_columns`).

## 7. Cost

Per link, the algorithm does a handful of 6×6 matrix–vector products, so the total is **O(n)**. Optimized implementations (Pinocchio, RBDL) do a 7-DOF arm's inverse dynamics in about a microsecond. Symbolic Lagrangian derivations, by contrast, grow roughly as O(n⁴) in expression size. That's why nobody derives humanoid dynamics by hand.

## 8. A worked example: one pendulum, by hand

Chain: $M_{01} = \mathrm{trans}(0, 0, -l)$, $\mathcal A_1 = (0,1,0,\ -l,0,0)$, $\mathcal G_1 = \operatorname{diag}(0,0,0,m,m,m)$. At rest ($\dot q = \ddot q = 0$) with $\dot{\mathcal V}_0 = (0,0,0,\ 0,0,9.81)$:

1. $\mathcal V_1 = 0$, and $\dot{\mathcal V}_1 = [\mathrm{Ad}_{T_{10}}]\dot{\mathcal V}_0$. For a pure acceleration $(0, a)$, the adjoint just rotates $a$: $\dot{\mathcal V}_1 = (0,\ R_{10}(0,0,9.81))$.
2. $\mathcal F_1 = \mathcal G_1\dot{\mathcal V}_1 = (0,\ m\,R_{10}(0,0,9.81))$: a pure upward force of $mg$ (in space terms), at the CoM.
3. $\tau_1 = \mathcal A_1^\top\mathcal F_1 = -l\cdot f_x$. With $R_{10} = R_y(q)^\top$, $f_x = m\cdot 9.81\cdot(-\sin q)$, so $\tau_1 = mgl\sin q$. ✓

Do this once on paper. After that, the code is a transcription.

## What the tests check

| Test | Property |
|---|---|
| `test_rnea_pendulum_gravity_torque` | §5 and §8 |
| `test_rnea_matches_double_pendulum_lagrangian` | the full $M\ddot q + h + g$, against a textbook closed form |
| `test_rnea_without_gravity_at_rest_is_zero` | the `gravity=False` switch |
| (every ABA/CRBA test) | RNEA is their oracle, so it has to be right first |

## Exercises

1. ★ Add a tip wrench $\mathcal F_{\text{tip}}$ (in the last frame) and show that its effect on $\tau$ is $J_b^\top\mathcal F_{\text{tip}}$. This is the **statics** relation behind force control.
2. Count the multiplications in your implementation per link. How does it compare with 2n calls to `se3.adjoint`?
3. Implement RNEA for a **floating base** (a humanoid): the base has 6 unactuated DOF, and $\dot{\mathcal V}_0$ is unknown. What equation determines it? (Preview of Mark XII.)
4. ★ Cross-check against Pinocchio (`pip install pin`) on a random chain. Remember to permute $(\omega, v) \leftrightarrow (v, \omega)$ and to move inertias to the joint frames.

## Read deeper

- Lynch & Park, ch. 8.3.
- Featherstone, RBDA, ch. 5.
- Luh, Walker & Paul, "On-line computational scheme for mechanical manipulators" (1980): the original O(n) RNEA paper.
