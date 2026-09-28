# Chapter 04 · One rigid body: the Newton–Euler equations

> Lab: used by `blueprint/dynamics.py` · Prerequisite: chapter 02

**The one idea:** a rigid body's dynamics fit in one 6-D equation, $\mathcal F = \mathcal G\dot{\mathcal V} - [\mathrm{ad}_{\mathcal V}]^\top\mathcal G\mathcal V$. The second term (gyroscopic effects, and the "fictitious" forces of a rotating frame) is exactly the correction for writing Newton's law in a frame that moves with the body.

---

## 1. Momentum of a rigid body

Put frame $\{b\}$ at the centre of mass (CoM). With body twist $\mathcal V_b = (\omega, v)$ and mass $m$:

| Quantity | Expression |
|---|---|
| linear momentum | $p = m\,v$ (in $\{b\}$ coordinates) |
| angular momentum about the CoM | $h = \mathcal I_c\,\omega$, with $\mathcal I_c = \int \rho(r)\,\big(\lVert r\rVert^2\mathbb 1 - rr^\top\big)\,dV$ |
| spatial momentum | $`\mathcal P = \begin{bmatrix} h \\ p\end{bmatrix} = \underbrace{\begin{bmatrix}\mathcal I_c & 0 \\ 0 & m\mathbb 1\end{bmatrix}}_{\mathcal G_b}\mathcal V_b`$ |
| kinetic energy | $K = \tfrac12\,\mathcal V_b^\top\mathcal G_b\,\mathcal V_b$ |

$\mathcal G_b$ is the **spatial inertia**. It's block-diagonal only because the frame is at the CoM (§4 shows what happens elsewhere).

## 2. Newton and Euler in a moving frame

Newton's and Euler's laws hold in an **inertial** frame: $\frac{d}{dt}(\text{momentum}) = $ applied force or moment. The body frame rotates, and a vector with constant inertial components has changing body components. For any vector $x$ with body coordinates $x_b$ and space coordinates $x_s = Rx_b$:

```math
\dot x_s = \tfrac{d}{dt}(Rx_b) = R\big(\dot x_b + \omega\times x_b\big).
```

