# Hardware Setup Guide

## Overview

The Alert TX-1 is built on the Adafruit ESP32-S3 Reverse TFT Feather, providing a compact and powerful platform for the retro beeper device. This guide covers all hardware requirements, assembly instructions, and wiring diagrams.

## Hardware Requirements

### Core Components

| Component | Model | Quantity | Purpose |
|-----------|-------|----------|---------|
| **Main Board** | Adafruit ESP32-S3 Reverse TFT Feather | 1 | Main microcontroller and display |
| **Push Buttons** | Tactile push buttons (6x6mm) | 4 | User input |
| **Buzzer** | Passive piezo buzzer | 1 | Audio alerts and ringtones |
| **LED** | Onboard LED (included) | 1 | Status indicators |
| **Enclosure** | 3D printed case | 1 | Device housing |

### Optional Components

| Component | Model | Quantity | Purpose |
|-----------|-------|----------|---------|
| **Battery** | LiPo 3.7V 500mAh | 1 | Portable power |
| **Speaker** | Small speaker (8Ω) | 1 | Enhanced audio |
| **Antenna** | WiFi antenna | 1 | Improved WiFi range |

## Pin Assignments

> **📋 Pinout Reference**: For a complete pinout reference with detailed GPIO mappings, circuit diagrams, and troubleshooting information, see [Pinout Reference](pinout-reference.md).

### Button Configuration

The Alert TX-1 uses 4 physical buttons arranged in a cellphone-style layout:

```
┌─────────┬─────────┐
│   UP    │         │
├─────────┤  RIGHT  │
│   MID   │         │
├─────────┤         │
│  DOWN   │         │
└─────────┴─────────┘
```

**Pin Assignments:**
```cpp
// Configure in src/config/settings.h
const int BUTTON_D0_PIN = 0;        // GPIO0 - Built-in D0/BOOT button
const int BUTTON_D1_PIN = 1;        // GPIO1 - Built-in D1 button  
const int BUTTON_D2_PIN = 2;        // GPIO2 - Built-in D2 button
const int BUTTON_EXTERNAL_PIN = 14; // GPIO14 (A4) - External button for 4th input
```

### Display Configuration

The Adafruit ESP32-S3 Reverse TFT Feather includes a built-in color TFT display that requires no external wiring:

```cpp
// Display is built-in and automatically configured by the board library
// No external wiring required for the TFT display
// The board handles all SPI connections internally
```

### Audio Configuration

```cpp
// Audio and LED pins
const int BUZZER_PIN = 15;  // GPIO15 (A3) - Passive buzzer
const int LED_PIN = 13;     // GPIO13 (D13) - Onboard LED
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
   - Check that the onboard LED works

### Step 2: Install Buttons

1. **Button Layout**
   ```
   ┌─────────┬─────────┐
   │   D0    │         │
   ├─────────┤ EXTERNAL│
   │   D1    │         │
   ├─────────┤         │
   │   D2    │         │
   └─────────┴─────────┘
   ```

2. **Wiring Instructions**
   - **Built-in buttons (D0, D1, D2)**: No external wiring required - these are physical buttons on the board
   - **External button**: Connect one terminal to GPIO14, other terminal to GND
   - Use pull-up resistors (internal to ESP32-S3)
   - Ensure buttons are properly debounced in software

3. **Button Specifications**
   - **Type**: Tactile push button (6x6mm)
   - **Rating**: 50mA, 12V
   - **Travel**: 0.25mm
   - **Force**: 160±50gf

### Step 3: Install Buzzer

1. **Buzzer Placement**
   - Position the buzzer for optimal sound projection
   - Ensure it doesn't interfere with other components
   - Consider adding a small enclosure for better audio

2. **Wiring**
   - Connect positive terminal to `BUZZER_PIN` (GPIO15)
   - Connect negative terminal to GND
   - Use appropriate wire gauge (22-24 AWG)

### Step 4: Power Management

1. **USB Power**
   - Primary power source during development
   - Provides 5V regulated power
   - Enables programming and debugging

2. **Battery Power (Optional)**
   - LiPo battery for portable operation
   - 3.7V nominal voltage
   - 500mAh capacity recommended
   - Built-in charging circuit on Feather

### Step 5: Enclosure Assembly

1. **3D Printed Case**
   - Print case components in PLA or ABS
   - Ensure proper fit for all components
   - Add ventilation holes for cooling

2. **Component Mounting**
   - Secure the Feather board with standoffs
   - Mount buttons in designated holes
   - Position buzzer for optimal sound
   - Add rubber feet for stability

## Wiring Diagram

```
Adafruit ESP32-S3 Reverse TFT Feather
┌─────────────────────────────────────────────────┐
│                                                 │
│  ┌─────────┐    ┌─────────────────────────────┐ │
│  │   D0    │    │                             │ │
│  │ Button  │────┤ GPIO0 (Built-in)            │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │   D1    │────┤ GPIO1 (Built-in)            │ │
│  │ Button  │    │                             │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │   D2    │────┤ GPIO2 (Built-in)            │ │
│  │ Button  │    │                             │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │External │────┤ GPIO14 (A4)                 │ │
│  │ Button  │    │                             │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │ Buzzer  │────┤ GPIO15 (A3)                 │ │
│  │  (+)    │    │                             │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │  LED    │────┤ GPIO13 (D13) - Onboard LED  │ │
│  │(onboard)│    │                             │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │  TFT    │────┤ Built-in Display            │ │
│  │ Display │    │ (GPIO40-45)                 │ │
│  │(built-in)│   │                             │ │
│  └─────────┘    │                             │ │
│                 │                             │ │
│  ┌─────────┐    │                             │ │
│  │ LiPo    │────┤ VBAT/GND (JST 2-PH)         │ │
│  │ Battery │    │                             │ │
│  └─────────┘    │                             │ │
│                 └─────────────────────────────┘ │
│                                                 │
└─────────────────────────────────────────────────┘
```

## Testing and Validation

### Initial Power-Up Test

1. **Connect USB Power**
   - Verify the TFT display initializes
   - Check that the onboard LED illuminates
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