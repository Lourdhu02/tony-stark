```text
 ███╗   ███╗██╗  ██╗    ██╗  ██╗██╗
 ████╗ ████║██║ ██╔╝    ╚██╗██╔╝██║    BLUEPRINT
 ██╔████╔██║█████╔╝      ╚███╔╝ ██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗      ██╔██╗ ██║    lie groups · spatial algebra · rnea · crba · aba
 ██║ ╚═╝ ██║██║  ██╗    ██╔╝ ██╗██║    weeks 91–100 · 3 systems · 9 chapters
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝  ╚═╝╚═╝
```

> Before you build the suit, you draw the blueprint. Before any simulator, controller or learned policy, there is the math of how rigid bodies move.

### `> cat mission.txt`

Derive, from first principles, the geometry and dynamics every robotics stack is built on. Implement all of it:
- Lie groups SO(3) and SE(3)
- recursive kinematics
- Newton–Euler dynamics
- the three canonical algorithms: RNEA, CRBA and ABA

Verify it against physics itself: closed-form Lagrangians, finite differences, the passivity identity and energy conservation.

**This is Phase 5 (advanced robotics), Year 2.** It's a textbook plus a lab. It deepens Marks V–VII, and it's the engine that Marks XII (optimal control and legged robots) and XIII (robot learning) run on.

### `> cat why.txt`

| You'll understand | Because… |
|---|---|
| Why rotations need Lie groups | EKFs, SLAM, calibration and every optimizer on poses break subtly without them |
| Twists, wrenches and adjoints | the language of every modern robotics paper and library (Pinocchio, Drake, MuJoCo) |
| RNEA, CRBA and ABA, line by line | they run inside every simulator and every MPC, millions of times per second; you'll be able to debug them, not just call them |
| Passivity ($\dot M - 2C$ skew) | the reason whole families of controllers are provably stable (Mark XII) |
| Contact and constraints | legged locomotion (XII) and the sim-to-real gap (XIII) live here |

### `> ./prereqs --check`

- [ ] Marks V (dynamics, quaternions), VI (PoE kinematics) and VII (probability, and the EKF), or equivalent
- [ ] Linear algebra: comfortable with 6×6 block matrices, congruence transforms and matrix exponentials
- [ ] `pip install -r lab/requirements.txt` (NumPy and pytest)

### `> cat concept_map`

```text
GEOMETRY                                   ch. 01–02
├── SO(3): exp (Rodrigues) · log (3 regimes) · quaternions · J_l, J_l⁻¹
└── SE(3): exp = (exp ω, J_l(ω) v) · adjoint Ad · bracket ad · wrench duality Adᵀ

KINEMATICS                                 ch. 03
└── V_i = Ad V_{i-1} + A_i q̇_i  ·  V̇_i = Ad V̇_{i-1} + ad_{V_i} A_i q̇_i + A_i q̈_i  ·  J_b

DYNAMICS                                   ch. 04–07
├── one body:   F = G V̇ − ad_Vᵀ G V                 (Newton–Euler, spatial form)
├── RNEA:       out (V, V̇) · in (F) · τ = Aᵀ F      O(n)    inverse dynamics
├── CRBA:       composite inertias → M(q)            O(n²)   mass matrix
├── passivity:  q̇ᵀ(Ṁ − 2C)q̇ = 0 → PD + gravity compensation is globally stable
└── ABA:        articulated inertias I^a A = 0       O(n)    forward dynamics

CONTACT                                    ch. 08
└── KKT · Gauss's principle · Baumgarte · complementarity · friction cones · how engines differ
```

---

### `> cat syllabus`

| Week | Read | Build | Checkpoint |
|---|---|---|---|
| **91** | [ch. 00](chapters/00-notation.md), [ch. 01](chapters/01-rotations.md) · Solà et al. §I–III | `so3.py` | `test_so3.py` green |
| **92** | [ch. 02](chapters/02-rigid-motions.md) · Lynch & Park ch. 3.3 | `se3.py` | `01-lie` 20/20 |
| **93** | Barfoot ch. 7 (Lie groups) · ch. 01–02 exercises ★ | Lie-group exercises: SLERP, the right Jacobian, screw extraction | exercises in the lab note |
| **94** | [ch. 03](chapters/03-kinematic-chains.md) · Featherstone ch. 4 | `kinematics.py` | `02-chains` 8/8 |
| **95** | [ch. 04](chapters/04-newton-euler.md) · Lynch & Park ch. 8.2 | `energy()`; the tennis-racket simulation (ch. 04, exercise 3) | energy values correct |
| **96** | [ch. 05](chapters/05-rnea.md) · Featherstone ch. 5 | `rnea()`: pendulum by hand, then code | the double-pendulum Lagrangian matches |
| **97** | [ch. 06](chapters/06-mass-matrix.md) · Slotine & Li ch. 9 | `mass_matrix()`; PD + gravity compensation (ch. 06, exercise 4) | passivity test green; $V(t)$ never increases |
| **98** | [ch. 07](chapters/07-aba.md) §1–4 · Featherstone ch. 7 | `aba()` | ABA inverts RNEA |
| **99** | ch. 07 §5–7 | energy conservation; `python -m blueprint.bench` | `03-dynamics` 14/14 |
| **100** | [ch. 08](chapters/08-contact.md) · Todorov (2014) | the constrained pendulum (ch. 08, exercise 1); boss fight | Mark XI post published |

