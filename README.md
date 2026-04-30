# Smart Vehicle Ignition System — RFID & Face Recognition

> A hardware-software dual-factor authentication system for vehicle access control, combining real-time facial recognition (OpenCV + face_recognition) with RFID card verification (MFRC522 + Arduino Uno). Built for ECE 547/647: Security Engineering at UMass Amherst.

**Team:** Adithya Kumar, Allen Gao, Poojitha Singh, Angaddeep Singh  
**Course:** ECE 547/647 — Security Engineering, University of Massachusetts Amherst  
**Demo Video:** [Watch on YouTube](https://youtu.be/OIoVZKDeuDg)

---

## Overview

Traditional vehicle security relies on a single factor — a key or key fob — which can be stolen or cloned. This project implements a **two-stage authentication protocol** inspired by multi-factor authentication (MFA) principles:

1. **Stage 1 — Biometric:** Facial recognition via webcam using Python and OpenCV. The system checks the live face against a database of authorized users.
2. **Stage 2 — Token:** Only if Stage 1 passes, the Arduino activates and waits for an authorized RFID card scan using the MFRC522 module.

If both stages pass → **green LED** (access granted / ignition enabled).  
If either stage fails → **red LED** (access denied).

---

## System Architecture

```
┌─────────────────────────────┐
│        Laptop / PC          │
│  ┌─────────────────────┐   │
│  │  Python Face Recog. │   │
│  │  (OpenCV +          │   │
│  │   face_recognition) │   │
│  └────────┬────────────┘   │
│           │ Serial (USB)    │
└───────────┼─────────────────┘
            │ "face_ok" signal
            ▼
┌─────────────────────────────┐
│        Arduino Uno          │
│  ┌─────────────────────┐   │
│  │  MFRC522 RFID Reader│   │
│  │  (SPI, pin 9–13)    │   │
│  └────────┬────────────┘   │
│           │                 │
│  ┌────────▼────────────┐   │
│  │  LED Feedback       │   │
│  │  Green (pin 7) ✅   │   │
│  │  Red   (pin 6) ❌   │   │
│  └─────────────────────┘   │
└─────────────────────────────┘
```

---

## Hardware

| Component | Purpose |
|-----------|---------|
| Arduino Uno | Microcontroller for RFID logic and LED control |
| MFRC522 RFID Module | Reads 13.56 MHz RFID card UID via SPI |
| Webcam (built-in or USB) | Captures live video for face recognition |
| Green LED + 1kΩ resistor | Access granted indicator |
| Red LED + 1kΩ resistor | Access denied indicator |
| Breadboard + Jumper wires | Circuit assembly |

### Wiring — MFRC522 to Arduino Uno

| MFRC522 Pin | Arduino Pin |
|-------------|-------------|
| SCK | 13 |
| MISO | 12 |
| MOSI | 11 |
| SDA | 10 |
| RST | 9 |
| GND | GND |
| 3.3V | 3.3V |

**LEDs:**
- Green LED anode → Pin 7 → cathode → 1kΩ → GND
- Red LED anode → Pin 6 → cathode → 1kΩ → GND

---

## Hardware Photos

### Circuit Build
![Circuit](docs/circuit.jpg)

### System Running — Face Recognized
![Demo](docs/demo.jpg)

---

## Software

### Dependencies

```bash
pip install face_recognition opencv-python pyserial
```

Arduino libraries (install via Arduino IDE Library Manager):
- `MFRC522` by GithubCommunity

### How It Works

**Python side (`face_rec.py`):**
- Establishes serial connection with Arduino at 9600 baud
- Loads authorized face images from `/known_faces/` folder and encodes them
- Opens webcam and continuously captures frames
- On each frame, detects faces and compares encodings using Euclidean distance
- If a match is found → sends `"face_ok"` signal to Arduino via serial
- Waits for Arduino to respond with `"rfid_ok"` within 10-second timeout
- If both verified → exits program (ignition granted)

**Arduino side (`rfid_auth.ino`):**
- Waits in idle state until it receives `"face_ok"` from Python over serial
- Activates RFID reader and waits for a card scan
- Reads card UID and compares to authorized UID list
- If match → lights green LED, sends `"rfid_ok"` back to Python
- If no match → lights red LED, waits for another scan

---

## Results

### Face Recognition Range
The system was tested at increasing distances from the camera:

| Distance | Recognition |
|----------|-------------|
| 3 ft | ✅ |
| 9 ft | ✅ |
| 12 ft | ✅ |
| 15 ft | ✅ |
| 20 ft | ✅ |
| > 20 ft | ❌ |

### RFID Scan Speed
Tested over 8 attempts. UID read time ranged from **2–4 ms**, with a typical read time of **~3 ms**.

### Obstruction Testing
| Obstruction | Recognized? |
|-------------|-------------|
| Glasses | ✅ |
| Hat | ✅ |
| Hat + Glasses | ✅ |
| Scarf (lower face) | ❌ |
| Face mask | ❌ |

### Known Limitation — Liveness Detection
The system was successfully fooled by a printed photo of an authorized user (100% spoof success rate). This is a known vulnerability of OpenCV-based face recognition without liveness detection — identified as a future improvement.

---

## Security Analysis

This project applies core security engineering principles:

| Principle | Implementation |
|-----------|---------------|
| Multi-factor Authentication | "Something you are" (face) + "Something you have" (RFID card) |
| Defence in Depth | Two independent verification stages — RFID only activates after face clears |
| Fail Secure | Red LED and access denial if either stage fails |
| Attack Surface Awareness | Identified photo spoofing and RFID cloning as attack vectors |

---

## Future Work

- **Liveness Detection** — Add blink/head-movement detection to prevent photo spoofing
- **RFID Encryption** — Replace plain UID matching with encrypted challenge-response
- **Real Ignition Control** — Replace LED with relay module connected to actual ignition circuit
- **Multi-user Support** — Register and manage multiple authorized faces and RFID tags
- **Mobile Alerts** — Add GSM module for SMS notification on failed attempts

---

## Repository Structure

```
├── README.md
├── src/
│   ├── face_rec.py          # Python face recognition + serial comm
│   └── rfid_auth.ino        # Arduino RFID verification sketch
├── known_faces/             # Folder for authorized user face images
│   └── (add .jpg/.png files here)
└── docs/
    ├── circuit.jpg          # Hardware build photo
    └── demo.jpg             # System running photo
```

---

## Course Context

Built for **ECE 547/647: Security Engineering** at the University of Massachusetts Amherst. The project applies course concepts including multi-factor authentication, threat modeling, attack surface analysis, and layered security design to a real embedded hardware prototype.
