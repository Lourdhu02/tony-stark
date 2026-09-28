```text
 ███╗   ███╗██╗  ██╗    ██╗  ██╗██╗██╗
 ████╗ ████║██║ ██╔╝    ╚██╗██╔╝██║██║    HULKBUSTER
 ██╔████╔██║█████╔╝      ╚███╔╝ ██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗      ██╔██╗ ██║██║    optimal control · ddp · mpc · legged locomotion
 ██║ ╚═╝ ██║██║  ██╗    ██╔╝ ██╗██║██║    weeks 101–114 · 5 systems · 10 chapters
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝  ╚═╝╚═╝╚═╝
```

> The Hulkbuster is the heaviest suit: a walking machine that must stay upright while things hit it. That's legged locomotion, and it runs on optimal control.

### `> cat mission.txt`

Master optimal control from its foundations to the controllers that make quadrupeds and humanoids walk:
- dynamic programming, LQR as DP, iLQR/DDP
- direct collocation, MPC
- the legged stack: LIPM and capture point, centroidal MPC, whole-body QP control

Build each on your Mark XI dynamics, then assemble a walking quadruped controller in MuJoCo.

**Phase 5 (advanced robotics), Year 2.** Chapters and lab scaffolding unlock when you arrive. This dossier is the full plan.

### `> cat why.txt`

| You'll understand | Because… |
|---|---|
| DP, Bellman and HJB | every optimal controller (and every RL algorithm in Mark XIII) is an approximation to the Bellman equation |
| iLQR/DDP, derived line by line | the workhorse of trajectory optimization, with Mark XI's derivatives in its inner loop |
| Direct collocation and NLP | how to plan motions with constraints (torque limits, contact, obstacles) |
| MPC in real time | the controller behind today's agile legged robots |
| ZMP, LIPM and capture point | the classic, still-deployed theory of balance |
| Centroidal MPC + whole-body QP | the modern two-layer architecture of legged control |

**The ML connection:** DDP is Newton's method on a trajectory, and MPC is amortization-free planning. Mark XIII revisits both from the learning side (value functions, MPPI, model-based RL). Knowing both sides is rare.

### `> ./prereqs --check`

- [ ] **Mark XI** (RNEA, CRBA, ABA, contact): the dynamics every chapter differentiates
- [ ] Mark V (LQR, MPC basics) and Boyd & Vandenberghe ch. 1–5 (convexity, duality)
- [ ] `pip install mujoco osqp`; clone MuJoCo Menagerie for the robot models

### `> cat chapters` *(planned: written when you arrive)*

| # | Chapter | Core result |
|---|---|---|
| 00 | The optimal control problem | continuous vs discrete time; running and terminal costs; path constraints |
| 01 | Dynamic programming and LQR | Bellman's principle; the HJB equation; the finite-horizon, time-varying LQR Riccati recursion **derived as DP** |
| 02 | Pontryagin and shooting | the minimum principle; costates; why single shooting is ill-conditioned |
| 03 | iLQR and DDP | the Q-function expansion; the backward pass; DDP's second-order dynamics terms; regularization; line search; control limits |
| 04 | Direct transcription | multiple shooting; trapezoidal and Hermite–Simpson collocation; NLP sparsity; SQP vs interior point |
| 05 | Model predictive control | receding horizon; terminal cost and stability; real-time iteration; warm starts; NMPC |
| 06 | Balance and walking | hybrid dynamics; ZMP; the LIPM; capture point; preview-control pattern generation |
| 07 | Centroidal dynamics | the centroidal momentum matrix; the single-rigid-body model; convex MPC with linearized friction cones |
| 08 | Whole-body control | task-space inverse dynamics as a QP; operational-space control; strict vs weighted hierarchies; contact wrench cones |
| 09 | The legged stack | gait scheduler → centroidal MPC (~30–100 Hz) → WBC QP (~500 Hz–1 kHz) → joint PD; where each layer fails |

---

### `> cat syllabus`

