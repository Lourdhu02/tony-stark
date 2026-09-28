# Roadmap

> "Tony Stark was able to build this in a cave! With a box of scraps!"

Stark is an engineering generalist who can take an idea from first principles to working hardware.
He understands every layer: physics, electronics, low-level software, control, perception and AI.
This roadmap aims for that same full-stack depth, from silicon up to an AI assistant that moves a robot.

## Assumptions

| | |
|---|---|
| Starting point | Working ML engineer: comfortable with Python, PyTorch, training and serving models. ML basics are skipped. |
| Time | About 10–12 hrs/week, which puts the core path at roughly 18 months. |
| Hardware | Simulation first. Marks I–III and V need no hardware. Hardware starts at Mark IV and is optional after that (see [HARDWARE.md](HARDWARE.md)). |
| Languages | C (Mark I), Rust (Marks II–III), C++ (robotics/ROS 2), Python (simulation prototypes, ML), CUDA (Mark VIII). |

## Rules of the lab

1. **Build it from scratch first, then use the library.** Write your own Kalman filter before you `import filterpy`.
2. **Every Mark ends with a demo and a write-up.** Record a GIF or video, add a README with results, and write down what broke.
3. **Exit criteria are measurable.** A Mark is done when its numbers are hit, not when the tutorial is finished.
4. **Tests and CI from day one.** Each project has a test suite that runs in GitHub Actions.
5. **Weekly lab note** in `lab-notes/` covering what you built, what you learned and what's next (use the [template](lab-notes/TEMPLATE.md)).

Weekly rhythm (about 11 hrs): 2 theory sessions of about 2 h, 2 build sessions of about 3 h, and 1 h for the lab note.

---

## Phase 1: Foundations (the cave)

### Mark I: "Box of Scraps" (weeks 1–6)
*Systems programming in C, plus numerical simulation from zero.*

| Build | What you learn |
|---|---|
| `stark-shell`: a Unix shell with pipes, redirection, background jobs and signals | processes, `fork`/`exec`, file descriptors, syscalls |
| `stark-malloc`: an allocator with free lists and coalescing, used via `LD_PRELOAD` | virtual memory, heap layout, fragmentation |
| `linalg.c`: a matrix library (mul, transpose, LU solve, inverse) with tests | memory layout, cache effects, numerical stability |
| `sim/`: Euler, RK4 and symplectic integrators simulating a pendulum and a spring-mass system | ODEs, numerical error, energy conservation |

