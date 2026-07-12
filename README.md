# SmartRing-Embedded
# SR08 BLE Ring Reverse Engineering Framework

## Project Overview

This project focuses on reverse engineering the proprietary Bluetooth Low Energy (BLE) communication protocol of the SR08 Smart Ring.

The objective is to analyze the ring's BLE communication without depending on the official vendor application and develop a custom Python-based framework to communicate directly with the device.

The framework uses BLE GATT analysis, packet capture, and custom command transmission to understand and control the communication between the smartphone host and the SR08 ring.

---

# Project Objectives

The main goals of this project are:

- Discover and analyze the SR08 BLE GATT structure.
- Identify vendor-specific BLE services and characteristics.
- Capture raw BLE notification packets.
- Reverse engineer communication commands.
- Develop a custom BLE communication framework.
- Build a foundation for future features such as display control, gesture recognition, and independent ring control.

---

# Technologies Used

| Technology | Purpose |
|---|---|
| Python | Framework development |
| Bleak | Bluetooth Low Energy communication |
| nRF Connect | BLE GATT analysis |
| Git | Version control |

---

# Current Progress

## Phase 1: BLE Device Discovery

The SR08 ring was successfully detected through BLE scanning.

Device discovered:

```
Name: SR08

MAC Address:
02:02:E3:FB:D6:C4
```

---

## Phase 2: GATT Service Analysis

The BLE GATT server of the ring was successfully accessed.

Discovered vendor-specific services:

```
Service:
000056ff-0000-1000-8000-00805f9b34fb
```

Communication characteristics:

### Write Characteristic

```
000033f3-0000-1000-8000-00805f9b34fb
```

Purpose:

- Sending custom commands to the ring


### Notification Characteristic

```
000033f4-0000-1000-8000-00805f9b34fb
```

Purpose:

- Receiving raw data packets from the ring

---

# Framework Architecture

```
SR08 Ring

     |
     |
     | BLE GATT Communication
     |
     v

Python Framework

     |
     |
 +-----------+
 |Connection |
 +-----------+

     |
 +-----------+
 | Notify    |
 | Handler   |
 +-----------+

     |
 +-----------+
 | Command  |
 | Writer   |
 +-----------+

```

---

# Phase 3: Custom BLE Communication

A custom Python framework was created using Bleak.

The framework can:

- Connect directly to SR08.
- Subscribe to BLE notifications.
- Send custom hexadecimal commands.
- Capture raw response packets.

---

# Handshake Reverse Engineering

A vendor handshake command was discovered.

Command transmitted:

```
5A
```

Communication channel:

```
WRITE:
0x33F3


NOTIFY:
0x33F4
```

Response captured:

```
5A 07 38 83 60 00 FF FF FF FF
01 0A 08 00 06 00 50 00 00 00
```

This confirmed successful two-way communication with the SR08 ring without using the official application.

---

# Packet Capture Example

Notification example:

```
===== NOTIFICATION =====

Sender:
000033f4-0000-1000-8000-00805f9b34fb


Data:

5A 07 38 83 60 00 FF FF FF FF
01 0A 08 00 06 00 50 00 00 00

```

---

# Project Screenshots

## BLE Device Detection

(Add screenshot here)

```
![BLE Detection](screenshots/device_detection.png)
```

---

## GATT Service Discovery

(Add screenshot here)

```
![GATT Services](screenshots/gatt_services.png)
```

---

## Notification Subscription

(Add screenshot here)

```
![Notification](screenshots/notification.png)
```

---

## Custom Handshake Response

(Add screenshot here)

```
![Handshake](screenshots/handshake.png)
```

---

# Future Work

## Protocol Decoder

Develop an automatic packet decoder:

```
Raw BLE Packet

        |

        v

Packet Parser

        |

        v

Command Identification

```

---

## Display Control

Investigate display-related BLE commands and implement:

- Custom display messages
- Number rendering
- Device information display

---

## Gesture-Based Control

Explore IMU sensor communication to implement:

- Gesture recognition
- Custom ring interactions
- Smartphone control commands

---

# Current Achievement

Successfully achieved:

✅ BLE communication without vendor application  
✅ GATT structure analysis  
✅ Vendor-specific service discovery  
✅ Custom command transmission  
✅ Notification packet capture  
✅ SR08 handshake reproduction  

---

# Author

Individual Embedded Systems Reverse Engineering Project
