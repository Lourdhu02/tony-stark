# Hardware

The plan is simulation first: **only Mark IV requires hardware.** Buy each kit when you reach its Mark, not before.
Prices are rough USD estimates and vary by region and supplier.

## Shopping list

| When | Item | Why | ~Cost |
|---|---|---|---|
| Mark II *(optional)* | iCE40 or Gowin FPGA board | run your Verilog CPU on real silicon | $25–70 |
| **Mark IV** | STM32 Nucleo board (e.g. F446RE) | bare-metal and RTOS work, with an on-board ST-LINK debugger | $15–25 |
| **Mark IV** | IMU breakout (BNO085 or MPU-6050) | I2C driver, sensor fusion | $5–25 |
| **Mark IV** | DC gear motor with quadrature encoder + TB6612FNG driver | PWM, encoders, PID speed control | $15–25 |
| **Mark IV** | Breadboard kit, jumpers, resistors, capacitors, LEDs | prototyping | $15–25 |
| **Mark IV** | Multimeter + temperature-controlled soldering iron + USB logic analyzer (8 ch, 24 MHz) | seeing what your hardware actually does | $40–70 |
| **Mark IV** | PCB fabrication (JLCPCB, PCBWay, …) | your first board | $10–30 |
| Mark IV *(recommended)* | Bench power supply with a current limit | the single best way not to destroy things | $40–80 |
| Mark IV *(optional)* | ESP32-S3 dev board | Wi-Fi/BLE telemetry; MQTT devices for JARVIS | $10–15 |
| Mark VI *(optional)* | SO-101 arm kit, follower (add the leader arm for teleop in Mark X) | real pick-and-place, imitation learning | $150–300 per arm |
| Mark VI | USB camera (global shutter if you can afford it) | vision for the arm | $20–60 |
| Mark VII *(optional)* | 2D lidar (e.g. RPLIDAR A1) | map your own room | $70–100 |
| Mark VIII | Jetson Orin Nano dev kit, or any CUDA GPU you already have | edge inference, the HUD | about $250 |
| Mark IX | USB microphone + speaker | the voice interface | $20–40 |
| Optional | 3D printer | mounts, enclosures, grippers | $200–400 |

**Minimum to finish the core path:** the Mark IV kit, about **$150**. Everything after that can stay in simulation.

## Lab setup

```text
desk
├── an ESD mat + wrist strap           static kills MCUs silently
├── a fume extractor or open window     when soldering
├── a parts organizer, labelled         you'll lose the 4.7 kΩ resistors otherwise
├── a USB hub with per-port switches    power-cycle boards without unplugging
└── a notebook                          wiring diagrams, pin maps, measured values
```

## Software toolchain by Mark

| Mark | Install |
|---|---|
| I | `gcc`, `make`, `gdb`, `valgrind`, `python3` (all standard on Linux or WSL2) |
| II | Rust (`rustup`) · nand2tetris tools (Java) · `riscv64-unknown-elf-gcc` · *(optional)* Verilator |
| III | `qemu-system-riscv64` · Rust target `riscv64gc-unknown-none-elf` · `loom` · the xv6 toolchain |
| IV | `arm-none-eabi-gcc` · OpenOCD or probe-rs · KiCad · PulseView (logic analyzer) · *(optional)* FreeRTOS / Embassy |
| V | Python: NumPy, SciPy, Matplotlib, OSQP |
| VI | ROS 2 Jazzy (Ubuntu 24.04) · MoveIt 2 · MuJoCo · OpenCV |
| VII | Gazebo · Nav2 · SLAM Toolbox · GTSAM · the KITTI odometry dataset |
| VIII | CUDA 12+ · Nsight Systems and Compute · TensorRT · ONNX |
| IX | faster-whisper · openWakeWord · Silero VAD · Piper · Mosquitto · an LLM with tool use |
| XI | Python 3.12 · NumPy · pytest (`pip install -r marks/mark-11-blueprint/lab/requirements.txt`) · *(optional)* Pinocchio for cross-checks |
| XII | MuJoCo + MuJoCo Menagerie · OSQP · *(optional)* Crocoddyl, acados |
| XIII | PyTorch (CUDA) · Gymnasium · MuJoCo Playground or Isaac Lab · LeRobot |

## Safety

These rules are general; Mark IV's dossier has the [detailed list](marks/mark-04-arc-reactor/README.md).

- **Current-limit everything new.** Set the bench supply's limit before connecting a circuit for the first time.
- **LiPo batteries:** use a balance charger, charge in a fire-safe bag, never short or puncture one, and never leave one charging unattended.
- **Moving machines:** clamp motors, keep the arm's workspace clear, and have a physical off switch within reach. Software e-stops are for convenience; the power switch is for safety.
- **Robot arm + AI (Mark IX):** reduce speed and torque limits during development, and never stand inside the workspace while an agent is in control.
- **No mains work.** Use certified power adapters only.
