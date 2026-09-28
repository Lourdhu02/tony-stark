```text
 ███╗   ███╗██╗  ██╗    ██╗██╗   ██╗
 ████╗ ████║██║ ██╔╝    ██║██║   ██║    ARC REACTOR
 ██╔████╔██║█████╔╝     ██║██║   ██║    ─────────────────────────────────────
 ██║╚██╔╝██║██╔═██╗     ██║╚██╗ ██╔╝    circuits · bare metal · rtos · motors · pcb
 ██║ ╚═╝ ██║██║  ██╗    ██║ ╚████╔╝     weeks 23–30 · 5 systems · hardware
 ╚═╝     ╚═╝╚═╝  ╚═╝    ╚═╝  ╚═══╝
```

> *"Proof that Tony Stark has a heart."* Every suit needs a power core, and every robot needs firmware.

### `> cat mission.txt`

Leave the simulator. Blink an LED by writing registers straight from the datasheet, write your own UART and I2C drivers, fuse an IMU, close a PID loop on a real motor at 1 kHz with measured jitter, and design the PCB it all lives on.

### `> cat why.txt`

| You'll learn | Because later… |
|---|---|
| Reading datasheets and reference manuals | Every sensor and actuator in VI–X is a datasheet first |
| Interrupts, timers, DMA and an RTOS | Mark V's controllers only work if they run *on time*; this is where timing becomes real |
| Sensor fusion on real, noisy data | Mark VII's filters are this complementary filter grown up |
| Motors, drivers and PWM | Every robot joint is a motor, a driver and a feedback loop |
| Schematics, layout and bring-up | You can build hardware, not just buy it: the Stark difference |

### `> ./prereqs --check`

- [ ] Mark I (C, pointers, bit manipulation) and Mark III (interrupts, concurrency)
- [ ] The **Mark IV kit** from [HARDWARE.md](../../HARDWARE.md) (about $150)
- [ ] Read the **safety** section below before powering anything

### `> cat concept_map`

```text
ELECTRONICS
├── Ohm's law · KVL/KCL · voltage dividers · RC time constants
├── pull-up/pull-down · open-drain · decoupling capacitors
├── MOSFETs as switches · H-bridges · flyback diodes (inductive kick)
└── power: LDO vs buck converter · current budget · LiPo safety

MCU (STM32F446 as the reference)
├── boot: vector table → Reset_Handler → copy .data, zero .bss → main
├── clocks: RCC enables every peripheral (forget it → silent failure)
├── GPIO: MODER · ODR · BSRR (atomic set/reset) · alternate functions
├── interrupts: NVIC · priorities · ISR discipline (short, no blocking)
├── timers: PWM · input capture · encoder mode · SysTick
├── buses: UART · I2C (addresses, ACK/NACK) · SPI · DMA
└── debug: SWD · OpenOCD/probe-rs · GDB · logic analyzer · printf-over-UART

FIRMWARE ARCHITECTURE
├── superloop → interrupts + flags → RTOS tasks + queues
├── timing: rate-monotonic priorities · jitter · watchdogs
└── FreeRTOS (C) or Embassy (async Rust)

PCB
└── schematic → footprints → layout (ground plane!) → DRC → Gerbers → fab → bring-up
```

---

### `> cat syllabus`

| Week | Theory | Build | Checkpoint |
|---|---|---|---|
| **23** | *Practical Electronics* ch. 2 · *Making Embedded Systems* ch. 1–3 | bench lab: dividers, RC charging, LED current; measure everything with the multimeter | the measured RC time constant is within 10% of theory |
| **24** | RM0390: RCC and GPIO chapters · linker scripts and startup code | `01-bare-metal`: blink with **no HAL**, your own linker script and vector table | LED at exactly 1 Hz, verified with the logic analyzer |
| **25** | RM0390: USART · the NVIC · SysTick | `01-bare-metal`: interrupt-driven UART driver, `printf` retarget, 1 ms tick | `hello @ 115200` + an echo terminal |
| **26** | the I2C spec (NXP UM10204, §3) · IMU datasheet · Madgwick (2010) | `02-imu`: an I2C driver from registers; complementary filter, then Madgwick | tilt within **±2°** of a protractor at 0/30/60° |
| **27** | timers (PWM + encoder mode) · H-bridge basics | `03-motor`: PWM → TB6612, quadrature encoder → RPM, PID at 1 kHz | step-response plot logged over UART |
| **28** | *Mastering the FreeRTOS Real Time Kernel* ch. 1–6 | `04-rtos`: sensor, control and telemetry tasks, with queues | control-loop jitter **< 10 µs p99** (GPIO toggle + logic analyzer) |
| **29** | Phil's Lab KiCad series | `05-pcb`: a Nucleo shield with the IMU, a motor driver, a status LED and test points | DRC clean; order placed |
| **30** | bring-up checklist | log motor data for Mark V system ID; boss fight when the board arrives | Mark IV post published |

### `> ls systems/`

| # | System | Exit criteria |
|---|---|---|
| 01 | `bare-metal`: startup, linker script, GPIO, UART, SysTick | builds with `-nostdlib` and no vendor HAL; UART echo at 115200 |
| 02 | `imu`: I2C driver, complementary + Madgwick filters | ±2° static accuracy; ≤ 1°/min drift at rest |
| 03 | `motor`: PWM, encoder, PID | holds commanded RPM **±5% under load**; no integrator windup on saturation |
| 04 | `rtos`: tasks, queues, timing | 1 kHz control with p99 jitter < 10 µs; no priority inversion (use mutex priority inheritance) |
| 05 | `pcb`: KiCad shield | the board is fabricated, and passes power-on, I2C and motor tests |