- **Resources:** *CS:APP* (Bryant & O'Hallaron) ch. 1–3 and 8–9; *OSTEP* (virtualization part); Beej's guides.
- **Exit criteria:** `ls -l | grep .c | wc -l > out.txt` works in your shell. The allocator can run `python3` via `LD_PRELOAD`. RK4 pendulum energy drift stays under 0.1% over 100 s, and you can explain why symplectic integration beats RK4 over long horizons.

### Mark II: "Silicon" (weeks 7–14)
*How computers actually work, from NAND gates to a RISC-V CPU.*

| Build | What you learn |
|---|---|
| nand2tetris Part 1: build a CPU from NAND gates | digital logic, ALU, memory, the fetch–decode–execute loop |
| CHIP-8 emulator in Rust | Rust basics, instruction decoding |
| RV32I emulator in Rust that runs C programs cross-compiled with `riscv64-unknown-elf-gcc` | ISA, ELF loading, calling conventions |
| *(optional)* a 5-stage pipelined RV32I core in Verilog, simulated with Verilator | pipelining, hazards, HDL |

- **Resources:** nand2tetris.org; Harris & Harris, *Digital Design and Computer Architecture: RISC-V Edition*; Ben Eater's 8-bit computer videos; *The Rust Book*.
- **Exit criteria:** the emulator passes the official `riscv-tests` rv32ui suite and runs a C program that prints Fibonacci numbers.

### Mark III: "The Kernel" (weeks 15–22)
*Operating systems, concurrency and networking.*

| Build | What you learn |
|---|---|
| Selected MIT 6.1810 xv6 labs (syscall, pgtbl, traps, cow, lock) | real kernel internals |
| `stark-os`: a tiny RISC-V kernel in Rust on QEMU (UART, traps, timer interrupts, round-robin scheduler) | bare metal, interrupts, context switching |
| Lock-free SPSC ring buffer and a work-stealing thread pool in Rust | atomics, memory ordering |
| Event-driven TCP server using `epoll` in C, load-tested | I/O models, the C10K problem |

- **Resources:** *OSTEP*; the xv6 book; Mara Bos, *Rust Atomics and Locks*; Philipp Oppermann's "Writing an OS in Rust".
- **Exit criteria:** `stark-os` boots and preemptively switches between 2 or more tasks. The ring buffer is race-free under `loom` and faster than a `Mutex<VecDeque>` in a benchmark. The server handles 10k concurrent connections.

---

## Phase 2: Hardware and control (the workshop)

### Mark IV: "Arc Reactor" (weeks 23–30)
*Electronics, embedded firmware and power: the energy core.*

| Build | What you learn |
|---|---|
| Bare-metal STM32: blink an LED by writing registers directly (no HAL), then a UART driver | memory-mapped I/O, datasheets, clock trees |
| I2C driver for an IMU plus complementary and Madgwick filters | sensor protocols, sensor fusion |
| FreeRTOS (or Rust Embassy) firmware with separate sensor, control and telemetry tasks | real-time scheduling, priorities, jitter |
| DC motor with encoder: PID speed control using PWM and an H-bridge | actuators, closed-loop control on real hardware |
| KiCad PCB: an IMU and motor-driver breakout board, sent to a fab house | schematics, layout, manufacturing |

- **Resources:** Elecia White, *Making Embedded Systems*; Scherz, *Practical Electronics for Inventors*; *The Art of Electronics* (as a reference); Phil's Lab (YouTube) for PCB design.
- **Exit criteria:** the motor holds its commanded RPM within ±5% under load, the control loop runs at a stable 1 kHz (measure the jitter), and the PCB arrives and works.

### Mark V: "Flight Stabilizers" (weeks 31–38)
*Control theory and dynamics, all in simulation.*

| Build | What you learn |
|---|---|
| Cart-pole from scratch: PID, then pole placement, then LQR, then MPC | state space, controllability, optimal control |
| System identification: fit a model to logged data from the Mark IV motor | least squares, frequency response |
| Quadrotor 6-DOF simulator from scratch, with cascaded PID attitude and position control | rigid-body dynamics, quaternions |
| Swing-up with energy shaping and trajectory optimization (direct collocation) | nonlinear control |

- **Resources:** Steve Brunton's *Control Bootcamp* (YouTube); Åström & Murray, *Feedback Systems*; Russ Tedrake, *Underactuated Robotics* (MIT 6.832, free online); Beard & McLain, *Small Unmanned Aircraft*.
- **Exit criteria:** LQR balances the cart-pole from 20° with actuator limits. MPC beats LQR when constraints are tight (show the plots). The simulated quadrotor tracks a figure-8 with under 10 cm RMS error.

---

## Phase 3: Robotics (the robots)

### Mark VI: "DUM-E" (weeks 39–48)
*Manipulators: kinematics, dynamics and pick-and-place.*

| Build | What you learn |
|---|---|
| FK and IK for a 2-link arm (analytic), then a 6-DOF arm (numerical Jacobian and damped least squares) | screw theory, the product of exponentials, singularities |
| Trajectory generation (quintic polynomials, trapezoidal velocity profiles) and computed-torque control | manipulator dynamics |
| MuJoCo simulation, followed by ROS 2 with MoveIt 2 | industry tooling |
| Vision-based pick-and-place: camera, color/object detection, grasp pose, IK, execution | perception–action loop |
| *(hardware)* An SO-101 arm (the LeRobot open-source arm) doing the same task for real | sim-to-real |

- **Resources:** Lynch & Park, *Modern Robotics* (free book, plus the Coursera specialization); the MuJoCo docs; the ROS 2 and MoveIt 2 tutorials; Hugging Face LeRobot.
- **Exit criteria:** at least 80% pick success on a randomly placed block in simulation (and at least 60% on real hardware, if you have the arm).

### Mark VII: "Autopilot" (weeks 49–58)
*Mobile robots: state estimation, SLAM and planning.*

| Build | What you learn |
|---|---|
| Differential-drive simulation with EKF localization, then a particle filter (MCL) | Bayes filters, the motion and sensor models you already know from ML |
| Occupancy-grid mapping from simulated LiDAR | inverse sensor models |
| Planners: A*, then RRT*, then a local planner (DWA or pure pursuit) | search, sampling-based planning |
| Monocular visual odometry with OpenCV on KITTI | epipolar geometry, bundle adjustment |
| Pose-graph SLAM with GTSAM, then the full Nav2 stack in Gazebo | factor graphs, loop closure |

- **Resources:** Thrun, Burgard & Fox, *Probabilistic Robotics*; Barfoot, *State Estimation for Robotics*; Cyrill Stachniss's lectures (YouTube); Hartley & Zisserman, *Multiple View Geometry* (as a reference).
- **Exit criteria:** the robot autonomously explores and maps an unseen simulated building and navigates to goals. VO drift is under 2% on a KITTI sequence.

---

## Phase 4: Intelligence (the AI)

### Mark VIII: "HUD" (weeks 59–66)
*Real-time perception at the edge. This builds on your ML strength, so go deep on performance.*

| Build | What you learn |
|---|---|
| Hand-written CUDA kernels: tiled matmul, softmax, layernorm, benchmarked against cuBLAS | GPU memory hierarchy, occupancy |
| Real-time detection, depth estimation and multi-object tracking, fused with the IMU | latency budgets, synchronization |
| Deploy with TensorRT or Triton (reuse your `triton-server` work), INT8 quantized | edge inference |
| A HUD overlay: world-locked annotations using a camera pose from VIO | AR fundamentals |

- **Resources:** Kirk & Hwu, *Programming Massively Parallel Processors*; the CUDA C++ Programming Guide; the TensorRT docs.
- **Exit criteria:** the full pipeline runs at 30 FPS or more with under 50 ms glass-to-glass latency on a Jetson (or a laptop GPU).

### Mark IX: "J.A.R.V.I.S." (weeks 67–76)
*The assistant that ties everything together.*

| Build | What you learn |
|---|---|
| Voice pipeline: wake word, streaming STT (Whisper), an LLM agent with tool use, streaming TTS | real-time audio, turn-taking |
| Tools: control the DUM-E simulator, query robot and sensor state, run code, and handle home automation over MQTT | agent design, tool schemas |
| Memory (episodic and semantic), plus an evaluation suite for tool-call accuracy | agent evaluation |
| A safety layer: any command that moves hardware needs explicit confirmation and passes workspace-limit checks | safe actuation |

- **Resources:** Whisper, an LLM API with tool use (or a local model), MQTT/Mosquitto, Home Assistant.
- **Exit criteria:** "JARVIS, pick up the red block and put it in the bin" works end to end in simulation, with at least 90% tool-call accuracy on your evaluation set.

### Mark X: "Suit Up" (week 77 onward): capstone
Pick one and integrate everything:
- **Mobile manipulator:** Mark VII's base plus Mark VI's arm, driven by JARVIS.
- **Learned policies:** imitation learning (ACT or diffusion policy via LeRobot) and RL with sim-to-real transfer.
- **Wearable:** an EMG- or gesture-controlled arm with the HUD.

---

## Continuous tracks (run alongside the Marks)

| Track | Plan |
|---|---|
| **Math** | Linear algebra (Strang, 18.06), probability, optimization (Boyd & Vandenberghe, *Convex Optimization*), Lie groups for robotics (Solà, *A micro Lie theory*). About 1 hr/week. |
| **Mechanical / CAD** | Onshape or FreeCAD from Mark IV onward. Design mounts and enclosures for every hardware Mark and 3D print them. |
| **Physics** | Rigid-body dynamics, and the basics of electromagnetism and circuits, alongside Marks IV–V. |
| **Writing** | One blog post per Mark. Teaching it is how you know you understand it. |

## Milestones at a glance

```
Month:  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19+
Mk I    ███
Mk II      ████
Mk III         ████
Mk IV              ████
Mk V                   ████
Mk VI                      █████
Mk VII                          █████
Mk VIII                              ████
Mk IX                                    █████
Mk X                                          ███████>
```
