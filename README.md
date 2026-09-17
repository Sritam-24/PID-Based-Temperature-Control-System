# 🌡️ PID-Based Temperature Control System

![Project Cover](images/project-cover.jpg)

A closed-loop **temperature control system** built with an **Arduino UNO**, an analog **temperature sensor**, an **IRF520 MOSFET module**, and a **12V DC bulb**, regulated by a software **PID (Proportional–Integral–Derivative) controller**. The system is backed by complete **MATLAB and Simulink analysis** covering open-loop/closed-loop behavior, controller design, and performance evaluation.

---

## 📑 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Hardware Components](#-hardware-components)
- [Software Used](#-software-used)
- [Circuit Diagram & Wiring](#-circuit-diagram--wiring)
- [Working Principle](#-working-principle)
- [Control Strategy](#-control-strategy)
- [Plant Model](#-plant-model)
- [MATLAB Analysis](#-matlab-analysis)
- [Simulink Model](#-simulink-model)
- [Repository Structure](#-repository-structure)
- [Getting Started](#-getting-started)
- [Tuning the PID Gains](#-tuning-the-pid-gains)
- [Future Improvements](#-future-improvements)
- [Learning Outcomes](#-learning-outcomes)
- [Authors](#-authors)

---

## 📖 Overview

This project implements a **closed-loop temperature control system**. The objective is to hold the temperature near a **halogen/DC bulb** at a fixed setpoint by continuously adjusting how brightly the bulb burns.

Unlike simple **ON/OFF (bang-bang) control** — where a heater is either fully on or fully off — this system keeps the bulb **always on** and instead varies its **brightness using PWM (Pulse Width Modulation)**. A temperature sensor feeds real-time readings back to the Arduino, which runs a PID control loop to compute exactly how much power the bulb should receive at every instant. This produces a much smoother, more stable, and more accurate temperature response than ON/OFF control.

The repository also includes full **MATLAB/Simulink modeling** of the plant, so the controller's design isn't just guesswork — it's grounded in transfer-function analysis, pole-zero placement, and step-response comparisons between the open-loop and closed-loop systems.

---

## ✨ Features

- Closed-loop (feedback) temperature control
- PID controller implemented directly in Arduino C++
- Continuous PWM brightness control (no relay clicking / no ON-OFF flicker)
- Real-time serial monitor output for live tuning and debugging
- Anti-windup protection on the integral term
- First-order plant model derived and analyzed in MATLAB
- Simulink block-diagram simulation of the full control loop
- Open-loop vs. closed-loop step-response comparison
- Pole-zero and stability analysis

---

## 🔩 Hardware Components

| Component | Purpose |
|---|---|
| Arduino UNO | Microcontroller running the PID control loop |
| Analog temperature sensor (LM35 / DHT11) | Measures the bulb's temperature |
| IRF520 MOSFET module | Switches/PWM-drives the high-current bulb load from a low-current Arduino pin |
| 12V DC Bulb | The "heating" actuator/plant being controlled |
| 12V DC Power Supply | Powers the bulb through the MOSFET |
| Breadboard | Prototyping platform |
| Jumper Wires | Connections between modules |
| Resistor (gate resistor) | Protects the MOSFET gate, limits inrush current |

> **Note:** The original circuit diagram (below) was drawn using a DHT11 module, while the firmware and write-up reference an LM35 analog sensor. Both use the same basic wiring idea (VCC, GND, and one signal pin into an Arduino analog/digital pin) — if you swap sensors, make sure the reading/conversion code in the `.ino` file matches the sensor you actually use.

---

## 💻 Software Used

- **Arduino IDE** – for writing and uploading the firmware
- **MATLAB** – for transfer function, step response, and pole-zero analysis
- **Simulink** – for block-diagram simulation of the closed-loop system
- **Control System Toolbox** – for PID design and performance evaluation

---

## 🔌 Circuit Diagram & Wiring

![Circuit Diagram](images/circuit-diagram.png)

| Wire Color | Connection |
|---|---|
| 🔴 Red | +5V (Arduino) / +12V (bulb supply) |
| ⚫ Black | GND |
| 🔵 Blue | Arduino Digital Pin **D9 (PWM)** → MOSFET Gate (through resistor) |
| 🟢 Green | Arduino Analog Pin **A0** → Sensor Data Pin |

**MOSFET (IRF520) Pinout**

| Pin | Function |
|---|---|
| G | Gate — receives the PWM signal from D9 |
| D | Drain — connects to the bulb's negative terminal |
| S | Source — connects to GND |

**How it's wired, in short:**
1. Sensor `VCC`/`GND` → Arduino `5V`/`GND`; sensor `DATA` → Arduino `A0`.
2. Arduino `D9` (PWM) → gate resistor → MOSFET `Gate`.
3. MOSFET `Source` → common `GND` rail.
4. MOSFET `Drain` → bulb's negative terminal.
5. Bulb's positive terminal → `+12V` supply; supply `GND` → common `GND` rail.
6. Arduino and 12V supply share a common ground (critical for the PWM signal to control the MOSFET correctly).

---

## ⚙️ Working Principle

1. The sensor continuously measures the temperature near the bulb.
2. The Arduino reads the analog voltage and converts it into a temperature value.
3. The measured temperature is compared against the desired **setpoint**.
4. The **PID controller** computes a control output based on the error, its accumulated history (integral), and its rate of change (derivative).
5. The Arduino converts that output into a **PWM signal** (0–255).
6. The PWM signal drives the **IRF520 MOSFET**, which regulates how much current reaches the bulb.
7. As bulb brightness changes, temperature changes — and the loop repeats continuously, driving the temperature toward the setpoint and holding it there.

This is a complete **closed-loop feedback control system**: the output (temperature) is measured and fed back to correct the input (bulb power).

---

## 🎛️ Control Strategy

**Setpoint Temperature:** `32°C`

**Error calculation:**

```
Error = Setpoint − Measured Temperature
```

**PID output:**

```
Output = Kp·Error + Ki·∫Error·dt + Kd·(dError/dt)
```

The controller gains used in the firmware:

| Gain | Value | Role |
|---|---|---|
| `Kp` | 15.0 | Reacts proportionally to the current error |
| `Ki` | 0.30 | Eliminates steady-state error by accumulating past error over time |
| `Kd` | 1.00 | Dampens overshoot by reacting to the rate of change of error |

The output is **clamped between 30 and 255** so the bulb never goes fully dark (minimum brightness) and never exceeds the Arduino's 8-bit PWM ceiling. The integral term is also **clamped (anti-windup)** between −100 and 100 to prevent it from growing unbounded while the system is far from the setpoint.

---

## 🧮 Plant Model

The temperature dynamics of the bulb system are approximated as a **first-order transfer function**:

```
G(s) = 1 / (10s + 1)
```

This model — a single pole at `s = −0.1`, time constant `τ = 10s` — is what all the MATLAB and Simulink analysis below is built on.

---

## 📊 MATLAB Analysis

The following analyses were performed on the plant model and the designed PID controller:

- Open-loop step response
- Closed-loop step response
- Pole-zero mapping
- Stability analysis
- PID controller design and gain selection
- Performance metrics evaluation

**Performance parameters evaluated:**

- Rise Time
- Settling Time
- Peak Time
- Overshoot
- Steady-State Error

**Step Response**

![MATLAB Step Response](images/matlab-step-response.jpeg)

**Pole-Zero Map**

![MATLAB Pole-Zero Map](images/matlab-pole-zero-map.jpeg)

**Numerical Results**

![MATLAB Results](images/matlab-results.png)

---

## 🔷 Simulink Model

![Simulink Model](images/simulink-model.png)

The Simulink model is built from:

- **Step Input** – represents the setpoint command
- **Sum Block** – computes the error (setpoint − feedback)
- **PID Controller** block
- **Plant Transfer Function** `1 / (10s + 1)`
- **Feedback Loop** – routes the plant output back to the sum block
- **Scope** – visualizes the resulting response

This block diagram mirrors exactly what the Arduino firmware does in software, letting you validate the controller design *before* deploying it to hardware.

---

## 🗂️ Repository Structure

```
PID-Based-Temperature-Control-System/
├── README.md                              # This file
├── Temperature_Control_System.ino         # Arduino firmware (PID controller)
├── images/
│   ├── project-cover.jpg                  # Repo cover / social preview image
│   ├── circuit-diagram.png                # Full wiring diagram
│   ├── simulink-model.png                 # Simulink block diagram
│   ├── matlab-step-response.jpeg          # Open vs closed loop step response
│   ├── matlab-pole-zero-map.jpeg          # Pole-zero analysis
│   └── matlab-results.png                 # Numerical performance results
└── docs/
    ├── Project-Report.docx                # Full written report
    └── MATLAB-Analysis-Presentation.pptx  # Slide deck of the MATLAB analysis
```

---

## 🚀 Getting Started

### Hardware Setup
1. Wire the circuit exactly as shown in the [circuit diagram](#-circuit-diagram--wiring) above.
2. Double-check that the Arduino and the 12V supply share a **common ground**.
3. Confirm MOSFET orientation (`G`, `D`, `S`) before powering on.

### Firmware Setup
1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Open `Temperature_Control_System.ino`.
3. Select **Board → Arduino UNO** and the correct **Port**.
4. Upload the sketch.
5. Open the **Serial Monitor** at `9600 baud` to watch live temperature, error, and PWM values.

### Simulation Setup (optional, but recommended before hardware testing)
1. Open MATLAB/Simulink.
2. Load the plant transfer function `G(s) = 1 / (10s + 1)`.
3. Use `pidtune()` or the model in `images/simulink-model.png` as a reference to reproduce the controller design.
4. Compare simulated step response against the real hardware response from the Serial Monitor.

---

## 🎚️ Tuning the PID Gains

If your setup overshoots, oscillates, or responds too slowly, adjust the gains in the `.ino` file:

```cpp
float Kp = 15.0;
float Ki = 0.30;
float Kd = 1.00;
```

| Symptom | Try |
|---|---|
| Too slow to reach setpoint | Increase `Kp` |
| Oscillates / overshoots a lot | Decrease `Kp`, increase `Kd` |
| Settles near, but never exactly at, the setpoint | Increase `Ki` slightly |
| Jittery, twitchy output | Decrease `Kd` |

Tune one gain at a time and re-upload between changes, watching the Serial Monitor output for feedback.

---

## 🔮 Future Improvements

- Automatic PID auto-tuning (e.g., Ziegler–Nichols implementation on-device)
- OLED/LCD live temperature display
- Data logging to SD card or serial CSV export
- Wi-Fi monitoring/telemetry using an ESP32
- Scaling up to an industrial heating element
- Sensor auto-calibration routine

---

## 🎓 Learning Outcomes

This project demonstrates practical, hands-on understanding of:

- Feedback control systems
- PID controller design and tuning
- MATLAB control system analysis
- Simulink modeling
- Embedded systems programming (Arduino/C++)
- PWM-based actuator control
- Temperature measurement with analog sensors
- Closed-loop system design end-to-end, from math to hardware

---

## 👥 Authors

**Shaftain Ali**
**Ali Zaman**

BS Intelligent Systems and Robotics (2023–27)
The Islamia University of Bahawalpur
