/**
 * AlertTX-1 - Enhanced UI Framework Version
 * Built on proven display initialization with themed Menu navigation
 * A = Up, B = Down, C = Select
 * 
 * Phase 1 Features:
 * - Theme system with 4 predefined themes
 * - Full-width menu items (220px vs 120px)
 * - Larger menu items (30px vs 15px height)
 * - Better spacing and visual design
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// Enhanced UI Framework with Theme Support
#include "src/ui/Menu.h"
#include "src/ui/core/Theme.h"
#include "src/hardware/ButtonManager.h"

// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// UI Components
Menu mainMenu(&tft);
ButtonManager buttonManager;

// Menu Actions (simple callbacks for now)
void alertsAction() {
  Serial.println("Alerts selected!");
  // TODO: Navigate to alerts screen
}

void gamesAction() {
  Serial.println("Games selected!");
  // TODO: Navigate to games screen
}

void settingsAction() {
  Serial.println("Settings selected!");
  
  // Demo: Theme switching on settings selection
  static int currentTheme = 0;
  const Theme* themes[] = {&THEME_DEFAULT, &THEME_TERMINAL, &THEME_AMBER, &THEME_HIGH_CONTRAST};
  const char* themeNames[] = {"Default", "Terminal", "Amber", "High Contrast"};
  
  currentTheme = (currentTheme + 1) % 4;
  ThemeManager::setTheme(themes[currentTheme]);
  
  Serial.print("Theme changed to: ");
  Serial.println(themeNames[currentTheme]);
  
  // Redraw with new theme
  drawUI();
}

// Menu Items (based on your working example)
MenuItem menuItems[] = {
  {"Alerts", 1, alertsAction},
  {"Games", 2, gamesAction}, 
  {"Settings", 3, settingsAction}
};

void setup(void) {
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("=== AlertTX-1 UI Framework ==="));

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

  // STEP 6: Setup menu system
  Serial.println("7. Setting up menu...");
  mainMenu.setItems(menuItems, 3);  // 3 menu items
  
  // STEP 7: Draw initial UI
  Serial.println("8. Drawing UI...");
  drawUI();
  
  Serial.println("=== UI Framework Ready! ===");
  Serial.println("A = Up, B = Down, C = Select");
}

void drawUI() {
  // Clear screen with theme background
  tft.fillScreen(ThemeManager::getBackground());
  
  // Title using theme colors
  tft.setTextColor(ThemeManager::getPrimaryText());
  tft.setTextSize(2);
  tft.setCursor(30, 20);
  tft.println("AlertTX-1");
  
  // Draw the enhanced menu (now with theme colors and larger items)
  mainMenu.draw();
}

void loop() {
  // Update button manager
  buttonManager.update();
  
  // Debug: Check raw button states periodically
  static unsigned long lastDebug = 0;
  if (millis() - lastDebug > 1000) {  // Every 1 second
    bool rawA = (digitalRead(0) == LOW);   // GPIO0 - BOOT button
    bool rawB = (digitalRead(1) == HIGH);  // GPIO1
    bool rawC = (digitalRead(2) == HIGH);  // GPIO2
    
    if (rawA || rawB || rawC) {
      Serial.printf("Raw buttons - A:%d B:%d C:%d\n", rawA, rawB, rawC);
    }
    
    lastDebug = millis();
  }
  
  // Handle menu navigation using ButtonManager
  if (buttonManager.wasPressed(ButtonManager::BUTTON_A)) {  // Up
    Serial.println("Up pressed - moving menu up");
    mainMenu.moveUp();
    drawUI();  // Redraw after navigation
  }
  
  if (buttonManager.wasPressed(ButtonManager::BUTTON_B)) {  // Down
    Serial.println("Down pressed - moving menu down");
    mainMenu.moveDown();
    drawUI();  // Redraw after navigation
  }
  
  if (buttonManager.wasPressed(ButtonManager::BUTTON_C)) {  // Select
    Serial.println("Select pressed - activating menu item");
    mainMenu.select();  // This will call the appropriate action function
  }
}