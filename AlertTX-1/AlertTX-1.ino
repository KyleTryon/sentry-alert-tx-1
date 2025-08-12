/**
 * Alert TX-1 - ESP32-S3 Reverse TFT Feather Beeper Device
 * 
 * A retro beeper device with:
 * - RTTTL ringtone playback
 * - BeeperHero rhythm game
 * - React-like UI framework
 * - MQTT connectivity
 * - Power management
 * 
 * Hardware: Adafruit ESP32-S3 Reverse TFT Feather
 * Display: Built-in 240x135 TFT
 * Buttons: Built-in A, B, C buttons
 * Audio: Passive buzzer on GPIO15
 */

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// AnyRtttl library (installed via Arduino CLI)
#include <anyrtttl.h>

// Alert TX-1 modules
#include "src/config/settings.h"
#include "src/hardware/ButtonManager.h"
#include "src/hardware/DisplayConfig.h"
#include "src/hardware/PowerManager.h"
#include "src/ringtones/RingtonePlayer.h"
#include "src/mqtt/MQTTClient.h"
#include "src/game/BeeperHeroGame.h"

// UI Framework - DISABLED FOR SIMPLE DEMO
// #include "src/ui/UIFramework.h"
// #include "src/ui/screens/MenuScreen.h"
// #include "src/ui/screens/DemoScreen.h"

// UI Manager and Screens - DISABLED FOR SIMPLE DEMO
// UIManager uiManager;
// MenuScreen* menuScreen = nullptr;
// DemoScreen* demo1Screen = nullptr;
// DemoScreen* demo2Screen = nullptr;
// DemoScreen* demo3Screen = nullptr;

// Define constants declared in settings.h
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";
const char* MQTT_BROKER = "your.mqtt.broker.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "AlertTX1_Device";
const char* MQTT_TOPIC_SUBSCRIBE = "alerttx1/messages";
const char* MQTT_TOPIC_PUBLISH = "alerttx1/status";

// ========================================
// Hardware Configuration
// ========================================

// Display (built-in on ESP32-S3 Reverse TFT Feather)
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Hardware managers
ButtonManager buttonManager;
DisplayConfig displayConfig;
PowerManager powerManager;
RingtonePlayer ringtonePlayer;
MQTTClient mqttClient;

// Game system
BeeperHeroGame* beeperHeroGame = nullptr;

// ========================================
// System State
// ========================================
bool systemInitialized = false;
unsigned long lastHeartbeat = 0;
unsigned long lastButtonCheck = 0;

// ========================================
// Setup Function
// ========================================
void setup() {
    Serial.begin(9600);
    delay(1000);
    
    Serial.println("SERIAL TEST STARTING");
    Serial.flush();
    
    Serial.println("Line 1");
    Serial.flush();
    delay(100);
    
    Serial.println("Line 2");
    Serial.flush();
    delay(100);
    
    Serial.println("Line 3");
    Serial.flush();
    delay(100);
    
    Serial.println("About to start display test...");
    Serial.flush();
    
    // Now try display
    Serial.println("DEBUG: About to start display initialization...");
    Serial.flush();
    
    Serial.println("Step 1: About to initialize display...");
    Serial.flush();
    // Initialize display first
    initializeDisplay();
    Serial.println("DEBUG: Display initialization complete!");
    Serial.flush();
    
    Serial.println("DEBUG: About to start splash screen...");
    Serial.flush();
    Serial.println("Step 2: About to show splash screen...");
    Serial.flush();
    // Show splash screen
    showSplashScreen();
    Serial.println("DEBUG: Splash screen complete!");
    Serial.flush();
    
    Serial.println("DEBUG: About to start hardware initialization...");
    Serial.flush();
    Serial.println("Step 3: About to initialize hardware...");
    Serial.flush();
    // Initialize hardware
    initializeHardware();
    Serial.println("DEBUG: Hardware initialization complete!");
    Serial.flush();
    
    Serial.println("DEBUG: About to start software systems initialization...");
    Serial.flush();
    Serial.println("Step 4: About to initialize software systems...");
    Serial.flush();
    // Initialize software systems
    initializeSoftwareSystems();
    Serial.println("DEBUG: Software systems initialization complete!");
    Serial.flush();
    
    Serial.println("DEBUG: About to start UI framework initialization...");
    Serial.flush();
    Serial.println("Step 5: About to initialize UI framework...");
    Serial.flush();
    // Initialize UI framework
    initializeUI();
    Serial.println("DEBUG: UI framework initialization complete!");
    
    Serial.println("DEBUG: About to start final setup...");
    Serial.flush();
    Serial.println("Step 6: About to finalize setup...");
    Serial.flush();
    // Final setup
    finalizeSetup();
    Serial.println("DEBUG: Final setup complete!");
    
    Serial.println("DEBUG: About to mark system as initialized...");
    Serial.flush();
    Serial.println("Alert TX-1 Ready!");
    Serial.flush();
    
    // Wait a moment and check button states before starting
    Serial.println("DEBUG: Checking initial button states...");
    Serial.flush();
    delay(1000); // Wait for any button bouncing to settle
    
    bool buttonA = digitalRead(BUTTON_A_PIN) == LOW;
    bool buttonB = digitalRead(BUTTON_B_PIN) == HIGH;
    bool buttonC = digitalRead(BUTTON_C_PIN) == HIGH;
    Serial.printf("DEBUG: Initial button states - A:%d B:%d C:%d\n", buttonA, buttonB, buttonC);
    Serial.flush();
    
    systemInitialized = true;
    Serial.println("DEBUG: Setup function complete!");
    Serial.flush();
}

