# **Alert TX-1: Retro 80's Beeper**

The **Alert TX-1** is a retro-inspired 80's style beeper built on the Adafruit ESP32-S3 Reverse TFT Feather. Its primary purpose is to **receive alerts from Sentry.io** via an intermediary MQTT service, providing a tangible, distraction-free notification system for your application errors, uptime monitoring, and other alerts.

## **✨ Features**

* **Sentry.io Integration:** Receives alerts from Sentry.io webhooks via an MQTT service  
* **React-like UI Framework:** Modern component-based architecture with 30 FPS performance  
* **14 Built-in Ringtones:** Classic tunes with 42% memory-optimized binary format  
* **Notification LED:** A 3mm green LED provides visual alerts  
* **BeeperHero Game:** Rhythm-based mini-game for entertainment  
* **Smart Power Management:** Low-power sleep modes with wake-on-button functionality

## **📦 Hardware Requirements**

* **[Adafruit ESP32-S3 Reverse TFT Feather](https://www.adafruit.com/product/5691)** (4MB Flash, 2MB PSRAM, STEMMA QT)  
* **[LiPo Battery 400mAh](https://www.adafruit.com/product/3898):** Portable power (designed for Feathers)  
* **[Passive Buzzer 9025](https://www.aliexpress.us/item/2251832678338636.html):** For ringtones and game sounds  
* **3mm Green LED:** For status indications  
* **3 Built-in Buttons:** Physical buttons A, B, C on the ESP32-S3 Feather board

## 🚀 **Build Status**

✅ **Successfully Compiling** - Ready for upload and testing!

- **Sketch Size:** 1,033,171 bytes (71% of ESP32-S3 storage)
- **Memory Usage:** 66,260 bytes (20% of RAM)  
- **14 Ringtones:** Optimized with binary format
- **All Libraries:** Successfully integrated

## **💻 Software Requirements**

* **Arduino IDE:** Version 1.8.19 or newer.  
* **ESP32 Boards Manager:** Version 2.0.x or newer.  
* **Arduino Libraries:** Adafruit GFX Library, Adafruit ST7735 and ST7789 Library, PubSubClient.

This repository includes the **Beeper-Service/** directory, which is intended for the external MQTT service. This service acts as a bridge, connecting **Sentry.io webhooks** to the MQTT broker that your Alert TX-1 device monitors.

## 📚 Documentation

**For complete technical documentation, build instructions, and UI framework details, see [AlertTX-1/README.md](AlertTX-1/README.md)**

Additional documentation is available in the [docs/](AlertTX-1/docs/) directory:

- **[USB Connection Guide](AlertTX-1/docs/usb-connection-guide.md)** - **NEW!** Complete Mac setup and upload instructions
- **[Hardware Setup Guide](AlertTX-1/docs/hardware-setup.md)** - Complete assembly instructions
- **[Pinout Reference](AlertTX-1/docs/pinout-reference.md)** - GPIO assignments and wiring
- **[UI Framework Guide](AlertTX-1/docs/ui-framework-integration.md)** - React-like framework architecture
- **[Ringtone Build System](AlertTX-1/docs/ringtone-build-system.md)** - Audio system documentation

## **🚀 Getting Started**

### **1\. Arduino IDE Setup**

1. **Download and Install Arduino IDE.**  
2. Add ESP32 Board Manager URL in File \> Preferences:  
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package\_esp32\_index.json  
3. **Install ESP32 Boards** via Tools \> Board \> Boards Manager....  
4. **Select Board:** Tools \> Board \> ESP32 Arduino \> Adafruit ESP32-S3 Feather.

### **2\. USB Connection and Setup**

**For detailed Mac-specific USB connection and upload instructions, see [USB Connection Guide](AlertTX-1/docs/usb-connection-guide.md)**

1. **Connect ESP32-S3 Feather to Mac** via USB-C cable
2. **Install drivers** if needed (Silicon Labs CP210x)
3. **Install Arduino CLI** - Use `brew install arduino-cli` or download from GitHub
4. **Verify connection** - Yellow CHG LED should illuminate
5. **Configure Arduino CLI** - Install ESP32 board support

### **3\. Library Installation**

**Libraries are automatically managed by Arduino CLI. Required libraries will be installed automatically during the first build.**

If you need to manually install libraries:
```bash
# Install required libraries via Arduino CLI
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "Adafruit ST7735 and ST7789 Library"
arduino-cli lib install "PubSubClient"
```

### **4\. Project Files**

1. **Clone the Repository:**  
   git clone https://github.com/your-username/AlertTX-1.git  
   cd AlertTX-1

   (Replace with your actual repository URL)  
2. **Open in Arduino IDE:** Open AlertTX-1.ino.

### **5\. Build and Upload**

```bash
# Navigate to the AlertTX-1 directory
cd AlertTX-1

# Generate ringtone data and build project (recommended)
make upload

# Or step by step:
make ringtones  # Generate ringtone data
make build      # Build Arduino project
make upload     # Upload to device (includes build)

# Monitor serial output
make monitor
```

**Alternative**: Use Arduino CLI directly:
```bash
# Compile and upload
arduino-cli compile --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
arduino-cli upload --fqbn esp32:esp32:adafruit_feather_esp32s3_reversetft .
```

### **6\. Configuration**

Edit `AlertTX-1/src/config/settings.h` to set your Wi-Fi credentials, MQTT broker details, and hardware pin assignments.

## **🎮 Usage**

### **Power Management**

The Alert TX-1 defaults to a low-power sleep mode. Perform a **long press** (hold for at least 1 second) on the designated power button to wake it up and enter active mode. It will return to sleep after INACTIVITY\_TIMEOUT\_MS of inactivity.

### **Navigation**

Use **Button A/B** to navigate up/down through menus and **Button C** to select items or go back.

### **Receiving Sentry.io Alerts**

Once connected to Wi-Fi and MQTT, the Alert TX-1 subscribes to the topic defined in settings.h (alerttx1/messages by default). When the Beeper-Service publishes a Sentry.io alert to this topic, the device will wake up, display the message, trigger the LED, and play a ringtone.

### **Ringtones**

The device includes 14 built-in ringtones with optimized binary format. Add custom RTTTL files to `data/ringtones/` and run `make ringtones` to include them. Access the "Ringtones" menu to select and preview sounds.

### **BeeperHero Game**

Access the "Games" menu and select "BeeperHero." Use buttons A, B, C to hit notes as they scroll down, following the rhythm of the selected ringtone.

## **🔧 Customization**

* **Ringtones:** Add RTTTL files to `data/ringtones/` and run `make ringtones`  
* **UI Components:** Create custom components using the React-like framework  
* **Game Difficulty:** Modify game settings in `src/config/settings.h`  
* **Hardware Integration:** Extend with sensors, displays, or other peripherals

## **🤝 Contributing**

Contributions are welcome\! Open an issue or submit a pull request for improvements or new features.

## **📄 License**

This project is open-source and available under the [MIT License](https://www.google.com/search?q=LICENSE).