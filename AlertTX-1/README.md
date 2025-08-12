# Alert TX-1 - ESP32-S3 Beeper Device

A simple beeper device built on the Adafruit ESP32-S3 Reverse TFT Feather with FlipperZero-inspired UI and RTTTL ringtone playback.

## 🚀 Project Status

✅ **Working and Ready to Use!**

- **Navigation:** A=Up, B=Down, C=Select - fully responsive
- **Display:** Clean menu-based interface
- **Audio:** Built-in ringtones with buzzer playback
- **Memory:** Efficient use of ESP32-S3 resources

## Features

### 🎵 Audio System
- **14 Built-in Ringtones** - Classic tunes from Mario, Zelda, Futurama, and more
- **RTTTL Playback** - Standard ringtone format support
- **Passive Buzzer** - Audio output on GPIO15

### 📱 Simple UI
- **FlipperZero Style** - Clean ">" selection indicator with yellow highlight
- **Menu Navigation** - Smooth up/down movement
- **Button Integration** - Responsive hardware button controls
- **Status Display** - Simple text-based interface

### 🔌 Hardware Integration
- **ESP32-S3 Reverse TFT Feather** - Built-in 240x135 color display
- **Physical Buttons** - Three built-in buttons for navigation
- **LED Support** - Status indicator integration
- **Battery Support** - Built-in LiPo charging

## Hardware Requirements

### Core Components
- **Adafruit ESP32-S3 Reverse TFT Feather** (Product ID: 5691)
- **Passive Piezo Buzzer** - Connected to GPIO15
- **LiPo Battery** (optional) - For portable operation

### Pin Configuration
- **Button A**: GPIO0 (built-in)
- **Button B**: GPIO1 (built-in)  
- **Button C**: GPIO2 (built-in)
- **Buzzer**: GPIO15 (A3)
- **Display**: Built-in TFT (GPIO40-45)
- **LED**: External 3mm Green LED (GPIO13)
- **NeoPixel**: Built-in RGB LED (GPIO33) - optional

See [Hardware Setup Guide](docs/hardware-setup.md) for detailed assembly instructions.

## Prerequisites

### Software Requirements
- **Arduino CLI**: Command-line Arduino development tool
- **ESP32-S3 Board Package**: ESP32 development board support
- **Python 3**: For build tools and data generation
- **Required Libraries**: Automatically installed via `make libraries`
- **Python Dependencies**: Automatically installed via `make python-deps` (Pillow for image processing)

## Quick Start

### 1. Hardware Setup
1. Connect LiPo battery to JST-PH connector for portable operation
2. Connect passive buzzer to GPIO15 (positive) and GND (negative)
3. Connect 3mm green LED to GPIO13 (anode via 220Ω resistor) and GND (cathode)
4. Ensure all built-in buttons are functional

### 2. Software Setup
1. Install Arduino IDE with ESP32 support
2. Clone this repository
3. Install required libraries:
   ```bash
   make libraries
   ```

### 3. Build and Upload
```bash
# Generate ringtone data
make ringtones

# Upload to device
make upload

# Monitor serial output (optional)
make monitor
```

### 4. Usage
- **Power On**: Device boots to main menu
- **Navigation**: Use Button A (Up), Button B (Down), Button C (Select)
- **Menu Items**: Navigate between Alerts, Games, and Settings

## Simple UI System

The Alert TX-1 uses a clean, simple menu system:

### Architecture
```
src/ui/
├── Menu.h/cpp      # Simple menu component
src/hardware/
├── ButtonManager/  # Button handling with debouncing
```

### How It Works
```cpp
// Basic menu setup
Menu mainMenu(&tft);
ButtonManager buttonManager;

// Menu items with callbacks
MenuItem menuItems[] = {
  {"Alerts", 1, alertsAction},
  {"Games", 2, gamesAction}, 
  {"Settings", 3, settingsAction}
};

// Navigation loop
if (buttonManager.wasPressed(ButtonManager::BUTTON_A)) {
    mainMenu.moveUp();
    drawUI();
}
```

See [UI Framework Guide](docs/UI_FRAMEWORK_GUIDE.md) for complete documentation.

## Ringtone System

### Built-in Ringtones
The device includes 14 classic ringtones:
- Mario Theme
- Zelda Theme  
- Futurama Theme
- The Simpsons
- Digimon Theme
- Spider-Man Theme
- Take On Me
- Never Gonna Give You Up
- And more!

### Adding Custom Ringtones
1. Add RTTTL files to `data/ringtones/`
2. Run `make ringtones` to regenerate data
3. Ringtones automatically appear in the menu

See [Ringtone Build System](docs/ringtone-build-system.md) for details.

## Development

### Build Commands
```bash
make libraries   - Install required Arduino libraries
make ringtones   - Generate ringtone data from RTTTL files
make upload      - Build and upload to device
make monitor     - Serial monitor for debugging
make clean       - Clean generated files
```

### Project Structure
```
AlertTX-1/
├── AlertTX-1.ino           # Main Arduino sketch
├── src/
│   ├── ui/                 # Simple menu system
│   ├── hardware/           # Button and LED control
│   ├── ringtones/          # Audio system
│   ├── mqtt/               # MQTT client (basic)
│   └── config/             # Hardware configuration
├── data/ringtones/         # RTTTL source files
├── docs/                   # Documentation
└── tools/                  # Build utilities
```

## Documentation

- [UI Framework Guide](docs/UI_FRAMEWORK_GUIDE.md) - Simple menu system documentation
- [Hardware Setup Guide](docs/hardware-setup.md) - Assembly instructions
- [Pinout Reference](docs/pinout-reference.md) - GPIO assignments
- [USB Connection Guide](docs/usb-connection-guide.md) - Setup and upload instructions

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add your improvements
4. Submit a pull request

### Adding New Ringtones
Add RTTTL files to `data/ringtones/` and run `make ringtones` to include them in the build.

## License

This project is open source. See LICENSE file for details.

## Hardware Links

- [Adafruit ESP32-S3 Reverse TFT Feather](https://www.adafruit.com/product/5691)
- [Passive Piezo Buzzer](https://www.adafruit.com/product/160)
- [LiPo Battery](https://www.adafruit.com/product/1578) (optional)

## Credits

- **AnyRtttl Library** - RTTTL playback support
- **Adafruit Libraries** - Display and hardware support
- **ESP32 Arduino Core** - ESP32 platform support

---

**Alert TX-1** - A simple, working beeper device for ESP32-S3!
