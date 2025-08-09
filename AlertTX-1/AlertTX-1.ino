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

// Alert TX-1 modules
#include "src/config/settings.h"
#include "src/hardware/ButtonManager.h"
#include "src/hardware/DisplayConfig.h"
#include "src/hardware/PowerManager.h"
#include "src/ringtones/RingtonePlayer.h"
#include "src/mqtt/MQTTClient.h"
#include "src/game/BeeperHeroGame.h"

// UI Framework
#include "src/ui/UIFramework.h"

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

// UI System
UIManager uiManager;
HomeScreen* homeScreen = nullptr;
RingtonesScreen* ringtonesScreen = nullptr;

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
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n" + String("=").repeat(50));
    Serial.println("Alert TX-1 Starting Up...");
    Serial.println("Hardware: ESP32-S3 Reverse TFT Feather");
    Serial.println("Version: 1.0.0");
    Serial.println("=".repeat(50));

    // Initialize display first
    initializeDisplay();
    
    // Show splash screen
    showSplashScreen();
    
    // Initialize hardware
    initializeHardware();
    
    // Initialize software systems
    initializeSoftwareSystems();
    
    // Initialize UI framework
    initializeUI();
    
    // Final setup
    finalizeSetup();
    
    Serial.println("Alert TX-1 Ready!");
    systemInitialized = true;
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
    
    // Critical system updates (every cycle)
    updateButtons();
    updateRingtonePlayer();
    updateUI();
    
    // Regular updates (throttled)
    if (now - lastHeartbeat > 100) { // 10Hz for less critical updates
        updatePowerManagement();
        updateMQTT();
        updateGame();
        lastHeartbeat = now;
    }
    
    // Handle low power mode
    powerManager.handleLowPowerMode();
    
    // Yield to other tasks
    yield();
}

// ========================================
// Initialization Functions
// ========================================

void initializeDisplay() {
    Serial.print("Initializing display... ");
    
    // Initialize the built-in TFT display
    display.init(135, 240); // 1.14" display
    display.setRotation(3);  // Landscape orientation
    display.fillScreen(ST77XX_BLACK);
    
    Serial.println("OK");
}

void showSplashScreen() {
    display.setTextColor(ST77XX_WHITE);
    display.setTextSize(2);
    
    // Center the text
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("Alert TX-1", 0, 0, &x1, &y1, &w, &h);
    int centerX = (240 - w) / 2;
    int centerY = (135 - h) / 2;
    
    display.setCursor(centerX, centerY);
    display.println("Alert TX-1");
    
    // Version info
    display.setTextSize(1);
    display.getTextBounds("ESP32-S3 Beeper", 0, 0, &x1, &y1, &w, &h);
    centerX = (240 - w) / 2;
    display.setCursor(centerX, centerY + 25);
    display.println("ESP32-S3 Beeper");
    
    delay(2000); // Show splash for 2 seconds
}

void initializeHardware() {
    Serial.println("Initializing hardware...");
    
    // Button manager
    Serial.print("  - Buttons... ");
    buttonManager.begin();
    Serial.println("OK");
    
    // Display configuration
    Serial.print("  - Display config... ");
    displayConfig.begin();
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
    
    // MQTT client
    Serial.print("  - MQTT client... ");
    mqttClient.begin();
    Serial.println("OK");
    
    // Generate ringtone data (if not already done)
    Serial.print("  - Ringtone data... ");
    int ringtoneCount = ringtonePlayer.getRingtoneCount();
    Serial.printf("OK (%d ringtones loaded)\n", ringtoneCount);
}

void initializeUI() {
    Serial.println("Initializing UI framework...");
    
    // Initialize the UI framework
    UIFramework::initialize();
    
    // Initialize UI manager
    uiManager.begin();
    
    // Create and register screens
    Serial.print("  - Creating screens... ");
    homeScreen = uiManager.registerScreen<HomeScreen>(&buttonManager, &ringtonePlayer);
    ringtonesScreen = uiManager.registerScreen<RingtonesScreen>(&ringtonePlayer);
    Serial.println("OK");
    
    // Set initial screen
    uiManager.setScreen(homeScreen);
    
    Serial.println("  - UI framework ready");
}

void finalizeSetup() {
    // Initialize game system (optional)
    Serial.print("Initializing BeeperHero game... ");
    beeperHeroGame = new BeeperHeroGame(&uiManager, &buttonManager, &ringtonePlayer);
    Serial.println("OK");
    
    // Play startup sound
    Serial.print("Playing startup sound... ");
    ringtonePlayer.playRingtoneByName("Mario");
    Serial.println("OK");
    
    // Clear splash screen
    display.fillScreen(ST77XX_BLACK);
}

// ========================================
// Update Functions
// ========================================

void updateButtons() {
    static unsigned long lastButtonUpdate = 0;
    unsigned long now = millis();
    
    if (now - lastButtonUpdate < 20) return; // 50Hz button polling
    
    buttonManager.update();
    
    // Map button presses to UI events
    if (buttonManager.isPressed(BUTTON_A_PIN)) {
        uiManager.handleEvent(1, 0); // Event type 1: Button A
    }
    
    if (buttonManager.isPressed(BUTTON_B_PIN)) {
        uiManager.handleEvent(2, 0); // Event type 2: Button B
    }
    
    if (buttonManager.isPressed(BUTTON_C_PIN)) {
        uiManager.handleEvent(3, 0); // Event type 3: Button C
    }
    
    lastButtonUpdate = now;
}

void updateRingtonePlayer() {
    ringtonePlayer.update();
}

void updateUI() {
    // Update UI manager
    uiManager.update();
    
    // Render UI (only if needed)
    uiManager.render(display);
}

void updatePowerManagement() {
    powerManager.update();
    
    // Handle low battery conditions
    if (powerManager.getBatteryLevel() < 0.1f) {
        // Show low battery warning
        Serial.println("Warning: Low battery!");
    }
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

void handleScreenNavigation() {
    // This function handles navigation between screens
    // It would be called by button handlers or UI events
    
    static int currentScreenIndex = 0;
    Screen* screens[] = { homeScreen, ringtonesScreen };
    const int screenCount = sizeof(screens) / sizeof(screens[0]);
    
    // Example navigation logic
    currentScreenIndex = (currentScreenIndex + 1) % screenCount;
    uiManager.setScreen(screens[currentScreenIndex]);
}

void printSystemStatus() {
    Serial.println("\n=== Alert TX-1 System Status ===");
    Serial.printf("Uptime: %lu ms\n", millis());
    Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Battery Level: %.1f%%\n", powerManager.getBatteryLevel() * 100);
    Serial.printf("Ringtones Available: %d\n", ringtonePlayer.getRingtoneCount());
    Serial.printf("UI Framework: %s\n", UIFramework::VERSION);
    Serial.println("================================\n");
}
