/**
 * AlertTX-1 - Phase 2 Component-Based UI Framework
 * Built on proven display initialization with component architecture
 * A = Up, B = Down, C = Select
 * 
 * Phase 2 Features:
 * - Component-based architecture with Screen management
 * - Theme system with 4 predefined themes
 * - Full-width menu items optimized for button navigation
 * - Screen navigation with ScreenManager
 * - Modern C++ callbacks and template-based components
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <ArduinoJson.h>

// Phase 2 Component-Based UI Framework  
#include "src/config/DisplayConfig.h"
#include "src/config/SettingsManager.h"
#include "src/ui/core/Theme.h"
#include "src/ui/core/Component.h"
#include "src/ui/core/Screen.h"
#include "src/ui/core/ScreenManager.h"
#include "src/ui/core/DisplayUtils.h"
#include "src/ui/components/MenuItem.h"
#include "src/ui/components/MenuContainer.h"
#include "src/ui/screens/MainMenuScreen.h"
#include "src/ui/screens/SplashScreen.h"
#include "src/ui/screens/AlertsScreen.h"
#include "src/hardware/ButtonManager.h"
#include "src/hardware/LED.h"
#include "src/ui/core/InputRouter.h"
#include "src/ringtones/RingtonePlayer.h"
#include "src/mqtt/MQTTClient.h"

// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Phase 2 Component-Based UI Framework
ScreenManager* screenManager;
MainMenuScreen* mainMenuScreen;
SplashScreen* splashScreen;
ButtonManager buttonManager;
InputRouter* inputRouter;
LED statusLed;

static void onMqttMessage(char* topic, uint8_t* payload, unsigned int length) {
  // Copy payload to null-terminated buffer
  static char buffer[512];
  unsigned int copyLen = (length < sizeof(buffer) - 1) ? length : (sizeof(buffer) - 1);
  memcpy(buffer, payload, copyLen);
  buffer[copyLen] = '\0';

  // Parse minimal JSON fields
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, buffer);
  if (err) {
    Serial.printf("MQTT JSON parse error: %s\n", err.c_str());
    return;
  }

  const char* title = doc["data"]["title"] | "Alert";
  const char* message = doc["data"]["message"] | "";
  const char* ts = doc["timestamp"] | "";

  // Derive short time (HH:MM) if timestamp present
  char timeBuf[6] = {0};
  if (ts && strlen(ts) >= 16) {
    // Expecting ISO: YYYY-MM-DDTHH:MM:SSZ
    timeBuf[0] = ts[11];
    timeBuf[1] = ts[12];
    timeBuf[2] = ':';
    timeBuf[3] = ts[14];
    timeBuf[4] = ts[15];
    timeBuf[5] = '\0';
  }

  AlertsScreen* alerts = AlertsScreen::getInstance();
  if (alerts) {
    alerts->addMessage(title, message, (timeBuf[0] ? timeBuf : ts));
  }
}

MQTTClient mqtt(onMqttMessage);

void setup(void) {
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("=== AlertTX-1 Phase 2 Component Framework ==="));

  // STEP 1: turn on backlite FIRST (from Adafruit example)
  Serial.println("1. Enabling backlight...");
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // STEP 2: turn on the TFT / I2C power supply (CRITICAL!)
  Serial.println("2. Enabling TFT power supply...");
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // STEP 3: initialize TFT (exact sequence from Adafruit)
  Serial.println("3. Initializing TFT...");
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3); // Landscape
  tft.fillScreen(ST77XX_BLACK);

  Serial.println(F("4. Display initialized successfully!"));

  // STEP 4: Initialize Settings Manager (persistent storage)
  Serial.println("5. Initializing settings manager...");
  SettingsManager::begin();
  
  // STEP 5: Initialize Theme System with saved preferences
  Serial.println("6. Initializing theme system...");
  ThemeManager::begin();  // Initialize with default theme
  ThemeManager::loadFromSettings();  // Load saved theme preference
  Serial.printf("   Active theme: %s\n", ThemeManager::getCurrentThemeName());

  // STEP 6: Initialize button manager
  Serial.println("7. Initializing button manager...");
  buttonManager.begin();

  // Initialize external status LED
  statusLed.begin(LED_PIN);

  // Initialize ringtone player and attach LED sync
  Serial.println("8. Initializing ringtone player...");
  ringtonePlayer.begin(BUZZER_PIN);
  ringtonePlayer.attachLed(&statusLed);
  ringtonePlayer.setLedSyncEnabled(true);

  // Initialize MQTT
  Serial.println("9. Initializing MQTT...");
  String ssid = SettingsManager::getWifiSsid();
  String pass = SettingsManager::getWifiPassword();
  String broker = SettingsManager::getMqttBroker();
  int port = SettingsManager::getMqttPort();
  String cid = SettingsManager::getMqttClientId();
  mqtt.begin(ssid.c_str(), pass.c_str(), broker.c_str(), port, cid.c_str());
  String sub = SettingsManager::getMqttSubscribeTopic();
  if (sub.length() > 0) {
    mqtt.subscribe(sub.c_str());
    Serial.printf("Subscribed to MQTT topic: %s\n", sub.c_str());
  }

  // STEP 7: Initialize Phase 2 Component Framework
  Serial.println("10. Initializing component framework...");
  screenManager = new ScreenManager(&tft);
  mainMenuScreen = new MainMenuScreen(&tft);
  splashScreen = new SplashScreen(&tft, mainMenuScreen);
  
  // STEP 8: Set up global screen manager access
  Serial.println("11. Setting up global screen manager...");
  GlobalScreenManager::setInstance(screenManager);
  inputRouter = new InputRouter(screenManager, &buttonManager);
  
  // STEP 9: Start with splash screen
  Serial.println("12. Starting with splash screen...");
  screenManager->pushScreen(splashScreen);
  
  Serial.println("=== Phase 2 Component Framework Ready! ===");
  Serial.println("Showing splash screen for 2 seconds...");
  Serial.println("A = Up, B = Down, C = Select (or press any key to skip splash)");
}

void loop() {
  // Route input centrally
  inputRouter->update();
  
  // Update framework
  screenManager->update();
  screenManager->draw();

  // Update audio and MQTT
  ringtonePlayer.update();
  mqtt.update();
  
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 30000) {
    Serial.println("=== Performance Stats ===");
    screenManager->printPerformanceStats();
    screenManager->printStackState();
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.println("=========================");
    lastDebug = millis();
  }
}