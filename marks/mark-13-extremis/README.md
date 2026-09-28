```text
 ███╗   ███╗██╗  ██╗    ██╗  ██╗██╗██╗██╗
 ████╗ ████║██║ ██╔╝    ╚██╗██╔╝██║██║██║    EXTREMIS
 ██╔████╔██║█████╔╝      ╚███╔╝ ██║██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗      ██╔██╗ ██║██║██║    rl · sim-to-real · imitation · diffusion · vlas
 ██║ ╚═╝ ██║██║  ██╗    ██╔╝ ██╗██║██║██║    weeks 115–128 · 6 systems · 10 chapters
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝  ╚═╝╚═╝╚═╝╚═╝
```

> Extremis rewrote its host to adapt. Robot learning does the same for controllers: policies that improve from data where hand-written control runs out.

### `> cat mission.txt`

Take your ML expertise into the physical world:
- derive and implement policy-gradient and actor-critic RL from scratch
- train locomotion in massively parallel simulation and make it survive the sim-to-real gap
- master imitation learning, from behaviour cloning to diffusion policies and vision-language-action models
- evaluate it all with the statistical rigour real robots demand

**Phase 5 (advanced robotics), Year 2.** Chapters and lab scaffolding unlock when you arrive. This dossier is the full plan.

### `> cat why.txt`

| You'll understand | Because… |
|---|---|
| Policy gradients, PPO and GAE, derived | the algorithm behind nearly every sim-trained legged controller shipped in the last five years |
| Off-policy RL (SAC, TD3) | sample efficiency when data is expensive, as it always is on hardware |
| Model-based RL and MPPI | where learning meets Mark XII's optimal control |
| Sim-to-real (domain randomization, teacher–student, actuator models) | the gap between a policy that walks in sim and one that walks |
| BC, DAgger, ACT and diffusion policies | how manipulation is learned from demonstrations today |
| VLAs and cross-embodiment data | the frontier: language-conditioned generalist policies |
| Evaluation with confidence intervals | "8/10 successes" is not a result |

**Your advantage is large here, and so is the trap.** You know deep learning. Robotics adds the parts that ML benchmarks hide: non-stationary physics, partial observability, safety, 50 Hz real-time inference, and data that costs an hour of teleoperation per 50 demos.

### `> ./prereqs --check`

- [ ] Marks XI (dynamics, contact) and XII (optimal control, the legged stack): the model-based baselines you'll compare against
- [ ] PyTorch fluency (you have it), plus a CUDA GPU for training
- [ ] `pip install mujoco gymnasium torch`; plus MuJoCo Playground or Isaac Lab (parallel simulation), and LeRobot

### `> cat chapters` *(planned: written when you arrive)*

| # | Chapter | Core result |
|---|---|---|
| 00 | Robot learning: the landscape | MDPs and POMDPs for robots; when to learn vs model; what's hard (sample cost, safety, partial observability, contact) |
| 01 | Policy gradients, from scratch | the likelihood-ratio derivation; baselines; advantage estimation; **GAE derived**; trust regions → the PPO clipped surrogate; the implementation details that matter |
| 02 | Off-policy actor-critic | the deadly triad; target networks; TD3's fixes; **SAC from the maximum-entropy objective** |
| 03 | Model-based RL | learned dynamics with uncertainty (ensembles); MPPI; world models; the link to Mark XII |
| 04 | Legged RL at scale | massively parallel simulation; reward design; curricula; asymmetric actor-critic; privileged teacher → student distillation |
| 05 | Sim-to-real | domain and dynamics randomization; system ID; actuator networks; latency and observation noise; hardware evaluation |
| 06 | Imitation learning | BC as supervised learning; **compounding error, O(T²ε) vs DAgger's O(Tε)**, proved; interactive vs offline data |
| 07 | Generative policies | action chunking (ACT); **diffusion policy**: the DDPM objective, sampling, and why it handles multimodality; flow matching |
| 08 | Foundation models for robots | VLAs (RT-2, OpenVLA, π0); action tokenization; cross-embodiment data (Open X-Embodiment); fine-tuning |
| 09 | Evaluating learned policies | success rates with Wilson intervals; distribution-shift suites; benchmarks (LIBERO, ManiSkill); safe RL basics |

---

### `> cat syllabus`