### `> cat safety.txt`

- **LiPo batteries:** use a balance charger only, charge in a fire-safe bag, never puncture, short or over-discharge (keep above 3.0 V per cell), and store at about 3.8 V per cell.
- **Motors:** they spin up unexpectedly when firmware crashes. Clamp or secure them, and keep fingers and hair clear during tests.
- **Current limits:** set the bench supply's current limit *before* connecting anything new, then raise it slowly.
- **Inductive kick:** motors and relays need flyback protection. Most H-bridge ICs include it, but check the datasheet.
- **Soldering:** ventilate, use a stand, and wash your hands (leaded solder). Never solder a powered board.
- **Mains:** this lab never touches mains voltage. Use certified adapters only.

### `> cat pitfalls.txt`

| Trap | Symptom | Fix |
|---|---|---|
| Peripheral clock not enabled | register writes "do nothing" | set the RCC enable bit first, *always* |
| Missing `volatile` on MMIO | works at `-O0`, breaks at `-O2` | every register access goes through `volatile` pointers |
| No I2C pull-ups | the bus is stuck low, or every transaction NACKs | 4.7 kΩ to 3.3 V on SDA and SCL (breakout boards often include them) |
| Heavy work in the ISR | missed interrupts, jitter | set a flag or push to a queue; do the work in a task |
| Derivative on noisy RPM | a chattering motor | low-pass the D term, or take the derivative on the measurement |
| Ground loops and no common ground | nonsense readings | every board shares ground with the logic analyzer and the supply |

### `> ./quiz`

<details><summary>1. Why does I2C need pull-up resistors?</summary>

I2C lines are **open-drain**: devices can only pull them low. The pull-ups return the lines high. That's what lets several devices share a wire safely, and it's how clock stretching and ACK work.
</details>

<details><summary>2. Why is writing to BSRR better than read-modify-write on ODR?</summary>

A BSRR write sets or resets specific pins **atomically** in a single store. With read-modify-write on ODR, an interrupt between the read and the write can change other pins, and your write then silently undoes the ISR's change.
</details>

<details><summary>3. What does a complementary filter do, and why does it work?</summary>

It blends a **high-passed gyro integral** (accurate short-term, but drifts) with a **low-passed accelerometer angle** (noisy, but correct on average): θ = α(θ + ω·dt) + (1 − α)·θ_accel. Each sensor covers the other's weakness. It's a steady-state Kalman filter in disguise.
</details>

<details><summary>4. Why does a DC motor respond to PWM as if it were a smooth voltage?</summary>

The motor's winding inductance and its mechanical inertia low-pass filter the switching. At a PWM frequency well above those time constants (typically 20 kHz or more, which is also inaudible), the motor sees the **average** voltage, which is duty × supply.
</details>

<details><summary>5. How does a quadrature encoder give you direction?</summary>

It has two channels, A and B, 90° out of phase. Which one leads tells you the direction. Counting every edge of both gives 4× resolution. STM32 timers decode this in hardware (encoder mode).
</details>

<details><summary>6. Your control loop runs at "1 kHz" but jitters by ±200 µs. Why does that matter?</summary>

The controller's math assumes a fixed dt. Jitter means the actual dt varies, so the D term and the integral are computed wrong, which effectively adds noise and phase lag. At high gains that can destabilize the loop. Timing is part of the controller.
</details>

<details><summary>7. What happens between reset and <code>main()</code> on a Cortex-M?</summary>

The CPU loads the initial stack pointer from vector-table word 0 and the PC from word 1 (`Reset_Handler`). The startup code copies `.data` from flash to RAM, zeroes `.bss`, optionally configures clocks and the FPU, then calls `main`.
</details>

<details><summary>8. What is priority inheritance?</summary>

When a high-priority task blocks on a mutex held by a low-priority task, the holder temporarily **inherits the high priority**, so medium-priority tasks can't preempt it. This prevents unbounded priority inversion (see Mark III, question 3).
</details>

### `> cat interview.txt`

- *"Walk me through what happens from reset to `main()` on a microcontroller."*
- *"The I2C sensor returns NACK. How do you debug it?"*
- *"Why do we need `volatile`, and when is it *not* enough?"*
- *"Design the firmware architecture for a 1 kHz control loop with telemetry."*
- *"Polling vs interrupts vs DMA: when do you use each?"*

### `> cat boss_fight`

**"The Reactor."** Your own PCB, running your own firmware:
- Set a motor RPM live over UART, and it holds within ±5% while you load the shaft with your fingers.
- IMU tilt streams to your laptop at 50 Hz.
- A logic-analyzer capture shows the control task firing every 1.000 ms ± 10 µs.
- You log a step response for Mark V's system identification.

### `> cat library`

| Source | Use |
|---|---|
| Elecia White, *Making Embedded Systems* (2nd ed.) | The embedded engineer's mindset |
| Scherz & Monk, *Practical Electronics for Inventors* | Circuits you'll actually wire |
| ST RM0390 (STM32F446 reference manual) + the datasheet + UM1724 (Nucleo-64 user manual) | Your real textbook for weeks 24–27 |
| NXP UM10204, *I2C-bus specification and user manual* | Week 26 |
| Barry, *Mastering the FreeRTOS Real Time Kernel* (free) | Week 28 |
| Madgwick, *An efficient orientation filter for inertial and inertial/magnetic sensor arrays* (2010) | Week 26 |
| Phil's Lab (YouTube) | KiCad, week 29 |
| Horowitz & Hill, *The Art of Electronics* | Reference, forever |

**Hardware:** the Mark IV kit in [HARDWARE.md](../../HARDWARE.md).
