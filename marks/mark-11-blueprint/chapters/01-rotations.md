# Chapter 01 · Rotations done right: SO(3)

> Lab: `blueprint/so3.py` · Tests: `01-lie/tests/test_so3.py` · Prerequisites: Mark V (quaternions, informally)

**The one idea:** rotations don't form a vector space; they form a **curved group**. Every correct algorithm on rotations works in the flat *tangent space* (the Lie algebra) and maps back with the exponential. Every incorrect one adds rotation matrices together and prays.

---

## 1. The group

A rotation is a matrix $R \in \mathbb R^{3\times 3}$ with

```math
SO(3) = \{\, R \;:\; R^\top R = \mathbb 1,\ \det R = +1 \,\}.
```

$SO(3)$ is a **group** under multiplication: it's closed ($R_1R_2 \in SO(3)$), associative, has an identity, and $R^{-1} = R^\top$. It is also a smooth 3-dimensional manifold. Nine numbers are constrained by six equations ($R^\top R = \mathbb 1$ is symmetric), which leaves three degrees of freedom. A space that is both a group and a smooth manifold is a **Lie group**.

**Why this matters:** $R_1 + R_2 \notin SO(3)$ in general, and neither is $\tfrac12(R_1 + R_2)$. Averaging, interpolating, differentiating or optimizing rotations "in coordinates" silently leaves the group. Everything in this chapter exists to avoid that.

## 2. Angular velocity lives in the tangent space

Let $R(t)$ be a rotation-valued curve. Differentiate the constraint $R^\top R = \mathbb 1$:

```math
\dot R^\top R + R^\top \dot R = 0
\quad\Longrightarrow\quad
(R^\top \dot R)^\top = -(R^\top \dot R).
```

So $R^\top\dot R$ is **skew-symmetric**. A 3×3 skew matrix has 3 free entries, which we collect into a vector:

```math
[\omega] = \begin{bmatrix} 0 & -\omega_3 & \omega_2 \\ \omega_3 & 0 & -\omega_1 \\ -\omega_2 & \omega_1 & 0\end{bmatrix},
\qquad [\omega]\,x = \omega \times x .
```

This gives two angular velocities:

```math
\dot R = R\,[\omega_b] \quad (\text{body}), \qquad \dot R = [\omega_s]\,R \quad (\text{space}), \qquad \omega_s = R\,\omega_b .
```

The set of skew matrices is the **Lie algebra** $\mathfrak{so}(3)$: the tangent space of $SO(3)$ at the identity. `hat` and `vee` are the isomorphism $\mathbb R^3 \leftrightarrow \mathfrak{so}(3)$.

Two identities you'll use constantly (prove both, they're one line each):

```math
[R\omega] = R\,[\omega]\,R^\top, \qquad\qquad [\omega]^3 = -\|\omega\|^2\,[\omega].
```

## 3. The exponential map: Rodrigues' formula

Rotating at a **constant** body rate $\omega$ gives $\dot R = R[\omega]$, a linear ODE whose solution is the matrix exponential:

```math
R(t) = R(0)\, e^{[\omega] t}, \qquad e^{[\omega]} = \sum_{k=0}^{\infty} \frac{[\omega]^k}{k!}.
```

Write $\omega = \theta\hat\omega$ with $\|\hat\omega\| = 1$. Then $[\hat\omega]^3 = -[\hat\omega]$, so every power collapses to $[\hat\omega]$ or $[\hat\omega]^2$:

```math
[\hat\omega]^{2k+1} = (-1)^k[\hat\omega], \qquad [\hat\omega]^{2k+2} = (-1)^k[\hat\omega]^2 \quad (k\ge 0).
```

Split the series into odd and even terms:

```math
e^{[\hat\omega]\theta}
= \mathbb 1 + \Big(\theta - \tfrac{\theta^3}{3!} + \tfrac{\theta^5}{5!} - \cdots\Big)[\hat\omega]
          + \Big(\tfrac{\theta^2}{2!} - \tfrac{\theta^4}{4!} + \cdots\Big)[\hat\omega]^2
```

```math
\boxed{\;e^{[\hat\omega]\theta} = \mathbb 1 + \sin\theta\,[\hat\omega] + (1 - \cos\theta)\,[\hat\omega]^2\;}
\qquad\text{(Rodrigues)}
```

