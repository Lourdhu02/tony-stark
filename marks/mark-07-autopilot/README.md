```text
 ███╗   ███╗██╗  ██╗    ██╗   ██╗██╗██╗
 ████╗ ████║██║ ██╔╝    ██║   ██║██║██║    AUTOPILOT
 ██╔████╔██║█████╔╝     ██║   ██║██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ╚██╗ ██╔╝██║██║    kalman · particle filters · slam · vo · planning
 ██║ ╚═╝ ██║██║  ██╗     ╚████╔╝ ██║██║    weeks 49–58 · 5 systems
 ╚═╝     ╚═╝╚═╝  ╚═╝      ╚═══╝  ╚═╝╚═╝
```

> *Where am I?* Every autonomous machine has to answer that question dozens of times a second, from noisy data.

### `> cat mission.txt`

Teach a robot to know where it is, map what it sees, and get where it's going. Build Kalman, extended Kalman and particle filters, occupancy mapping, pose-graph SLAM, visual odometry and A*/RRT* planners from scratch. Then run the real Nav2 stack and beat it (or understand exactly why you can't).

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| Bayes filters (KF / EKF / PF) | Estimation is inside every autonomous system: drones, cars, phones, AR headsets (VIII) |
| SLAM as nonlinear least squares | The same Gauss–Newton/LM machinery as bundle adjustment, calibration and your ML optimizers |
| Visual odometry | The HUD (VIII) needs camera pose to anchor its overlays |
| Search and sampling-based planning | JARVIS (IX) plans tasks; the mobile manipulator (X) plans paths |
| Consistency testing (NEES) | How you *prove* a filter isn't lying about its uncertainty |

**The ML connection:** a Kalman filter is exact Bayesian inference for linear Gaussian models, and the particle filter is sequential importance sampling. Your probabilistic-modelling background makes this the Mark where you'll move fastest.

### `> ./prereqs --check`

- [ ] Mark V (state space, rotations) and Mark VI (SE(3), ROS 2)
- [ ] Probability: Gaussians, conditioning, Bayes' rule, covariance propagation (you likely have this)
- [ ] ROS 2 Jazzy + Gazebo installed; the KITTI odometry dataset (sequence 00) downloaded

### `> cat concept_map`

```text
ESTIMATION
├── Bayes filter:  predict  bel⁻(x) = ∫ p(x | u, x′) bel(x′)
│                  update   bel(x)  ∝ p(z | x) bel⁻(x)
├── KF: linear-Gaussian → closed form · gain K = P Hᵀ (H P Hᵀ + R)⁻¹
├── EKF: linearize f, h with Jacobians · fails with strong nonlinearity
├── (UKF: sigma points, no Jacobians; worth knowing about)
├── PF: particles · importance weights · low-variance resampling · MCL
└── consistency: NEES / NIS chi-square tests

MAPPING AND SLAM
├── occupancy grids: log-odds updates · inverse sensor model
├── scan matching: ICP
├── pose-graph SLAM: nodes = poses · edges = relative measurements
│   └── solve by nonlinear least squares (Gauss–Newton / LM) · sparse structure
├── loop closure: recognize places → add edges → correct accumulated drift
└── tools: GTSAM (factor graphs) · SLAM Toolbox (ROS 2)

VISION GEOMETRY
├── features (ORB) · matching · RANSAC
├── epipolar geometry: essential matrix (5-point) → R, t (scale unknown!)
├── triangulation · PnP · bundle adjustment
└── evaluation: KITTI drift metrics · ATE / RPE

PLANNING
├── graph search: Dijkstra · A* (admissible heuristic → optimal)
├── sampling: PRM · RRT · RRT* (asymptotically optimal)
├── local: pure pursuit · DWA · costmaps with inflation
└── Nav2: behavior trees · planners · controllers · recovery behaviours
```

---

### `> cat syllabus`

| Week | Theory | Build | Checkpoint |
|---|---|---|---|
| **49** | *Probabilistic Robotics* ch. 2–3 · Barfoot ch. 3 (linear-Gaussian estimation) | `01-filters`: 1D KF → 2D diff-drive sim → EKF localization with known landmarks | EKF passes **NEES** consistency (≈ 95% inside the χ² bounds) |
| **50** | PR ch. 5–6 (motion and measurement models) | noise models in the sim; odometry-only vs EKF comparison | a plot of the uncertainty ellipse vs the truth |
| **51** | PR ch. 4 + ch. 8 (particle filter, MCL) | MCL with low-variance resampling on a known map | recovers from **kidnapping** within 50 updates |
| **52** | PR ch. 9 (occupancy grid mapping) | `02-mapping`: ray-cast lidar sim, log-odds grid | the map of a sim building matches ground truth, IoU ≥ 0.9 |
| **53** | LaValle ch. 5 (sampling-based planning) · A* | `03-planning`: A* on a costmap, RRT, RRT*, pure pursuit | the RRT* path cost decreases as samples increase (plot it) |
| **54** | Stachniss's graph-SLAM lectures · Grisetti et al., "A Tutorial on Graph-Based SLAM" | `04-slam`: a 2D pose-graph optimizer (Gauss–Newton, sparse) **from scratch** | the Manhattan (M3500) dataset converges |
| **55** | Dellaert, *Factor Graphs and GTSAM* (tutorial) | the same problem in GTSAM; loop closures from scan matching | your χ² is within 1% of GTSAM's |
| **56** | Hartley & Zisserman ch. 9 (epipolar geometry) | `05-vo`: ORB → essential matrix + RANSAC → pose chain on KITTI 00 | drift < **5%** (with ground-truth scale) |
| **57** | Nav2 + SLAM Toolbox docs | TurtleBot3 in Gazebo: SLAM Toolbox + Nav2; swap in your planner as a plugin (stretch) | Nav2 reaches 5 goals |
| **58** | evaluation methodology | boss fight; comparison table (yours vs Nav2) | Mark VII post published |

