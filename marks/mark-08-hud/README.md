```text
 ███╗   ███╗██╗  ██╗    ██╗   ██╗██╗██╗██╗
 ████╗ ████║██║ ██╔╝    ██║   ██║██║██║██║    HUD
 ██╔████╔██║█████╔╝     ██║   ██║██║██║██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ╚██╗ ██╔╝██║██║██║    cuda · tensorrt · real-time perception · ar
 ██║ ╚═╝ ██║██║  ██╗     ╚████╔╝ ██║██║██║    weeks 59–66 · 4 systems · gpu
 ╚═╝     ╚═╝╚═╝  ╚═╝      ╚═══╝  ╚═╝╚═╝╚═╝
```

> The HUD is what makes the suit feel like magic: the world, annotated, with no perceptible lag.

### `> cat mission.txt`

Go below PyTorch. Write CUDA kernels by hand and measure them against cuBLAS with a roofline. Then build a real-time perception pipeline (detection, depth and tracking, fused with an IMU) that renders world-locked annotations at 30 FPS or more with under 50 ms glass-to-glass latency.

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| The GPU execution and memory model | Every model you deploy (IX, X) is bounded by this |
| Roofline analysis | You'll know *why* a kernel is slow before you touch it |
| Quantization and TensorRT | Edge robots can't afford FP32 or a datacenter GPU |
| Latency engineering end to end | JARVIS's voice loop and the robot's perception loop live or die by latency budgets |
| Tracking (Mark VII's Kalman filter again) | Identity over time is what turns detections into understanding |

**Your advantage:** you already train and serve models. This Mark goes *under* the framework to where performance is actually decided, then back up to a full real-time system.

### `> ./prereqs --check`

- [ ] Mark I (the cache/tiling manual), Mark III (concurrency) and Mark VII (Kalman filter, camera geometry)
- [ ] An NVIDIA GPU: a Jetson Orin Nano, or any laptop/desktop card with CUDA 12+
- [ ] A USB camera

### `> cat concept_map`

```text
GPU MODEL
├── grid → blocks → warps (32 threads, SIMT) → threads
├── memory: registers → shared memory (per block) → L2 → global (DRAM/HBM)
├── coalescing: a warp's adjacent threads should touch adjacent addresses
├── shared-memory bank conflicts · occupancy · warp divergence
├── patterns: map · tiled matmul · reduction · scan · warp shuffles
└── tools: Nsight Systems (timeline) · Nsight Compute (per kernel) · roofline

INFERENCE ENGINEERING
├── ONNX export → TensorRT engine · FP16 · INT8 (PTQ calibration)
├── layer fusion · dynamic shapes · CUDA streams · pinned memory
└── latency vs throughput: batching helps one, and hurts the other

REAL-TIME PIPELINE
├── capture → GPU preprocess → detect → NMS → track (SORT: KF + Hungarian)
├── depth (stereo or monocular network) → 3D positions
├── pose (AprilTags → VIO) → project world points into the image (pinhole)
└── budget: every stage timed · glass-to-glass measured, not estimated
```

---

### `> cat syllabus`

| Week | Theory | Build | Checkpoint |
|---|---|---|---|
| **59** | PMPP ch. 1–3 | `01-kernels`: vector add; measure the bandwidth you achieve vs the peak | ≥ 70% of peak DRAM bandwidth |
| **60** | PMPP ch. 4–5 (compute architecture, memory) | naive → shared-memory tiled SGEMM; place both on a roofline | tiled ≥ 5× naive |
| **61** | PMPP ch. 6 (performance) + ch. 10 (reduction) · Nsight Compute | register tiling, vectorized loads; softmax and layernorm with warp shuffles | SGEMM **≥ 50% of cuBLAS** at n = 4096 |
| **62** | TensorRT developer guide (INT8 calibration) | `02-engine`: detector → ONNX → TensorRT FP16 → INT8 | INT8 mAP within 1 point of FP32; latency table |
| **63** | SORT paper (Bewley et al., 2016) | `03-pipeline`: capture, preprocessing, detection, NMS, SORT tracking (reuse your KF) | stable track IDs through occlusions |
| **64** | stereo / monocular depth · pinhole back-projection | depth fusion → 3D positions for tracked objects | distance error < 10% at 1–3 m |
| **65** | AprilTag pose · (VIO overview) | `04-hud`: world-locked labels from camera pose; render the overlay | labels stay on objects while the camera moves |
| **66** | latency measurement methodology | glass-to-glass test (film an LED and the screen together with a high-FPS phone); boss fight | Mark VIII post published |

### `> ls systems/`

| # | System | Exit criteria |
|---|---|---|
| 01 | `kernels`: vector add, SGEMM, reduction, softmax, layernorm | SGEMM ≥ 50% of cuBLAS (n = 4096); memory-bound kernels ≥ 70% of peak bandwidth; every kernel on a roofline plot |
| 02 | `engine`: TensorRT FP16/INT8 | INT8 within 1 mAP of FP32; p50/p99 latency reported |
| 03 | `pipeline`: detection + depth + tracking | ≥ 30 FPS sustained; IDs stable through 1 s occlusions |
| 04 | `hud`: world-locked AR overlay | **< 50 ms glass-to-glass**, measured |

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| Uncoalesced access | 10× slower than expected | make `threadIdx.x` index the contiguous dimension |
| Timing kernels with CPU timers | nonsense numbers | CUDA events, or Nsight; remember that launches are async |
| Bank conflicts in the shared-memory transpose | a mysterious 2–32× slowdown | pad shared arrays (`[32][33]`) |
| Calibrating INT8 on the wrong data | accuracy collapses | calibrate on representative frames from *your* camera |
| Measuring FPS instead of latency | "60 FPS" but it feels laggy | pipelining hides latency in throughput; measure glass-to-glass |
| CPU pre/postprocessing | the GPU sits idle 60% of the time | move resize/normalize to the GPU; overlap with streams |

### `> ./quiz`

<details><summary>1. What does memory coalescing mean, and why does it matter?</summary>

When the 32 threads of a warp access consecutive addresses, the hardware serves them with a few wide memory transactions. Scattered accesses need many transactions. Global memory is the bottleneck for most kernels, so coalescing is often the single largest factor in performance.
</details>

<details><summary>2. How do you know whether a kernel is memory-bound or compute-bound?</summary>

Compute its **arithmetic intensity** (FLOPs per byte moved) and compare it with the machine's ridge point (peak FLOP/s ÷ peak bandwidth). Below the ridge it's memory-bound: optimize data movement. Above it, it's compute-bound: optimize instruction throughput. Nsight Compute's roofline view shows this directly.
</details>

<details><summary>3. Why does tiling with shared memory speed up matmul?</summary>

Each element loaded from global memory into a shared tile is reused by many threads (TILE times), which raises effective arithmetic intensity. It's the same idea as Mark I's cache blocking, with a software-managed cache.
</details>

<details><summary>4. What is warp divergence?</summary>

Threads in a warp execute in lockstep. If they take different branches, the warp runs *both* paths serially with some threads masked off. Keep branching coherent within warps.
</details>

<details><summary>5. How does INT8 post-training quantization work, and why calibrate?</summary>

Weights and activations are mapped to 8-bit integers with per-tensor or per-channel scales. Calibration runs representative data through the network to choose activation ranges (for example by minimizing KL divergence or taking percentiles). Bad ranges clip important values or waste resolution.
</details>

<details><summary>6. Why does batching improve throughput but hurt latency?</summary>

Larger batches use the GPU more efficiently, so throughput rises, but each frame waits for its batch to fill and then for the whole batch to finish, so latency rises. Real-time perception usually runs batch 1 and overlaps pipeline stages instead.
</details>

<details><summary>7. What does a tracker add on top of a detector?</summary>

Identity over time, smoothing of noisy boxes, and prediction through missed detections and occlusions. SORT does this with a Kalman filter per track, plus Hungarian matching of predictions to detections.
</details>

### `> cat interview.txt`

- *"This kernel is slow. Walk me through how you'd optimize it."*
- *"Explain the roofline model."*
- *"How does INT8 quantization affect accuracy, and how do you control it?"*
- *"Design a perception pipeline with a 50 ms budget on a Jetson."*
- *"FPS is fine but users say it lags. What's going on?"*

### `> cat boss_fight`

**"Heads-up display."** A live camera feed, on the Jetson, with:
- tracked objects carrying stable IDs and **distances in metres**
- labels **locked to the world** that stay put as you walk around
- a matrix-green telemetry panel in the corner showing FPS, per-stage latency and GPU usage

Film it. Put the glass-to-glass measurement in the lab note.

### `> cat library`

| Source | Use |
|---|---|
| Hwu, Kirk & El Hajj, *Programming Massively Parallel Processors* (4th ed.) | The spine: ch. 1–6 and 10 |
| NVIDIA *CUDA C++ Programming Guide* · *Best Practices Guide* · Nsight docs | Reference |
| Stanford CS149 (lectures online) | Parallel-computing theory |
| NVIDIA TensorRT developer guide | Week 62 |
| Bewley et al., *Simple Online and Realtime Tracking* (2016) | Week 63 |
| Hartley & Zisserman, ch. 6 (camera models) | Weeks 64–65 |

**Hardware:** a Jetson Orin Nano (about $250), or any CUDA GPU you have, plus a USB camera.
