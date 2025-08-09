# Alert TX-1 - ESP32-S3 Beeper Device

A retro beeper device built on the Adafruit ESP32-S3 Reverse TFT Feather with modern React-like UI framework, RTTTL ringtone playback, and BeeperHero rhythm game.

## Features

### 🎵 Audio System
- **14 Built-in Ringtones** - Classic tunes from Mario, Zelda, Futurama, and more
- **AnyRtttl Integration** - Professional RTTTL playback with binary format optimization (42% memory savings)
- **Non-blocking Audio** - Music plays alongside UI and other systems

### 🎮 BeeperHero Game
- **Rhythm Game** - Play along to ringtones with precise timing
- **Visual Feedback** - Notes scroll across the display
- **Scoring System** - Track your accuracy and combo streaks

### 📱 React-like UI Framework
- **Component-based Architecture** - Reusable UI components (Button, Label, Menu, ProgressBar)
- **Declarative Rendering** - Components declare how they should look
- **State Management** - Automatic dirty tracking and efficient re-rendering
- **30 FPS Performance** - Smooth, responsive interface
- **Memory Efficient** - Optimized for embedded systems

### 🔌 Hardware Integration
- **ESP32-S3 Reverse TFT Feather** - Built-in 240x135 color display
- **Physical Buttons** - Three built-in buttons (A, B, C) for navigation
- **Passive Buzzer** - High-quality audio output on GPIO15
- **Battery Management** - Built-in LiPo charging and monitoring

### 🌐 Connectivity
- **MQTT Support** - Remote alert notifications
- **WiFi Integration** - Wireless connectivity
- **Power Management** - Sleep modes for battery conservation

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
- **LED**: Built-in NeoPixel (GPIO33)

See [Hardware Setup Guide](docs/hardware-setup.md) for detailed assembly instructions.

## Quick Start

### 1. Hardware Setup
1. Connect passive buzzer to GPIO15 (positive) and GND (negative)
2. Optional: Connect LiPo battery for portable operation
3. Ensure all built-in buttons are functional

### 2. Software Installation
1. Install Arduino IDE with ESP32 support
2. Clone this repository
3. Install required libraries:
   - Adafruit GFX Library
   - Adafruit ST7789 Library
   - Any other dependencies listed in the project

### 3. Build and Upload
```bash
# Generate ringtone data
make ringtones

# Build and upload to device
make upload

# Monitor serial output
make monitor
```

### 4. Usage
- **Power On**: Device boots to home screen
- **Navigation**: Use Button A/B to navigate, Button C to select
- **Ringtones**: Access ringtone menu to play/select sounds
- **Game**: Start BeeperHero rhythm game from main menu

## UI Framework

The Alert TX-1 features a custom React-like UI framework designed for embedded systems:

### Architecture
```
src/ui/
├── core/           # Core framework (UIComponent, Screen, UIManager)
├── components/     # Reusable components (Button, Label, Menu)
├── screens/        # Full-screen views (HomeScreen, RingtonesScreen)
└── renderer/       # Display utilities
```

### Creating Custom Screens
```cpp
class CustomScreen : public Screen {
    void build() override {
        // Add components
        titleLabel = addComponent<Label>(10, 10, 220, 20, "Custom Screen");
        
        myButton = addComponent<Button>(10, 50, 100, 30, "Click Me", [this]() {
            Serial.println("Button clicked!");
        });
    }
};
```

### Component Examples
```cpp
// Label with styling
Label* label = addComponent<Label>(10, 10, 100, 20, "Hello", COLOR_WHITE);
label->setTextSize(2);
label->setCenterAlign(true);

// Interactive button
Button* btn = addComponent<Button>(10, 50, 80, 30, "OK", [this]() {
    handleOKClick();
});

// Progress indicator
ProgressBar* progress = addComponent<ProgressBar>(10, 90, 200, 15, 0.75f);
progress->setColors(COLOR_GREEN, COLOR_DARK_GRAY);

// Scrollable menu
Menu* menu = addComponent<Menu>(10, 10, 220, 100);
menu->addItem("Option 1", [this]() { selectOption1(); });
menu->addItem("Option 2", [this]() { selectOption2(); });
```

See [UI Framework Integration Guide](docs/ui-framework-integration.md) for complete documentation.

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

### Binary Format Optimization
- Automatic conversion to binary format
- 42% memory savings over text RTTTL
- Faster loading and playback
- Maintains full compatibility with AnyRtttl library

See [Ringtone Build System](docs/ringtone-build-system.md) for details.

## BeeperHero Game

A rhythm game where you play along to ringtones:

1. **Select Song**: Choose from available ringtones
2. **Hit Notes**: Press buttons as notes scroll across screen
3. **Score Points**: Timing accuracy determines your score
4. **Build Combos**: Consecutive hits increase your multiplier

### Controls
- **Button A**: Hit note lane 1
- **Button B**: Hit note lane 2
- **Button C**: Hit note lane 3

See [Game Documentation](src/game/README.md) for more details.

## Development

### Build System
```bash
make ringtones    # Generate ringtone data
make build        # Compile project
make upload       # Upload to device
make monitor      # Serial monitor
make clean        # Clean generated files
make help         # Show all commands
```

### Project Structure
```
AlertTX-1/
├── AlertTX-1.ino           # Main Arduino sketch
├── src/
│   ├── ui/                 # UI framework
│   ├── hardware/           # Hardware abstraction
│   ├── ringtones/          # Audio system
│   ├── game/               # BeeperHero game
│   ├── mqtt/               # MQTT client
│   └── config/             # Configuration
├── data/
│   └── ringtones/          # RTTTL source files
├── docs/                   # Documentation
└── tools/                  # Build utilities
```

### Performance Characteristics
- **Frame Rate**: 30 FPS UI rendering
- **Memory Usage**: ~2-4KB for typical screen
- **Audio Latency**: <10ms for button response
- **Battery Life**: 8-12 hours typical usage
- **Boot Time**: <2 seconds

## Documentation

- [Hardware Setup Guide](docs/hardware-setup.md) - Complete assembly instructions
- [Pinout Reference](docs/pinout-reference.md) - GPIO assignments and wiring
- [UI Framework Guide](docs/ui-framework-integration.md) - Framework architecture and usage
- [Ringtone Build System](docs/ringtone-build-system.md) - Audio system documentation
- [AnyRtttl Integration](docs/anyrtttl-integration.md) - Audio library details
- [Power Management](docs/power-management.md) - Battery and sleep modes

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add your improvements
4. Update documentation
5. Submit a pull request

### Adding New Components
The UI framework is designed for easy extension. See the [UI Framework Guide](docs/ui-framework-integration.md) for details on creating custom components and screens.

### Adding New Ringtones
Simply add RTTTL files to `data/ringtones/` and run `make ringtones` to include them in the build.

## License

This project is open source. See LICENSE file for details.

## Hardware Links

- [Adafruit ESP32-S3 Reverse TFT Feather](https://www.adafruit.com/product/5691)
- [Passive Piezo Buzzer](https://www.adafruit.com/product/160)
- [LiPo Battery](https://www.adafruit.com/product/1578) (optional)

## Credits

- **AnyRtttl Library** - Professional RTTTL playback
- **Adafruit Libraries** - Display and hardware support
- **ESP32 Arduino Core** - ESP32 platform support
- **RTTTL Community** - Ringtone collection

---

**Alert TX-1** - Where retro meets modern embedded development! 🎵🎮📱