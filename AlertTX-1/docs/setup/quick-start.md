# Quick Start Guide

Get your AlertTX-1 up and running in 5 minutes! This guide covers the essentials to start receiving alerts.

## 📦 What You'll Need

### Hardware
- **Adafruit ESP32-S3 Reverse TFT Feather** ([Product #5691](https://www.adafruit.com/product/5691))
- **Passive Buzzer** (9025 size recommended)
- **3mm Green LED** + 220Ω resistor
- **USB-C cable** for programming
- **Optional**: LiPo battery (400mAh recommended)

### Software
- **Arduino IDE** or **PlatformIO**
- **Python 3.7+** (for build tools)
- **Git** (to clone the repository)

## 🚀 Quick Setup

### 1. Clone the Repository
```bash
git clone https://github.com/techsquidtv/sentry_alert_tx-1.git
cd sentry_alert_tx-1/AlertTX-1
```

### 2. Install Dependencies
```bash
# Install Python dependencies
make python-deps

# Install Arduino libraries
make libraries
```

### 3. Wire the Components
Connect these three components to your ESP32-S3 Feather:

| Component | Connection |
|-----------|------------|
| **Buzzer (+)** | GPIO14 (A4) |
| **Buzzer (-)** | GND |
| **LED Anode** | GPIO18 (A0) via 220Ω resistor |
| **LED Cathode** | GND |

> **Note**: The board has 3 built-in buttons (A, B, C) - no external buttons needed!

### 4. Configure MQTT (Optional)
Edit `AlertTX-1/AlertTX-1.ino` to add your MQTT settings:

```cpp
// WiFi Configuration
const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-password";

// MQTT Configuration  
const char* mqtt_server = "your-mqtt-broker";
const int mqtt_port = 1883;
const char* mqtt_topic = "alerts/beeper";
```

### 5. Build and Upload
```bash
# Build and upload in one command
make upload

# Or with serial monitor
make dev
```

## 🎮 Using Your AlertTX-1

### Controls
- **Button A** (GPIO0): Up/Previous
- **Button B** (GPIO1): Down/Next  
- **Button C** (GPIO2): Select/Enter
- **Long Press Any** (1.5s): Back/Exit

### Navigation
```
Splash Screen (2s)
    ↓
Main Menu
    ├── Alerts    → View received alerts
    ├── Games     → Play BeeperHero, Pong, Snake
    └── Settings  → Configure themes, ringtones
```

### First Boot Checklist
✅ Display shows "SENTRY" splash screen  
✅ Buzzer plays startup sound  
✅ LED blinks during boot  
✅ Main menu appears after splash  
✅ Buttons navigate menu items  

## 📡 Sending Test Alerts

### MQTT Message Format
Send JSON messages to your configured MQTT topic:

```json
{
  "issue_id": "PROJ-123",
  "project_name": "My Project",
  "title": "Error in production",
  "culprit": "api/endpoint",
  "level": "error"
}
```

### Test with mosquitto_pub
```bash
mosquitto_pub -h your-mqtt-broker -t "alerts/beeper" -m '{
  "issue_id": "TEST-001",
  "project_name": "Test Alert",
  "title": "This is a test alert",
  "level": "warning"
}'
```

## 🎨 Customization

### Change Theme
1. Navigate to **Settings → Themes**
2. Browse with A/B buttons (preview in real-time)
3. Press C to save selection

### Change Ringtone  
1. Navigate to **Settings → Ringtones**
2. Select from 16+ built-in ringtones
3. Hear preview on selection

### Add Custom Ringtones
1. Add `.rtttl.txt` files to `data/ringtones/`
2. Run `make ringtones`
3. Rebuild and upload

## 🎮 Play Games

The AlertTX-1 includes three built-in games:

1. **BeeperHero** - Guitar Hero-style rhythm game
2. **Pong** - Classic arcade game
3. **Snake** - Retro snake game

Access games from **Main Menu → Games**.

## 🔧 Troubleshooting

### Display Issues
If the display only shows backlight:
- Check [Display Troubleshooting Guide](display-troubleshooting.md)
- Ensure proper board selection in Arduino IDE

### No Sound
- Verify buzzer wiring (GPIO14/A4)
- Check buzzer polarity
- Test with different ringtone

### Build Errors
```bash
# Clean and rebuild
make clean
make build
```

## 📚 Next Steps

- **[Hardware Setup](hardware-setup.md)** - Detailed assembly instructions
- **[Pinout Reference](pinout-reference.md)** - Complete wiring diagram
- **[UI Framework](../development/ui-framework.md)** - Develop custom screens
- **[Contributing](../../CONTRIBUTING.md)** - Join the project!

## 🆘 Getting Help

- **Issues**: [GitHub Issues](https://github.com/techsquidtv/sentry_alert_tx-1/issues)
- **Discussions**: [GitHub Discussions](https://github.com/techsquidtv/sentry_alert_tx-1/discussions)

---

**Happy Beeping!** 🎵
