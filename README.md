<h1 align="center">Door Locker Security System</h1>

<p align="center">
A dual-microcontroller, password-protected door locking system built on two ATmega32 units and a layered software architecture.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/MCU-ATmega32-blue" />
  <img src="https://img.shields.io/badge/Language-Embedded%20C-green" />
  <img src="https://img.shields.io/badge/Clock-8%20MHz-orange" />
  <img src="https://img.shields.io/badge/Simulation-Proteus-lightgrey" />
</p>

---

## Table of Contents

1. [Overview](#1-overview)
2. [System Architecture](#2-system-architecture)
3. [System Operation](#3-system-operation)
4. [Hardware Components](#4-hardware-components)
5. [Driver Description](#5-driver-description)
6. [Communication Protocol](#6-communication-protocol)
7. [Simulation](#7-simulation)
8. [Repository Structure](#8-repository-structure)
9. [Tools and Technologies](#9-tools-and-technologies)
10. [Author](#10-author)

---

## 1. Overview

The Door Locker Security System allows a user to unlock a door using a 5-digit password. The system is split across two ATmega32 microcontrollers operating at 8 MHz, each with a clearly separated responsibility:

- **HMI_ECU (Human Machine Interface):** interacts with the user only. It reads input from the keypad and displays messages on the LCD.
- **Control_ECU:** performs all processing and decision-making, including password verification, door actuation, and alarm activation.

The two units communicate over UART. The password is stored in an external EEPROM connected to the Control_ECU through I2C, so it persists after power loss.

---

## 2. System Architecture

Both ECUs follow a three-layer architecture:

| Layer | HMI_ECU | Control_ECU |
|-------|---------|-------------|
| **Application** | User interface logic | Password checking, door control, alarm logic |
| **HAL** (Hardware Abstraction Layer) | LCD, Keypad | Buzzer, External EEPROM, DC Motor |
| **MCAL** (Microcontroller Abstraction Layer) | GPIO, UART, Timer1 | GPIO, I2C (TWI), UART, Timer1, PWM (Timer0) |

---

## 3. System Operation

### Step 1: Create a System Password
1. The LCD prompts the user to enter a password of 5 digits. Each digit is displayed as `*`.
2. The user presses the enter key, then re-enters the same password for confirmation.
3. The HMI_ECU sends both passwords to the Control_ECU over UART.
4. If they match, the password is saved in the EEPROM and the system proceeds to the main menu. Otherwise, Step 1 is repeated.

### Step 2: Main Menu
The LCD continuously displays the main options:

```
+ : Open Door
- : Change Pass
```

### Step 3: Open Door (`+`)
1. The user enters the password, which the HMI_ECU forwards to the Control_ECU.
2. The Control_ECU compares it with the password stored in the EEPROM.
3. If correct:
   - The motor rotates **clockwise for 15 seconds** (LCD: "Door is Unlocking").
   - The motor **holds for 3 seconds**.
   - The motor rotates **anti-clockwise for 15 seconds** (LCD: "Door is Locking").

### Step 4: Change Password (`-`)
1. The user enters the current password for verification.
2. If correct, the system returns to Step 1 so a new password can be created.

### Step 5: Security Lockout
If the password is entered incorrectly at Step 3 or Step 4, the user gets up to three attempts in total. After **three consecutive failures**:
- The buzzer is activated for **1 minute**.
- An error message is displayed on the LCD for 1 minute.
- The system is locked and no keypad input is accepted during this period.
- The system then returns to the main menu.

---

## 4. Hardware Components

| Component | Connected To | Purpose |
|-----------|--------------|---------|
| ATmega32 (x2) | n/a | Main controllers, 8 MHz |
| 2x16 LCD | HMI_ECU | Displays prompts and status messages |
| 4x4 Keypad | HMI_ECU | Password and menu input |
| External EEPROM (I2C) | Control_ECU | Non-volatile password storage |
| DC Motor | Control_ECU | Door lock actuator (max speed via Timer0 PWM) |
| Buzzer | Control_ECU | Alarm on repeated failed attempts |

---

## 5. Driver Description

| Driver | Layer | Description |
|--------|-------|-------------|
| **GPIO** | MCAL | Shared across both ECUs |
| **UART** | MCAL | Configured through `UART_ConfigType` (data bits, parity, stop bits, baud rate) |
| **I2C (TWI)** | MCAL | Configured through `TWI_ConfigType` (address, bit rate); used for EEPROM access |
| **Timer1** | MCAL | Interrupt-based with callback; supports Normal and Compare modes via `Timer1_ConfigType` |
| **PWM (Timer0)** | MCAL | Runs the DC motor at maximum speed |
| **LCD** | HAL | 2x16 character display |
| **Keypad** | HAL | 4x4 matrix keypad |
| **DC Motor** | HAL | Direction and speed control |
| **Buzzer** | HAL | `Buzzer_init()`, `Buzzer_on()`, `Buzzer_off()` |
| **External EEPROM** | HAL | Read/write over I2C |

---

## 6. Communication Protocol

The HMI_ECU and Control_ECU exchange data over UART. The HMI_ECU sends the entered password(s) and menu selections, and the Control_ECU replies with the verification result so the HMI_ECU can display the appropriate message.

---

## 7. Simulation

The system was simulated in Proteus. The project file is `Door_Locker_Simulation.pdsprj`.

![Simulation](docs/Simulation.png)

---

## 8. Repository Structure

```
Door-Locker-Security-System/
├── HMI_ECU/                          # LCD, Keypad, UART, Timer1, GPIO
├── Control_ECU/                      # EEPROM, Buzzer, DC Motor, I2C, UART, Timer1, PWM, GPIO
├── docs/
│   └── Simulation.png                # Proteus simulation screenshot
├── Door_Locker_Simulation.pdsprj     # Proteus project file
└── README.md
```

---

## 9. Tools and Technologies

- **Microcontroller:** ATmega32 @ 8 MHz
- **Language:** Embedded C
- **IDE:** Eclipse (AVR toolchain)
- **Simulation:** Proteus
- **Concepts:** layered architecture, interrupts, callback functions, configuration structures, UART, I2C

---
