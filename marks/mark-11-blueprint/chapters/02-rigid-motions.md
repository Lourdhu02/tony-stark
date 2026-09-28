# Chapter 02 · Rigid motions: SE(3), twists and wrenches

> Lab: `blueprint/se3.py` · Tests: `01-lie/tests/test_se3.py` · Prerequisite: chapter 01

**The one idea:** a rigid motion is a rotation *and* a translation, coupled. The coupling is exactly what makes screws, adjoints and the gyroscopic terms of dynamics appear. Treat position and orientation separately and you'll get the dynamics subtly wrong.

---

## 1. The group SE(3)

```math
T = \begin{bmatrix} R & p \\ 0 & 1 \end{bmatrix}, \qquad
T_1 T_2 = \begin{bmatrix} R_1R_2 & R_1p_2 + p_1 \\ 0 & 1\end{bmatrix}, \qquad
T^{-1} = \begin{bmatrix} R^\top & -R^\top p \\ 0 & 1\end{bmatrix}.
```

$SE(3)$ is a 6-dimensional Lie group. It acts on points in homogeneous coordinates: $\tilde p_a = T_{ab}\,\tilde p_b$. Always use the closed-form inverse. `np.linalg.inv` is slower, and it doesn't return an exactly orthogonal $R$.

## 2. Twists: velocity of a rigid body

Differentiate a pose $T(t) = T_{sb}(t)$. Just as $R^\top\dot R \in \mathfrak{so}(3)$, both products below are in $\mathfrak{se}(3)$:

```math
T^{-1}\dot T = \begin{bmatrix} R^\top\dot R & R^\top\dot p \\ 0 & 0\end{bmatrix} = [\mathcal V_b],
\qquad
\dot T\,T^{-1} = \begin{bmatrix} \dot R R^\top & \dot p - \dot RR^\top p \\ 0 & 0\end{bmatrix} = [\mathcal V_s].
```

| Twist | $\omega$ | $v$ | Physical meaning of $v$ |
|---|---|---|---|
| body $\mathcal V_b$ | $\omega_b$ | $R^\top \dot p$ | velocity of the body's origin, in body coordinates |
| space $\mathcal V_s$ | $\omega_s$ | $\dot p - \omega_s\times p$ | velocity of the (imaginary) body point that is **currently at the space origin** |

The space twist's $v$ is the one that confuses everyone. Accept it: it's what makes the frame-change rule linear (§5).

## 3. The exponential: screw motions

A constant twist $\xi = (\omega, v)$, integrated for unit time, gives $e^{[\xi]}$. As in chapter 01, powers of $[\xi]$ collapse. With $\theta = \|\omega\| \ne 0$:

```math
[\xi]^k = \begin{bmatrix}[\omega]^k & [\omega]^{k-1}v \\ 0 & 0\end{bmatrix} \quad (k\ge1)
\;\;\Longrightarrow\;\;
e^{[\xi]} = \begin{bmatrix} e^{[\omega]} & \Big(\displaystyle\sum_{k\ge0}\tfrac{[\omega]^k}{(k+1)!}\Big) v \\ 0 & 1 \end{bmatrix}.
```

That series is **exactly the left Jacobian** from chapter 01:

```math
\boxed{\; e^{[\xi]} = \begin{bmatrix} \exp(\omega) & J_l(\omega)\,v \\ 0 & 1 \end{bmatrix} \;}
\qquad
J_l(\omega) = \mathbb 1 + \frac{1-\cos\theta}{\theta^2}[\omega] + \frac{\theta-\sin\theta}{\theta^3}[\omega]^2 .
```

Lynch & Park write the translation part as $G(\theta)\,\hat v$ for a unit screw axis with $\omega = \hat\omega\theta$, $v = \hat v\theta$. Expanding $J_l(\hat\omega\theta)\,\hat v\,\theta$ gives their $G(\theta) = \mathbb 1\theta + (1-\cos\theta)[\hat\omega] + (\theta - \sin\theta)[\hat\omega]^2$. Same object.