// ========================================
// Main Loop
// ========================================
void loop() {
    if (!systemInitialized) {
        delay(100);
        return;
    }
    
    unsigned long now = millis();
    
    // Simple updates - minimal processing
    updateButtons();
    updateUI();
    
    // Less frequent updates
    if (now - lastHeartbeat > 500) { // 2Hz for less critical updates
        updateRingtonePlayer();
        updatePowerManagement();
        updateGame();
        lastHeartbeat = now;
    }
    
    // Simple yield
    yield();
}

// ========================================
// Initialization Functions
// ========================================

void initializeDisplay() {
    Serial.print("Initializing display... ");
    Serial.flush();
    
    // Turn on backlight first
    Serial.println("DEBUG: About to set up backlight pin...");
    Serial.flush();
    pinMode(TFT_BACKLIGHT, OUTPUT);
    Serial.println("DEBUG: Backlight pin mode set...");
    Serial.flush();
    digitalWrite(TFT_BACKLIGHT, HIGH);
    Serial.println("Backlight on");
    Serial.flush();
    
    // Initialize the built-in TFT display
    Serial.println("DEBUG: About to call display.init()...");
    Serial.flush();
    display.init(135, 240); // 1.14" display
    Serial.println("Display init done");
    Serial.flush();
    
    Serial.println("DEBUG: About to set display rotation...");
    Serial.flush();
    display.setRotation(3);  // Landscape orientation
    Serial.println("Rotation set");
    Serial.flush();
    
    Serial.println("DEBUG: About to fill screen black...");
    Serial.flush();
    display.fillScreen(ST77XX_BLACK);
    Serial.println("Screen filled black");
    Serial.flush();
    
    Serial.println("OK");
    Serial.flush();
}

void showSplashScreen() {
    Serial.println("DEBUG: Starting splash screen...");
    Serial.flush();
    
    display.setTextColor(ST77XX_WHITE);
    Serial.println("DEBUG: Text color set...");
    Serial.flush();
    
    display.setTextSize(2);
    Serial.println("DEBUG: Text size set...");
    Serial.flush();
    
    // Center the text
    int16_t x1, y1;
    uint16_t w, h;
    Serial.println("DEBUG: About to get text bounds...");
    Serial.flush();
    display.getTextBounds("Alert TX-1", 0, 0, &x1, &y1, &w, &h);
    Serial.println("DEBUG: Text bounds calculated...");
    Serial.flush();
    int centerX = (240 - w) / 2;
    int centerY = (135 - h) / 2;
    
    Serial.println("DEBUG: About to set cursor and print Alert TX-1...");
    Serial.flush();
    display.setCursor(centerX, centerY);
    display.println("Alert TX-1");
    Serial.println("DEBUG: Alert TX-1 text printed...");
    Serial.flush();
    
    // Version info
    display.setTextSize(1);
    Serial.println("DEBUG: About to print version info...");
    Serial.flush();
    display.getTextBounds("ESP32-S3 Beeper", 0, 0, &x1, &y1, &w, &h);
    centerX = (240 - w) / 2;
    display.setCursor(centerX, centerY + 25);
    display.println("ESP32-S3 Beeper");
    Serial.println("DEBUG: Version info printed...");
    Serial.flush();
    
    Serial.println("DEBUG: About to delay 2 seconds...");
    Serial.flush();
    delay(2000); // Show splash for 2 seconds
    Serial.println("DEBUG: Splash screen delay complete...");
    Serial.flush();
}

