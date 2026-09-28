# tony-stark

> "Is it better to be feared or respected? I say, is it too much to ask for both?"

A self-enhancement lab for becoming a full-stack engineer who goes from silicon to software to robots to AI,
built one **Mark** at a time, like the suits.

The full plan, with resources and exit criteria for each Mark, is in **[ROADMAP.md](ROADMAP.md)**.
The shopping list is in [HARDWARE.md](HARDWARE.md).

## The Armory

| Mark | Codename | Domain | Status |
|---|---|---|---|
| I | Box of Scraps | C systems programming, numerical simulation | ⬜ planned |
| II | Silicon | Computer architecture, RISC-V emulator (Rust) | ⬜ planned |
| III | The Kernel | OS, concurrency, networking | ⬜ planned |
| IV | Arc Reactor | Electronics, embedded firmware, PCB design | ⬜ planned |
| V | Flight Stabilizers | Control theory, dynamics, quadrotor simulation | ⬜ planned |
| VI | DUM-E | Robot arms: kinematics, pick-and-place | ⬜ planned |
| VII | Autopilot | State estimation, SLAM, motion planning | ⬜ planned |
| VIII | HUD | CUDA, real-time edge perception | ⬜ planned |
| IX | J.A.R.V.I.S. | Voice + LLM agent that controls the robots | ⬜ planned |
| X | Suit Up | Capstone integration | ⬜ planned |

Status: ⬜ planned · 🟨 in progress · ✅ exit criteria met

## Layout

```
tony-stark/
├── ROADMAP.md           # the plan
├── HARDWARE.md          # what to buy, and when
├── marks/
│   └── mark-XX-name/    # one folder per Mark, each project with its own README and tests
├── lab-notes/           # weekly notes (copy TEMPLATE.md)
└── .github/workflows/   # CI for every project
```

## Rules of the lab

1. Build it from scratch first, then use the library.
2. Every Mark ends with a demo and a write-up.
3. Exit criteria are measurable numbers, not vibes.
4. Tests and CI from day one.
5. Write a lab note every week.