| Week | Read | Build | Checkpoint |
|---|---|---|---|
| **115** | ch. 00–01 · Sutton & Barto ch. 13 · Schulman et al. (GAE, 2015) | `01-ppo`: REINFORCE → actor-critic with GAE (PyTorch, from scratch) | CartPole and Pendulum solved |
| **116** | ch. 01 (PPO) · Schulman et al. (PPO, 2017) · "The 37 implementation details of PPO" | PPO on MuJoCo HalfCheetah and Ant | within 10% of CleanRL's reported returns |
| **117** | ch. 02 · Haarnoja et al. (SAC, 2018) · Fujimoto et al. (TD3, 2018) | `02-sac` | SAC beats PPO's sample efficiency on HalfCheetah (plot it) |
| **118** | ch. 03 · Chua et al. (PETS, 2018) · Williams et al. (MPPI) | MPPI with a learned ensemble model on the cart-pole | the swing-up solved with less than 10% of PPO's samples |
| **119** | ch. 04 · Rudin et al. (2022) | `03-legged-rl`: a quadruped in parallel simulation | flat-ground walking in < 1 hour of training |
| **120** | ch. 04 (teacher–student) · Lee et al. (2020) · Kumar et al. (RMA, 2021) | a privileged teacher → a proprioceptive student | the student reaches ≥ 90% of the teacher on rough terrain |
| **121** | ch. 05 · Tobin et al. (2017) · Hwangbo et al. (2019) | `04-sim2real`: train with and without DR; test on held-out dynamics | robustness curves (friction, mass, latency) |
| **122** | ch. 05 | head-to-head: your RL policy vs your Mark XII MPC+WBC on the same pushes and terrain | an honest comparison table |
| **123** | ch. 06 · Ross, Gordon & Bagnell (DAgger, 2011) | `05-imitation`: BC vs DAgger, with your Mark XII controller as the expert | the compounding-error plot matches theory |
| **124** | ch. 07 · Zhao et al. (ACT, 2023) · Chi et al. (Diffusion Policy, 2023) | `06-generative`: ACT and diffusion policy on Push-T | success rates with 95% CIs |
| **125** | ch. 07 (flow matching) | diffusion vs flow-matching heads; inference-latency study | a latency/success trade-off plot |
| **126** | ch. 08 · RT-2, OpenVLA, π0 papers · Open X-Embodiment | fine-tune a small VLA (e.g. SmolVLA in LeRobot) on SO-101 or sim data | language-conditioned success on 2 tasks |
| **127** | ch. 09 | the evaluation suite: 50+ trials per condition, shift tests, CIs | the eval report |
| **128** | — | boss fight + write-up | Mark XIII post published |

### `> ls systems/` *(lab scaffolding unlocks when you arrive)*

| # | System | Exit criteria |
|---|---|---|
| 01 | `ppo`: from-scratch PPO + GAE | within 10% of CleanRL's published returns on HalfCheetah and Ant (3 seeds, reported with CIs) |
| 02 | `sac`: from-scratch SAC | matches CleanRL's SAC within 10%; a sample-efficiency comparison vs PPO |
| 03 | `legged-rl`: parallel-sim quadruped + teacher–student | walks on rough terrain; the student reaches ≥ 90% of the teacher's return |
| 04 | `sim2real`: randomization study | a DR policy that holds ≥ 80% success on held-out dynamics where the non-DR policy fails |
| 05 | `imitation`: BC vs DAgger | DAgger beats BC at equal expert-label budgets; the compounding-error scaling is plotted |
| 06 | `generative`: ACT, diffusion and flow policies (+ VLA fine-tune) | Push-T success with 95% Wilson CIs over ≥ 50 trials; the VLA completes 2 language-specified tasks |

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| No observation normalization | PPO "doesn't learn" | running mean/std normalization; it's in the implementation details for a reason |
| Reward hacking | the quadruped vibrates forward on its knees | penalize joint torques and accelerations and foot slip; watch videos, not just curves |
| Evaluating on the training distribution | "robust" policies fail on a new floor | held-out randomization ranges; report per-condition success |
| BC on multimodal demos with an MSE loss | the robot averages "go left" and "go right" into a collision | generative policies (diffusion, flow) or action chunking |
| 10 trials, 8 successes, called "80%" | a result that won't replicate | the 95% Wilson interval for 8/10 is about [0.49, 0.94]; run more trials |
| Ignoring inference latency | a policy that works offline and jitters online | measure end-to-end latency; action chunking and asynchronous inference |

### `> ./quiz`

<details><summary>1. Derive the policy-gradient estimator in one line.</summary>

$\nabla_\theta\,\mathbb E_{\tau\sim\pi_\theta}[R(\tau)] = \mathbb E\big[R(\tau)\,\nabla_\theta\log\pi_\theta(\tau)\big]$, and $\nabla\log\pi_\theta(\tau) = \sum_t\nabla\log\pi_\theta(a_t|s_t)$, because the dynamics terms don't depend on θ. That's why it's model-free.
</details>

<details><summary>2. Why does subtracting a baseline not bias the gradient?</summary>

$\mathbb E_{a\sim\pi}[b(s)\nabla\log\pi(a|s)] = b(s)\nabla\sum_a\pi(a|s) = b(s)\nabla 1 = 0$. It changes only the variance, and a good baseline (the value function) reduces it dramatically.
</details>

<details><summary>3. What does λ trade off in GAE?</summary>