void initializeHardware() {
    Serial.println("Initializing hardware...");
    
    // Button manager
    Serial.print("  - Buttons... ");
    buttonManager.begin();
    Serial.println("OK");
    
    // Display configuration (already handled in initializeDisplay)
    Serial.print("  - Display config... ");
    // DisplayConfig is used statically, no begin() method needed
    Serial.println("OK");
    
    // Power management
    Serial.print("  - Power management... ");
    powerManager.begin();
    Serial.println("OK");
    
    // Audio system
    Serial.print("  - Audio (Buzzer)... ");
    ringtonePlayer.begin(BUZZER_PIN);
    Serial.println("OK");
}

void initializeSoftwareSystems() {
    Serial.println("Initializing software systems...");
    
    // MQTT client - DISABLED FOR NOW
    Serial.print("  - MQTT client... ");
    // mqttClient.begin();
    Serial.println("DISABLED (no WiFi)");
    
    // Generate ringtone data (if not already done) - DISABLED FOR NOW
    Serial.print("  - Ringtone data... ");
    // int ringtoneCount = ringtonePlayer.getRingtoneCount();
    // Serial.printf("OK (%d ringtones loaded)\n", ringtoneCount);
    Serial.println("DISABLED (no ringtone data)");
}

// Simple manual UI demo variables
int currentScreen = 0; // 0=menu, 1=option1, 2=option2, 3=option3
unsigned long lastScreenChange = 0;
bool screenChanged = false;

void initializeUI() {
    Serial.println("Initializing simple UI demo...");
    
    // No complex UI framework for now
    currentScreen = 0;
    lastScreenChange = millis();
    screenChanged = true;
    
    Serial.println("  - Simple UI demo ready");
}

void finalizeSetup() {
    // Initialize game system (optional) - DISABLED FOR SIMPLE DEMO
    // Serial.print("Initializing BeeperHero game... ");
    // beeperHeroGame = new BeeperHeroGame(&uiManager, &buttonManager, &ringtonePlayer);
    // Serial.println("OK");
    
    // Play startup sound - DISABLED FOR SIMPLE DEMO
    // Serial.print("Playing startup sound... ");
    // ringtonePlayer.playRingtoneByName("Mario");
    // Serial.println("OK");
    
    // Clear splash screen
    display.fillScreen(ST77XX_BLACK);
}

// ========================================
// Update Functions
// ========================================

void updateButtons() {
    static unsigned long lastButtonUpdate = 0;
    static unsigned long startupTime = 0;
    unsigned long now = millis();
    
    // Initialize startup time on first call
    if (startupTime == 0) {
        startupTime = now;
    }
    
    // Don't process buttons for the first 3 seconds
    if (now - startupTime < 3000) {
        return;
    }
    
    // Check buttons every 500ms (2Hz) - very slow for debugging
    if (now - lastButtonUpdate < 500) return;
    
    // Read button states directly
    bool buttonA = digitalRead(BUTTON_A_PIN) == LOW;
    bool buttonB = digitalRead(BUTTON_B_PIN) == HIGH;
    bool buttonC = digitalRead(BUTTON_C_PIN) == HIGH;
    
    // Always show button states for debugging
    Serial.printf("DEBUG: Buttons - A:%d B:%d C:%d Screen:%d\n", 
                 buttonA, buttonB, buttonC, currentScreen);
    Serial.flush();
    
    // Simple button handling - only if button is pressed
    if (buttonA) {
        Serial.println("DEBUG: Button A detected!");
        Serial.flush();
        currentScreen = (currentScreen + 1) % 4;
        screenChanged = true;
        Serial.printf("DEBUG: Screen now: %d\n", currentScreen);
        Serial.flush();
    }
    
    if (buttonB) {
        Serial.println("DEBUG: Button B detected!");
        Serial.flush();
        if (currentScreen == 0) currentScreen = 3;
        else currentScreen = currentScreen - 1;
        screenChanged = true;
        Serial.printf("DEBUG: Screen now: %d\n", currentScreen);
        Serial.flush();
    }
    
    if (buttonC) {
        Serial.println("DEBUG: Button C detected!");
        Serial.flush();
        currentScreen = 0;
        screenChanged = true;
        Serial.printf("DEBUG: Screen now: %d\n", currentScreen);
        Serial.flush();
    }
    
    lastButtonUpdate = now;
}