| Week | Read | Build | Checkpoint |
|---|---|---|---|
| **101** | ch. 00–01 · *Underactuated*: the DP and LQR chapters | finite-horizon LQR via your own Riccati recursion | matches the DARE solution at long horizons |
| **102** | ch. 02–03 · Tassa, Erez & Todorov (2012) | `01-ilqr`: iLQR on the cart-pole using **Mark XI** dynamics | swing-up converges |
| **103** | ch. 03 (DDP, control limits) · Tassa et al. (2014) | the acrobot swing-up; box-constrained iLQR | converges under torque limits |
| **104** | ch. 04 · Kelly (2017, SIAM Review) | `02-collocation`: Hermite–Simpson + an NLP solver | defects < 1e-6; cost within 1% of iLQR |
| **105** | ch. 05 · Rawlings, Mayne & Diehl ch. 1–2 | NMPC on the cart-pole with iLQR as the inner solver; warm starts | 100 Hz MPC holds against disturbances |
| **106** | ch. 06 · Kajita et al. (2003) · Kajita's textbook, the ZMP chapters | `03-lipm-walk`: preview-control pattern generator | 20 steps with the ZMP inside the support polygon |
| **107** | ch. 06 (capture point) · Pratt et al. (2006) | push recovery with capture-point stepping | recovers from pushes up to a measured limit |
| **108** | ch. 07 · Orin, Goswami & Lee (2013) | centroidal momentum computed from Mark XI's CRBA | matches finite differences |
| **109** | ch. 07 · Di Carlo et al. (2018) | `04-quadruped-mpc`: single-rigid-body convex MPC (OSQP) in MuJoCo | stands and walks in place |
| **110** | ch. 07 (gait scheduling, swing trajectories) | trotting; velocity commands | trots at 0.5 m/s for 30 s |
| **111** | ch. 08 · Khatib (1987) | `05-wbc`: task-space inverse dynamics QP | tracks base and swing-foot tasks within friction cones |
| **112** | ch. 08 (hierarchies) · Wensing et al. (survey) | MPC → WBC → joint PD integration | the full stack trots; per-layer timing logged |
| **113** | ch. 09 | robustness: pushes, payloads, rough terrain | a failure taxonomy with plots |
| **114** | — | boss fight + write-up | Mark XII post published |

### `> ls systems/` *(lab scaffolding unlocks when you arrive)*

| # | System | Exit criteria |
|---|---|---|
| 01 | `ilqr`: iLQR/DDP with regularization, line search and box constraints | acrobot swing-up in < 100 iterations; gradients verified against finite differences |
| 02 | `collocation`: Hermite–Simpson transcription + NLP | defects < 1e-6; cost within 1% of the iLQR solution on the same problem |
| 03 | `lipm-walk`: preview control + capture point | 20 steps with the ZMP inside the support polygon; push recovery by stepping |
| 04 | `quadruped-mpc`: convex centroidal MPC | a 0.5 m/s trot for 30 s in MuJoCo; each MPC solve < 5 ms |
| 05 | `wbc`: task-space QP | friction-cone and torque-limit constraints respected; each QP solve < 1 ms |

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| $Q_{uu}$ not positive definite | iLQR diverges, or steps uphill | Levenberg–Marquardt regularization, adapted every iteration |
| No line search | cost oscillates | a backtracking line search on the forward-pass step size α |
| Single shooting over long horizons | exploding gradients | multiple shooting or collocation |
| MPC without a terminal cost | a short-horizon MPC walks off a cliff | a terminal cost from LQR, or a longer horizon |
| An infeasible WBC QP | the robot collapses when a foot slips | slack variables on low-priority tasks; never on the contact constraints |
| Ignoring solve time | beautiful plans that arrive too late | measure solve latency every cycle; warm start |

### `> ./quiz`

<details><summary>1. State Bellman's principle of optimality, and write the discrete-time Bellman equation.</summary>

Any tail of an optimal trajectory is itself optimal for the tail problem. $V_k(x) = \min_u\big[\ell(x,u) + V_{k+1}(f(x,u))\big]$ with $V_N = \ell_f$. LQR is the case where $V$ stays exactly quadratic.
</details>

<details><summary>2. What's the difference between iLQR and DDP?</summary>

Both expand the Q-function to second order around the current trajectory. DDP keeps the **second-order dynamics terms** ($f_{xx}, f_{uu}, f_{ux}$ contracted with $V_x$). iLQR drops them, which is cheaper per iteration and usually almost as good, since those terms vanish near a solution where the dynamics are nearly linear over a step.
</details>

<details><summary>3. Why and how do you regularize the DDP backward pass?</summary>

$Q_{uu}$ must be positive definite for the local minimization to be well posed and the step to be a descent direction. Add $\mu\mathbb 1$ (or regularize $V_{xx}$), increasing μ when the pass fails and decreasing it after success. This is Levenberg–Marquardt on trajectories.
</details>

<details><summary>4. Shooting vs collocation: what does each optimize over?</summary>

Shooting optimizes over controls only (states come from simulation), so it's always dynamically feasible but badly conditioned over long horizons. Collocation optimizes over states *and* controls, with dynamics as equality constraints. It's better conditioned, it can start from infeasible guesses, and it handles state constraints naturally, at the price of a larger (but sparse) NLP.
</details>

