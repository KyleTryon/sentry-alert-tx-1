#include "SettingsScreen.h"
#include "../core/ScreenManager.h"

// Static members
SettingsScreen* SettingsScreen::instance = nullptr;

const char* SettingsScreen::RINGTONE_NAMES[] = {
    "Classic Ring",
    "Mario Theme", 
    "Nokia Tune",
    "Zelda Theme",
    "Futurama"
};

const Theme* SettingsScreen::themes[THEME_COUNT] = {
    &THEME_DEFAULT,
    &THEME_TERMINAL,
    &THEME_AMBER,
    &THEME_HIGH_CONTRAST
};

SettingsScreen::SettingsScreen(Adafruit_ST7789* display)
    : Screen(display, "Settings", 3), currentRingtoneIndex(0) {
    
    instance = this;  // Set static instance for callbacks
    
    // Create settings menu container
    settingsMenu = new MenuContainer(display, 10, 50);
    
    // Add menu to screen components
    addComponent(settingsMenu);
    
    // Setup menu items
    setupMenu();
    
    Serial.println("SettingsScreen created");
}

SettingsScreen::~SettingsScreen() {
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
    Screen::draw();
    
    // Draw title
    DisplayUtils::drawTitle(display, "Settings");
    
    // Components are drawn by Screen::draw()
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
    
    cycleRingtone();
    
    // Force redraw to show new ringtone info
    markForFullRedraw();
}

void SettingsScreen::onThemeSelected() {
    Serial.println("SettingsScreen: Theme selected");
    
    cycleTheme();
    
    // Force full redraw with new theme
    markForFullRedraw();
}

void SettingsScreen::onSystemInfoSelected() {
    Serial.println("SettingsScreen: System Info selected");
    
    showSystemInfo();
}

// Static callback wrappers
void SettingsScreen::ringtoneCallback() {
    if (instance) {
        instance->onRingtoneSelected();
    }
}

void SettingsScreen::themeCallback() {
    if (instance) {
        instance->onThemeSelected();
    }
}

void SettingsScreen::systemInfoCallback() {
    if (instance) {
        instance->onSystemInfoSelected();
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
    settingsMenu->addMenuItem("Theme", 2, themeCallback);
    settingsMenu->addMenuItem("System Info", 3, systemInfoCallback);
    
    // Auto-layout the menu
    settingsMenu->autoLayout();
    
    Serial.println("Created SettingsScreen menu items");
}

void SettingsScreen::cycleRingtone() {
    currentRingtoneIndex = (currentRingtoneIndex + 1) % MAX_RINGTONES;
    
    Serial.printf("Ringtone changed to: %s\n", RINGTONE_NAMES[currentRingtoneIndex]);
    
    // TODO: In the future, this could play a preview of the ringtone
    // using the RingtonePlayer system
}

void SettingsScreen::cycleTheme() {
    currentThemeIndex = (currentThemeIndex + 1) % THEME_COUNT;
    ThemeManager::setTheme(themes[currentThemeIndex]);
    
    const char* themeNames[] = {"Default", "Terminal", "Amber", "High Contrast"};
    Serial.printf("Theme changed to: %s\n", themeNames[currentThemeIndex]);
}

void SettingsScreen::showSystemInfo() {
    Serial.println("=== SYSTEM INFORMATION ===");
    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("CPU frequency: %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("Flash size: %u KB\n", ESP.getFlashChipSize() / 1024);
    Serial.printf("Sketch size: %u bytes\n", ESP.getSketchSize());
    Serial.printf("Free sketch space: %u bytes\n", ESP.getFreeSketchSpace());
    Serial.println("========================");
    
    // TODO: Could display this information on screen instead of just serial
    Serial.println("System info displayed in serial monitor");
}