Apply that to linear and angular momentum (the CoM frame's origin moves with the body, and $\dot{\mathcal I}_c = 0$ in body coordinates):

```math
f = m\big(\dot v + \omega\times v\big), \qquad\qquad m_c = \mathcal I_c\,\dot\omega + \omega\times\mathcal I_c\,\omega .
```

The second equation is **Euler's equation**, and $\omega\times\mathcal I_c\omega$ is the gyroscopic term. It's why a spinning top precesses, and why a quadrotor's yaw couples into its pitch and roll at high spin.

**Watch out:** $\dot v$ here is the rate of change of the *body-frame components* of the origin's velocity. It is **not** the inertial acceleration of the CoM, which is $R(\dot v + \omega\times v)$. Mixing these two up is how RNEA implementations gain "free" centripetal forces.

## 3. The spatial form

Stack the two laws. Compute $-[\mathrm{ad}_{\mathcal V}]^\top\mathcal G\mathcal V$ using $`[\mathrm{ad}_{\mathcal V}]^\top = \begin{bmatrix} -[\omega] & -[v] \\ 0 & -[\omega]\end{bmatrix}`$ (the transpose of a skew matrix is its negative):

```math
-[\mathrm{ad}_{\mathcal V}]^\top\begin{bmatrix}\mathcal I_c\omega \\ m v\end{bmatrix}
= \begin{bmatrix} \omega\times\mathcal I_c\omega + v\times mv \\ \omega\times mv\end{bmatrix}
= \begin{bmatrix} \omega\times\mathcal I_c\omega \\ m\,\omega\times v\end{bmatrix}
\qquad (v\times v = 0).
```

That's exactly the extra terms from §2. So:

```math
\boxed{\;\mathcal F_b = \mathcal G_b\,\dot{\mathcal V}_b - [\mathrm{ad}_{\mathcal V_b}]^\top\,\mathcal G_b\,\mathcal V_b\;}
```

In words: *wrench = inertia × acceleration + the correction for describing momentum in a moving frame.* Since $-[\mathrm{ad}_{\mathcal V}]^\top$ is the wrench-bracket, this is the coordinate-free statement $\mathcal F = \dot{\mathcal P} + \mathcal V\times^*\mathcal P$, which is Featherstone's form.

The formula in the box holds **in any body-fixed frame**, not only the CoM frame, as long as $\mathcal G_b$ is the spatial inertia expressed in that frame (Lynch & Park, §8.2). The CoM choice just makes $\mathcal G_b$ block-diagonal.

## 4. Spatial inertia in another frame

Kinetic energy doesn't depend on the frame used to compute it. With $\mathcal V_b = [\mathrm{Ad}_{T_{ba}}]\mathcal V_a$:

```math
\tfrac12\mathcal V_a^\top\mathcal G_a\mathcal V_a = \tfrac12\mathcal V_b^\top\mathcal G_b\mathcal V_b
\quad\Longrightarrow\quad
\boxed{\;\mathcal G_a = [\mathrm{Ad}_{T_{ba}}]^\top\,\mathcal G_b\,[\mathrm{Ad}_{T_{ba}}]\;}
```

Expanding the blocks with $T_{ba} = (\mathbb 1, r)$, meaning frame $\{a\}$ sits at position $r$ in the CoM frame with no rotation, gives

```math
\mathcal G_a = \begin{bmatrix}\mathcal I_c + m[r]^\top[r] & m[r]^\top \\ m[r] & m\mathbb 1\end{bmatrix} .
```

The top-left block is the **parallel-axis theorem**, which you get for free. The off-diagonal blocks couple rotation and translation whenever the frame isn't at the CoM. This same congruence transform, $X^\top\mathcal G X$, is how CRBA and ABA push inertia from child to parent (chapters 06–07).

## 5. Physical consistency

Not every symmetric 6×6 matrix is the inertia of a real body. $\mathcal G$ is physically realizable if and only if:

1. $m > 0$;
2. $\mathcal I_c \succ 0$ (or $\succeq 0$ for idealized point masses);
3. the principal moments satisfy the **triangle inequality** $I_1 \le I_2 + I_3$ (and its permutations), because $I_1 = \int(y^2+z^2)$ and so on, with density $\ge 0$.

`chain.random_chain` builds $\mathcal I_c = \operatorname{diag}(y+z, x+z, x+y)$ with $x, y, z > 0$, which satisfies all three by construction.

**The ML connection:** when you *learn* inertial parameters (system identification in Mark V, or learned dynamics in Mark XIII), unconstrained regression happily returns physically impossible inertias. Those make simulators unstable or non-passive. Parameterizing through a pseudo-inertia matrix constrained to be PSD (Wensing, Kim & Slotine) enforces physical consistency with a convex constraint.

## 6. Energy

The power delivered by the applied wrench is

```math
\mathcal V^\top\mathcal F = \mathcal V^\top\mathcal G\dot{\mathcal V} - \underbrace{\mathcal V^\top[\mathrm{ad}_{\mathcal V}]^\top\mathcal G\mathcal V}_{=\;([\mathrm{ad}_{\mathcal V}]\mathcal V)^\top\mathcal G\mathcal V\;=\;0}
= \frac{d}{dt}\Big(\tfrac12\mathcal V^\top\mathcal G\mathcal V\Big).
```

The gyroscopic term **does no work**, because $[\mathrm{ad}_{\mathcal V}]\mathcal V = 0$. This one-line fact becomes the passivity property of whole robots in chapter 06, and it's why `test_energy_is_conserved_in_simulation` can demand $10^{-6}$ relative drift.

## Exercises

1. ★ Derive Euler's equation $m_c = \mathcal I_c\dot\omega + \omega\times\mathcal I_c\omega$ from $\frac{d}{dt}(R\,\mathcal I_c\,\omega)$ in the space frame.
2. Verify the block formula for $\mathcal G_a$ in §4 by expanding $[\mathrm{Ad}]^\top\mathcal G[\mathrm{Ad}]$.
3. A body spins freely about its intermediate principal axis. Simulate Euler's equation and observe the **Dzhanibekov (tennis-racket) effect**. Which property of $\mathcal I_c$ causes it?
4. Show that the triangle inequality is necessary: construct a density function for which $I_1 > I_2 + I_3$ is required, and show it would need negative mass.

## Read deeper

- Lynch & Park, ch. 8.2 (single rigid body dynamics).
- Featherstone, RBDA, ch. 2 (spatial vector algebra), for the spatial cross products $\times$ and $\times^*$.
- Wensing, Kim & Slotine, "Linear Matrix Inequalities for Physically Consistent Inertial Parameter Identification" (IEEE Robotics and Automation Letters).
