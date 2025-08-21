# Software Installation Guide

Complete guide for setting up the development environment for AlertTX-1.

## 📋 Prerequisites

### Operating System Support
- ✅ **macOS** (10.14+)
- ✅ **Linux** (Ubuntu 18.04+, Debian, Arch)
- ✅ **Windows** (10/11 with WSL2)

### Required Software
1. **Python 3.7+** with pip
2. **Arduino CLI** or **Arduino IDE 2.0+**
3. **Git** for version control
4. **Make** (build automation)

## 🔧 Installation Steps

### 1. Install Python Dependencies

#### macOS
```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Python
brew install python3

# Verify installation
python3 --version
pip3 --version
```

#### Linux (Ubuntu/Debian)
```bash
# Update package list
sudo apt update

# Install Python and pip
sudo apt install python3 python3-pip

# Verify installation
python3 --version
pip3 --version
```

#### Windows (WSL2)
```bash
# In WSL2 terminal
sudo apt update
sudo apt install python3 python3-pip make
```

### 2. Install Arduino Environment

#### Option A: Arduino IDE (Recommended for Beginners)
1. Download from [arduino.cc](https://www.arduino.cc/en/software)
2. Install for your platform
3. Open Arduino IDE
4. Add ESP32 board support:
   - File → Preferences
   - Additional Board Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
5. Tools → Board → Board Manager
6. Search "esp32" and install "esp32 by Espressif Systems"

#### Option B: Arduino CLI (Recommended for Advanced Users)
```bash
# Install Arduino CLI
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
sudo mv bin/arduino-cli /usr/local/bin/

# Configure
arduino-cli config init
arduino-cli core update-index

# Install ESP32 core
arduino-cli core install esp32:esp32

# Install required libraries
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "Adafruit ST7789"
arduino-cli lib install "ArduinoJson"
arduino-cli lib install "PubSubClient"
```

### 3. Clone the Repository
```bash
# Clone the project
git clone https://github.com/techsquidtv/sentry_alert_tx-1.git
cd sentry_alert_tx-1/AlertTX-1

# Install Python dependencies
pip3 install -r tools/requirements.txt
```

### 4. Install Project Libraries

The project includes a Makefile for easy setup:

```bash
# Install all required libraries automatically
make libraries

# Or install Python dependencies only
make python-deps
```

### Manual Library Installation (if needed):
```bash
# Using Arduino CLI
arduino-cli lib install "Adafruit GFX Library"@1.11.5
arduino-cli lib install "Adafruit ST7789"@1.10.0
arduino-cli lib install "ArduinoJson"@6.21.3
arduino-cli lib install "PubSubClient"@2.8
arduino-cli lib install "Adafruit NeoPixel"@1.11.0
```

## 🛠️ Development Tools

### Recommended VS Code Extensions
1. **Arduino** (Microsoft)
2. **C/C++** (Microsoft)
3. **Python** (Microsoft)
4. **Serial Monitor**

### VS Code Setup
```json
// .vscode/settings.json
{
  "arduino.path": "/Applications/Arduino.app/Contents/MacOS",
  "arduino.commandPath": "arduino-cli",
  "arduino.additionalUrls": [
    "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json"
  ],
  "C_Cpp.default.includePath": [
    "${workspaceFolder}/src/**",
    "${workspaceFolder}/lib/**"
  ]
}
```

## 📦 Board Configuration

### Select the Correct Board

#### In Arduino IDE
1. Tools → Board → ESP32 Arduino → **Adafruit Feather ESP32-S3 Reverse TFT**
2. Tools → Port → Select your device port

#### Using Arduino CLI
```bash
# List connected boards
arduino-cli board list

# Compile for the correct board
arduino-cli compile --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft AlertTX-1.ino
```

### Important Board Settings
- **Board**: Adafruit Feather ESP32-S3 Reverse TFT
- **Upload Speed**: 921600
- **CPU Frequency**: 240MHz
- **Flash Mode**: QIO
- **Flash Size**: 4MB
- **Partition Scheme**: Default 4MB with spiffs

## 🔨 Build System

The project uses Make for automation:

```bash
# Show all available commands
make help

# Common commands
make build      # Compile the project
make upload     # Build and upload to device
make dev        # Upload and open serial monitor
make clean      # Clean build artifacts
make ringtones  # Generate ringtone data
make icons      # Convert PNG icons to headers
```

### Makefile Configuration

The Makefile auto-detects your environment, but you can override:

```bash
# Override Arduino CLI path
ARDUINO_CLI=/custom/path/arduino-cli make build

# Override board FQBN
BOARD_FQBN=esp32:esp32:custom_board make build

# Override port
PORT=/dev/ttyUSB0 make upload
```

## 🐛 Troubleshooting

### Common Issues

#### "Board not found"
```bash
# Install ESP32 board support
arduino-cli core install esp32:esp32

# Update board index
arduino-cli core update-index
```

#### "Library not found"
```bash
# Reinstall all libraries
make clean-libs
make libraries
```

#### "Permission denied on port"
```bash
# Linux/macOS: Add user to dialout group
sudo usermod -a -G dialout $USER
# Logout and login again

# Or use sudo (not recommended)
sudo make upload
```

#### Python Module Errors
```bash
# Ensure pip is up to date
pip3 install --upgrade pip

# Reinstall requirements
pip3 install -r tools/requirements.txt --force-reinstall
```

### Verify Installation

Run the verification script:
```bash
# Check all dependencies
python3 tools/verify_setup.py
```

Expected output:
```
✅ Python 3.7+ found
✅ Arduino CLI found
✅ ESP32 core installed
✅ Required libraries installed
✅ Build tools ready
```

## 📚 Next Steps

1. **[Hardware Setup](hardware-setup.md)** - Wire up your components
2. **[Quick Start](quick-start.md)** - Get running in 5 minutes
3. **[Build System](../development/build-system.md)** - Advanced build options

## 🔗 Additional Resources

- [ESP32 Arduino Core Documentation](https://docs.espressif.com/projects/arduino-esp32/)
- [Adafruit Feather ESP32-S3 Guide](https://learn.adafruit.com/esp32-s3-reverse-tft-feather)
- [Arduino CLI Documentation](https://arduino.github.io/arduino-cli/)

---

**Need help?** Check our [Troubleshooting Guide](../troubleshooting.md) or open an [issue](https://github.com/techsquidtv/sentry_alert_tx-1/issues).