With $\omega = 0$ (pure translation), $J_l = \mathbb 1$ and $e^{[\xi]} = (\mathbb 1, v)$. The formula needs no special case once $J_l$ has its small-angle branch.

**The logarithm** reverses it: $\omega = \log R$, then $v = J_l^{-1}(\omega)\,p$.

## 4. Screws: the geometry behind every joint

**Chasles–Mozzi theorem:** every rigid displacement is a rotation about some line combined with a translation along that same line, which is a **screw motion**. A screw axis is written:

```math
\mathcal S = \begin{bmatrix}\hat\omega \\ -\hat\omega\times q + h\,\hat\omega\end{bmatrix}
\qquad
\begin{aligned}
&\hat\omega:\ \text{unit axis direction} \\
&q:\ \text{any point on the axis} \\
&h:\ \text{pitch (m of translation per rad)}
\end{aligned}
```

| Joint | Screw axis |
|---|---|
| revolute | $h = 0$: $\ \mathcal S = (\hat\omega,\ -\hat\omega\times q)$ |
| prismatic | $\hat\omega = 0$, $\lVert v\rVert = 1$: $\ \mathcal S = (0,\ \hat v)$ |
| helical (a lead screw) | $h \ne 0$ |

`chain.revolute_axis(ω, q)` builds exactly the first row. The test `test_screw_motion_fixes_its_axis` checks the defining property: points on the axis don't move.

## 5. The adjoint: changing the frame of a twist

A twist expressed in $\{b\}$ must be re-expressed in $\{a\}$. Conjugating the matrix form:

```math
T_{ab}\,[\mathcal V_b]\,T_{ab}^{-1}
= \begin{bmatrix} R[\omega]R^\top & -R[\omega]R^\top p + Rv \\ 0 & 0\end{bmatrix}
= \begin{bmatrix} [R\omega] & p\times R\omega + Rv \\ 0 & 0 \end{bmatrix}.
```

(The last step uses $[R\omega] = R[\omega]R^\top$ from chapter 01 and $-[R\omega]p = p\times R\omega$.) So $\mathcal V_a = [\mathrm{Ad}_{T_{ab}}]\,\mathcal V_b$ with

```math
\boxed{\;[\mathrm{Ad}_T] = \begin{bmatrix} R & 0 \\ [p]R & R\end{bmatrix}\;}
\qquad
[\mathrm{Ad}_{T_1T_2}] = [\mathrm{Ad}_{T_1}][\mathrm{Ad}_{T_2}], \qquad [\mathrm{Ad}_T]^{-1} = [\mathrm{Ad}_{T^{-1}}].
```

Because conjugation passes through the exponential series term by term, the tests check the group-level statement:

```math
T\,e^{[\xi]}\,T^{-1} = e^{[\mathrm{Ad}_T\,\xi]} .
```

**Reading it physically:** $[p]R\,\omega$ is the extra linear velocity that a rotation about a *distant* axis induces at the new origin, $v = \omega\times r$. That block is the lever arm.

## 6. The small adjoint: the Lie bracket

Differentiate $[\mathrm{Ad}_{\exp(t\,\xi_1)}]\,\xi_2$ at $t = 0$ and you get the **Lie bracket** $[\xi_1, \xi_2] = [\xi_1][\xi_2] - [\xi_2][\xi_1]$, which in vector form is

```math
[\mathrm{ad}_{\xi_1}]\,\xi_2, \qquad
[\mathrm{ad}_{\xi}] = \begin{bmatrix} [\omega] & 0 \\ [v] & [\omega]\end{bmatrix}.
```

