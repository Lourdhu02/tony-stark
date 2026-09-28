# Self-assessment

> "2/10" is a feeling. This page turns it into a number you can move.

Score yourself **0–5** in each domain using the evidence column, not your gut. Only claim a level if you can do what the level describes **today**, without looking anything up.
Re-score at the end of every Mark and commit the table. The diff is your progress report.

## Levels

| Level | Name | You can… |
|---|---|---|
| 0 | Unaware | …not name the core ideas |
| 1 | Literate | …explain the core ideas in plain words and follow a tutorial |
| 2 | User | …use the standard tools and libraries to get results |
| 3 | Builder | …build the core pieces **from scratch**, with tests, and explain why they work |
| 4 | Engineer | …debug it under real constraints (timing, memory, noise, hardware) and optimize it with measurements |
| 5 | Architect | …design systems around it, make trade-offs, and teach it |

**Stark score** = the average of all domains × 2, which gives a score out of 10.
A 10/10 means level 5 everywhere, which is a lifetime goal. **The target for this lab is 7/10 after Phase 4 (level 3 or better everywhere, and level 4 or better in half the domains), and 8/10 after Phase 5 (Marks XI–XIII push dynamics, control and learning to level 4).**

## Domains and evidence

| Domain | Level 1 looks like | Level 3 looks like | Level 5 looks like | Mark |
|---|---|---|---|---|
| **C and memory** | Explain stack vs heap, and what a pointer is | Write `malloc` and a shell; read `-fsanitize` reports fluently | Design a memory-safe C API; find UB in others' code by eye | I |
| **Numerics** | Know what floating-point error is | LU with pivoting and RK4 from scratch; predict error order | Choose stable algorithms for new problems; reason about conditioning | I, V |
| **Computer architecture** | Explain fetch–decode–execute | CPU from gates; a RISC-V emulator passing the official tests | Reason about pipelines, caches and branch predictors when optimizing real code | II |
| **OS and concurrency** | Explain processes vs threads | A kernel with traps and a scheduler; lock-free structures verified with `loom` | Diagnose production deadlocks and tail latency; design concurrent systems | III |
| **Electronics and embedded** | Ohm's law, and what a GPIO is | Bare-metal drivers from the datasheet; an RTOS app; a working PCB | Design a board plus firmware for a product: power, EMI, timing budgets | IV |
| **Control** | Explain feedback and PID | PID, LQR and MPC from scratch on nonlinear sims; tune on hardware | Choose and justify controllers for new systems; robustness analysis | V |
| **Dynamics and simulation** | Explain F = ma in 3D | A 6-DOF rigid-body simulator with quaternions from scratch; RNEA/CRBA/ABA derived and implemented | Build differentiable or contact-rich simulators; know where sims lie | I, V, XI |
| **Kinematics and manipulation** | Explain FK vs IK | Numerical IK and trajectory generation from scratch; sim pick-and-place | Whole-body control; grasp planning; manipulation under uncertainty | VI |
| **Estimation and SLAM** | Explain what a Kalman filter does | EKF, PF and pose-graph SLAM from scratch; VO on KITTI | Design estimators for new sensor suites; tune for degeneracies | VII |
| **Planning** | Explain A* | A*, RRT* and a local planner; the Nav2 stack running | Planning under uncertainty; kinodynamic planning at speed | VII |
| **GPU and performance** | Explain why GPUs are fast | Tiled CUDA kernels within 2× of cuBLAS; a TensorRT deploy | Profile-driven optimization of full pipelines; custom fused kernels | VIII |
| **Perception** | Run a pretrained detector | Real-time detection, depth and tracking fused with an IMU | Design perception stacks with latency and failure budgets | VIII |
| **AI agents** | Call an LLM API | A voice agent with tools, memory, evals and a safety layer | Agents that operate physical systems reliably, with measured failure modes | IX |
| **Optimal control and legged locomotion** | Explain what MPC does | iLQR/DDP and collocation from scratch; a centroidal MPC + WBC quadruped trotting in sim | Design whole-body MPC for new robots; reason about hybrid dynamics and contact schedules | XII |
| **Robot learning** | Train a policy with a library | PPO/SAC from scratch; a sim-to-real-robust locomotion policy; diffusion policies evaluated with CIs | Choose between learning and model-based control with evidence; design data and eval strategies | XIII |
| **Mechanical and CAD** | Use a 3D printer | Design printable mounts and mechanisms in CAD | Design for manufacturing; tolerances; mechanism synthesis | IV–X |

## Scorecard

Copy this row block at each checkpoint. Be ruthless.

| Domain | Baseline | After I | After II | After III | After IV | After V | After VI | After VII | After VIII | After IX | After X | After XI | After XII | After XIII |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| C and memory | | | | | | | | | | | | | | |
| Numerics | | | | | | | | | | | | | | |
| Computer architecture | | | | | | | | | | | | | | |
| OS and concurrency | | | | | | | | | | | | | | |
| Electronics and embedded | | | | | | | | | | | | | | |
| Control | | | | | | | | | | | | | | |
| Dynamics and simulation | | | | | | | | | | | | | | |
| Kinematics and manipulation | | | | | | | | | | | | | | |
| Estimation and SLAM | | | | | | | | | | | | | | |
| Planning | | | | | | | | | | | | | | |
| GPU and performance | | | | | | | | | | | | | | |
| Perception | | | | | | | | | | | | | | |
| AI agents | | | | | | | | | | | | | | |
| Optimal control and legged locomotion | | | | | | | | | | | | | | |
| Robot learning | | | | | | | | | | | | | | |
| Mechanical and CAD | | | | | | | | | | | | | | |
| **Stark score /10** | | | | | | | | | | | | | | |

### What "2/10" probably means today

For a working ML engineer, a typical baseline looks like this:
- level 2–3 in *Perception* and *AI agents*
- level 1–2 in *C*, *Numerics* and *GPU*
- level 0–1 in most of the rest

That averages to about 1, which is a Stark score of about 2. Completing the core path is designed to move every domain to 3 or more, which puts the score at **6–7/10**.
