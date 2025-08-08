# **Alert TX-1: Retro 80's Beeper**

The **Alert TX-1** is a retro-inspired 80's style beeper built on the Adafruit ESP32-S3 Reverse TFT Feather. Its primary purpose is to **receive alerts from Sentry.io** via an intermediary MQTT service, providing a tangible, distraction-free notification system for your application errors, uptime monitoring, and other alerts.

## **✨ Features**

* **Sentry.io Integration:** Receives alerts from Sentry.io webhooks via an MQTT service.  
* **Wireless Messaging:** Communicates over Wi-Fi using the MQTT protocol.  
* **Retro UI:** Features a basic, colored menu system with navigation, styled like an 80's cellphone.  
* **Intuitive Navigation:** Uses four physical buttons for menu navigation and interaction.  
* **Customizable Ringtones:** Plays custom melodies on a passive buzzer.  
* **Notification LED:** An onboard LED provides visual alerts.  
* **Beeper Hero Mini-Game:** A simple rhythm-based game for entertainment.  
* **Smart Power Management:** Defaults to a low-power sleep mode and wakes with a long button press, eliminating the need for a physical power switch.

## **📦 Hardware Requirements**

* **Adafruit ESP32-S3 Reverse TFT Feather** (4MB Flash, 2MB PSRAM, STEMMA QT)  
* **4 Push Buttons:** Three vertical on left, one on right.  
* **Passive Buzzer:** For ringtones and game sounds.  
* **Onboard LED:** For status indications.

## **💻 Software Requirements**

* **Arduino IDE:** Version 1.8.19 or newer.  
* **ESP32 Boards Manager:** Version 2.0.x or newer.  
* **Arduino Libraries:** Adafruit GFX Library, Adafruit ST7735 and ST7789 Library, PubSubClient.

This repository includes the **Beeper-Service/** directory, which is intended for the external MQTT service. This service acts as a bridge, connecting **Sentry.io webhooks** to the MQTT broker that your Alert TX-1 device monitors.

## 📚 Documentation

Comprehensive documentation is available in the [docs/](AlertTX-1/docs/) directory:

- **[Documentation Index](AlertTX-1/docs/README.md)** - Complete documentation overview
- **[UI System Design](AlertTX-1/docs/ui-system-design.md)** - UI architecture and design patterns
- **[Hardware Setup](AlertTX-1/docs/hardware-setup.md)** - Hardware requirements and assembly
- **[Software Architecture](AlertTX-1/docs/software-architecture.md)** - Code structure and components

## **🚀 Getting Started**

### **1\. Arduino IDE Setup**

1. **Download and Install Arduino IDE.**  
2. Add ESP32 Board Manager URL in File \> Preferences:  
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package\_esp32\_index.json  
3. **Install ESP32 Boards** via Tools \> Board \> Boards Manager....  
4. **Select Board:** Tools \> Board \> ESP32 Arduino \> Adafruit ESP32-S3 Feather.

### **2\. Library Installation**

Install required libraries via Sketch \> Include Library \> Manage Libraries...:

* Adafruit GFX Library  
* Adafruit ST7735 and ST7789 Library  
* PubSubClient

### **3\. Project Files**

1. **Clone the Repository:**  
   git clone https://github.com/your-username/AlertTX-1.git  
   cd AlertTX-1

   (Replace with your actual repository URL)  
2. **Open in Arduino IDE:** Open AlertTX-1.ino.

### **4\. Configuration**

Edit src/config/settings.h to set your Wi-Fi credentials, MQTT broker details, and hardware pin assignments.

// src/config/settings.h  
\#ifndef SETTINGS\_H  
\#define SETTINGS\_H

// WiFi Settings  
const char\* WIFI\_SSID \= "YourWiFiSSID";         // \<--- CHANGE THIS  
const char\* WIFI\_PASSWORD \= "YourWiFiPassword"; // \<--- CHANGE THIS

// MQTT Settings  
const char\* MQTT\_BROKER \= "your.mqtt.broker.com"; // \<--- CHANGE THIS (e.g., "broker.hivemq.com")  
const int MQTT\_PORT \= 1883;  
const char\* MQTT\_CLIENT\_ID \= "AlertTX1\_Device";  
const char\* MQTT\_TOPIC\_SUBSCRIBE \= "alerttx1/messages"; // Topic to receive messages  
const char\* MQTT\_TOPIC\_PUBLISH \= "alerttx1/status";    // Topic to publish status (optional)

// Hardware Pin Definitions - See docs/pinout-reference.md for complete pin assignments
// Using built-in buttons only
const int BUTTON\_A\_PIN \= 0;         // GPIO0 - Built-in D0/BOOT button (Button A)  
const int BUTTON\_B\_PIN \= 1;         // GPIO1 - Built-in D1 button (Button B)  
const int BUTTON\_C\_PIN \= 2;         // GPIO2 - Built-in D2 button (Button C)  
const int BUZZER\_PIN \= 15;           // GPIO15 (A3) - Passive buzzer

// Display Settings (Built-in TFT - automatically configured by board library)
// See docs/pinout-reference.md for complete TFT pin information

// Power Management Settings  
const unsigned long INACTIVITY\_TIMEOUT\_MS \= 60000; // 60 seconds before entering low power mode  
const unsigned long LONG\_PRESS\_THRESHOLD\_MS \= 1000; // 1 second for long press detection

// Game Settings  
const int GAME\_SPEED\_LEVEL \= 1; // Initial game speed

\#endif // SETTINGS\_H

### **5\. Upload to ESP32**

1. Connect your Adafruit ESP32-S3 Feather.  
2. Select the correct **Port** in Arduino IDE.  
3. Click **Upload**.

## **🎮 Usage**

### **Power Management**

The Alert TX-1 defaults to a low-power sleep mode. Perform a **long press** (hold for at least 1 second) on the designated power button to wake it up and enter active mode. It will return to sleep after INACTIVITY\_TIMEOUT\_MS of inactivity.

### **Navigation**

Use the **Left Column Buttons** (Up/Mid/Down) for menu navigation and the **Right Side Button** for selection or backing out.

### **Receiving Sentry.io Alerts**

Once connected to Wi-Fi and MQTT, the Alert TX-1 subscribes to the topic defined in settings.h (alerttx1/messages by default). When the Beeper-Service publishes a Sentry.io alert to this topic, the device will wake up, display the message, trigger the LED, and play a ringtone.

### **Ringtones**

Default ringtones are in src/ringtones/ringtones.h. You can define new ones in data/ringtones.json (requires code to load from SPIFFS/LittleFS). Select and preview ringtones via the "Ringtones" menu.

### **Beeper Hero Game**

Access the "Games" menu and select "Beeper Hero." Use the physical buttons to hit notes as they scroll down.

## **🔧 Customization**

* **Ringtones:** Modify src/ringtones/ringtones.h or add JSON files to data/ringtones.json.  
* **Fonts:** Place custom bitmap fonts in data/fonts/.  
* **UI Themes:** Adjust colors and drawing routines in src/ui/UIManager.cpp.  
* **Game Difficulty:** Modify GAME\_SPEED\_LEVEL in src/config/settings.h.

## **🤝 Contributing**

Contributions are welcome\! Open an issue or submit a pull request for improvements or new features.

## **📄 License**

This project is open-source and available under the [MIT License](https://www.google.com/search?q=LICENSE).