### `> ls systems/`

| # | System | Tests | Brief |
|---|---|---|---|
| 01 | `lie`: SO(3), SE(3), quaternions, Jacobians, adjoints | 20 | [README](01-lie/README.md) |
| 02 | `chains`: FK, link twists, body Jacobian | 8 | [README](02-chains/README.md) |
| 03 | `dynamics`: RNEA, CRBA, ABA, energy | 14 | [README](03-dynamics/README.md) |

All three systems share one Python package, [`lab/blueprint`](lab/blueprint). You write `so3.py`, `se3.py`, `kinematics.py` and `dynamics.py`; `chain.py` (robot models), `sim.py` (RK4) and `bench.py` are given. Every test is an **independent oracle**:
- series matrix exponentials
- finite differences of forward kinematics
- the textbook double-pendulum Lagrangian
- RNEA columns as the check on CRBA
- the passivity identity
- energy conservation

```sh
make test                       # Mark XI dashboard
make -C 03-dynamics test        # one system, full pytest output
```

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| $(v, \omega)$ vs $(\omega, v)$ | cross-checks against Pinocchio fail by a permutation | angular first here; convert only at library boundaries (ch. 00 §6) |
| $T_{i-1,i}$ vs $T_{i,i-1}$ | kinematics right, twists wrong | the finite-difference tests pinpoint it |
| Forgetting $c_i = \mathrm{ad}_{\mathcal V_i}\mathcal A_i\dot q_i$ | RNEA is right at rest but wrong in motion | ch. 03 §3 |
| $\mathrm{ad}$ vs $\mathrm{ad}^\top$ | gyroscopic terms with the wrong sign; energy drifts | ch. 04 §3: wrenches use the transpose |
| Gravity sign | the arm "falls" upward | $\dot{\mathcal V}_0 = (0, -g)$ with $g = (0, 0, -9.81)$ |
| log near π | NaNs once per million random rotations | the symmetric-part branch (ch. 01 §4) |

### `> ./quiz`

<details><summary>1. Why can't you average two rotation matrices?</summary>

$SO(3)$ is not a vector space: $(R_1 + R_2)/2$ is generally not orthogonal. Average in the tangent space instead, $R_1\exp\!\big(\tfrac12\log(R_1^\top R_2)\big)$, or use quaternion SLERP.
</details>

<details><summary>2. Why does the SO(3) log need a special case near θ = π?</summary>

The generic formula extracts the axis from $R - R^\top = 2\sin\theta[\hat\omega]$, and $\sin\theta \to 0$ at π. The axis information moves into the symmetric part, $(R + \mathbb 1)/2 = \hat\omega\hat\omega^\top$.
</details>

<details><summary>3. What is the v in a space twist, physically?</summary>

The velocity of the body point that is **currently at the space frame's origin**, which may lie far outside the physical body. That choice is what makes $\mathcal V_s = [\mathrm{Ad}_{T_{sb}}]\mathcal V_b$ linear.
</details>

<details><summary>4. Why do wrenches transform with the transpose of the adjoint?</summary>

Power $\mathcal F^\top\mathcal V$ must be frame-independent. If twists transform as $\mathcal V_a = [\mathrm{Ad}]\mathcal V_b$, preserving power forces $\mathcal F_b = [\mathrm{Ad}]^\top\mathcal F_a$.
</details>

<details><summary>5. What does the term $-[\mathrm{ad}_{\mathcal V}]^\top\mathcal G\mathcal V$ represent, and how much work does it do?</summary>

The gyroscopic and velocity-product forces that come from writing Newton's law in a rotating body frame ($\omega\times\mathcal I\omega$ and $m\,\omega\times v$). It does **zero** work, because $\mathcal V^\top[\mathrm{ad}_{\mathcal V}]^\top\mathcal G\mathcal V = ([\mathrm{ad}_{\mathcal V}]\mathcal V)^\top\mathcal G\mathcal V = 0$.
</details>

