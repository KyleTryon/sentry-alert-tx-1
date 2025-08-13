# Pinout Reference - Alert TX-1

## Overview

This document provides a comprehensive reference for the pin assignments used in the Alert TX-1 project, based on the Adafruit ESP32-S3 Reverse TFT Feather board.

**Note**: This project uses Arduino/C++ (not CircuitPython).

## Board Information

- **Board**: Adafruit ESP32-S3 Reverse TFT Feather
- **Product ID**: 5691
- **Documentation**: 
  - [Adafruit ESP32-S3 Reverse TFT Feather Pinouts](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/pinouts)
  - [Power Management Guide](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/power-management)

## Pin Assignment Summary

### Project-Specific Pins

| Function | GPIO | CircuitPython Name | Description | Connection |
|----------|------|-------------------|-------------|------------|
| **Button A** | GPIO0 | D0/BOOT | Built-in D0/BOOT button | Physical button on board |
| **Button B** | GPIO1 | D1 | Built-in D1 button | Physical button on board |
| **Button C** | GPIO2 | D2 | Built-in D2 button | Physical button on board |
| **Buzzer** | GPIO15 | A3 | Passive piezo buzzer | Positive terminal |
| **LED** | GPIO13 | D13 | External 3mm Green LED | Anode via 220Ω resistor |

### Built-in Features (No External Wiring Required)

| Feature | GPIO | CircuitPython Name | Description |
|---------|------|-------------------|-------------|
| **TFT Display** | GPIO40-45 | - | 1.14" IPS TFT (240x135) |
| **TFT Power** | GPIO7 | TFT_I2C_POWER | Display power control |
| **CHG LED** | Hardware | - | Yellow charging LED (hardware controlled) |
| **NeoPixel** | GPIO33 | NEOPIXEL | RGB status LED |
| **NeoPixel Power** | GPIO21 | NEOPIXEL_POWER | NeoPixel power control |
| **STEMMA QT I2C** | GPIO3/4 | SDA/SCL | I2C connector |
| **Battery Monitor** | I2C | - | MAX17048 (0x36) |
| **Reset Button** | Hardware | - | Physical reset button (no GPIO) |

### Power and Control Pins

| Pin | Function | Description |
|-----|----------|-------------|
| **VBAT** | Battery | LiPo battery connection (JST 2-PH) |
| **USB** | USB Power | USB-C power input |
| **3.3V** | Regulated Power | 3.3V output (500mA max) |
| **GND** | Ground | Common ground |
| **EN** | Enable | 3.3V regulator enable |
| **RST** | Reset | Board reset |

### Optional Sense Pins (Project Mods)

These are optional and not connected by default. If you mod your hardware, you can set the GPIO numbers in `src/config/settings.h`.

| Function | GPIO | Description |
|----------|------|-------------|
| **VBUS Sense** | User-defined | Detects 5V USB presence (via voltage divider) |
| **CHG LED Sense** | User-defined | Reads charger LED state to infer charging (debounced ~1.5s) |

- The CHG LED is hardware-controlled as described in Adafruit’s guide, and not exposed as a GPIO by default. You may add an optical or electrical sense circuit to a spare GPIO if needed. See Adafruit’s Power Management docs for behavior details. 
  - Reference: [Adafruit Power Management](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/power-management)

## Detailed Pin Information

### Button Pins

The Alert TX-1 uses the three built-in buttons on the board, labeled A, B, and C.

```cpp
// Button and LED configuration in settings.h
const int BUTTON_A_PIN = 0;        // GPIO0 - Built-in D0/BOOT button (Button A)
const int BUTTON_B_PIN = 1;        // GPIO1 - Built-in D1 button (Button B)
const int BUTTON_C_PIN = 2;        // GPIO2 - Built-in D2 button (Button C)
const int LED_PIN = 13;            // GPIO13 - External 3mm Green LED
```

**Built-in Buttons:**
- **Button A (D0)**: Physical button on board (GPIO0) - also serves as BOOT button
  - Pulled HIGH by default, signal goes LOW when pressed
  - Use `if (digitalRead(0) == LOW)` in Arduino to detect press
- **Button B (D1)**: Physical button on board (GPIO1)
  - Pulled LOW by default, signal goes HIGH when pressed
  - Use `if (digitalRead(1) == HIGH)` in Arduino to detect press
- **Button C (D2)**: Physical button on board (GPIO2)
  - Pulled LOW by default, signal goes HIGH when pressed
  - Use `if (digitalRead(2) == HIGH)` in Arduino to detect press

**Reset Button:**
- **Reset Button**: Physical button on the front of the board (opposite D0, D1, D2)
- Hardware reset button - cannot be used for input
- Resets the device when pressed
- Used for bootloader entry (hold D0 + press Reset)

### Audio and LED Pins

```cpp
const int BUZZER_PIN = 15;  // GPIO15 (A3)
const int LED_PIN = 13;     // GPIO13 (D13)
```

**Buzzer Wiring:**
- Connect buzzer positive (+) terminal to GPIO15
- Connect buzzer negative (-) terminal to GND
- Use PWM output for tone generation

**LED Wiring:**
- Connect LED anode (long leg) to GPIO13 through a 220Ω resistor
- Connect LED cathode (short leg) to GND
- LED specifications: 3mm Green LED, ~2.0V forward voltage, 20mA forward current

### TFT Display

The built-in TFT display uses the following pins:
- **TFT_CS**: GPIO42
- **TFT_DC**: GPIO40
- **TFT_RESET**: GPIO41
- **TFT_BACKLIGHT**: GPIO45
- **TFT_I2C_POWER**: GPIO7 (power control - automatically managed by board library)

**Note**: The display is automatically configured by the board library. No external wiring is required.

