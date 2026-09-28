# The library

> The whole reading list, deliberately short. Each book says which Mark it serves and how to read it.
> Anything not on this page waits until a dossier asks for it.

**How to read this table:**
- **Core:** read the chapters listed in the Mark dossier, cover to cover.
- **Reference:** look things up; don't read linearly.
- **Free:** legally available online at no cost from the authors or publisher.

## Systems and architecture

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| Bryant & O'Hallaron, *Computer Systems: A Programmer's Perspective* (3rd ed.) | Core | I, III | The spine of Phase 1. Ch. 2–3 (data, machine code), 6 (memory hierarchy), 8 (processes and signals), 9 (virtual memory and `malloc`). Pair with CMU 15-213 lectures. |
| Arpaci-Dusseau, *Operating Systems: Three Easy Pieces* (OSTEP) | Core · free | I, III | Short, funny chapters. Virtualization for Mark I, concurrency for Mark III. |
| Drepper, *What Every Programmer Should Know About Memory* (2007) | Reference · free | I, VIII | Sections 3 and 6 explain why your matmul is slow. |
| Goldberg, *What Every Computer Scientist Should Know About Floating-Point Arithmetic* (1991) | Reference · free | I | Read once, before LU. Rounding error is real. |
| Nisan & Schocken, *The Elements of Computing Systems* (nand2tetris, 2nd ed.) | Core | II | Chapters 1–5: NAND to a working CPU. Do every project. |
| Harris & Harris, *Digital Design and Computer Architecture: RISC-V Edition* | Core | II | Ch. 6–7: the ISA and microarchitecture. Your emulator's reference. |
| *The RISC-V Instruction Set Manual, Volume I* | Reference · free | II, III | The actual spec. Your emulator is correct when it matches this. |
| Klabnik & Nichols, *The Rust Programming Language* | Core · free | II, III | Ch. 1–10 before Mark II, ch. 15–16 before Mark III. |
| Bos, *Rust Atomics and Locks* | Core · free | III | Memory ordering explained properly. Build every chapter's primitive. |
| Cox, Kaashoek & Morris, *xv6: a simple, Unix-like teaching operating system* | Core · free | III | Read alongside the MIT 6.1810 labs. |
| Oppermann, *Writing an OS in Rust* (blog series) | Reference · free | III | Targets x86-64; translate the ideas to RISC-V. |

## Electronics and embedded

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| White, *Making Embedded Systems* (2nd ed.) | Core | IV | Architecture, interrupts, timing and power, from someone who ships products. |
| Scherz & Monk, *Practical Electronics for Inventors* | Core | IV | Ch. 2–4: the circuits you'll actually wire. |
| Horowitz & Hill, *The Art of Electronics* (3rd ed.) | Reference | IV | The bible. Never read linearly. |
| Your MCU's reference manual (e.g. ST RM0390 for the STM32F446) | Core | IV | The real textbook for bare metal. Learn to read the register maps. |
| Phil's Lab (YouTube) | Reference · free | IV | KiCad PCB design end to end. |

## Math

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| Strang, *Introduction to Linear Algebra* + MIT 18.06 (OCW) | Core · lectures free | I, V, VII | Lectures 1–10 cover LU, spaces and eigenvalues. Watch at 1.5× and do the problem sets. |
| Trefethen & Bau, *Numerical Linear Algebra* | Reference | I, VII | When Strang says "solve it", this says how, stably. |
| Boyd & Vandenberghe, *Convex Optimization* | Reference · free | V, VII | Ch. 1–5 before MPC. |
| Solà, Deray & Atchuthan, *A micro Lie theory for state estimation in robotics* (arXiv:1812.01537) | Core · free | VI, VII | Rotations done right in 30 pages. |
| Hairer, Lubich & Wanner, *Geometric numerical integration illustrated by the Störmer–Verlet method* (Acta Numerica, 2003) | Reference | I, V | Why symplectic integrators win. |

## Control and dynamics

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| Brunton, *Control Bootcamp* (YouTube) | Core · free | V | Start here: state space, controllability, LQR and the Kalman filter in about 30 short videos. |
| Åström & Murray, *Feedback Systems* (2nd ed.) | Core · free | V | The textbook for PID, frequency response and robustness. |
| Tedrake, *Underactuated Robotics* (MIT 6.832, online) | Core · free | V, X | Cart-pole, acrobot, trajectory optimization. Uses Drake. |
| Beard & McLain, *Small Unmanned Aircraft: Theory and Practice* | Core | V | Quadrotor dynamics and cascaded control, with sim projects. |