<details><summary>6. Why is setting $\dot{\mathcal V}_0 = (0, -g)$ equivalent to applying gravity to every link?</summary>

Gravity on a link at its CoM equals $\mathcal G_i(0, R^\top g)$, so it's equivalent to subtracting $(0, R^\top g)$ from that link's acceleration. The acceleration recursion is linear in $\dot{\mathcal V}_0$, and adjoints merely rotate pure linear accelerations, so starting from $(0, -g)$ applies exactly that shift to every link. It's the equivalence principle.
</details>

<details><summary>7. Why is $\dot M - 2C$ skew-symmetric, and why do controllers care?</summary>

With the Christoffel choice of $C$, $(\dot M - 2C)_{ij} = \sum_k(\partial_iM_{jk} - \partial_jM_{ik})\dot q_k$, which flips sign under $i\leftrightarrow j$. It makes the robot passive, so a Lyapunov function $\tfrac12\dot q^\top M\dot q + \tfrac12e^\top K_pe$ proves that PD + gravity compensation is globally stable with no model of $M$ or $C$.
</details>

<details><summary>8. What is the articulated-body inertia, and what is $\mathcal I^a_i\mathcal A_i$?</summary>

It's the inertia a subtree presents at its root when its joints are free (with known torques) rather than locked. $\mathcal I^a_i\mathcal A_i = 0$: no inertial force can be transmitted along a joint's free direction.
</details>

<details><summary>9. ABA is O(n) and CRBA + Cholesky is O(n³). Why does MuJoCo still build M?</summary>

Its contact solver needs $M$ (and its factorization) explicitly to form the constraint-space problem ($J_cM^{-1}J_c^\top$ and Gauss's principle). With sparse factorization for tree structure, the cost is modest.
</details>

<details><summary>10. What does Gauss's principle of least constraint say?</summary>

The constrained acceleration is the one closest to the unconstrained acceleration in the kinetic-energy metric $M$, among all accelerations satisfying the constraints. It's equivalent to the KKT system, and it's the foundation of MuJoCo's contact model.
</details>

### `> cat interview.txt`

- *"Walk me through RNEA. Where does the gravity term come from?"*
- *"Why is the SO(3) logarithm numerically tricky, and how do you implement it robustly?"*
- *"Explain the adjoint representation of SE(3) and where it shows up in dynamics."*
- *"Forward dynamics for a 30-DOF humanoid at 10 kHz: which algorithm, and why?"*
- *"Prove that PD plus gravity compensation is stable for any robot arm."*
- *"How do MuJoCo, Bullet and Drake differ in contact modelling, and why does that matter for sim-to-real?"*

### `> cat boss_fight`

**"The blueprint holds."**
1. **Control from first principles:** a random 7-DOF arm, simulated with *your* ABA, reaches a target posture from a random start under PD + gravity compensation, where the gravity term comes from *your* RNEA. Plot the Lyapunov function $V(t)$: it must be monotonically non-increasing.
2. **Trust, but verify:** cross-check RNEA, CRBA and ABA against Pinocchio on 100 random chains, agreeing to $10^{-9}$ after the $(\omega,v)\leftrightarrow(v,\omega)$ conversion. Document every convention you had to translate.
3. **Contact:** the constrained-particle pendulum (ch. 08, exercise 1) tracks your ABA pendulum for 10 s with Baumgarte stabilization, with the constraint drift plotted.

### `> cat library`

| Source | Use |
|---|---|
| **This Mark's chapters 00–08** | the spine |
| Featherstone, *Rigid Body Dynamics Algorithms* (2008) | the definitive reference: ch. 2 (spatial algebra), 4–7, 8, 11 |
| Lynch & Park, *Modern Robotics*, ch. 3, 4, 5, 8 | the notation used here |
| Solà, Deray & Atchuthan, *A micro Lie theory…* (arXiv:1812.01537) | Lie groups for roboticists |
| Barfoot, *State Estimation for Robotics*, ch. 7 | $SO(3)$/$SE(3)$ Jacobians, rigorously |
| Murray, Li & Sastry, *A Mathematical Introduction to Robotic Manipulation* (free) | the classic twists-and-wrenches text |
| Slotine & Li, *Applied Nonlinear Control*, ch. 9 | passivity-based robot control |
| Carpentier et al., "The Pinocchio C++ library" (2019) | a production implementation of everything here |
| Todorov (2014), Stewart (2000) | contact dynamics (ch. 08) |

**Hardware:** none. It's pure math and simulation.
