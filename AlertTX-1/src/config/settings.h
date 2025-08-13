#ifndef SETTINGS_H
#define SETTINGS_H

// WiFi Settings
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// MQTT Settings
extern const char* MQTT_BROKER;
extern const int MQTT_PORT;
extern const char* MQTT_CLIENT_ID;
extern const char* MQTT_TOPIC_SUBSCRIBE; // Topic to receive messages
extern const char* MQTT_TOPIC_PUBLISH;    // Topic to publish status (optional)

// Hardware Pin Definitions - Adafruit ESP32-S3 Reverse TFT Feather
// Using built-in buttons only

const int BUTTON_A_PIN = 0;        // GPIO0 - Built-in D0/BOOT button (Button A)
const int BUTTON_B_PIN = 1;        // GPIO1 - Built-in D1 button (Button B)
const int BUTTON_C_PIN = 2;        // GPIO2 - Built-in D2 button (Button C)

// Audio Pin
const int BUZZER_PIN = 15;          // GPIO15 (A3) - Passive buzzer

// LED Pin
const int LED_PIN = 13;             // GPIO13 (D13) - External 3mm Green LED

// Optional: USB VBUS/Charging sense pin
// If your hardware routes USB 5V (VBUS) to a GPIO via a divider/level shifter,
// set this to the GPIO number to enable USB power detection. Otherwise keep -1.
const int VBUS_SENSE_PIN = -1;      // -1 = not available; otherwise set to GPIO

// Optional: Charger CHG LED sense pin (advanced/mod only)
// If you wire the CHG LED (or charger status) to a safe GPIO, you can set this
// to that GPIO to detect CHG LED state. Otherwise keep -1.
const int CHG_SENSE_PIN = -1;       // -1 = not available; otherwise set to GPIO

// Note: CHG LED is hardware controlled and shows battery charging status
// NeoPixel at GPIO33 is also available for RGB status indicators

// Built-in Features (these are automatically configured by the board)
// TFT Display: Built-in, uses GPIO40-45 for control
// NeoPixel: GPIO33 (built-in) - RGB status LED
// STEMMA QT I2C: GPIO3 (SDA), GPIO4 (SCL)

// Adafruit ESP32-S3 Reverse TFT Feather (240x135)
#define DISPLAY_TYPE_ESP32S3_TFT_FEATHER

#ifdef DISPLAY_TYPE_ESP32S3_TFT_FEATHER
  // TFT Display Pins are already defined by the board variant:
  // TFT_CS = 42, TFT_DC = 40, TFT_RST = 41, TFT_BACKLIGHT = 45
  
  // Add missing SPI pins for Arduino_GFX
  #ifndef TFT_SCLK
    #define TFT_SCLK 12  // SPI Clock
  #endif
  #ifndef TFT_MOSI
    #define TFT_MOSI 11  // SPI MOSI
  #endif
  #ifndef TFT_MISO
    #define TFT_MISO 13  // SPI MISO (not used for display but defined for compatibility)
  #endif
  

  // Display library
  #define USE_ST7789_DISPLAY
  #define TFT_BACKLIGHT 45
#endif

// Power Management Settings
const unsigned long INACTIVITY_TIMEOUT_MS = 60000; // 60 seconds before entering low power mode
const unsigned long LONG_PRESS_THRESHOLD_MS = 1000; // 1 second for long press detection

// Game Settings
const int GAME_SPEED_LEVEL = 1; // Initial game speed

#endif // SETTINGS_H