void updateRingtonePlayer() {
    ringtonePlayer.update();
}

void updateUI() {
    // Simple manual UI rendering
    if (screenChanged) {
        Serial.printf("DEBUG: Rendering screen %d\n", currentScreen);
        Serial.flush();
        
        // Clear screen
        display.fillScreen(ST77XX_BLACK);
        
        // Render based on current screen
        switch (currentScreen) {
            case 0: // Menu
                renderMenu();
                break;
            case 1: // Option 1
                renderOption1();
                break;
            case 2: // Option 2
                renderOption2();
                break;
            case 3: // Option 3
                renderOption3();
                break;
        }
        
        screenChanged = false;
        lastScreenChange = millis();
    }
}

void renderMenu() {
    // Title
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2);
    display.setCursor(60, 10);
    display.print("DEMO MENU");
    
    // Options
    display.setTextSize(1);
    display.setCursor(20, 40);
    display.print("Option 1");
    display.setCursor(20, 60);
    display.print("Option 2");
    display.setCursor(20, 80);
    display.print("Option 3");
    
    // Instructions
    display.setCursor(10, 110);
    display.print("A=Next B=Prev C=Menu");
}

void renderOption1() {
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2);
    display.setCursor(60, 10);
    display.print("OPTION 1");
    
    display.setTextSize(1);
    display.setCursor(20, 50);
    display.print("Welcome to Option 1!");
    display.setCursor(20, 70);
    display.print("This is a simple demo.");
    
    display.setCursor(10, 110);
    display.print("Press any button");
}

void renderOption2() {
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2);
    display.setCursor(60, 10);
    display.print("OPTION 2");
    
    display.setTextSize(1);
    display.setCursor(20, 50);
    display.print("Welcome to Option 2!");
    display.setCursor(20, 70);
    display.print("Another demo screen.");
    
    display.setCursor(10, 110);
    display.print("Press any button");
}

void renderOption3() {
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2);
    display.setCursor(60, 10);
    display.print("OPTION 3");
    
    display.setTextSize(1);
    display.setCursor(20, 50);
    display.print("Welcome to Option 3!");
    display.setCursor(20, 70);
    display.print("Final demo screen.");
    
    display.setCursor(10, 110);
    display.print("Press any button");
}

void updatePowerManagement() {
    // Temporarily disabled to prevent deep sleep during UI demo testing
    // powerManager.update();
    
    // Handle low battery conditions - DISABLED FOR DEMO
    // if (powerManager.getBatteryLevel() < 0.1f) {
    //     // Show low battery warning
    //     Serial.println("Warning: Low battery!");
    // }
}

void updateMQTT() {
    mqttClient.update();
}

void updateGame() {
    if (beeperHeroGame) {
        beeperHeroGame->update();
    }
}

// ========================================
// Utility Functions
// ========================================



void printSystemStatus() {
    Serial.println("\n=== Alert TX-1 System Status ===");
    Serial.printf("Uptime: %lu ms\n", millis());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Battery Level: %.1f%%\n", powerManager.getBatteryLevel() * 100);
    Serial.printf("Ringtones Available: %d\n", ringtonePlayer.getRingtoneCount());
    Serial.printf("UI Demo: Simple Manual UI\n");
    Serial.println("================================\n");
}