### `> ls systems/`

| # | System | Exit criteria |
|---|---|---|
| 01 | `filters`: KF, EKF, MCL | EKF NEES-consistent; MCL solves global localization and kidnapping |
| 02 | `mapping`: occupancy grid | IoU ≥ 0.9 against the ground-truth map |
| 03 | `planning`: A*, RRT, RRT*, pure pursuit | A* is optimal on test grids; RRT* converges towards the optimum; the follower tracks with < 10 cm error |
| 04 | `slam`: pose graph | the from-scratch GN solver matches GTSAM's final χ² within 1% on M3500 |
| 05 | `vo`: monocular visual odometry | KITTI 00 translational drift **< 5%** with GT scale (stretch: stereo, < 2%) |

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| An overconfident filter | the estimate ignores measurements and drifts off | inflate Q; check NEES; don't hand-tune R to "look good" |
| Angle wrap-around in residuals | the EKF diverges near ±π | wrap every angular innovation to (−π, π] |
| Particle deprivation | all particles collapse onto the wrong pose | low-variance resampling; inject random particles; resample only when N_eff is low |
| Dense matrices in SLAM | the solver takes minutes | exploit sparsity (a sparse Cholesky of the normal equations) |
| Monocular scale | the trajectory is right in shape, wrong in size | expected: scale is unobservable from one camera; use GT scale, stereo or an IMU |
| An inadmissible heuristic | A* returns suboptimal paths | the heuristic must never overestimate the true cost |

### `> ./quiz`

<details><summary>1. What does the Kalman gain balance?</summary>

The uncertainty in the prediction (P) against the noise in the measurement (R). A large P or small R means a large gain, so you trust the measurement. A small P or large R means a small gain, so you trust the prediction. The KF is the optimal (minimum-variance) balance for linear-Gaussian systems.
</details>

<details><summary>2. When does the EKF fail?</summary>

When the nonlinearity is strong relative to the uncertainty, the first-order linearization is poor. The filter becomes **inconsistent**: its covariance says it's confident while its error is large. Bad initial estimates and multimodal beliefs (such as global localization) also break it.
</details>

<details><summary>3. Why do particle filters resample, and what can go wrong?</summary>

Without resampling, weight concentrates on a few particles (degeneracy), and the rest waste computation. Resampling duplicates good particles. Done too often, it destroys diversity (**particle deprivation**). The remedies are low-variance resampling and resampling only when the effective sample size drops.
</details>

<details><summary>4. Why store occupancy as log-odds?</summary>

Bayesian updates become **additions** (l ← l + inverse_sensor_model − prior), which are numerically stable, and the representation avoids saturating at exactly 0 or 1.
</details>

<details><summary>5. What makes A* optimal?</summary>

An **admissible** heuristic, one that never overestimates the remaining cost. With a consistent (monotone) heuristic, each node is expanded at most once.
</details>

<details><summary>6. RRT vs RRT*?</summary>

RRT quickly finds *a* feasible path, but its quality doesn't improve with more samples. RRT* rewires the tree to reduce cost as samples grow, and it's **asymptotically optimal**.
</details>

<details><summary>7. Why does loop closure matter so much in SLAM?</summary>

Odometry error accumulates without bound. Recognizing a previously visited place adds a constraint that ties the current pose to an old one. Optimizing the graph then spreads the correction back along the whole trajectory.
</details>

<details><summary>8. Why can't monocular VO recover absolute scale?</summary>

Images are invariant to uniformly scaling the scene and the translation together, so the essential matrix gives t only up to scale. Scale needs extra information: stereo baseline, an IMU, known object sizes or ground truth.
</details>

### `> cat interview.txt`

- *"Explain the Kalman filter twice: once to a PM, once to an engineer."*
- *"EKF vs particle filter: when would you choose each?"*
- *"How does graph-based SLAM work? Why is it a least-squares problem?"*
- *"Design localization for a warehouse robot. What sensors, and what fails?"*
- *"Your robot's map is bent. Where do you look first?"*

### `> cat boss_fight`

**"Autopilot engaged."** Drop the robot into an **unseen** Gazebo building:
1. It explores and maps the building with your pose-graph SLAM and occupancy grid.
2. It localizes on the finished map with your MCL.
3. It reaches 5 goals with your A* + pure pursuit, with zero collisions.

Then run the same mission with SLAM Toolbox + Nav2 and publish the table: time, path length, collisions, CPU use. Honest numbers either way.

### `> cat library`

| Source | Use |
|---|---|
| Thrun, Burgard & Fox, *Probabilistic Robotics* | The spine: ch. 2–6, 8–9 |
| Barfoot, *State Estimation for Robotics* (2nd ed., free PDF) | Rigorous derivations, Lie groups |
| Cyrill Stachniss: SLAM and mobile-robotics lectures (YouTube) | Weeks 54–55 |
| Grisetti, Kümmerle, Stachniss & Burgard, "A Tutorial on Graph-Based SLAM" (2010) | Week 54 |
| Dellaert & Kaess, *Factor Graphs for Robot Perception* · the GTSAM tutorials | Week 55 |
| LaValle, *Planning Algorithms* (free) | Week 53 |
| Hartley & Zisserman, *Multiple View Geometry* | Week 56 |
| Nav2 and SLAM Toolbox docs · the KITTI odometry benchmark | Weeks 56–57 |

**Hardware:** none (Gazebo, KITTI). *Optional:* a cheap lidar (such as an RPLIDAR A1) to map your own room.
