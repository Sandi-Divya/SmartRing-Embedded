# SmartRing-Embedded: SR08 BLE Ring Reverse Engineering & Hardware Replication

## Project Overview

This project focuses on reverse engineering the proprietary Bluetooth Low Energy (BLE) communication protocol and internal hardware architecture of the closed-source **SR08 Smart Ring**. 

Due to the closed nature of the original firmware and vendor application, the project follows a two-tier reverse engineering methodology:
1. **Protocol Analysis:** Intercepting, analyzing, and recreating the proprietary BLE GATT service layer using Python (`Bleak`) and GATT sniffers.
2. **Hardware Disassembly & Silicon Identification:** Teardown of the physical ring to identify the onboard System-on-Chip (SoC) and discrete peripheral components.
3. **Hardware Replication & Embedded Firmware Implementation:** Recreating the ring's subsystem on the identical target SoC development board (Dialog/Renesas DA14585 Cortex-M0) to build custom firmware that drives peripheral displays via direct BLE commands.

---

## Technical Stack

| Domain | Technology / Hardware | Purpose |
|---|---|---|
| **Host Framework** | Python 3, `Bleak` | Custom BLE client & automated packet injection |
| **Firmware Development** | C (C99), Keil MDK-ARM (ARM Compiler 6) | Bare-metal SDK 6 firmware implementation |
| **Target Silicon / SoC** | Renesas / Dialog DA14585 (ARM Cortex-M0) | Exact target microcontroller found in hardware teardown |
| **Development Kit** | DA14585 Pro/Basic Dev Kit | Hardware emulation and logic debugging |
| **Display Hardware** | Common-Anode 7-Segment LED display | Low-level display subsystem replication |
| **Analysis Tools** | nRF Connect, Wireshark, J-Link Debugger | Packet inspection, GATT discovery, SWD tracing |

---

## Weekly Engineering Progress

### Protocol Discovery & GATT Reverse Engineering
* Scanned and established connection with the closed-source SR08 Smart Ring without vendor mobile app dependencies.
* Discovered proprietary vendor service UUID:
  * **Primary Service:** `000056ff-0000-1000-8000-00805f9b34fb`
  * **Write Characteristic:** `000033f3-0000-1000-8000-00805f9b34fb` (Custom command transmission)
  * **Notify Characteristic:** `000033f4-0000-1000-8000-00805f9b34fb` (Telemetry & ACK feedback)
* Developed initial Python-based BLE framework (`Bleak`) to transmit test payloads.
* Reverse engineered the proprietary **Vendor Handshake**:
  * **Command Sent:** `0x5A`
  * **Response Captured:** `5A 07 38 83 60 00 FF FF FF FF 01 0A 08 00 06 00 50 00 00 00`

---

### Hardware Teardown & Silicon Identification
* Disassembled the sealed physical SR08 Smart Ring casing to inspect internal circuitry.
* Performed chip-level identification on the micro-PCB:
  * Identified the main controller as the **Dialog / Renesas DA14585 BLE SoC** (Ultra-low power ARM Cortex-M0).
  * Traced power lines, battery charging circuitry, and display segment routing lines.
* Mapped out the hardware replication strategy: transition from pure black-box protocol sniffing to developing custom, open firmware directly on the DA14585 platform.

---

### SDK Environment Setup & Custom GATT Architecture
* Configured the **Dialog SDK 6 (6.0.24.1464)** environment within Keil uVision for DA14585 target targets.
* Configured Custom Profile (`custs1`) GATT database definitions to emulate the ring's command and telemetry endpoints.
* Implemented kernel message routing in `user_peripheral.c` to catch incoming `CUSTS1_VAL_WRITE_IND` events directly from the BLE stack.

---

### Midterm Evaluation Milestone: Full Hardware-Firmware Replication & Display Demo
* Built the physical display replication circuit using a common-anode 7-segment display wired to DA14585 Port 2 GPIO pins (`P2_0`, `P2_1`, `P2_2`, `P2_3`, `P2_5`, `P2_6`, `P2_7`).
* Resolved shared-line hardware conflicts and pull-up parasitic leakage on SPI Flash shared lines (`P2_4`/`P2_6`).
* Created a dedicated low-level display driver (`display.c`) with a customized bit-level lookup table (LUT) for digits `0` through `9`.
* Implemented non-blocking visual diagnostic timers (`app_easy_timer`) for active status indication.
* **Demonstrated working end-to-end prototype:**
  * Device advertises as an open peripheral.
  * Central host (nRF Connect / Python Bleak) connects and writes numeric command bytes.
  * DA14585 receives packets over BLE and drives the physical 7-segment display in real-time.

---

## System Architecture

```text
[ Smartphone / Host App ]
           │
           │  (BLE Write: '0'-'9' or 0x00-0x09)
           ▼
[ DA14585 BLE Radio Stack ]
           │
           ▼
[ user_catch_rest_hndl() / custs1 ]
           │
           ▼
[ parse_and_display_val() ]
           │
           ▼
[ display_show_digit() / Segment LUT ]
           │
           ▼
[ Port 2 GPIO Pins (P2_0 - P2_7) ] ──▶ [ Physical 7-Segment LED Display ]
```

### Week 8: Hardware Subsystem Integration & Power Telemetry

* **Capacitive Touch Emulation & Gesture Engine:**
  * Configured the hardware Wake-Up Controller (`WKUPCT`) on pin `P1_3` with a 40ms debounce window for active-LOW edge triggering.
  * Implemented a multi-tap gesture detection state machine:
    * **Single Tap:** Evaluated via a 350ms non-blocking window timer to increment and display a local counter (`0`–`9`).
    * **Double Tap:** Triggers real-time power telemetry and displays the current battery level tens digit.
  * Added an automated 2-second timeout callback to shut off display segments post-interaction for energy conservation.

* **Internal Power Rail Telemetry:**
  * Integrated the DA14585 General Purpose ADC (`GP_ADC`) to monitor the internal supply rail (`VBAT3V`) using `adc_get_vbat_sample`.
  * Implemented a calibrated voltage-to-percentage mapping transfer function ($0\%$ at $\approx 2.6\text{V}$, $100\%$ at $\approx 3.3\text{V}$).
  * Configured background periodic sampling every 5 seconds to maintain current battery telemetry.

* **Hardware Mapping & Pin Configuration:**
  * `P1_0`: Output – Diagnostic LED pulse.
  * `P1_3`: Input Pull-Up – Wake-Up Controller / Touch interrupt.
  * `P0_0`–`P0_7`: Output – 7-segment display matrix bus.

## Project Structure

```text
├── src/
│   ├── platform/
│   │   └── user_periph_setup.c   # Pin reservations, pad modes, and peripheral clocks
│   ├── user_app/
│   │   ├── user_peripheral.c     # Main application state machine, ADC, WKUPCT, BLE handlers
│   │   ├── user_peripheral.h     # Application prototypes and data structures
│   │   ├── display.c             # 7-segment display lookup and GPIO driving logic
│   │   └── display.h             # Display driver interface
└── Keil_5/
    └── ble_app_peripheral.uvprojx # Keil uVision project workspace


