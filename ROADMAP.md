# Roadmap

> Stark is an engineering generalist who can take an idea from first principles to working hardware.
> This roadmap builds that same full-stack depth: from silicon to an AI that safely moves a robot.

This page is the **overview**. The depth lives in each Mark's dossier: the week-by-week syllabus, project specs, pitfalls, a quiz with answers, interview questions and a boss fight.
The [skill tree](docs/skill-tree.md) shows how the Marks depend on each other, and [the method](docs/method.md) shows how to work through them.

## Assumptions

| | |
|---|---|
| **Starting point** | Working ML engineer: Python, PyTorch, training and serving models. ML basics are skipped; ML intuition is used everywhere. |
| **Time** | 10–12 hrs/week ([the weekly loop](docs/method.md#the-weekly-budget-about-11-hours)): about 18 months for Marks I–IX, then the capstone. Phase 5 (Marks XI–XIII) is Year 2. |
| **Hardware** | Simulation first. Only Mark IV *requires* hardware (about $150). See [HARDWARE.md](HARDWARE.md). |
| **Languages** | C (I, III, IV) · Rust (II, III) · Python (V–IX) · C++ (ROS 2, as needed) · CUDA (VIII) |
| **Target** | A Stark score of **7/10** on the [self-assessment](docs/self-assessment.md) after Phase 4 (level 3, "builder", or better in every domain), and **8/10** after Phase 5 |

## The thirteen Marks

| Mark | Codename | Weeks | The question it answers | Headline exit criterion | Dossier |
|---|---|---|---|---|---|
| **I** | Box of Scraps | 1–6 | What is a computer *actually* doing when my code runs? | 79/79 tests; `python3` runs on your `malloc`, inside your shell | [open →](marks/mark-01-box-of-scraps) |
| **II** | Silicon | 7–14 | How does hardware execute an instruction? | a RISC-V emulator passing the official `rv32ui` tests | [open →](marks/mark-02-silicon) |
| **III** | The Kernel | 15–22 | How do many things share one machine safely? | a preemptive Rust kernel on QEMU; a loom-verified SPSC queue | [open →](marks/mark-03-the-kernel) |
| **IV** | Arc Reactor | 23–30 | How does code touch the physical world? | a motor held ±5% at 1 kHz with p99 jitter < 10 µs, on your own PCB | [open →](marks/mark-04-arc-reactor) |
| **V** | Flight Stabilizers | 31–38 | How do you make an unstable system behave? | a cart-pole swing-up + LQR catch; a quadrotor figure-8 < 10 cm RMS | [open →](marks/mark-05-flight-stabilizers) |
| **VI** | DUM-E | 39–48 | How does a robot arm reach, grasp and place? | ≥ 80% vision-guided pick success over 50 randomized trials | [open →](marks/mark-06-dum-e) |
| **VII** | Autopilot | 49–58 | Where am I, what's around me, how do I get there? | autonomous exploration and navigation of an unseen building with your SLAM | [open →](marks/mark-07-autopilot) |
| **VIII** | HUD | 59–66 | How do you see the world in real time on a small computer? | ≥ 30 FPS, < 50 ms glass-to-glass, world-locked AR labels | [open →](marks/mark-08-hud) |
| **IX** | J.A.R.V.I.S. | 67–76 | How does an AI safely act on the physical world? | ≥ 90% tool accuracy, 0 unsafe actions, e-stop < 200 ms | [open →](marks/mark-09-jarvis) |
| **X** | Suit Up | 77+ | Can I integrate it all into one system? | a capstone meeting its design-doc metric, with confidence intervals | [open →](marks/mark-10-suit-up) |
| **XI** | Blueprint | 91–100 | What are the exact equations of motion, and how do engines compute them? | your RNEA, CRBA and ABA agree with physics: Lagrangian, passivity and energy (42 tests) | [open →](marks/mark-11-blueprint) |
| **XII** | Hulkbuster | 101–114 | How do you plan and control motion optimally, including walking? | a MuJoCo quadruped trots under your centroidal MPC + whole-body QP | [open →](marks/mark-12-hulkbuster) |
| **XIII** | Extremis | 115–128 | When should a robot learn instead, and how does learning survive reality? | RL vs MPC head-to-head; diffusion policy vs a scripted pipeline, with CIs | [open →](marks/mark-13-extremis) |

## Phases and checkpoints

| Phase | Marks | You become able to… | Checkpoint |
|---|---|---|---|
| **1 · Foundations** (the cave) | I–III | reason about any program down to the cache line, instruction and syscall | re-score: C, Numerics, Architecture and OS reach 3 |
| **2 · Hardware and control** (the workshop) | IV–V | make physical things move precisely, on time | re-score: Embedded, Control and Dynamics reach 3 |
| **3 · Robotics** (the robots) | VI–VII | build robots that perceive, localize, plan and manipulate | re-score: Kinematics, Estimation and Planning reach 3 |
| **4 · Intelligence** (the AI) | VIII–X | put real-time AI on robots, safely, and prove it works | re-score: GPU, Perception and Agents reach 3–4 → **Stark score ≥ 7** |
| **5 · Advanced robotics** (Year 2) | XI–XIII | derive and implement the dynamics, optimal control and learning at the frontier of the field | re-score: Dynamics, Optimal control and Robot learning reach 4 → **Stark score ≥ 8** |

## Timeline

```text
week    1         11        21        31        41        51        61        71        81        91        101       111       121
Mk I    ██████
Mk II         ████████
Mk III                ████████
Mk IV                         ████████
Mk V                                  ████████
Mk VI                                         ██████████
Mk VII                                                  ██████████
Mk VIII                                                           ████████
Mk IX                                                                     ██████████
Mk X                                                                                ██████████████
Mk XI                                                                                             ██████████
Mk XII                                                                                                      ██████████████
Mk XIII                                                                                                                   ██████████████
        └──────────────────────────────────────────────────────────────────────────── phases 1–4 ┘└─────────────────────────── phase 5 ┘
```

Slipping is normal. A Mark is done when its exit criteria are met, not when its weeks run out. Never skip a Mark's exit criteria to stay on schedule; move the schedule instead.

## Rules of the lab

1. **Build it from scratch first, then use the library.** Write your own Kalman filter before you `import filterpy`.
2. **Every Mark ends with a boss fight and a write-up.** Record it, publish it, and write down what broke.
3. **Exit criteria are numbers.** Not "I finished the tutorial".
4. **Tests and CI from day one.** A completed system never regresses (the `COMPLETED` gate).
5. **Weekly lab note** in [`lab-notes/`](lab-notes). No note, no week.

## Continuous tracks (alongside the Marks)

| Track | Plan |
|---|---|
| **Math** | Strang 18.06 (Marks I, V), probability (VII), optimization (Boyd, V and VII), Lie groups (Solà, V–VII). About 1 hr/week. |
| **Mechanical / CAD** | Onshape or FreeCAD from Mark IV onward: design and 3D-print a mount or enclosure for every hardware Mark |
| **Writing** | One public post per Mark: the boss fight, the numbers and the lessons |
| **Spaced review** | 20 minutes every Sunday: 5 random quiz questions from finished Marks ([why](docs/method.md#the-loop)) |
