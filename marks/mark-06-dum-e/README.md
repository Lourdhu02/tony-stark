```text
 ███╗   ███╗██╗  ██╗    ██╗   ██╗██╗
 ████╗ ████║██║ ██╔╝    ██║   ██║██║    DUM-E
 ██╔████╔██║█████╔╝     ██║   ██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ╚██╗ ██╔╝██║    se(3) · kinematics · dynamics · ros 2 · pick & place
 ██║ ╚═╝ ██║██║  ██╗     ╚████╔╝ ██║    weeks 39–48 · 4 systems · sim + optional arm
 ╚═╝     ╚═╝╚═╝  ╚═╝      ╚═══╝  ╚═╝
```

> DUM-E has been holding a fire extinguisher since 2008. Time to give him a real job.

### `> cat mission.txt`

Make an arm do useful work. That means the full stack: rigid-body math, forward and inverse kinematics, trajectories and dynamics-based control from scratch, then the industry stack (ROS 2, MoveIt 2, MuJoCo) and a camera-guided pick-and-place that works on randomized scenes.

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| SE(3): transforms, twists, screw axes | Every robot, camera and map in VII–X is a tree of SE(3) transforms |
| Jacobians and singularities | IK, force control and teleoperation all run through the Jacobian |
| Trajectory generation and computed-torque control | Smooth, safe motion is what separates robots from demos |
| ROS 2, MoveIt 2 and MuJoCo | The tools every robotics team uses; JARVIS (IX) drives the arm through them |
| Camera calibration and hand–eye | Perception is useless until pixels map to the robot's coordinates |

### `> ./prereqs --check`

- [ ] Mark V (rotations, dynamics, control) and Mark I (linear algebra)
- [ ] Ubuntu 24.04 (or Docker) with ROS 2 Jazzy, which is the supported distro for 24.04
- [ ] *Optional hardware:* the SO-101 leader/follower arm kit and a USB camera ([HARDWARE.md](../../HARDWARE.md))

### `> cat concept_map`

```text
GEOMETRY
├── SO(3) rotations · SE(3) poses as 4×4 homogeneous transforms
├── twists (ω, v) · screw axes · exp: se(3) → SE(3) · log for errors
└── frames: space {s} · body {b} · tool {t} · camera {c}

KINEMATICS
├── forward: product of exponentials  T(θ) = e^[S₁]θ₁ ⋯ e^[Sₙ]θₙ · M
├── velocity: space/body Jacobian · singularities (rank loss) · manipulability
└── inverse: analytic (2-link, law of cosines) · Newton–Raphson · damped least squares

MOTION
├── trajectories: cubic/quintic time scaling · trapezoidal velocity profiles
├── dynamics: M(θ)θ̈ + c(θ, θ̇) + g(θ) = τ
└── control: PD + gravity compensation · computed torque · (impedance: stretch)

STACK
├── description: URDF / MJCF
├── ROS 2: nodes · topics · services · actions · tf2 · launch files
├── MoveIt 2: planning scene · collision checking · planners · execution
└── MuJoCo: fast contacts · actuators · a viewer for debugging

PERCEPTION FOR MANIPULATION
└── pinhole model · intrinsics + distortion · depth → 3D · hand–eye AX = XB · segmentation → grasp pose
```

---

### `> cat syllabus`

| Week | Theory (*Modern Robotics*) | Build | Checkpoint |
|---|---|---|---|
| **39** | ch. 2–3: configuration space, rigid-body motions | `01-se3`: exp/log for SO(3) and SE(3), adjoints; property-based tests | `log(exp(ξ)) == ξ` for 10k random twists |
| **40** | ch. 4: forward kinematics (PoE) | FK for a 2-link and a 6-DOF arm; visualize | FK matches MuJoCo to 1e-9 |
| **41** | ch. 5: velocity kinematics and statics | space and body Jacobians; a manipulability ellipsoid plot | the numerical Jacobian matches the analytic one |
| **42** | ch. 6: inverse kinematics | `02-ik`: 2-link analytic; 6-DOF Newton–Raphson → damped least squares | ≥ 99% convergence over 1000 random reachable poses |
| **43** | ch. 9: trajectory generation · ch. 8 (skim): dynamics | quintic and trapezoidal profiles; gravity compensation in MuJoCo | the arm holds any pose with zero commanded motion |
| **44** | ch. 11: robot control | `03-control`: computed-torque tracking in MuJoCo | tracking RMS < 1° on a figure-8 in joint space |
| **45** | ROS 2 Jazzy tutorials: nodes, topics, services, actions, tf2, URDF | your arm's URDF; a joint-state publisher; RViz | the arm moves in RViz from your own node |
| **46** | MoveIt 2 tutorials | MoveIt 2 config; plan around obstacles; execute in sim | a collision-free plan around a box |
| **47** | OpenCV camera calibration · hand–eye (Tsai–Lenz) | `04-pick`: calibrate, detect a colored block, pixel → 3D → grasp → IK → execute | first successful sim pick |
| **48** | evaluation methodology | 50 randomized trials; failure taxonomy; boss fight | Mark VI post published |