## Robotics

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| Lynch & Park, *Modern Robotics* + Coursera specialization | Core · free PDF | VI | Screw theory, the product of exponentials, Jacobians. Watch the short videos. |
| Corke, *Robotics, Vision and Control* (3rd ed., Python) | Reference | VI, VII | Great code-first companion. |
| Thrun, Burgard & Fox, *Probabilistic Robotics* | Core | VII | Ch. 2–8: Bayes filters, the KF/EKF/PF, and motion and sensor models. |
| Barfoot, *State Estimation for Robotics* (2nd ed.) | Core · free PDF | VII | Modern, rigorous, Lie-group native. |
| Stachniss, *Mobile Sensing and Robotics* / SLAM lectures (YouTube) | Core · free | VII | The best SLAM lectures that exist. |
| LaValle, *Planning Algorithms* | Reference · free | VII | Sampling-based planning (RRT is his). |
| Hartley & Zisserman, *Multiple View Geometry in Computer Vision* (2nd ed.) | Reference | VII, VIII | Epipolar geometry and bundle adjustment. |
| ROS 2 documentation + Nav2 + MoveIt 2 tutorials | Core · free | VI, VII | Do the official tutorials. They're good. |

## GPU and AI

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| Hwu, Kirk & El Hajj, *Programming Massively Parallel Processors* (4th ed.) | Core | VIII | Ch. 1–6: the CUDA model, memory and tiling. Ch. 10: reductions. |
| NVIDIA, *CUDA C++ Programming Guide* and the TensorRT docs | Reference · free | VIII | Official, and current. |
| Stanford CS149, *Parallel Computing* (lectures online) | Reference · free | VIII | The theory behind SIMD, threads and GPUs. |
| Sutton & Barto, *Reinforcement Learning: An Introduction* (2nd ed.) | Reference · free | X | You likely know the basics. Revisit ch. 13 (policy gradients). |
| Hugging Face LeRobot docs and course | Core · free | VI, X | Imitation-learning policies (ACT, diffusion) on the SO-101 arm. |

## Advanced robotics (Phase 5)

| Book / course | Role | Mark | Notes |
|---|---|---|---|
| **Mark XI chapters 00–08** (this repo) | Core | XI | Graduate derivations written for this lab; read them before the books below |
| Featherstone, *Rigid Body Dynamics Algorithms* (2008) | Core | XI | The definitive text on RNEA, CRBA, ABA, closed loops and contact |
| Murray, Li & Sastry, *A Mathematical Introduction to Robotic Manipulation* | Reference · free | XI | The classic twists-and-wrenches text |
| Slotine & Li, *Applied Nonlinear Control* | Reference | XI, XII | Ch. 9: passivity-based and adaptive robot control |
| Rawlings, Mayne & Diehl, *Model Predictive Control: Theory, Computation, and Design* | Core · free | XII | MPC stability, computation, real-time iteration |
| Kelly, "An introduction to trajectory optimization" (SIAM Review, 2017) | Core | XII | Direct collocation, hands-on |
| Kajita et al., *Introduction to Humanoid Robotics* | Core | XII | ZMP, LIPM, preview control |
| Wensing et al., "Optimization-based control for dynamic legged robots" (IEEE T-RO) | Core | XII | A survey of the modern legged stack |
| Levine, CS285 *Deep Reinforcement Learning* (lectures online) | Core · free | XIII | The course companion for Mark XIII |
| The papers listed in the Mark XII and XIII dossiers | Core | XII, XIII | Read in full; reproduce one figure each |

## The shelf, ranked by when you need it

```text
NOW          CS:APP · OSTEP · Drepper · Goldberg · Strang 18.06
MONTH 2      nand2tetris · Harris & Harris · The Rust Book
MONTH 4      xv6 book · Rust Atomics and Locks
MONTH 6      Making Embedded Systems · Practical Electronics · your MCU manual
MONTH 8      Control Bootcamp · Feedback Systems · Underactuated · Beard & McLain
MONTH 10     Modern Robotics · micro Lie theory
MONTH 12     Probabilistic Robotics · Barfoot · Stachniss lectures
MONTH 15     PMPP · CUDA guide
MONTH 17+    LeRobot · Sutton & Barto (review)
YEAR 2       Mark XI chapters · Featherstone · Murray–Li–Sastry → Rawlings–Mayne–Diehl · Kajita → CS285 · papers
```