In code you pass $\omega$, not $(\hat\omega, \theta)$. Substituting $[\hat\omega] = [\omega]/\theta$:

```math
\exp(\omega) = \mathbb 1 + \frac{\sin\theta}{\theta}[\omega] + \frac{1-\cos\theta}{\theta^2}[\omega]^2 .
```

**Numerics near θ = 0.** Both coefficients are $0/0$. Use their Taylor expansions: $\sin\theta/\theta = 1 - \theta^2/6 + \dots$ and $(1-\cos\theta)/\theta^2 = \tfrac12 - \theta^2/24 + \dots$. Below $\theta \approx 10^{-8}$, $\mathbb 1 + [\omega] + \tfrac12[\omega]^2$ is exact to machine precision. The test `test_exp_small_angle_is_stable` checks this all the way down to $\omega = 0$.

## 4. The logarithm

`log` inverts `exp` on $\theta \in [0, \pi]$. From Rodrigues:

```math
\operatorname{tr} R = 1 + 2\cos\theta \;\Rightarrow\; \theta = \arccos\!\Big(\frac{\operatorname{tr}R - 1}{2}\Big),
\qquad
R - R^\top = 2\sin\theta\,[\hat\omega] \;\Rightarrow\; \omega = \frac{\theta}{2\sin\theta}\,(R - R^\top)^\vee .
```

(Clip the arccos argument to $[-1, 1]$: rounding error can push it to $1.0000000000000002$, and `arccos` then returns NaN.)

This fails in two places, where $\sin\theta \to 0$:

| Regime | Problem | Fix |
|---|---|---|
| $\theta \to 0$ | $\theta/(2\sin\theta) \to \tfrac12$, but it's computed as 0/0 | $\omega \approx \tfrac12 (R - R^\top)^\vee$ (first order) |
| $\theta \to \pi$ | $R - R^\top \to 0$: the axis information vanishes from the skew part | take it from the **symmetric** part instead (below) |

**Near π.** Rodrigues at $\theta = \pi$ gives $R = \mathbb 1 + 2[\hat\omega]^2$. Using $[\hat\omega]^2 = \hat\omega\hat\omega^\top - \mathbb 1$ (for unit $\hat\omega$):

```math
\frac{R + \mathbb 1}{2} = \hat\omega\,\hat\omega^\top .
```

Every column of $\hat\omega\hat\omega^\top$ is a multiple of $\hat\omega$. Pick the column with the **largest diagonal entry**, which is the best-conditioned one, and normalize it. The sign is ambiguous, but that's inherent: $\pm\pi\hat\omega$ are the same rotation. That's why the test checks $\exp(\log R) = R$, not $\log R = \omega$, at π.

## 5. Quaternions

A unit quaternion $q = (w, x, y, z) = (\cos\tfrac\theta2,\ \sin\tfrac\theta2\,\hat\omega)$ represents the rotation by $\theta$ about $\hat\omega$.