### `> ls systems/`

| # | System | Exit criteria |
|---|---|---|
| 01 | `se3`: a Lie-group utility library | exp/log/adjoint are property-tested; used by every later Mark |
| 02 | `ik`: analytic + numerical IK | ≥ **99%** of 1000 random reachable targets reached to within 1 mm and 0.01 rad; handles singular configurations without blowing up |
| 03 | `control`: trajectories + computed torque | joint tracking RMS **< 1°** in MuJoCo with an honest dynamics model |
| 04 | `pick`: calibrated vision + MoveIt 2 | **≥ 80%** success over 50 randomized sim trials (≥ 60% on the real SO-101, if you have it) |

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| Mixing space and body frames | IK converges to mirror-image poses | write the frame in every variable name (`T_sb`, `J_b`) |
| Undamped pseudo-inverse near singularities | joint velocities explode | damped least squares with λ ≈ 0.01–0.1, or clamp the step size |
| Angle wrapping | IK "fails" at ±π | wrap joint errors to (−π, π] |
| Bad calibration | picks miss by a consistent offset | re-run hand–eye; check the reprojection error (< 0.5 px) |
| Sim-to-real on contact | perfect sim grasps slip in reality | add friction noise and object-mass randomization in sim |

### `> ./quiz`

<details><summary>1. Why prefer the product of exponentials over Denavit–Hartenberg?</summary>

The PoE formula needs only a base frame, the home pose M, and each joint's screw axis expressed in one frame. There's no per-link frame assignment and none of DH's special cases for parallel or intersecting axes. It also connects directly to Lie-group tools (twists, adjoints, Jacobians).
</details>

<details><summary>2. What is a kinematic singularity, physically?</summary>

A configuration where the Jacobian loses rank: some end-effector direction can't be produced by any joint velocity. Near it, achieving motion in *nearby* directions demands huge joint speeds. The classic case is a fully stretched arm.
</details>

<details><summary>3. Why damped least squares instead of the pseudo-inverse for IK?</summary>

Δθ = Jᵀ(JJᵀ + λ²I)⁻¹e bounds the step size near singularities. It trades a little accuracy for stability, so the solver stays sane when J is nearly singular. It's the Levenberg–Marquardt idea you know from optimization.
</details>

<details><summary>4. Why quintic rather than cubic time scaling?</summary>

A quintic can set position, velocity **and acceleration** at both ends, so acceleration is continuous and jerk is bounded. That's smoother on motors and gearboxes, and less likely to excite vibrations.
</details>

<details><summary>5. What does computed-torque control do?</summary>

It uses the dynamics model to cancel the nonlinear terms: τ = M(θ)(θ̈_d + K_d ė + K_p e) + c + g. What's left is linear, decoupled error dynamics that you tune like a PD loop. It only works as well as your model does.
</details>

<details><summary>6. What problem does hand–eye calibration solve?</summary>

It finds the fixed transform X between the camera and the robot's hand (or base) from several robot poses A and camera observations B of a calibration target, by solving AX = XB. Without it, detected pixels can't be converted into robot coordinates.
</details>

<details><summary>7. What changed from ROS 1 to ROS 2?</summary>

There's no central master: discovery and transport use DDS. Quality-of-service settings control reliability and durability. It has real-time-friendlier executors, first-class security and lifecycle nodes, and it's multi-platform.
</details>

### `> cat interview.txt`

- *"Derive the IK for a planar 2-link arm."*
- *"What happens to your controller near a singularity?"*
- *"Design a pick-and-place system end to end. Where will it fail?"*
- *"Explain a Jacobian to a new grad in two minutes."*
- *"Your arm overshoots on fast moves. Walk me through the diagnosis."*

### `> cat boss_fight`

**"DUM-E, fetch."** Press Enter:
1. The camera finds the red block anywhere on the table (randomized pose, lighting and distractor blocks).
2. MoveIt 2 plans a collision-free path, and the arm picks the block and drops it in the bin.
3. It does **10 in a row** in MuJoCo.

With the SO-101, run it on the real arm and publish the success rate with its failure breakdown.

### `> cat library`

| Source | Use |
|---|---|
| Lynch & Park, *Modern Robotics* (free PDF) + Coursera specialization + the `modern_robotics` library (to check your answers) | The spine: ch. 2–6, 8–9, 11 |
| Corke, *Robotics, Vision and Control* (3rd ed.) | Code-first reference |
| Solà et al., *A micro Lie theory…* | SE(3), revisited |
| ROS 2 Jazzy docs, MoveIt 2 tutorials, MuJoCo docs | Weeks 43–47 (MuJoCo), 45–47 (ROS 2, MoveIt 2) |
| OpenCV: `calibrateCamera` and `calibrateHandEye` docs | Week 47 |
| Hugging Face LeRobot + the SO-101 build guide | Real hardware (and Mark X) |

**Hardware:** optional. The SO-101 arm and a USB camera; see [HARDWARE.md](../../HARDWARE.md).
