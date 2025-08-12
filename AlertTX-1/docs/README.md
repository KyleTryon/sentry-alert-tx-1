# AlertTX-1 Documentation

Welcome to the AlertTX-1 documentation! This ESP32-S3 based device provides a modern retro beeper experience with a component-based UI framework.

## 🚀 Quick Start

### For New Users
1. **[Hardware Setup](hardware-setup.md)** - Board setup and component wiring
2. **[Display Troubleshooting](display-troubleshooting.md)** - Fix common display issues
3. **[UI Framework Guide](ui-framework-guide.md)** - Understanding the component system

### For Contributors  
1. **[UI Framework Guide](ui-framework-guide.md)** - Component architecture and APIs
2. **[Hardware Setup](hardware-setup.md)** - Understanding the hardware platform
3. **[Pinout Reference](pinout-reference.md)** - Complete GPIO mapping

## 📚 Documentation Guide

### 🎨 User Interface
- **[UI Framework Guide](ui-framework-guide.md)** - Component-based UI system, themes, and screens
- **[Icon System](icon-system.md)** - Converting PNG icons to RGB565 headers

### 🔧 Hardware & Setup
- **[Hardware Setup](hardware-setup.md)** - ESP32-S3 Feather board configuration
- **[Display Troubleshooting](display-troubleshooting.md)** - Critical display initialization fixes
- **[Pinout Reference](pinout-reference.md)** - Complete GPIO assignments and wiring
- **[Power Management](power-management.md)** - Battery optimization strategies

### 🎵 Audio System
- **[Audio System Guide](audio-system.md)** - RTTTL ringtones and audio integration

## 🏗️ Architecture Overview

The AlertTX-1 uses a modern component-based architecture:

```
AlertTX-1/src/
├── config/           # Hardware configuration
├── ui/               # UI Framework
│   ├── core/         # Component, Screen, ScreenManager, Theme
│   ├── components/   # MenuItem, MenuContainer, Clickable
│   └── screens/      # MainMenu, Alerts, Settings, etc.
├── hardware/         # Button, LED, Buzzer management
├── icons/           # RGB565 icon data
└── ringtones/       # RTTTL audio data
```

## 🎮 Controls

- **Button A (GPIO0):** Navigate Up/Previous
- **Button B (GPIO1):** Navigate Down/Next  
- **Button C (GPIO2):** Select/Enter
- **Long Press (1.5s):** Go back to previous screen

## 🎯 Current Features

- **Modern UI Framework** - Component-based architecture with screens and navigation
- **Multiple Themes** - Default, Terminal, Amber, and High Contrast themes
- **Icon System** - 26+ RGB565 icons with flexible sizing
- **Audio System** - RTTTL ringtone playback with build system
- **Navigation** - Full screen hierarchy with back navigation
- **Hardware Integration** - Optimized for ESP32-S3 Reverse TFT Feather

## 📖 Additional Resources

- **[Adafruit ESP32-S3 Reverse TFT Feather](https://learn.adafruit.com/adafruit-esp32-s3-reverse-tft-feather)** - Official hardware guide
- **[Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)** - Graphics library documentation
- **[ESP32-S3 Reference](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)** - ESP32-S3 technical documentation

---

**Need help?** Start with the [Hardware Setup Guide](hardware-setup.md) and [UI Framework Guide](ui-framework-guide.md) for comprehensive getting started information.
