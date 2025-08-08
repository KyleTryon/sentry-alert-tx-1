#ifndef SETTINGS_H
#define SETTINGS_H

// WiFi Settings
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// MQTT Settings
const char* MQTT_BROKER = "your.mqtt.broker.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "AlertTX1_Device";
const char* MQTT_TOPIC_SUBSCRIBE = "alerttx1/messages"; // Topic to receive messages
const char* MQTT_TOPIC_PUBLISH = "alerttx1/status";    // Topic to publish status (optional)

// Hardware Pin Definitions - Adafruit ESP32-S3 Reverse TFT Feather
// Using built-in buttons only

const int BUTTON_A_PIN = 0;        // GPIO0 - Built-in D0/BOOT button (Button A)
const int BUTTON_B_PIN = 1;        // GPIO1 - Built-in D1 button (Button B)
const int BUTTON_C_PIN = 2;        // GPIO2 - Built-in D2 button (Button C)

// Audio Pin
const int BUZZER_PIN = 15;          // GPIO15 (A3) - Passive buzzer

// Note: No programmable LED on this board
// CHG LED is hardware controlled and shows battery charging status

// Built-in Features (these are automatically configured by the board)
// TFT Display: Built-in, uses GPIO40-45 for control
// NeoPixel: GPIO33 (built-in) - RGB status LED
// STEMMA QT I2C: GPIO3 (SDA), GPIO4 (SCL)

// Adafruit ESP32-S3 Reverse TFT Feather (240x135)
// #define DISPLAY_TYPE_ESP32S3_TFT_FEATHER

#ifdef DISPLAY_TYPE_ESP32S3_TFT_FEATHER
  // TFT Display Pins (built-in display)
  const int TFT_CS = 42;        // GPIO42 - Chip Select
  const int TFT_DC = 40;        // GPIO40 - Data/Command
  const int TFT_RESET = 41;     // GPIO41 - Reset
  const int TFT_BACKLIGHT = 45; // GPIO45 - Backlight control
  
  // Display dimensions
  const int DISPLAY_WIDTH = 240;
  const int DISPLAY_HEIGHT = 135;
  
  // Display library
  #define USE_ST7789_DISPLAY
#endif

// Power Management Settings
const unsigned long INACTIVITY_TIMEOUT_MS = 60000; // 60 seconds before entering low power mode
const unsigned long LONG_PRESS_THRESHOLD_MS = 1000; // 1 second for long press detection

// Game Settings
const int GAME_SPEED_LEVEL = 1; // Initial game speed

#endif // SETTINGS_H
