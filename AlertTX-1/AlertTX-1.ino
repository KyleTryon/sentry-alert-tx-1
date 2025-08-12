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

// Phase 2 Component-Based UI Framework  
#include "src/config/DisplayConfig.h"
#include "src/ui/core/Theme.h"
#include "src/ui/core/Component.h"
#include "src/ui/core/Screen.h"
#include "src/ui/core/ScreenManager.h"
#include "src/ui/core/DisplayUtils.h"
#include "src/ui/components/MenuItem.h"
#include "src/ui/components/MenuContainer.h"
#include "src/ui/screens/MainMenuScreen.h"
#include "src/ui/screens/SplashScreen.h"
#include "src/hardware/ButtonManager.h"

// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Phase 2 Component-Based UI Framework
ScreenManager* screenManager;
MainMenuScreen* mainMenuScreen;
SplashScreen* splashScreen;
ButtonManager buttonManager;

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

  // STEP 4: Initialize Theme System
  Serial.println("5. Initializing theme system...");
  ThemeManager::begin();  // Initialize with default theme
  Serial.println("   Active theme: Default (FlipperZero style)");

  // STEP 5: Initialize button manager
  Serial.println("6. Initializing button manager...");
  buttonManager.begin();

  // STEP 6: Initialize Phase 2 Component Framework
  Serial.println("7. Initializing component framework...");
  screenManager = new ScreenManager(&tft);
  mainMenuScreen = new MainMenuScreen(&tft);
  splashScreen = new SplashScreen(&tft, mainMenuScreen);
  
  // STEP 7: Set up global screen manager access
  Serial.println("8. Setting up global screen manager...");
  GlobalScreenManager::setInstance(screenManager);
  
  // STEP 8: Start with splash screen
  Serial.println("9. Starting with splash screen...");
  screenManager->pushScreen(splashScreen);
  
  Serial.println("=== Phase 2 Component Framework Ready! ===");
  Serial.println("Showing splash screen for 2 seconds...");
  Serial.println("A = Up, B = Down, C = Select (or press any key to skip splash)");
}

void loop() {
  // Update button manager
  buttonManager.update();
  
  // Handle long presses for back navigation (any button)
  if (buttonManager.isLongPressed(ButtonManager::BUTTON_A) || 
      buttonManager.isLongPressed(ButtonManager::BUTTON_B) || 
      buttonManager.isLongPressed(ButtonManager::BUTTON_C)) {
    Serial.println("Long press detected - navigating back");
    screenManager->popScreen();
  }
  
  // Handle regular button presses and route to ScreenManager
  if (buttonManager.wasPressed(ButtonManager::BUTTON_A)) {  // Up
    Serial.println("Up pressed - routing to screen manager");
    screenManager->handleButtonPress(ButtonInput::BUTTON_A);
  }
  
  if (buttonManager.wasPressed(ButtonManager::BUTTON_B)) {  // Down
    Serial.println("Down pressed - routing to screen manager");
    screenManager->handleButtonPress(ButtonInput::BUTTON_B);
  }
  
  if (buttonManager.wasPressed(ButtonManager::BUTTON_C)) {  // Select
    Serial.println("Select pressed - routing to screen manager");
    screenManager->handleButtonPress(ButtonInput::BUTTON_C);
  }
  
  // Update and draw the Phase 2 component framework
  screenManager->update();
  screenManager->draw();
  
  // Debug: Performance monitoring (reduced frequency during splash)
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 30000) {  // Every 30 seconds (reduced from 10)
    Serial.println("=== Performance Stats ===");
    screenManager->printPerformanceStats();
    screenManager->printStackState();
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.println("=========================");
    lastDebug = millis();
  }
}