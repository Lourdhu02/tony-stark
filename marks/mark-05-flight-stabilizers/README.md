```text
 ███╗   ███╗██╗  ██╗    ██╗   ██╗
 ████╗ ████║██║ ██╔╝    ██║   ██║    FLIGHT STABILIZERS
 ██╔████╔██║█████╔╝     ██║   ██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ╚██╗ ██╔╝    state space · pid · lqr · mpc · quadrotors
 ██║ ╚═╝ ██║██║  ██╗     ╚████╔╝     weeks 31–38 · 4 systems
 ╚═╝     ╚═╝╚═╝  ╚═╝      ╚═══╝
```

> The difference between flying and falling is a feedback loop.

### `> cat mission.txt`

Learn control properly, from state space to LQR to MPC, by building every controller from scratch against simulations you also build from scratch. Then fly a 6-DOF quadrotor through a figure-8 in your own simulator.

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| State-space models, stability, controllability | The language every robotics paper is written in |
| PID done right: windup, filtering, tuning from a model | 90% of the controllers in industry, including every joint in Mark VI |
| LQR (optimal control) | The dual of the Kalman filter (VII); the backbone of balancing robots |
| MPC | How modern robots handle constraints: torque limits, obstacles, contact |
| 3D rotations and rigid-body dynamics | Mark VI's kinematics, Mark VII's SLAM and Mark VIII's AR all live in SE(3) |
| System identification | Turning Mark IV's real motor data into a model you can design against |

**The ML connection:** LQR is dynamic programming with a quadratic value function. MPC is online optimization. Trajectory optimization is gradient-based planning. You already have the optimization intuition; this Mark gives it physics.

### `> ./prereqs --check`

- [ ] Mark I (integrators, linear algebra) and Mark IV (the motor logs)
- [ ] Linear algebra: eigenvalues, and matrix exponentials at a conceptual level (Strang 18.06 lectures 21–23)
- [ ] Python with NumPy/SciPy/Matplotlib: control *design* happens in Python; the final controllers get ported to C

### `> cat concept_map`

```text
MODELS
├── nonlinear: ẋ = f(x, u)  ──linearize at equilibrium──▶  ẋ = A x + B u
├── discretize (zero-order hold): x[k+1] = A_d x[k] + B_d u[k]
└── system ID: fit A, B (or K/(τs + 1)) from logged data by least squares

ANALYSIS
├── stability: continuous Re(λ) < 0 · discrete |λ| < 1
├── controllability: rank [B  AB  A²B  …] = n
└── frequency view: Bode plots · phase and gain margins (robustness)

CONTROLLERS
├── PID: P (now) · I (the past: removes offset) · D (the future: damps)
│   └── production details: anti-windup · D on measurement · D filtering
├── pole placement: choose where the eigenvalues go
├── LQR: minimize Σ xᵀQx + uᵀRu → Riccati → u = −Kx
├── MPC: at each step solve a QP over a horizon with constraints; apply the first u
└── nonlinear: energy shaping (swing-up) · trajectory optimization (direct collocation)

3D
├── rotations: matrices · quaternions (unit, q ≡ −q) · axis-angle · so(3) exp/log
├── rigid body: Newton–Euler equations  m·v̇ = F,  J·ω̇ + ω × Jω = τ
└── quadrotor: 4 thrusts → mixer → collective thrust + 3 torques · cascaded loops
```

---

### `> cat syllabus`

| Week | Theory | Build | Checkpoint |
|---|---|---|---|
| **31** | Brunton *Control Bootcamp*: overview → state space → stability · *Feedback Systems*: the modeling chapters | `01-cartpole`: derive the equations (Lagrangian), simulate with your RK4, PID on the angle | PID holds the pole for 10 s from 5° |
| **32** | Bootcamp: controllability, pole placement · linearization | linearize at the upright position; eigenvalues; pole placement | closed-loop poles exactly where you placed them |
| **33** | Bootcamp: LQR · derive the discrete Riccati recursion | LQR via Riccati iteration **from scratch**; check against `scipy.linalg.solve_discrete_are` | catches 20° with \|F\| ≤ 10 N |
| **34** | Boyd & Vandenberghe ch. 1–4 (skim) · MPC as a QP | `02-mpc`: a condensed QP, solved with OSQP | MPC beats LQR when force limits bind (plot both) |
| **35** | least squares · first-order plus dead time models | `03-sysid`: fit Mark IV motor logs → model → retune PID → verify on hardware | the model predicts step response RMS within 5% |
| **36** | *micro Lie theory* §I–IV · Beard & McLain ch. 2–3 (coordinate frames, kinematics and dynamics) | `04-quadrotor`: a 6-DOF simulator with quaternion state | energy and angular momentum conserved when there's no input |
| **37** | Beard & McLain: autopilot design (successive loop closure) | cascaded attitude (inner) + position (outer) control, and the mixer | hover holds to ±2 cm; recovers from a 30° attitude kick |
| **38** | *Underactuated*: the trajectory optimization chapter · energy-shaping swing-up | cart-pole swing-up + LQR catch; boss fight | Mark V post published |

### `> ls systems/`

