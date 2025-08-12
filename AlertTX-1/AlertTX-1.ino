/**
 * Alert TX-1 - ESP32-S3 Reverse TFT Feather with UI Framework
 * 
 * Enhanced version using the FlipperZero-inspired UI framework with
 * the proven Adafruit GFX + ST7789 library stack.
 * 
 * Features:
 * - Event-driven architecture
 * - Theme system with multiple color schemes
 * - Responsive button handling
 * - Status bar with battery, WiFi, and time
 * - Scrollable menu navigation
 * - Icon system integration
 * - 30+ FPS performance
 * 
 * Hardware: Adafruit ESP32-S3 Reverse TFT Feather
 * Display: Built-in 240x135 TFT (ST7789)
 * Buttons: Built-in A (GPIO0), B (GPIO1), C (GPIO2)
 * Audio: Passive buzzer on GPIO15
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <time.h>

// Core configuration
#include "src/config/settings.h"

// Hardware components
#include "src/hardware/LED.h"
#include "src/hardware/Buzzer.h"

// New UI Framework
#include "src/ui/core/UIManager.h"
#include "src/ui/core/Theme.h"
#include "src/ui/screens/MainMenuScreen.h"
#include "src/hardware/ButtonManager.h"

// ========================================
// Hardware Configuration for ESP32-S3 Reverse TFT Feather
// ========================================

// Display pin definitions (from Adafruit documentation)
#define TFT_CS        7
#define TFT_RST       40 
#define TFT_DC        39
#define TFT_BACKLIGHT 45
// Note: TFT_I2C_POWER and TFT_BACKLITE are predefined by the board variant

// Initialize display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Hardware managers
LED statusLED;
Buzzer buzzer;
ButtonManager buttonManager;

// UI Framework
UIManager uiManager;
MainMenuScreen mainMenuScreen;

// ========================================
// System State
// ========================================
bool systemInitialized = false;
unsigned long bootTime = 0;
int themeIndex = 0;
const Theme* themes[] = { &THEME_AMBER, &THEME_MONOCHROME, &THEME_GREEN, &THEME_INVERTED };
const int themeCount = 4;

// ========================================
// Setup Function
// ========================================
void setup() {
    Serial.begin(115200);
    bootTime = millis();
    
    Serial.println("=== Alert TX-1 UI Framework Demo ===");
    Serial.println("Initializing system...");
    
    // Initialize display
    initializeDisplay();
    
    // Show splash screen
    showSplashScreen();
    
    // Initialize hardware
    initializeHardware();
    
    // Initialize UI framework
    initializeUI();
    
    systemInitialized = true;
    unsigned long initTime = millis() - bootTime;
    
    Serial.printf("System initialized in %lu ms\n", initTime);
    Serial.println("=== System Ready ===");
    
    // Play startup sound
    // buzzer.playStartupMelody(); // TODO: Implement this method
}

// ========================================
// Main Loop
// ========================================
void loop() {
    if (!systemInitialized) return;
    
    // Update UI framework (handles everything)
    uiManager.update();
    
    // Update hardware status for status bar
    updateSystemStatus();
    
    // Handle special button combinations
    handleSpecialInputs();
    
    // Small delay to prevent overwhelming the system
    delay(1);
}

// ========================================
// Display Initialization
// ========================================
void initializeDisplay() {
    Serial.println("Initializing display...");
    
    // CRITICAL: Enable TFT power supply first (REQUIRED for ESP32-S3 Reverse TFT Feather)
    Serial.println("  - Enabling TFT power supply...");
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);
    
    // Turn on backlight
    Serial.println("  - Enabling backlight...");
    pinMode(TFT_BACKLITE, OUTPUT);  // Note: TFT_BACKLITE not TFT_BACKLIGHT
    digitalWrite(TFT_BACKLITE, HIGH);
    
    // Initialize display
    Serial.println("  - Initializing ST7789 display...");
    tft.init(135, 240);  // Initialize with the dimensions (portrait)
    tft.setRotation(3);  // Rotate to landscape
    
    // Clear screen
    tft.fillScreen(ST77XX_BLACK);
    
    Serial.println("Display initialized successfully");
}

// ========================================
// Splash Screen
// ========================================
void showSplashScreen() {
    Serial.println("Showing splash screen...");
    
    tft.fillScreen(ST77XX_BLACK);
    
    // Title
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.setCursor(40, 40);
    tft.println("AlertTX-1");
    
    // Subtitle
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(60, 70);
    tft.println("UI Framework Demo");
    
    // Version
    tft.setCursor(80, 85);
    tft.println("v1.0.0");
    
    // Instructions
    tft.setCursor(20, 110);
    tft.println("Press any button to continue");
    
    // Wait for button press or timeout
    unsigned long startTime = millis();
    bool buttonPressed = false;
    
    while ((millis() - startTime) < 3000 && !buttonPressed) {
        // Simple button check
        if (digitalRead(0) == LOW || digitalRead(1) == HIGH || digitalRead(2) == HIGH) {
            buttonPressed = true;
        }
        delay(10);
    }
    
    Serial.println("Splash screen complete");
}

// ========================================
// Hardware Initialization
// ========================================
void initializeHardware() {
    Serial.println("Initializing hardware...");
    
    // Initialize LED
    statusLED.begin(13); // Red LED on pin 13
    statusLED.on();
    
    // Initialize buzzer
    buzzer.begin(15); // Buzzer on pin 15
    
    // Initialize buttons with UI event system
    buttonManager.begin(uiManager.getEventSystem(), &statusLED, &buzzer);
    
    Serial.println("Hardware initialized successfully");
}

// ========================================
// UI Framework Initialization
// ========================================
void initializeUI() {
    Serial.println("Initializing UI framework...");
    
    // Initialize UI manager with the correct display type
    uiManager.begin(&tft, &buttonManager);
    
    // Set default theme
    uiManager.setTheme(THEME_AMBER);
    
    // Add main menu screen
    uiManager.addScreen(&mainMenuScreen);
    
    Serial.println("UI framework initialized successfully");
    
    // Print UI stats
    uiManager.printStats();
}

// ========================================
// System Status Updates
// ========================================
void updateSystemStatus() {
    static unsigned long lastUpdate = 0;
    static int batteryLevel = 100;
    
    if (millis() - lastUpdate > 5000) { // Update every 5 seconds
        // Simulate battery drain
        batteryLevel--;
        if (batteryLevel < 0) batteryLevel = 100;
        
        // Update main menu screen status
        mainMenuScreen.updateBatteryLevel(batteryLevel);
        mainMenuScreen.updateConnectivityStatus(false, false); // TODO: real status
        
        // Update time
        unsigned long seconds = (millis() / 1000) % 86400;
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        mainMenuScreen.updateSystemTime(hours, minutes, secs);
        
        lastUpdate = millis();
    }
}

// ========================================
// Special Input Handling
// ========================================
void handleSpecialInputs() {
    static unsigned long lastCheck = 0;
    
    if (millis() - lastCheck > 100) { // Check every 100ms
        // Long press on Button A + Button B = Theme change
        if (buttonManager.isPressed(ButtonManager::BUTTON_A) && 
            buttonManager.isPressed(ButtonManager::BUTTON_B)) {
            
            static unsigned long lastThemeChange = 0;
            if (millis() - lastThemeChange > 1000) { // Prevent rapid theme changes
                // Cycle through themes
                themeIndex = (themeIndex + 1) % themeCount;
                uiManager.setTheme(*themes[themeIndex]);
                
                // Play theme change sound
                buzzer.playTone(800, 50);
                delay(50);
                buzzer.playTone(1000, 50);
                
                Serial.printf("Theme changed to: %s\n", getThemeName(themeIndex));
                lastThemeChange = millis();
            }
        }
        
        // Long press on Button C = Performance stats
        if (buttonManager.isLongPressed(ButtonManager::BUTTON_C)) {
            Serial.println("\n=== Performance Stats ===");
            uiManager.printStats();
            Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
            Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
            Serial.println("========================\n");
        }
        
        lastCheck = millis();
    }
}

// ========================================
// Debug and Utility Functions
// ========================================
void printSystemInfo() {
    Serial.println("\n=== System Information ===");
    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
    Serial.printf("Free Heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
    Serial.println("=========================\n");
}

// Theme names for debugging
const char* getThemeName(int index) {
    switch (index) {
        case 0: return "FlipperZero Amber";
        case 1: return "Monochrome";
        case 2: return "Pip-Boy Green";
        case 3: return "Inverted";
        default: return "Unknown";
    }
}