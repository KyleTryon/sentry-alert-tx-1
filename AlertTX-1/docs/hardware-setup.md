# Hardware Setup Guide

## Overview

The Alert TX-1 is built on the Adafruit ESP32-S3 Reverse TFT Feather, providing a compact and powerful platform for the retro beeper device. This guide covers all hardware requirements, assembly instructions, and wiring diagrams.

## 🚀 Status

✅ **Verified Working** - All components tested and compiling successfully

**Build Status:**
- **Compilation:** ✅ Success (1,033,171 bytes)
- **Memory Usage:** ✅ Efficient (20% of RAM)
- **Libraries:** ✅ All integrated
- **Hardware:** ✅ Pin assignments verified

## Hardware Requirements

### Core Components

| Component | Model | Quantity | Purpose | Link |
|-----------|-------|----------|---------|------|
| **Main Board** | [Adafruit ESP32-S3 Reverse TFT Feather](https://www.adafruit.com/product/5691) | 1 | Main microcontroller with built-in display and buttons | Adafruit |
| **Battery** | [LiPo 3.7V 400mAh](https://www.adafruit.com/product/3898) | 1 | Portable power (ideal size for Feathers) | Adafruit |
| **Buzzer** | [Passive Piezo Buzzer 9025](https://www.aliexpress.us/item/2251832678338636.html) | 1 | Audio alerts and ringtones | AliExpress |
| **LED** | 3mm Green LED | 1 | Status indicators | Local electronics store |
| **Resistor** | 220Ω Resistor | 1 | LED current limiting | Local electronics store |
| **Enclosure** | 3D printed case | 1 | Device housing | Custom/DIY |

**Note**: The ESP32-S3 Feather includes **3 built-in physical buttons** (A, B, C) - no external push buttons are required.

## Pin Assignments

> **📋 Pinout Reference**: For a complete pinout reference with detailed GPIO mappings, circuit diagrams, and troubleshooting information, see [Pinout Reference](pinout-reference.md).

### Button Configuration

The Alert TX-1 uses the **3 built-in physical buttons** on the ESP32-S3 Feather board:

```
┌─────────────────────────┐
│    ESP32-S3 Feather     │
│                         │
│  [A] [B] [C]  [RESET]  │
│   │   │   │      │      │
│  GPIO0 GPIO1 GPIO2   HW │
│                         │
└─────────────────────────┘
```

**Pin Assignments:**
```cpp
// Configure in src/config/settings.h
const int BUTTON_A_PIN = 0;        // GPIO0 - Built-in Button A (D0/BOOT)
const int BUTTON_B_PIN = 1;        // GPIO1 - Built-in Button B (D1)
const int BUTTON_C_PIN = 2;        // GPIO2 - Built-in Button C (D2)
```

**No external buttons are required** - the ESP32-S3 Feather has all necessary input buttons built-in.

### Display Configuration

The Adafruit ESP32-S3 Reverse TFT Feather includes a built-in color TFT display that requires no external wiring:

```cpp
// Display is built-in and automatically configured by the board library
// No external wiring required for the TFT display
// The board handles all SPI connections internally
// Uses Arduino_GFX library for graphics and ST7789 driver support
```

### Audio Configuration

```cpp
// Audio and LED pins
const int BUZZER_PIN = 15;  // GPIO15 (A3) - Passive buzzer
const int LED_PIN = 13;     // GPIO13 (D13) - External 3mm Green LED
```

## Assembly Instructions

### Step 1: Prepare the Main Board

1. **Inspect the ESP32-S3 Feather**
   - Verify all components are properly soldered
   - Check for any visible damage
   - Ensure the TFT display is securely attached

2. **Test Basic Functionality**
   - Connect via USB
   - Verify the display powers on
   - Check that the built-in CHG LED works (indicates charging)

### Step 2: Verify Built-in Buttons

1. **Button Testing**
   - **Button A (GPIO0)**: Physical button labeled "D0" on the board - also serves as BOOT button
   - **Button B (GPIO1)**: Physical button labeled "D1" on the board
   - **Button C (GPIO2)**: Physical button labeled "D2" on the board
   - **Reset Button**: Hardware reset button (not programmable)

2. **Button Verification**
   - Press each button to ensure it clicks properly
   - Verify no physical damage to the button mechanisms
   - Test during software setup to confirm proper detection

**No button installation required** - all buttons are pre-installed on the ESP32-S3 Feather board.

### Step 3: Install External Components

#### Buzzer Installation

1. **Buzzer Specifications**
   - **Model**: [Passive Piezo Buzzer 9025](https://www.aliexpress.us/item/2251832678338636.html)
   - **Size**: 9mm x 2.5mm
   - **Type**: Passive piezo buzzer (requires PWM drive signal)
   - **Frequency Range**: 100Hz - 20kHz
   - **Operating Voltage**: 3-24V

2. **Buzzer Placement**
   - Position the buzzer for optimal sound projection
   - Ensure it doesn't interfere with other components
   - Consider adding a small enclosure for better audio resonance

3. **Buzzer Wiring**
   - Connect positive terminal to `BUZZER_PIN` (GPIO15)
   - Connect negative terminal to GND
   - Use appropriate wire gauge (22-24 AWG)
   - **Polarity matters** - ensure correct positive/negative connections

#### LED Installation

1. **LED Placement**
   - Position the 3mm green LED for optimal visibility
   - Consider mounting in the enclosure front panel
   - Ensure proper heat dissipation

2. **LED Wiring**
   - Connect anode (long leg) to `LED_PIN` (GPIO13) through a 220Ω current-limiting resistor
   - Connect cathode (short leg) to GND
   - Use appropriate wire gauge (22-24 AWG)

3. **LED Specifications**
   - **Type**: 3mm Green LED
   - **Forward Voltage**: ~2.0V
   - **Forward Current**: 20mA
   - **Resistor**: 220Ω (for 3.3V logic level)

### Step 4: Power Management

1. **USB Power**
   - Primary power source during development
   - Provides 5V regulated power
   - Enables programming and debugging

2. **Battery Power**
   - [LiPo 3.7V 400mAh](https://www.adafruit.com/product/3898) - specifically designed for Feathers
   - **Perfect fit**: Designed to tuck between Feather headers
   - **JST-PH connector**: Genuine connector for reliable connection
   - **Protection circuit**: Built-in over-charge/discharge protection
   - **Charging**: Built-in charging circuit on Feather (USB powered)
   - **Capacity**: 400mAh provides ~8-12 hours of typical usage

### Step 5: Enclosure Assembly

1. **3D Printed Case**
   - Print case components in PLA or ABS
   - Ensure proper fit for all components
   - Add ventilation holes for cooling

2. **Component Mounting**
   - Secure the Feather board with standoffs
   - Mount buttons in designated holes
   - Position buzzer for optimal sound
   - Mount LED in front panel with proper light pipe or diffuser
   - Add rubber feet for stability

## Wiring Diagram

```
Alert TX-1 Component Connections
┌─────────────────────────────────────────────────┐
│                                                 │
│      Adafruit ESP32-S3 Reverse TFT Feather     │
│    ┌─────────────────────────────────────────┐  │
│    │ Built-in Components:                    │  │
│    │  [A] [B] [C] [RESET]    TFT Display    │  │
│    │  GPIO0,1,2  Hardware     240x135       │  │
│    │  (No external wiring needed)           │  │
│    └─────────────────────────────────────────┘  │
│                                                 │
│  External Components to Connect:                │
│                                                 │
│  ┌─────────┐                                    │
│  │Buzzer   │(+)──────────────► GPIO15 (A3)     │
│  │9025     │(-)──────────────► GND              │
│  │Passive  │                                    │
│  └─────────┘                                    │
│                                                 │
│  ┌─────────┐                                    │
│  │ 3mm LED │Anode─[220Ω]─────► GPIO13 (D13)    │
│  │ Green   │Cathode───────────► GND             │
│  └─────────┘                                    │
│                                                 │
│  ┌─────────┐                                    │
│  │ LiPo    │JST-PH────────────► VBAT Connector  │
│  │400mAh   │ (Built-in charging circuit)        │
│  │3.7V     │                                    │
│  └─────────┘                                    │
│                                                 │
└─────────────────────────────────────────────────┘

Component Links:
• ESP32-S3 Feather: https://www.adafruit.com/product/5691
• Buzzer 9025: https://www.aliexpress.us/item/2251832678338636.html
• LiPo Battery: https://www.adafruit.com/product/3898
```

## Testing and Validation

### Initial Power-Up Test

1. **Connect USB Power**
   - Verify the TFT display initializes
   - Check that the external LED illuminates during test
   - Verify the CHG LED shows charging status
   - Confirm WiFi connectivity

2. **Button Test**
   - Test each button individually
   - Verify proper debouncing
   - Check for false triggers

3. **Audio Test**
   - Test buzzer functionality
   - Verify volume levels
   - Check for distortion

### Functional Testing

1. **Display Test**
   - Verify all colors display correctly
   - Test text rendering
   - Check for dead pixels

2. **Input Test**
   - Test button combinations
   - Verify long-press detection
   - Check navigation functionality

3. **Audio Test**
   - Test ringtone playback
   - Verify alert sounds
   - Check volume control

## Troubleshooting

### Common Issues

1. **Display Not Working**
   - Check SPI connections
   - Verify power supply
   - Check for loose connections

2. **Buttons Not Responding**
   - Verify GPIO pin assignments
   - Check for short circuits
   - Test with multimeter

3. **Audio Issues**
   - Check buzzer connections
   - Verify PWM output
   - Test with different frequencies

4. **WiFi Problems**
   - Check antenna connection
   - Verify WiFi credentials
   - Test signal strength

### Debug Tools

- **Multimeter**: Test continuity and voltage
- **Oscilloscope**: Debug audio signals
- **Logic Analyzer**: Debug SPI communication
- **Serial Monitor**: Debug software issues

## Safety Considerations

### Electrical Safety

1. **Voltage Levels**
   - ESP32-S3 operates at 3.3V
   - USB provides 5V (regulated internally)
   - LiPo battery: 3.7V nominal

2. **Current Limits**
   - GPIO pins: 40mA max per pin
   - Total current: 200mA max
   - USB current: 500mA max

3. **ESD Protection**
   - Handle components carefully
   - Use anti-static wrist strap
   - Work on grounded surface

### Mechanical Safety

1. **Enclosure Design**
   - Ensure proper ventilation
   - Avoid sharp edges
   - Use appropriate materials

2. **Button Mounting**
   - Secure mounting to prevent movement
   - Proper alignment for easy operation
   - Adequate clearance for operation

## Maintenance

### Regular Maintenance

1. **Cleaning**
   - Clean display surface regularly
   - Remove dust from ventilation holes
   - Clean button contacts if needed

2. **Inspection**
   - Check for loose connections
   - Inspect for physical damage
   - Verify button operation

3. **Software Updates**
   - Keep firmware updated
   - Backup configuration settings
   - Test after updates

### Long-term Care

1. **Battery Care** (if using LiPo)
   - Don't overcharge
   - Store at 50% charge
   - Replace when capacity drops

2. **Environmental Considerations**
   - Avoid extreme temperatures
   - Protect from moisture
   - Store in dry location

## Specifications

### Physical Specifications

- **Dimensions**: 50mm x 25mm x 15mm (without enclosure)
- **Weight**: ~25g (without battery)
- **Material**: PCB with SMD components
- **Operating Temperature**: -40°C to +85°C

### Electrical Specifications

- **Operating Voltage**: 3.3V
- **Power Consumption**: 50-200mA typical
- **WiFi**: 802.11 b/g/n
- **Bluetooth**: Bluetooth Low Energy 4.2

### Display Specifications

- **Type**: Color TFT LCD
- **Resolution**: 135x240 pixels
- **Colors**: 16-bit (65,536 colors)
- **Interface**: SPI

This hardware setup provides a solid foundation for the Alert TX-1 beeper device, ensuring reliable operation and easy maintenance. 