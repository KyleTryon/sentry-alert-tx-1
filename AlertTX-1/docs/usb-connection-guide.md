# USB Connection and Upload Guide for Mac

## Overview

This guide provides step-by-step instructions for connecting your **Adafruit ESP32-S3 Reverse TFT Feather** to your Mac and uploading the Alert TX-1 code using **Arduino CLI** and the project's **Makefile**. This process works for both Intel and Apple Silicon Macs.

## 🚀 Quick Start

1. **Connect USB Cable** - Use USB-C cable to connect ESP32-S3 Feather to Mac
2. **Install Drivers** - Install Silicon Labs CP210x USB drivers (if needed)
3. **Install Arduino CLI** - Set up command-line tools
4. **Upload Code** - Use `make upload` command

## Prerequisites

### Hardware Requirements
- **Mac** (Intel or Apple Silicon) running macOS 10.15 or later
- **Adafruit ESP32-S3 Reverse TFT Feather** board
- **USB-C to USB-C cable** (or USB-C to USB-A with adapter)
- **Internet connection** for downloading drivers and tools

### Software Requirements
- **Arduino CLI** (command-line interface)
- **Python 3** (for ringtone data generation)
- **ESP32 Board Support** (automatically installed via arduino-cli)

## Step 1: Hardware Connection

### Connect the ESP32-S3 Feather

1. **Locate the USB-C Port**
   - The ESP32-S3 Feather has a **USB-C port** on the edge of the board
   - It's labeled "USB" on the board silk screen

2. **Connect USB Cable**
   - Use a **USB-C to USB-C cable** (recommended)
   - Or use **USB-C to USB-A cable** with adapter if needed
   - **Important**: Ensure the cable supports **data transfer** (not just charging)

3. **Verify Connection**
   - **Yellow CHG LED** should illuminate (indicates USB power)
   - **Red PWR LED** should be on (indicates 3.3V power)
   - **Blue NeoPixel** may blink briefly during boot

### USB Cable Requirements

```
Recommended Cables:
┌─────────────────────────────────────────────────┐
│ USB-C to USB-C (Preferred)                     │
│ • Full Thunderbolt 3/4 cable                   │
│ • USB 3.1 Gen 1 cable                          │
│ • Any USB-C cable that supports data transfer  │
│                                                 │
│ USB-C to USB-A (Alternative)                   │
│ • USB 3.0 cable with adapter                   │
│ • Ensure adapter supports data transfer        │
└─────────────────────────────────────────────────┘
```

