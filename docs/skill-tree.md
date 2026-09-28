# Skill tree

> Every capability in this lab unlocks something downstream. This page shows what each piece of knowledge is *for*, so you never study something without knowing why.

## The whole tree

```mermaid
flowchart TD
    classDef found fill:#050805,stroke:#00ff41,color:#00ff41
    classDef hw fill:#050805,stroke:#39d353,color:#39d353
    classDef robo fill:#050805,stroke:#7ee787,color:#7ee787
    classDef ai fill:#050805,stroke:#a5f3a5,color:#a5f3a5

    subgraph P1["PHASE 1 · FOUNDATIONS"]
        C["C & memory model<br/>Mark I"]:::found
        LA["Numerical linear algebra<br/>Mark I"]:::found
        ODE["Numerical integration<br/>Mark I"]:::found
        PROC["Processes, pipes, signals<br/>Mark I"]:::found
        ARCH["Digital logic → CPU<br/>Mark II"]:::found
        RUST["Rust<br/>Mark II"]:::found
        OS["Kernels, interrupts, scheduling<br/>Mark III"]:::found
        CONC["Concurrency & atomics<br/>Mark III"]:::found
    end

    subgraph P2["PHASE 2 · HARDWARE + CONTROL"]
        EMB["Bare-metal firmware & RTOS<br/>Mark IV"]:::hw
        ELEC["Circuits, sensors, PCB<br/>Mark IV"]:::hw
        CTRL["Feedback control: PID → LQR → MPC<br/>Mark V"]:::hw
        DYN["Rigid-body dynamics<br/>Mark V"]:::hw
    end

    subgraph P3["PHASE 3 · ROBOTICS"]
        KIN["Kinematics & manipulation<br/>Mark VI"]:::robo
        EST["State estimation: KF / EKF / PF<br/>Mark VII"]:::robo
        SLAM["SLAM & mapping<br/>Mark VII"]:::robo
        PLAN["Motion planning<br/>Mark VII"]:::robo
        ROS["ROS 2 ecosystem<br/>Mark VI–VII"]:::robo
    end

    subgraph P4["PHASE 4 · INTELLIGENCE"]
        GPU["CUDA & edge inference<br/>Mark VIII"]:::ai
        PERC["Real-time perception<br/>Mark VIII"]:::ai
        AGENT["LLM agents with tools<br/>Mark IX"]:::ai
        POLICY["Learned policies: IL / RL<br/>Mark X"]:::ai
    end

    subgraph P5["PHASE 5 · ADVANCED ROBOTICS"]
        RBD["Lie groups & RBD algorithms<br/>Mark XI"]:::ai
        OC["Optimal control: DDP · MPC<br/>Mark XII"]:::ai
        LEG["Legged locomotion: centroidal · WBC<br/>Mark XII"]:::ai
        RL["Robot learning: RL · IL · VLA<br/>Mark XIII"]:::ai
    end

    style P1 fill:#0a120a,stroke:#00ff41,color:#00ff41
    style P2 fill:#0a120a,stroke:#39d353,color:#39d353
    style P3 fill:#0a120a,stroke:#7ee787,color:#7ee787
    style P4 fill:#0a120a,stroke:#a5f3a5,color:#a5f3a5
    linkStyle default stroke:#2ea043,stroke-width:1.2px
    style P5 fill:#0a120a,stroke:#d2f7d2,color:#d2f7d2

    C --> PROC --> OS
    C --> ARCH --> OS
    C --> RUST --> OS
    RUST --> CONC
    OS --> CONC
    C --> EMB
    OS --> EMB
    ELEC --> EMB
    ODE --> DYN
    LA --> CTRL
    DYN --> CTRL
    EMB --> CTRL
    LA --> KIN
    DYN --> KIN
    CTRL --> KIN
    LA --> EST
    CTRL --> EST
    EST --> SLAM
    KIN --> PLAN
    SLAM --> PLAN
    CONC --> ROS
    KIN --> ROS
    ARCH --> GPU
    CONC --> GPU
    GPU --> PERC
    EST --> PERC
    PERC --> AGENT
    ROS --> AGENT
    PLAN --> POLICY
    PERC --> POLICY
    AGENT --> POLICY
    DYN --> RBD
    KIN --> RBD
    EST --> RBD
    RBD --> OC
    CTRL --> OC
    OC --> LEG
    RBD --> LEG
    OC --> RL
    POLICY --> RL
    LEG --> RL
```

## Why each edge exists

| From → To | Because |
|---|---|
| C → Processes | `fork`, `exec`, `pipe` and `dup2` are C APIs over kernel objects. You can't reason about them without pointers and file descriptors. |
| Digital logic → Kernels | Interrupts, privilege levels and page tables are hardware features. The kernel is software that choreographs them. |
| Numerical integration → Rigid-body dynamics | Every simulator is `state += f(state)·dt` done carefully. Mark I's pendulum is the seed of Mark V's quadrotor. |
| Linear algebra → Control | LQR is a Riccati equation. Controllability is a matrix rank. Stability is eigenvalues. |
| Linear algebra → State estimation | A Kalman filter is linear algebra plus Gaussians: `K = P·Hᵀ·(H·P·Hᵀ + R)⁻¹`. |
| Control → State estimation | Estimation is the mathematical dual of control: the Kalman filter's Riccati equation is LQR's, transposed. The separation principle then lets you design the two independently (LQG). |
| Firmware → Control | A controller running at 1 kHz with 200 µs of jitter is a different controller. Timing is part of the math. |
| Concurrency → ROS 2 | ROS 2 is callbacks, executors, QoS and shared memory. It's a concurrency framework wearing a robotics hat. |
| Computer architecture → CUDA | GPU performance is about memory hierarchy, coalescing and occupancy, which is Mark I's cache lesson at 1000× scale. |
| Perception + ROS → Agents | JARVIS can only act on a world it can perceive, through interfaces the robot exposes. |
| Dynamics + Kinematics + Estimation → RBD algorithms (XI) | Mark XI derives, from first principles, the machinery Marks V–VII used as black boxes |
| RBD + Control → Optimal control (XII) | iLQR/DDP and MPC differentiate and roll out exactly the RNEA/ABA you wrote in Mark XI |
| Optimal control → Robot learning (XIII) | RL approximates the same Bellman equation; the MPC controllers become baselines, experts and teachers for learned policies |

## Your ML head start

You already have strong nodes in this tree that most robotics engineers don't. Here's where they plug in:

| You already know | It becomes |
|---|---|
| Gradient descent, Adam | Trajectory optimization, nonlinear least squares (Gauss–Newton, Levenberg–Marquardt) for SLAM |
| Probabilistic models, Bayes | Bayes filters: the Kalman filter is a closed-form Bayesian update |
| Backprop, autodiff | Jacobians for EKF and IK, and differentiable simulation |
| Training loops, eval discipline | Measurable exit criteria, benchmark suites, sim-to-real evaluation |
| PyTorch / TensorRT / Triton serving | Mark VIII edge deployment on day one |
| Transformers, VLMs | Mark IX agents and Mark X vision-language-action policies |

The Marks deliberately spend most of your time on the nodes you don't have yet (the left and middle of the tree) and move fast through the nodes you do.
