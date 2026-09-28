```text
 ███╗   ███╗██╗  ██╗    ██╗  ██╗
 ████╗ ████║██║ ██╔╝    ╚██╗██╔╝    SUIT UP
 ██╔████╔██║█████╔╝      ╚███╔╝     ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗      ██╔██╗     integration · learned policies · the capstone
 ██║ ╚═╝ ██║██║  ██╗    ██╔╝ ██╗    week 77+ · 1 flagship
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝  ╚═╝
```

> Every Mark before this was a component. This one is the suit.

### `> cat mission.txt`

Pick **one** capstone, write a real design doc, and build it to a measurable standard. You'll integrate at least three earlier Marks, and you'll likely add learned policies (imitation learning, RL or a vision-language-action model) where hand-written control runs out.

### `> cat why.txt`

The skill tree converges here. Integration is its own skill, and it's the one most engineers never practise:
- interfaces between subsystems
- failure handling across them
- latency budgets that span the whole stack
- evaluation of the *system*, not of the parts

This capstone is your portfolio centrepiece and your strongest interview story.

### `> ./prereqs --check`

- [ ] At least Marks I–VII complete, plus the Marks your chosen track needs (below)
- [ ] A self-assessment re-score: know which domains you're strengthening

---

### `> ls tracks/`

#### Track A · Mobile manipulator: "fetch"

```text
JARVIS (IX) ─▶ task plan ─▶ Nav2 + your SLAM (VII) ─▶ go to room
                         └─▶ HUD perception (VIII) ─▶ find object ─▶ DUM-E pick (VI)
                         └─▶ return · hand over · report
```

- **Needs:** VI, VII, VIII, IX. Sim-first (Gazebo or Isaac Sim); hardware optional.
- **Exit:** "fetch the red cup from the kitchen" succeeds in **≥ 80% of 20 randomized sim trials**, with a Wilson 95% confidence interval reported. Runs 30 minutes without human intervention.

#### Track B · Learned manipulation: "teach, don't code"

```text
teleoperate (SO-101 leader → follower) ─▶ 50 demos ─▶ train ACT and a diffusion policy (LeRobot)
     ─▶ evaluate vs your Mark VI scripted pipeline ─▶ fine-tune a small VLA ─▶ language-conditioned tasks
```

- **Needs:** VI (and the SO-101 arm pair), VIII. Your ML strength makes this the fastest track.
- **Exit:** the learned policy **matches or beats** the scripted Mark VI pipeline on 3 tasks over 20 trials each. The generalization gap (seen vs unseen object positions) is reported with CIs.

#### Track C · Wearable: "the suit, literally"

```text
IMU glove or EMG band (IV) ─▶ on-device gesture model (TinyML, int8) ─▶ BLE
     ─▶ DUM-E teleop (VI) with haptic feedback · state shown on the HUD (VIII)
```

- **Needs:** IV, V, VI, VIII.
- **Exit:** ≥ 95% gesture accuracy on held-out users; control latency < 100 ms end to end; the arm follows gestures for a 5-minute task.

### `> cat learned_policies.txt`

Where hand-written control runs out, learning takes over. The map:

| Approach | Idea | Key reference |
|---|---|---|
| Behaviour cloning / **ACT** | Supervised learning from demonstrations; predicts action *chunks* with a transformer | Zhao et al., *Learning Fine-Grained Bimanual Manipulation with Low-Cost Hardware* (2023) |
| **Diffusion Policy** | A denoising diffusion model over action sequences; handles multimodal demos | Chi et al., *Diffusion Policy* (2023) |
| **VLA models** | A vision-language backbone fine-tuned to output actions; language-conditioned | OpenVLA (2024) · π0 (Physical Intelligence, 2024) · SmolVLA (Hugging Face, 2025) |
| **RL + sim-to-real** | Train in a massively parallel sim, transfer with domain randomization | Schulman et al., PPO (2017) · Tobin et al., *Domain Randomization* (2017) |

### `> cat design_doc_template`

Your capstone starts with this document, written *before* any code:

```text
1. Problem & success metric      what "done" means, as a number, with trials and CI
2. System diagram                every subsystem, every interface (topics, rates, types)
3. Latency & compute budget      per stage, end to end
4. Failure modes & mitigations   a risk register; what the safety layer enforces
5. Evaluation protocol           randomization, trial count, stats, baselines
6. Milestones                    2-week slices, each ending in a demo
7. What I'll learn               which self-assessment domains move, and to what
```

### `> cat boss_fight`

**The final demo.** A single unedited video of the capstone meeting its exit metric, a public write-up with the design doc and eval results, and a 20-minute talk (for a meetup, or recorded). Then re-score the [self-assessment](../../docs/self-assessment.md). The target is **7/10 or more**.

### `> cat library`

| Source | Use |
|---|---|
| Hugging Face LeRobot docs and course | Tracks A and B: datasets, ACT, diffusion, VLA fine-tuning |
| Tedrake, *Underactuated Robotics*: the RL and imitation chapters | Background |
| Sutton & Barto, ch. 13 (policy gradient methods) | RL refresher |
| The papers in the table above | Read in full, and reproduce one figure |
| Isaac Lab / MuJoCo Playground | Massively parallel sim for RL |