| Property | Consequence |
|---|---|
| $q$ and $-q$ give the same $R$ (**double cover**) | fix a sign convention; this repo uses $w \ge 0$. Error quaternions must take the short way round (Mark V) |
| composition is the Hamilton product, 16 multiplies | cheaper than a 3×3 matrix product (27) |
| renormalizing is trivial ($q/\lVert q\rVert$) | integrators drift off the unit sphere; you pull them back in one line |
| no trig in composition | fast on MCUs (Mark IV's Madgwick filter is quaternion-native) |

**Matrix → quaternion (Shepperd's method).** The textbook formula $w = \tfrac12\sqrt{1 + \operatorname{tr}R}$ divides by $4w$ for the other components. When $\operatorname{tr}R \to -1$ ($\theta \to \pi$), $w \to 0$ and precision collapses. Shepperd's fix is to compute whichever of $w, x, y, z$ is **largest** first, from the matching diagonal combination, then divide by it. This is `test_quaternion_handles_negative_trace`.

## 6. Perturbations and the left Jacobian

Optimizers and filters need "small changes to a rotation". The right way to perturb is **in the tangent space**, then map back:

```math
R' = \exp(\delta)\,R \qquad (\text{left, or global, perturbation}), \qquad \delta \in \mathbb R^3 \text{ small}.
```

**The key question:** if we perturb the *exponential coordinates*, $\omega \to \omega + \delta$, how does the rotation change? Not by $\exp(\delta)$, because rotations don't commute. There is a matrix $J_l(\omega)$ such that

```math
\exp(\omega + \delta) = \exp\!\big(J_l(\omega)\,\delta\big)\,\exp(\omega) + O(\|\delta\|^2).
```

**Derivation.** Differentiate $R(t) = \exp(\omega(t))$ and ask which space angular velocity results. Expanding the series and collecting terms (Barfoot, ch. 7, does it in full) gives

```math
J_l(\omega) = \sum_{k=0}^\infty \frac{[\omega]^k}{(k+1)!}
= \mathbb 1 + \frac{1-\cos\theta}{\theta^2}[\omega] + \frac{\theta - \sin\theta}{\theta^3}[\omega]^2 .
```

The closed form uses the same odd/even collapse as Rodrigues. Its inverse also has a closed form:

```math
J_l^{-1}(\omega) = \mathbb 1 - \tfrac12[\omega] + \Big(\frac{1}{\theta^2} - \frac{1+\cos\theta}{2\theta\sin\theta}\Big)[\omega]^2 ,
```

and the right Jacobian is $J_r(\omega) = J_l(-\omega)$. The test checks the defining property numerically with $\|\delta\| \approx 10^{-6}$, where the residual must be $O(10^{-12})$.

**Where you'll use it:**
- an EKF on rotations (Mark VII, done properly)
- Gauss–Newton on $SO(3)$ in SLAM and calibration
- chapter 02's SE(3) exponential, where $J_l$ reappears as the $V$ matrix: you'll implement it once and use it twice

## 7. Integrating angular velocity

Given body rate $\omega_b(t)$ from a gyro or a simulator:

| Scheme | Update | Stays on SO(3)? |
|---|---|---|
| naive Euler | $R_{k+1} = R_k + R_k[\omega_b]\Delta t$ | **no**: the determinant drifts, and columns shear |
| Euler + re-orthonormalize | …then SVD or Gram–Schmidt | only approximately; adds bias |
| **Lie–Euler** | $R_{k+1} = R_k\,\exp(\omega_b\Delta t)$ | **yes, exactly**, to machine precision |

The Lie–Euler update is still 1st order in time, but it never leaves the group. Higher-order Lie-group integrators (RKMK, Crouch–Grossman) exist and follow the same principle. Mark V's quadrotor simulator should use this.

---

## What the tests check

| Test | Property |
|---|---|
| `test_hat_is_cross_product` | $[\omega]v = \omega\times v$, skewness, vee∘hat = id |
| `test_exp_is_a_rotation` / `…_matches_matrix_exponential` | Rodrigues is exactly the matrix exponential |
| `test_exp_small_angle_is_stable` | the Taylor branch |
| `test_log_inverts_exp`, `test_log_near_zero_and_near_pi` | all three regimes of §4 |
| `test_left_jacobian_*` | the defining property of §6 and the closed-form inverse |
| `test_quaternion_*` | the $w \ge 0$ convention and Shepperd's branches |

## Exercises

1. ★ Prove $[R\omega] = R[\omega]R^\top$. Then explain in one sentence why it means "angular velocity transforms like a vector".
2. Show that $\exp$ is **not** injective on $\mathbb R^3$: find $\omega_1 \ne \omega_2$ with $\exp(\omega_1) = \exp(\omega_2)$. Which ball in $\mathbb R^3$ makes it injective?
3. ★ Derive the $J_l$ series from $\frac{d}{dt}\exp(\omega(t))$ using $\frac{d}{dt}e^{A(t)} = \int_0^1 e^{sA}\dot A\,e^{(1-s)A}\,ds$.
4. Implement quaternion SLERP and show it equals $R_0\exp(t\log(R_0^\top R_1))$.
5. Integrate a constant $\omega_b = (0.3, -1.1, 2.0)$ rad/s for 60 s at $\Delta t = 0.01$ with naive Euler and with Lie–Euler. Plot $\|R^\top R - \mathbb 1\|_F$ over time.

## Read deeper

- Solà, Deray & Atchuthan, *A micro Lie theory for state estimation in robotics* (arXiv:1812.01537), §I–III. **Read this next.**
- Barfoot, *State Estimation for Robotics*, ch. 7 (matrix Lie groups), for the full derivations of $J_l$ and $J_r$.
- Lynch & Park, *Modern Robotics*, ch. 3.2.
