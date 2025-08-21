#include "MainMenuScreen.h"
#include "../core/ScreenManager.h"
#include "AlertsScreen.h"
#include "GamesScreen.h"
#include "SettingsScreen.h"
#include "HardwareTestScreen.h"
#include "../../hardware/Buzzer.h"
#include "../../hardware/LED.h"

// Static members
MainMenuScreen* MainMenuScreen::instance = nullptr;

const Theme* MainMenuScreen::themes[THEME_COUNT] = {
    &THEME_DEFAULT,
    &THEME_TERMINAL,
    &THEME_AMBER,
    &THEME_HIGH_CONTRAST
};

MainMenuScreen::MainMenuScreen(Adafruit_ST7789* display)
    : Screen(display, "MainMenu", 1), alertsScreen(nullptr), gamesScreen(nullptr), 
      settingsScreen(nullptr), hardwareTestScreen(nullptr) {
    
    instance = this;  // Set static instance for callbacks
    
    // Create main menu container
    mainMenu = new MenuContainer(display, 10, 50);
    
    // Add menu to screen components
    addComponent(mainMenu);
    
    // Initialize child screens
    initializeScreens();
    
    // Setup menu items
    setupMenu();
    
    Serial.println("MainMenuScreen created");
}

MainMenuScreen::~MainMenuScreen() {
    // Clean up child screens
    cleanupScreens();
    
    // Components are automatically cleaned up by Screen destructor
    instance = nullptr;
    Serial.println("MainMenuScreen destroyed");
}

void MainMenuScreen::enter() {
    Screen::enter();
    
    Serial.println("Entered MainMenuScreen");
    updateTitle();
    
    // Reset menu selection
    mainMenu->setSelectedIndex(0);
}

void MainMenuScreen::exit() {
    Screen::exit();
    Serial.println("Exited MainMenuScreen");
}

void MainMenuScreen::update() {
    Screen::update();
    
    // MainMenuScreen doesn't need special update logic
    // Components handle their own updates
}

void MainMenuScreen::draw() {
    Screen::draw();
    
    // Draw title
    drawTitle("Alert TX-1");
    
    // Components are drawn by Screen::draw()
}

void MainMenuScreen::handleButtonPress(int button) {
    // Route button presses to the menu
    if (mainMenu) {
        mainMenu->handleButtonPress(button);
    }
}

void MainMenuScreen::setupMenu() {
    if (!mainMenu) return;
    
    createMenuItems();
    
    Serial.println("MainMenuScreen menu setup complete");
}

void MainMenuScreen::cycleTheme() {
    currentThemeIndex = (currentThemeIndex + 1) % THEME_COUNT;
    ThemeManager::setTheme(themes[currentThemeIndex]);
    
    const char* themeNames[] = {"Default", "Terminal", "Amber", "High Contrast"};
    Serial.printf("Theme changed to: %s\n", themeNames[currentThemeIndex]);
    
    // Force full redraw with new theme
    markForFullRedraw();
}

void MainMenuScreen::onAlertsSelected() {
    Serial.println("MainMenuScreen: Alerts selected");
    
    if (!alertsScreen) {
        Serial.println("ERROR: AlertsScreen not initialized!");
        return;
    }
    
    // Navigate to AlertsScreen
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(alertsScreen);
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

void MainMenuScreen::onGamesSelected() {
    Serial.println("MainMenuScreen: Games selected");
    
    if (!gamesScreen) {
        Serial.println("ERROR: GamesScreen not initialized!");
        return;
    }
    
    // Navigate to GamesScreen
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(gamesScreen);
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

void MainMenuScreen::onSettingsSelected() {
    Serial.println("MainMenuScreen: Settings selected");
    
    if (!settingsScreen) {
        Serial.println("ERROR: SettingsScreen not initialized!");
        return;
    }
    
    // Navigate to SettingsScreen
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(settingsScreen);
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

void MainMenuScreen::onHardwareTestSelected() {
    Serial.println("MainMenuScreen: Hardware Test selected");
    
    if (!hardwareTestScreen) {
        Serial.println("ERROR: HardwareTestScreen not initialized!");
        return;
    }
    
    // Navigate to HardwareTestScreen
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->pushScreen(hardwareTestScreen);
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

// Static callback wrappers
void MainMenuScreen::alertsCallback() {
    if (instance) {
        instance->onAlertsSelected();
    }
}

void MainMenuScreen::gamesCallback() {
    if (instance) {
        instance->onGamesSelected();
    }
}

void MainMenuScreen::settingsCallback() {
    if (instance) {
        instance->onSettingsSelected();
    }
}

void MainMenuScreen::hardwareTestCallback() {
    if (instance) {
        instance->onHardwareTestSelected();
    }
}

// Private helper methods

void MainMenuScreen::createMenuItems() {
    // Clear existing items
    mainMenu->clear();
    
    // Add menu items using the MenuContainer interface
    mainMenu->addMenuItem("Alerts", 1, alertsCallback);
    mainMenu->addMenuItem("Games", 2, gamesCallback);
    mainMenu->addMenuItem("Settings", 3, settingsCallback);
    mainMenu->addMenuItem("Hardware Test", 4, hardwareTestCallback);
    
    // Auto-layout the menu
    mainMenu->autoLayout();
    
    Serial.println("Created MainMenuScreen menu items");
}

void MainMenuScreen::updateTitle() {
    // Title is drawn in draw() method
    // This could update dynamic title content if needed
}

void MainMenuScreen::initializeScreens() {
    Serial.println("MainMenuScreen: Initializing child screens...");
    
    // Create screen instances
    alertsScreen = new AlertsScreen(display);
    gamesScreen = new GamesScreen(display);
    settingsScreen = new SettingsScreen(display);
    
    // For HardwareTestScreen, we need to get reference to LED
    // This should be available globally
    extern LED statusLed;
    hardwareTestScreen = new HardwareTestScreen(display, &statusLed);
    
    Serial.println("MainMenuScreen: All child screens initialized");
}

void MainMenuScreen::cleanupScreens() {
    Serial.println("MainMenuScreen: Cleaning up child screens...");
    
    // Delete screen instances
    if (alertsScreen) {
        delete alertsScreen;
        alertsScreen = nullptr;
    }
    
    if (gamesScreen) {
        delete gamesScreen;
        gamesScreen = nullptr;
    }
    
    if (settingsScreen) {
        delete settingsScreen;
        settingsScreen = nullptr;
    }
    
    if (hardwareTestScreen) {
        delete hardwareTestScreen;
        hardwareTestScreen = nullptr;
    }
    
    Serial.println("MainMenuScreen: Child screens cleaned up");
}