<details><summary>5. Why does MPC need a terminal cost?</summary>

A finite horizon makes the controller myopic, and closed-loop stability isn't guaranteed. A terminal cost approximating the true cost-to-go (for example an LQR value function), optionally with a terminal constraint set, restores the stability guarantee.
</details>

<details><summary>6. What is the ZMP, and when is the ZMP criterion insufficient?</summary>

The point on the ground where the net moment of the contact forces has no horizontal component. Keeping it inside the support polygon means the foot won't tip. It assumes flat, coplanar contacts with sufficient friction, and it says nothing about the robot's overall ability to avoid falling (the capture point addresses that).
</details>

<details><summary>7. What is the capture point in the LIPM?</summary>

$\xi = x + \dot x/\omega$ with $\omega = \sqrt{g/z_0}$: the point where the robot must step to come to rest. Its dynamics $\dot\xi = \omega(\xi - p_{\text{ZMP}})$ are unstable, and balance control is about keeping $\xi$ controllable within the reachable support region.
</details>

<details><summary>8. Why do quadruped MPCs use a single-rigid-body (centroidal) model?</summary>

The legs are light relative to the body, so the body's 6-DOF dynamics under contact forces capture what matters. With a fixed orientation linearization and linearized friction pyramids, the MPC becomes a **convex QP**, small and fast enough to solve in milliseconds. The whole-body details go to the WBC layer.
</details>

<details><summary>9. Weighted vs strict task hierarchies in whole-body control?</summary>

Weighted: a single QP with task weights, simple and fast, but high-priority tasks can be compromised. Strict: lexicographic (a sequence of QPs, or null-space projections), so lower tasks never disturb higher ones, at more computational cost. Contact and physics constraints are always hard constraints, never tasks.
</details>

### `> cat interview.txt`

- *"Derive the iLQR backward pass."*
- *"How would you make MPC run at 1 kHz on a robot?"*
- *"Explain ZMP and capture point. Which would you use for push recovery, and why?"*
- *"Design the control architecture for a quadruped. What runs at what rate?"*
- *"Your trajectory optimizer converges in sim but the robot falls. What do you check?"*

### `> cat boss_fight`

**"Hulkbuster walks."** A MuJoCo quadruped (from MuJoCo Menagerie) runs *your* whole stack:
- a gait scheduler, then *your* centroidal MPC, then *your* whole-body QP, then joint PD
- keyboard velocity commands: trot at 0.5 m/s, turn, stop
- it survives lateral pushes, and you publish the largest push it withstands
- plots of per-layer solve times prove the rates hold

**Stretch:** swap in an iLQR whole-body MPC using Mark XI dynamics, and compare.

### `> cat library`

| Source | Use |
|---|---|
| Tedrake, *Underactuated Robotics* (online): DP, LQR, trajectory optimization, walking | the spine for ch. 00–06 |
| Tassa, Erez & Todorov, "Synthesis and stabilization of complex behaviors through online trajectory optimization" (IROS 2012) | iLQR in practice |
| Tassa, Mansard & Todorov, "Control-limited differential dynamic programming" (ICRA 2014) | box-constrained DDP |
| Li & Todorov (2004) · Mayne (1966) | iLQR and DDP origins |
| Kelly, "An introduction to trajectory optimization: how to do your own direct collocation" (SIAM Review, 2017) | ch. 04 |
| Betts, *Practical Methods for Optimal Control Using Nonlinear Programming* | reference |
| Rawlings, Mayne & Diehl, *Model Predictive Control: Theory, Computation, and Design* (free PDF) | ch. 05 |
| Kajita et al., "Biped walking pattern generation by using preview control of zero-moment point" (ICRA 2003) · Kajita et al., *Introduction to Humanoid Robotics* | ch. 06 |
| Pratt et al., "Capture point: a step toward humanoid push recovery" (Humanoids 2006) | ch. 06 |
| Orin, Goswami & Lee, "Centroidal dynamics of a humanoid robot" (Autonomous Robots, 2013) | ch. 07 |
| Di Carlo et al., "Dynamic locomotion in the MIT Cheetah 3 through convex model-predictive control" (IROS 2018) | ch. 07 |
| Khatib, "A unified approach for motion and force control of robot manipulators: the operational space formulation" (1987) | ch. 08 |
| Wensing et al., "Optimization-based control for dynamic legged robots" (IEEE Transactions on Robotics) | a survey of the whole field |
| Crocoddyl, acados, OSQP, MuJoCo Menagerie | tools |

**Hardware:** none (MuJoCo). A decent CPU helps with real-time MPC.