| # | System | Exit criteria |
|---|---|---|
| 01 | `cartpole`: model, PID, pole placement, LQR | LQR balances from **20°** with \|F\| ≤ 10 N; the Riccati solver matches SciPy to 1e-9 |
| 02 | `mpc` | with tight force limits, MPC succeeds where saturated LQR fails; both are plotted |
| 03 | `sysid` | the identified motor model predicts held-out step responses within 5% RMS; the retuned PID improves settling time |
| 04 | `quadrotor`: a 6-DOF sim + cascaded control | tracks a figure-8 (2 m, 8 s period) with **< 10 cm RMS** error |

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| Integrator windup | huge overshoot after saturation | clamp the integral, or use back-calculation anti-windup |
| Derivative kick | an output spike on setpoint steps | take the derivative of the *measurement*, not the error |
| Linearizing at the wrong point | LQR gains that destabilize the real system | linearize at the equilibrium you want to hold (pole up: θ = π if down is 0) |
| Euler angles in 3D | a gimbal-lock explosion at 90° pitch | quaternions (normalize every step) or rotation matrices |
| Inner loop too slow | the cascaded controller oscillates | the inner loop needs about 5–10× the outer loop's bandwidth |
| Simulating with perfect actuators | great in sim, useless on hardware | model saturation, motor lag and sensor noise from day one |

### `> ./quiz`

<details><summary>1. What do the eigenvalues of A tell you about ẋ = Ax?</summary>

Their real parts set growth or decay (stable if every Re λ < 0), and their imaginary parts set oscillation frequency. For discrete systems x[k+1] = Ax[k], stability means every |λ| < 1.
</details>

<details><summary>2. How do you test controllability, and what does failing it mean?</summary>

rank[B, AB, …, Aⁿ⁻¹B] = n. If the rank is lower, some direction in state space can't be influenced by the input. No controller can place those poles, and if they're unstable, the system can't be stabilized.
</details>

<details><summary>3. In LQR, what are you trading off with Q and R?</summary>

Q penalizes state error (performance) and R penalizes control effort (actuator use). Large Q/R gives an aggressive controller with big inputs; small Q/R gives a gentle one with slower convergence. The *ratio* is what matters.
</details>

<details><summary>4. What is integrator windup, and how do you prevent it?</summary>

When the actuator saturates, the integral keeps accumulating error it can't act on. When the error flips sign, that stored integral causes a large overshoot. The fixes are clamping the integrator, conditional integration, or back-calculation.
</details>

<details><summary>5. Why do quadrotors use cascaded control, and what's the rule for the loop rates?</summary>

Position is controlled by tilting, and tilt is controlled by torques. The inner attitude loop must be much faster than the outer position loop (about 5–10× in bandwidth), so the outer loop can treat attitude as "instantly achieved".
</details>

<details><summary>6. Why use quaternions instead of Euler angles?</summary>

There's no gimbal lock, composition is cheap, and they interpolate smoothly. The costs: you must keep them unit length (renormalize), and q and −q represent the same rotation (double cover), so choose the shortest path when computing errors.
</details>

<details><summary>7. What can MPC do that LQR can't?</summary>

It enforces **constraints** explicitly (input limits, state limits, obstacles) by solving an optimization at every step over a finite horizon. LQR assumes unconstrained inputs, so when it saturates, its optimality and sometimes its stability are lost. MPC pays for this with online compute.
</details>

<details><summary>8. What is the separation principle?</summary>

For linear systems with Gaussian noise, you can design the optimal state estimator (a Kalman filter) and the optimal state-feedback controller (LQR) **independently**, and then combine them (LQG). The combination is optimal. That's why Marks V and VII can be learned separately.
</details>

### `> cat interview.txt`

- *"Tune a PID on a system you've never seen. What's your procedure?"*
- *"Explain LQR to a software engineer."*
- *"When is MPC worth the compute?"*
- *"How does a quadcopter stay level? What does each loop do?"*
- *"Why might a controller that works in simulation oscillate on hardware?"*

### `> cat boss_fight`

**"Balance and swing."**
1. The cart-pole starts **hanging down**. Energy shaping pumps it up, and when it's within 20° of vertical, your LQR catches it and holds it with force limits active. Render the animation as ASCII in the terminal.
2. Your quadrotor flies a figure-8 in your simulator with < 10 cm RMS error while you inject 20% motor noise.
3. Post the side-by-side plot: your Mark IV motor's real step response vs your identified model.

### `> cat library`

| Source | Use |
|---|---|
| Steve Brunton, *Control Bootcamp* (YouTube, free) | The spine of weeks 31–33 |
| Åström & Murray, *Feedback Systems* (2nd ed., free) | PID, frequency response, robustness |
| Russ Tedrake, *Underactuated Robotics* (free, online) | Cart-pole, swing-up, trajectory optimization |
| Beard & McLain, *Small Unmanned Aircraft: Theory and Practice* | The quadrotor chapters |
| Solà et al., *A micro Lie theory for state estimation in robotics* | Rotations, done right |
| Boyd & Vandenberghe, *Convex Optimization* (free) | Background for MPC |
| OSQP documentation | The QP solver for MPC |

**Hardware:** reuses the Mark IV motor. Everything else is simulation.