### LED Status Indicators

**CHG LED (Hardware Controlled):**
- **CHG LED**: Yellow LED that indicates battery charging status
- **When charging**: LED is lit
- **When fully charged**: LED turns off
- **No battery**: LED may blink rapidly (expected behavior)
- **Not programmable**: This LED is controlled by the charging circuit

Reference: [Adafruit Power Management](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/power-management)

### Button Behavior Differences

Important: The built-in buttons have different default states and behaviors:

| Button | Default State | When Pressed | Arduino Code |
|--------|---------------|--------------|--------------|
| **A (D0)** | HIGH | Goes LOW | `if (digitalRead(0) == LOW)` |
| **B (D1)** | LOW | Goes HIGH | `if (digitalRead(1) == HIGH)` |
| **C (D2)** | LOW | Goes HIGH | `if (digitalRead(2) == HIGH)` |

This difference is important for deep sleep wake-up functionality and proper button detection in your code.

## Pin Availability and Conflicts

### Reserved Pins (Do Not Use)

| GPIO | Function | Reason |
|------|----------|--------|
| GPIO0 | BOOT/D0 Button | Built-in button A (used by project) |
| GPIO1 | D1 Button | Built-in button B (used by project) |
| GPIO2 | D2 Button | Built-in button C (used by project) |
| GPIO3 | SDA | I2C data line (STEMMA QT) |
| GPIO4 | SCL | I2C clock line (STEMMA QT) |
| GPIO7 | TFT_I2C_POWER | Display and STEMMA QT power control |
| GPIO8 | A5 | ADC input |
| GPIO13 | D13 | External LED (used by project) |
| GPIO15 | A3 | Buzzer (used by project) |
| GPIO21 | NEOPIXEL_POWER | NeoPixel power control |
| GPIO33 | NEOPIXEL | RGB LED |
| GPIO40-45 | TFT Display | Built-in display interface |

### Available Pins for Future Expansion

| GPIO | CircuitPython Name | Functions Available |
|------|-------------------|-------------------|
| GPIO5 | D5 | GPIO, ADC1_CH4, Touch T5 |
| GPIO6 | D6 | GPIO, ADC1_CH5, Touch T6 |
| GPIO9 | D9 | GPIO, ADC1_CH8, Touch T9 |
| GPIO10 | D10 | GPIO, ADC1_CH9, Touch T10 |
| GPIO11 | D11 | GPIO, ADC2_CH0, Touch T11 |
| GPIO16 | A2 | GPIO, ADC2_CH5, Touch T16 |
| GPIO17 | A1 | GPIO, ADC2_CH6, Touch T17 |
| GPIO18 | A0 | GPIO, ADC2_CH7, Touch T18 |
| GPIO35 | MOSI | GPIO, SPI MOSI |
| GPIO36 | SCK | GPIO, SPI SCK |
| GPIO37 | MISO | GPIO, SPI MISO |
| GPIO38 | RX | GPIO, UART RX |
| GPIO39 | TX | GPIO, UART TX |

## Arduino Pin References

**Note**: This project uses Arduino/C++ (not CircuitPython).

When using Arduino, pins are referenced by GPIO number:

```cpp
// Button pins (A, B, C are built-in buttons)
const int BUTTON_A_PIN = 0;         // Button A, pulled HIGH, goes LOW when pressed
const int BUTTON_B_PIN = 1;         // Button B, pulled LOW, goes HIGH when pressed
const int BUTTON_C_PIN = 2;         // Button C, pulled LOW, goes HIGH when pressed

// Audio and LED
const int BUZZER_PIN = 15;
const int LED_PIN = 13;              // External 3mm Green LED via 220Ω resistor
```

## Troubleshooting

### Common Issues

1. **Buttons Not Responding**
   - Verify GPIO pin numbers match settings.h
   - Check for proper GND connections
   - Ensure internal pull-up resistors are enabled

2. **Buzzer Not Working**
   - Verify GPIO15 connection
   - Check buzzer polarity
   - Ensure PWM is properly configured

3. **NeoPixel Not Working**
   - NeoPixel is at GPIO33
   - Power controlled by GPIO21 (NEOPIXEL_POWER)
   - Check board library installation
   - Verify correct board selection in IDE

4. **Display Issues**
   - Built-in display should work automatically
   - Check board library installation
   - Verify correct board selection in IDE

### Pin Testing

Use this simple test code to verify pin connections:

```cpp
void testPins() {
  // Test buttons (A needs INPUT, B/C need INPUT_PULLUP)
  pinMode(BUTTON_A_PIN, INPUT);           // Button A is pulled HIGH externally
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);    // Button B is pulled LOW externally
  pinMode(BUTTON_C_PIN, INPUT_PULLUP);    // Button C is pulled LOW externally
  
  // Test buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  tone(BUZZER_PIN, 1000, 100); // 1kHz for 100ms
  
  // Test LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Turn on LED
  delay(500);
  digitalWrite(LED_PIN, LOW);  // Turn off LED
  
  // Test NeoPixel (Arduino example)
  // #include <Adafruit_NeoPixel.h>
  // Adafruit_NeoPixel pixels(1, 33, NEO_GRB + NEO_KHZ800);
  // pixels.begin();
  // pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Red
  // pixels.show();
  // delay(500);
  // pixels.setPixelColor(0, pixels.Color(0, 0, 0));   // Off
  // pixels.show();
}
```

## Version History

- **v1.0**: Initial pinout documentation based on Adafruit ESP32-S3 Reverse TFT Feather
- **v1.1**: Updated with actual GPIO assignments and conflict resolution 
- **v1.2**: Added optional sense pins for USB (VBUS) and CHG LED and linked Adafruit power docs 