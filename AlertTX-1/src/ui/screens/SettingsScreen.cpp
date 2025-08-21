#include "SettingsScreen.h"
#include "../core/ScreenManager.h"
#include "ThemeSelectionScreen.h"
#include "RingtonesScreen.h"
#include "SystemInfoScreen.h"
#include "../../config/SettingsManager.h"
#include "../../ringtones/RingtonePlayer.h"
#include "../../hardware/LED.h"
#include <WiFi.h>

// Access to global hardware
extern LED statusLed;

// Static members
SettingsScreen* SettingsScreen::instance = nullptr;

const char* SettingsScreen::RINGTONE_NAMES[] = {
    "Classic Ring",
    "Mario Theme", 
    "Nokia Tune",
    "Zelda Theme",
    "Futurama"
};

SettingsScreen::SettingsScreen(Adafruit_ST7789* display)
    : Screen(display, "Settings", 3), currentRingtoneIndex(0), themeSelectionScreen(nullptr) {
    
    instance = this;  // Set static instance for callbacks
    
    // Create settings menu container
    settingsMenu = new MenuContainer(display, 10, 50);
    
    // Add menu to screen components
    addComponent(settingsMenu);
    
    // Initialize child screens
    initializeChildScreens();
    
    // Setup menu items
    setupMenu();
    
    // Set up draw regions for efficient rendering
    addDrawRegion(DirectDrawRegion::STATIC, [this, display]() { 
        DisplayUtils::drawTitle(display, "Settings");
    });
    
    Serial.println("SettingsScreen created");
}

SettingsScreen::~SettingsScreen() {
    // Clean up child screens
    cleanupChildScreens();
    
    // Components are automatically cleaned up by Screen destructor
    instance = nullptr;
    Serial.println("SettingsScreen destroyed");
}

void SettingsScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("SETTINGS");
    Serial.println("Entered SettingsScreen");
    
    // Reset menu selection
    settingsMenu->setSelectedIndex(0);
    
    // Ensure LED state matches flashlight setting
    bool flashlightOn = SettingsManager::getFlashlightEnabled();
    if (flashlightOn) {
        statusLed.on();
        ringtonePlayer.setLedSyncEnabled(false);
    }
}

void SettingsScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("SETTINGS");
    Serial.println("Exited SettingsScreen");
}

void SettingsScreen::update() {
    Screen::update();
    // SettingsScreen doesn't need special update logic
    // Components handle their own updates
}

void SettingsScreen::draw() {
    // Base class handles drawing based on dirty regions
    Screen::draw();
}

void SettingsScreen::handleButtonPress(int button) {
    Serial.printf("SettingsScreen: Button %d pressed\n", button);
    
    // Check for long press back navigation
    // Note: This will be enhanced when long press detection is integrated
    
    // For now, route to menu for navigation
    if (settingsMenu) {
        settingsMenu->handleButtonPress(button);
    }
}

// Settings actions

void SettingsScreen::onRingtoneSelected() {
    Serial.println("SettingsScreen: Ringtone selected");
    if (ringtonesScreen) {
        ScreenManager* manager = GlobalScreenManager::getInstance();
        if (manager) {
            manager->pushScreen(ringtonesScreen);
            Serial.println("SettingsScreen: Navigated to ringtones");
        }
    }
}

void SettingsScreen::onThemesSelected() {
    Serial.println("SettingsScreen: Themes selected - navigating to theme selection");
    
    navigateToThemeSelection();
}

void SettingsScreen::onSystemInfoSelected() {
    Serial.println("SettingsScreen: System Info selected");
    navigateToSystemInfo();
}

void SettingsScreen::onFlashlightSelected() {
    Serial.println("SettingsScreen: Flashlight selected");
    
    // Toggle flashlight state
    bool currentState = SettingsManager::getFlashlightEnabled();
    bool newState = !currentState;
    
    // Save the new state
    SettingsManager::setFlashlightEnabled(newState);
    
    // Apply the state to the LED and toggle ringtone LED sync
    if (newState) {
        // Turn on flashlight and disable LED sync with ringtones
        statusLed.on();
        ringtonePlayer.setLedSyncEnabled(false);
        Serial.println("Flashlight: ON (LED sync disabled)");
    } else {
        // Turn off flashlight and re-enable LED sync
        statusLed.off();
        ringtonePlayer.setLedSyncEnabled(true);
        Serial.println("Flashlight: OFF (LED sync enabled)");
    }
    
    // Update menu to show current state
    if (settingsMenu) {
        // Update the menu item text to show current state
        // Flashlight is the 4th item (index 3)
        MenuItem* flashlightItem = settingsMenu->getItem(3);
        if (flashlightItem) {
            flashlightItem->setLabel(newState ? "Flashlight [ON]" : "Flashlight [OFF]");
            settingsMenu->markDirty();
        }
    }
}

