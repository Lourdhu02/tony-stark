# Chapter 00 · Notation and conventions

> Half the bugs in robot dynamics are convention mismatches. Read this page twice, and keep it open.

Rigid-body dynamics has no single standard notation. Lynch & Park, Featherstone, Murray–Li–Sastry, Pinocchio, Drake and MuJoCo each make different choices. These chapters follow **Lynch & Park** (*Modern Robotics*), because you met that notation in Mark VI and because it maps cleanly onto code. Where the major libraries differ, the table in §6 says how.

## 1. Frames and transforms

| Symbol | Meaning |
|---|---|
| $\{s\}$ or $\{0\}$ | the fixed space (world) frame |
| $\{b\}$, $\{i\}$ | a body frame; $\{i\}$ is attached to link $i$ |
| $T_{ab} \in SE(3)$ | pose of frame $\{b\}$ expressed in frame $\{a\}$. It maps coordinates in $\{b\}$ to coordinates in $\{a\}$: $p_a = T_{ab}\, p_b$ |
| $R_{ab},\ p_{ab}$ | rotation and translation blocks of $T_{ab}$ |
| $T_{ab}\,T_{bc} = T_{ac}$ | subscripts cancel like fractions; if they don't line up, there's a bug |

**Rule:** a variable name in code carries its frames. `T_sb`, `V_b`, `J_s`. Code that writes `T` or `V` without frames is where the bugs hide.

## 2. Twists and wrenches: angular first

```math
\mathcal V = \begin{bmatrix}\omega \\ v\end{bmatrix} \in \mathbb R^6
\qquad\qquad
\mathcal F = \begin{bmatrix} m \\ f \end{bmatrix} \in \mathbb R^6
\qquad\qquad
\text{power} = \mathcal F^\top \mathcal V = m\cdot\omega + f\cdot v
```

- A **twist** $\mathcal V_b$ expressed in $\{b\}$ has $\omega_b$, the angular velocity in $\{b\}$ coordinates, and $v_b$, the linear velocity of the body point **currently at the origin of $\{b\}$**, also in $\{b\}$ coordinates.
- A **wrench** $\mathcal F_b$ has $m_b$, the moment about the origin of $\{b\}$, and $f_b$, the force, both in $\{b\}$ coordinates.
- Twists and wrenches are **dual**. Their pairing is power, which doesn't depend on the frame used to compute it. That single fact generates every transformation rule for forces (chapter 02, §8).

## 3. Hats, adjoints and brackets

| Symbol | Definition | Code |
|---|---|---|
| $[\omega] \in \mathfrak{so}(3)$ | skew matrix with $[\omega]x = \omega\times x$ | `so3.hat` |
| $[\mathcal V] \in \mathfrak{se}(3)$ | $`\begin{bmatrix}[\omega] & v\\ 0 & 0\end{bmatrix}`$ | `se3.hat` |
| $[\mathrm{Ad}_T]$ | $`\begin{bmatrix}R & 0\\ [p]R & R\end{bmatrix}`$: changes the frame of a twist | `se3.adjoint` |
| $[\mathrm{ad}_{\mathcal V}]$ | $`\begin{bmatrix}[\omega] & 0\\ [v] & [\omega]\end{bmatrix}`$: the Lie bracket | `se3.ad` |
| $[\mathrm{Ad}_T]^\top$, $[\mathrm{ad}_{\mathcal V}]^\top$ | the same operations on **wrenches** (the duals) | `.T` |

## 4. Chains

For a serial chain with joints $i = 1, \dots, n$:

| Symbol | Meaning | Code (`chain.py`) |
|---|---|---|
| $M_{i-1,i}$ | pose of $\{i\}$ in $\{i-1\}$ at the home configuration $q = 0$ | `chain.M[i-1]` |
| $\mathcal A_i$ | screw axis of joint $i$, **expressed in $\{i\}$** | `chain.A[i-1]` |
| $\mathcal G_i$ | spatial inertia of link $i$ in $\{i\}$ | `chain.G[i-1]` |
| $T_{i-1,i}(q_i)$ | $M_{i-1,i}\, e^{[\mathcal A_i] q_i}$ | |
| $T_{i,i-1}(q_i)$ | $e^{-[\mathcal A_i] q_i}\, M_{i,i-1}$, the inverse | |
| $\mathcal V_i,\ \dot{\mathcal V}_i$ | body twist of link $i$ and its time derivative, in $\{i\}$ | |
| $\mathcal F_i$ | wrench transmitted **from link $i-1$ to link $i$** through joint $i$, in $\{i\}$ | |

**Our frames sit at each link's centre of mass.** That makes $\mathcal G_i = \operatorname{diag}(\mathcal I_c, m\,\mathbb 1)$ block-diagonal and keeps the Newton–Euler equations clean. Featherstone puts frames at the joints instead; both are valid, and the algorithms are the same apart from where $\mathcal G$ gets its off-diagonal blocks.

## 5. Gravity

$g = (0, 0, -9.81)\ \mathrm{m/s^2}$ in $\{0\}$. The dynamics algorithms model gravity by pretending the base accelerates **upward**: $\dot{{\mathcal V}}_0 = (0, -g)$. Chapter 05 §4 proves this is exact.

## 6. Rosetta stone

| | This book / Lynch & Park | Featherstone (RBDA) | Pinocchio | MuJoCo |
|---|---|---|---|---|
| twist order | $(\omega, v)$ | $(\omega, v)$ | $(v, \omega)$ | varies by API (`mj_objectVelocity` returns $(\omega, v)$) |
| name | twist $\mathcal V$ | spatial motion vector $\hat v$ | `Motion` | — |
| frame change | $[\mathrm{Ad}_T]$ | Plücker transform $^B X_A$ | `SE3.action` | — |
| bracket | $[\mathrm{ad}_{\mathcal V}]$ | $v\times$ (`crm`) | `motion.cross` | — |
| wrench bracket | $-[\mathrm{ad}_{\mathcal V}]^\top$ | $v\times^*$ (`crf`) | `motion.cross(force)` | — |
| inertia frame | link CoM (here) | usually the joint frame | the joint frame | the body's inertial frame (CoM, principal axes) |

When you cross-check against a library, and you should (chapter exercises do), **permute $(\omega, v) \leftrightarrow (v, \omega)$ first.**

## 7. How to read these chapters

Each chapter follows the same arc:

```text
intuition  →  definitions  →  derivation (every step)  →  algorithm  →  what the tests check  →  exercises
```

Derive before you read the derivation. Cover the page, attempt it on paper, then compare. The exercises marked ★ are the ones worth doing twice.
