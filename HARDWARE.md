# Hardware

The plan is simulation first: **Marks I–III, V and VII need no hardware.** Buy each kit only when you reach its Mark.
Prices are rough USD estimates and vary by region and supplier.

| When | Item | Why | ~Cost |
|---|---|---|---|
| Mark IV | STM32 Nucleo board (e.g. F446RE) | bare-metal and RTOS work, with an on-board debugger | $15–25 |
| Mark IV | ESP32-S3 dev board | Wi-Fi/BLE telemetry, later MQTT for JARVIS | $10–15 |
| Mark IV | IMU (BNO085 or MPU-6050) | I2C driver, sensor fusion | $5–25 |
| Mark IV | DC gear motor with encoder, plus a TB6612FNG driver | PID speed control | $15–25 |
| Mark IV | Breadboard kit, jumpers, resistors, capacitors | prototyping | $15–25 |
| Mark IV | Multimeter, soldering iron, USB logic analyzer | debugging real hardware | $40–70 |
| Mark IV | PCB fabrication (JLCPCB, PCBWay, …) | your first board | $10–30 |
| Mark VI | SO-101 arm kit (LeRobot open-source arm) | real pick-and-place, imitation learning | $150–300 |
| Mark VI | USB camera | vision for the arm | $20–40 |
| Mark VIII | Jetson Orin Nano dev kit (or use a laptop GPU) | edge inference, the HUD | about $250 |
| Mark IX | USB microphone and speaker | voice interface | $20–40 |
| Optional | 3D printer | mounts, enclosures, grippers | $200–400 |

**Minimum spend to finish the core path:** Mark IV kit (about $150).
Everything after that can stay in simulation.