Bias vs variance. λ = 0 gives the one-step TD advantage (low variance, biased by value errors); λ = 1 gives Monte Carlo returns minus a baseline (unbiased, high variance). GAE is an exponentially weighted average of k-step estimators.
</details>

<details><summary>4. Why does PPO clip the probability ratio?</summary>

It keeps each update inside an approximate trust region. The clipped surrogate removes the incentive to move the ratio $\pi_\theta/\pi_{\text{old}}$ outside $[1-\epsilon, 1+\epsilon]$, which prevents the destructive large policy updates that on-policy data can't correct.
</details>

<details><summary>5. What is the "deadly triad"?</summary>

Function approximation + bootstrapping + off-policy data together can make value learning diverge. Target networks, clipped double-Q (TD3/SAC) and conservative updates are the practical mitigations.
</details>

<details><summary>6. Why does behaviour cloning suffer compounding error, and how does DAgger fix it?</summary>

A BC policy trained on the expert's state distribution makes small errors that take it to states it never saw, where it errs more. The expected cost grows as O(T²ε) over horizon T. DAgger collects expert labels **on the learner's own state distribution** and aggregates them, which gives O(Tε).
</details>

<details><summary>7. Why do diffusion policies handle multimodal demonstrations better than MSE regression?</summary>

MSE regression predicts the conditional **mean**, and the mean of two valid modes is often invalid. A diffusion model learns the full conditional distribution and samples one mode at a time.
</details>

<details><summary>8. What is a privileged teacher, and why distil it into a student?</summary>

A teacher policy trained in simulation with access to privileged state (terrain heights, friction, true velocities) learns fast. A student that sees only real sensors learns to imitate it (and to infer the hidden state from history), which gives a deployable policy.
</details>

<details><summary>9. What's the cost of too much domain randomization?</summary>

Conservatism. The policy optimizes for the whole randomized distribution and becomes robust but timid, trading peak performance for coverage. Automatic or adaptive DR, and system ID to narrow the ranges, manage the trade-off.
</details>

### `> cat interview.txt`

- *"Derive PPO's objective. Which implementation details actually matter?"*
- *"How would you get an RL locomotion policy from sim to a real quadruped?"*
- *"BC vs DAgger vs diffusion policy: when would you use each?"*
- *"What's a VLA, and what are its failure modes?"*
- *"How many trials do you need to claim policy A beats policy B?"*
- *"When would you choose MPC over RL for a legged robot, and vice versa?"*

### `> cat boss_fight`

**"Extremis."** Two arenas, one evaluation standard:
1. **Locomotion:** your Mark XII quadruped learns to walk with *your* PPO in parallel simulation, with randomization and teacher–student distillation. Pit it against *your* MPC+WBC controller on the same pushes, payloads and terrain, and publish the table: success with CIs, speed, energy per metre, and the largest recoverable push.
2. **Manipulation:** collect 50 SO-101 teleop demos (or use sim). Train a diffusion policy and compare it against *your* Mark VI scripted pipeline over ≥ 50 trials, with CIs. Then fine-tune a small VLA and command it in language, which gives JARVIS (Mark IX) a learned skill.

### `> cat library`

| Source | Use |
|---|---|
| Sutton & Barto, *Reinforcement Learning: An Introduction* (2nd ed., free) | foundations; ch. 13 |
| Sergey Levine, CS285 *Deep Reinforcement Learning* (UC Berkeley, lectures online) | the course companion for ch. 01–03 and 06 |
| Schulman et al.: TRPO (2015), GAE (2015), PPO (2017) | ch. 01 |
| Huang et al., "The 37 implementation details of Proximal Policy Optimization" (ICLR blog track, 2022) + CleanRL | ch. 01, and the lab baselines |
| Haarnoja et al., SAC (2018) · Fujimoto et al., TD3 (2018) | ch. 02 |
| Chua et al., PETS (2018) · Williams et al., MPPI · Hafner et al., Dreamer | ch. 03 |
| Rudin et al., "Learning to walk in minutes using massively parallel deep RL" (CoRL) | ch. 04 |
| Lee et al., "Learning quadrupedal locomotion over challenging terrain" (Science Robotics, 2020) · Kumar et al., RMA (RSS 2021) | ch. 04 |
| Tobin et al., "Domain randomization…" (2017) · Hwangbo et al., "Learning agile and dynamic motor skills for legged robots" (Science Robotics, 2019) | ch. 05 |
| Ross & Bagnell (2010) · Ross, Gordon & Bagnell, DAgger (AISTATS 2011) | ch. 06 |
| Zhao et al., ACT (RSS 2023) · Chi et al., Diffusion Policy (RSS 2023) | ch. 07 |
| RT-2 (2023) · OpenVLA (2024) · π0 (2024) · Open X-Embodiment (2023) | ch. 08 |
| Hugging Face LeRobot (docs and course) · MuJoCo Playground / Isaac Lab | tools |

**Hardware:** a CUDA GPU (a single consumer card is enough for everything here). The SO-101 arm is optional (sim works).