**Why you care:**
1. $\mathrm{ad}$ measures **how much two motions fail to commute.** It shows up whenever a twist is differentiated in a moving frame: the velocity-product accelerations of chapter 03, and the Coriolis and gyroscopic terms of chapter 04.
2. It's antisymmetric, $[\mathrm{ad}_a]b = -[\mathrm{ad}_b]a$, so $[\mathrm{ad}_\xi]\xi = 0$. That identity simplifies the chain derivations.

## 7. Wrenches: forces and moments together

A wrench $\mathcal F = (m, f)$ expressed in $\{b\}$ holds the moment about $\{b\}$'s origin and the force. **Power is frame-independent**, so for the same physical wrench and twist seen from $\{a\}$ and $\{b\}$:

```math
\mathcal F_a^\top \mathcal V_a = \mathcal F_b^\top \mathcal V_b
\;\;\text{with}\;\;
\mathcal V_a = [\mathrm{Ad}_{T_{ab}}]\mathcal V_b
\;\;\Longrightarrow\;\;
\boxed{\;\mathcal F_b = [\mathrm{Ad}_{T_{ab}}]^\top \mathcal F_a\;}
```

Wrenches transform with the **transpose** of the adjoint, in the *opposite direction* to twists. Every force-propagation step in RNEA (chapter 05) is this line. Written out, $m_b = R^\top(m_a + f_a\times p)$ is the familiar "moment of a force about a new point".

Likewise, $-[\mathrm{ad}_{\mathcal V}]^\top$ is the bracket acting on wrenches. It appears as the gyroscopic term $\omega\times\mathcal I\omega$ in chapter 04.

## 8. Implementation notes

- `exp` should call `so3.exp` and `so3.left_jacobian`, which you already have. **Don't re-derive them.**
- `adjoint` and `ad` are pure block assembly. Unit-test them against the matrix identities, not against another formula you wrote.
- Keep $\omega$ first. If you ever import Pinocchio, write a single `to_pinocchio(xi)` that swaps the halves, and never do it inline.

## What the tests check

| Test | Property |
|---|---|
| `test_exp_matches_matrix_exponential` | the closed form of §3 equals the series |
| `test_pure_translation` | the $\omega = 0$ branch |
| `test_log_inverts_exp`, `test_inverse` | round trips |
| `test_adjoint_conjugation`, `test_adjoint_is_a_homomorphism` | §5 |
| `test_small_adjoint_is_the_lie_bracket` | §6 |
| `test_screw_motion_fixes_its_axis` | §4 |

## Exercises

1. ★ Derive the body-to-space twist relation $\mathcal V_s = [\mathrm{Ad}_{T_{sb}}]\mathcal V_b$ directly from $[\mathcal V_s] = \dot TT^{-1}$ and $[\mathcal V_b] = T^{-1}\dot T$.
2. Given a twist $\mathcal V = (\omega, v)$ with $\omega\ne0$, recover its screw axis: pitch $h = \omega^\top v/\|\omega\|^2$, and the point on the axis closest to the origin, $q = \omega\times v/\|\omega\|^2$. Verify numerically.
3. ★ Show that $[\mathrm{ad}_\xi] = \frac{d}{dt}\big|_{t=0}[\mathrm{Ad}_{\exp(t\xi)}]$.
4. A wrench of pure force $f = (0, 0, -10)$ N acts at point $r = (0.5, 0, 0)$ in $\{s\}$. Write it as a wrench in $\{s\}$, then in a frame $\{b\}$ located at $r$. Which one has zero moment, and why?
5. Implement `se3.interpolate(T0, T1, t) = T0 · exp(t · log(T0⁻¹ T1))` and plot the path of the origin. Why is it a helix, not a straight line?

## Read deeper

- Lynch & Park, *Modern Robotics*, ch. 3.3 (the whole chapter is excellent).
- Murray, Li & Sastry, *A Mathematical Introduction to Robotic Manipulation* (free PDF), ch. 2: the classic treatment of twists and wrenches.
- Solà et al., *micro Lie theory*, §IV and appendix D (SE(3)).