// Static callback wrappers
void SettingsScreen::ringtoneCallback() {
    if (instance) {
        instance->onRingtoneSelected();
    }
}

void SettingsScreen::themeCallback() {
    if (instance) {
        instance->onThemesSelected();
    }
}

void SettingsScreen::systemInfoCallback() {
    if (instance) {
        instance->onSystemInfoSelected();
    }
}

void SettingsScreen::flashlightCallback() {
    if (instance) {
        instance->onFlashlightSelected();
    }
}

// Private helper methods

void SettingsScreen::setupMenu() {
    if (!settingsMenu) return;
    
    createMenuItems();
    
    Serial.println("SettingsScreen menu setup complete");
}

void SettingsScreen::createMenuItems() {
    // Clear existing items
    settingsMenu->clear();
    
    // Add settings menu items
    settingsMenu->addMenuItem("Ringtone", 1, ringtoneCallback);
    settingsMenu->addMenuItem("Themes", 2, themeCallback);
    settingsMenu->addMenuItem("System Info", 3, systemInfoCallback);
    
    // Add flashlight with current state
    bool flashlightOn = SettingsManager::getFlashlightEnabled();
    settingsMenu->addMenuItem(flashlightOn ? "Flashlight [ON]" : "Flashlight [OFF]", 
                              4, flashlightCallback);
    
    // Auto-layout the menu
    settingsMenu->autoLayout();
    
    Serial.println("Created SettingsScreen menu items");
}

void SettingsScreen::cycleRingtone() {
    int total = ringtonePlayer.getRingtoneCount();
    if (total <= 0) return;
    if (currentRingtoneIndex < 0 || currentRingtoneIndex >= total) {
        currentRingtoneIndex = SettingsManager::getRingtoneIndex();
    }
    currentRingtoneIndex = (currentRingtoneIndex + 1) % total;
    const char* name = ringtonePlayer.getRingtoneName(currentRingtoneIndex);
    Serial.printf("Ringtone changed to: %s (%d/%d)\n", name ? name : "(unknown)", currentRingtoneIndex + 1, total);
    SettingsManager::setRingtoneIndex(currentRingtoneIndex);
    ringtonePlayer.playRingtoneByIndex(currentRingtoneIndex);
}

void SettingsScreen::navigateToThemeSelection() {
    if (!themeSelectionScreen) {
        Serial.println("ERROR: ThemeSelectionScreen not initialized!");
        return;
    }
    
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(themeSelectionScreen);
        Serial.println("SettingsScreen: Navigated to theme selection");
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

void SettingsScreen::navigateToSystemInfo() {
    if (!systemInfoScreen) {
        Serial.println("ERROR: SystemInfoScreen not initialized!");
        return;
    }
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(systemInfoScreen);
        Serial.println("SettingsScreen: Navigated to system info");
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

void SettingsScreen::showSystemInfo() {
    Serial.println("=== SYSTEM INFORMATION ===");
    // WiFi status
    bool wifiConnected = (WiFi.status() == WL_CONNECTED);
    String activeSsid = wifiConnected ? WiFi.SSID() : SettingsManager::getWifiSsid();
    Serial.printf("WiFi SSID: %s\n", activeSsid.c_str());
    Serial.printf("WiFi connected: %s\n", wifiConnected ? "Yes" : "No");
    if (wifiConnected) {
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    }
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("CPU frequency: %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash size: %u KB\n", ESP.getFlashChipSize() / 1024);
    Serial.printf("Sketch size: %u bytes\n", ESP.getSketchSize());
    Serial.printf("Free sketch space: %u bytes\n", ESP.getFreeSketchSpace());
    Serial.println("========================");
    
    // TODO: Could display this information on screen instead of just serial
    Serial.println("System info displayed in serial monitor");
}

// Screen management

void SettingsScreen::initializeChildScreens() {
    Serial.println("SettingsScreen: Initializing child screens...");
    
    // Create theme selection screen
    themeSelectionScreen = new ThemeSelectionScreen(display);
    ringtonesScreen = new RingtonesScreen(display);
    systemInfoScreen = new SystemInfoScreen(display);
    
    Serial.println("SettingsScreen: Child screens initialized");
}

void SettingsScreen::cleanupChildScreens() {
    Serial.println("SettingsScreen: Cleaning up child screens...");
    
    if (themeSelectionScreen) {
        delete themeSelectionScreen;
        themeSelectionScreen = nullptr;
    }
    if (ringtonesScreen) {
        delete ringtonesScreen;
        ringtonesScreen = nullptr;
    }
    
    Serial.println("SettingsScreen: Child screens cleaned up");
}