**⚠️ Avoid**: Charging-only USB-C cables (they won't work for programming)

## Step 2: Driver Installation

### Check if Drivers are Needed

1. **Open Terminal** and run:
   ```bash
   ls /dev/tty.usb*
   ```

2. **If you see entries like**:
   - `/dev/tty.usbserial-*` → Drivers already installed ✅
   - `/dev/tty.usbmodem*` → Drivers already installed ✅
   - **No entries** → Need to install drivers

### Install Silicon Labs CP210x Drivers

**For most recent ESP32-S3 boards, drivers should work automatically. If you encounter issues:**

1. **Download Drivers**
   - Visit: [Silicon Labs CP210x Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
   - Download **macOS version** for your Mac type:
     - **Intel Macs**: `CP210x_MacOS_Drivers.pkg`
     - **Apple Silicon Macs**: `CP210x_macOS_Drivers.pkg`

2. **Install Drivers**
   ```bash
   # For Intel Macs
   sudo installer -pkg CP210x_MacOS_Drivers.pkg -target /
   
   # For Apple Silicon Macs
   sudo installer -pkg CP210x_macOS_Drivers.pkg -target /
   ```

3. **Restart Terminal** or run:
   ```bash
   sudo killall -9 com.apple.usbd
   ```

4. **Verify Installation**
   ```bash
   ls /dev/tty.usb*
   # Should show: /dev/tty.usbserial-XXXXXX
   ```

## Step 3: Arduino CLI Setup

### Install Arduino CLI

1. **Install via Homebrew** (recommended)
   ```bash
   # Install Homebrew if not already installed
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   
   # Install Arduino CLI
   brew install arduino-cli
   ```

2. **Alternative Installation** (manual download)
   ```bash
   # Download from GitHub releases
   # Visit: https://github.com/arduino/arduino-cli/releases
   # Download: arduino-cli_latest_macOS_64bit.tar.gz
   
   # Extract and install
   tar -xzf arduino-cli_latest_macOS_64bit.tar.gz
   sudo mv arduino-cli /usr/local/bin/
   ```

3. **Verify Installation**
   ```bash
   arduino-cli version
   # Should show version information
   ```

### Configure Arduino CLI

1. **Create Configuration**
   ```bash
   arduino-cli config init
   ```

2. **Add ESP32 Board Support**
   ```bash
   # Add ESP32 board manager URL
   arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   
   # Update board index
   arduino-cli core update-index
   
   # Install ESP32 core
   arduino-cli core install esp32:esp32
   ```

3. **Verify ESP32 Installation**
   ```bash
   arduino-cli board listall | grep esp32
   # Should show ESP32 boards including Adafruit ESP32-S3 Feather
   ```

## Step 4: Project Setup

### Clone and Setup Project

1. **Clone Repository** (if not already done)
   ```bash
   git clone https://github.com/your-username/AlertTX-1.git
   cd AlertTX-1/AlertTX-1
   ```

2. **Verify Project Structure**
   ```bash
   ls -la
   # Should show:
   # AlertTX-1.ino
   # Makefile
   # src/
   # data/
   # tools/
   ```

3. **Check Makefile Commands**
   ```bash
   make help
   # Shows all available commands
   ```

## Makefile Commands Reference

### Available Commands

```bash
make detect-board - Check if ESP32-S3 Feather is connected
make help          # Show all available commands
make ringtones     # Generate ringtone data from RTTTL files (with smart caching)
make build         # Build Arduino project (includes ringtones)
make upload        # Upload to device (includes board detection)
make monitor       # Start serial monitor (includes board detection)
make dev           # Upload and automatically start monitor (dev mode)
make clean         # Remove generated files and cache
make all           # Generate ringtones and build (default)
```

### Command Examples

```bash
# Check board connection first
make detect-board   # Verify board is connected and ready

# Full development cycle (recommended for development)
make dev           # Upload and start monitoring automatically

# Quick upload only (uses caching for speed)
make upload        # Regenerate ringtones (if needed), build, and upload

# Monitor only
make monitor       # Start serial monitor for debugging

# Force regeneration (ignores cache)
make clean         # Clear cache and generated files
make ringtones     # Regenerate ringtone data
```

## Step 5: Upload Code

### Method 1: Using Makefile (Recommended)

1. **Check Board Connection** (optional but recommended)
   ```bash
   make detect-board
   ```

2. **Generate Ringtone Data** (with caching)
   ```bash
   make ringtones
   ```

3. **Build and Upload**
   ```bash
   make upload
   ```

4. **Monitor Serial Output** (optional)
   ```bash
   make monitor
   ```

### Method 2: Dev Mode (Recommended for Development)

**New in Alert TX-1**: Use dev mode for an improved development workflow:

```bash
# Upload and automatically start monitoring
make dev
```

This command:
- ✅ Checks board connection
- ✅ Generates ringtone data (with smart caching)
- ✅ Builds the project
- ✅ Uploads to device
- ✅ Automatically starts serial monitor
- 🎯 Perfect for iterative development

### Method 3: Direct Arduino CLI Commands

1. **List Available Ports**
   ```bash
   arduino-cli board list
   # Should show your ESP32-S3 Feather port
   ```

2. **Compile Project**
   ```bash
   arduino-cli compile --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
   ```

3. **Upload to Device**
   ```bash
   arduino-cli upload --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
   ```

## Step 6: Board Detection

### Automatic Board Detection

The Alert TX-1 project includes automatic board detection:

```bash
# Check if board is connected and ready
make detect-board
```

**What it checks:**
- ✅ USB connection status
- ✅ Driver installation
- ✅ Arduino CLI availability
- ✅ Port assignment
- ✅ Board compatibility

**Example output when connected:**
```
🔍 Detecting ESP32-S3 Feather board...
✅ Board detected: Adafruit ESP32-S3 Reverse TFT Feather
   Port: /dev/tty.usbserial-XXXXXX
   Label: USB Serial Device
   Protocol: serial
🎯 Board is ready for upload/monitoring
```

**Example output when not connected:**
```
🔍 Detecting ESP32-S3 Feather board...
❌ Adafruit ESP32-S3 Reverse TFT Feather not found
   Available ports:
     - /dev/cu.debug-console (/dev/cu.debug-console)

🔧 Troubleshooting tips:
   1. Check USB cable connection
   2. Install drivers if needed (Silicon Labs CP210x)
   3. Try a different USB port
   4. Check if board appears in System Preferences > Network

⚠️  Board not connected - some commands may fail
```

### Method 3: Step-by-Step Build Process

1. **Generate Ringtone Data**
   ```bash
   python3 tools/generate_ringtone_data.py
   ```

2. **Compile Project**
   ```bash
   arduino-cli compile --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
   ```

3. **Upload to Device**
   ```bash
   arduino-cli upload --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
   ```

## Step 6: Verification and Testing

### Verify Upload Success

1. **Check Serial Monitor**
   ```bash
   make monitor
   # Or: arduino-cli monitor --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft
   ```

2. **Expected Output**
   ```
   [INFO] Alert TX-1 Starting...
   [INFO] Display initialized
   [INFO] Button manager ready
   [INFO] Ringtone player ready
   ```

3. **Check Device Behavior**
   - **TFT Display** should show home screen
   - **Built-in buttons** should respond to presses
   - **LED** should illuminate during startup
   - **Buzzer** should play test tone (if connected)

### Troubleshooting Upload Issues

#### Common Issues and Solutions

1. **Port Not Found**
   ```bash
   Error: "Port not found" or "No serial ports available"
   
   Solution:
   • Check USB cable (try different cable)
   • Verify drivers are installed
   • Run: arduino-cli board list
   • Try different USB port
   ```

2. **Upload Timeout**
   ```bash
   Error: "Failed to connect to ESP32: Timed out"
   
   Solution:
   • Hold BOOT button (Button A) while uploading
   • Release BOOT button after upload starts
   • Try lower upload speed (115200)
   ```

3. **Permission Denied**
   ```bash
   Error: "Permission denied: /dev/tty.usbserial-XXXXXX"
   
   Solution:
   sudo chmod 666 /dev/tty.usbserial-XXXXXX
   # Or add user to dialout group
   sudo usermod -a -G dialout $USER
   ```

4. **Compilation Errors**
   ```bash
   Error: "Library not found" or compilation errors
   
   Solution:
   • Check if ESP32 core is installed: arduino-cli core list
   • Install missing core: arduino-cli core install esp32:esp32
   • Verify board selection is correct
   ```

## Step 7: Development Workflow

### Daily Development Process

1. **Start Development Session**
   ```bash
   cd AlertTX-1/AlertTX-1
   make clean  # Clean previous builds
   ```

2. **Make Changes**
   - Edit source files in `src/` directory
   - Add new ringtones to `data/ringtones/`

3. **Test Changes**
   ```bash
   make upload  # Build and upload
   make monitor # Monitor output (optional)
   ```

### Adding New Ringtones

1. **Add RTTTL File**
   ```bash
   # Add your .rtttl file to data/ringtones/
   cp your-ringtone.rtttl data/ringtones/
   ```

2. **Regenerate Data**
   ```bash
   make ringtones
   ```

3. **Upload Updated Code**
   ```