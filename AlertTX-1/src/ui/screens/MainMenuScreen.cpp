#include "MainMenuScreen.h"
#include "../core/ScreenManager.h"

// Static members
MainMenuScreen* MainMenuScreen::instance = nullptr;

const Theme* MainMenuScreen::themes[THEME_COUNT] = {
    &THEME_DEFAULT,
    &THEME_TERMINAL,
    &THEME_AMBER,
    &THEME_HIGH_CONTRAST
};

MainMenuScreen::MainMenuScreen(Adafruit_ST7789* display)
    : Screen(display, "MainMenu", 1) {
    
    instance = this;  // Set static instance for callbacks
    
    // Create main menu container
    mainMenu = new MenuContainer(display, 10, 50);
    
    // Add menu to screen components
    addComponent(mainMenu);
    
    // Setup menu items
    setupMenu();
    
    Serial.println("MainMenuScreen created");
}

MainMenuScreen::~MainMenuScreen() {
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
    
    // TODO: Navigate to AlertsScreen when it's implemented
    // For now, just provide feedback
    Serial.println("  -> Alerts functionality not yet implemented");
}

void MainMenuScreen::onGamesSelected() {
    Serial.println("MainMenuScreen: Games selected");
    
    // TODO: Navigate to GamesScreen when it's implemented
    // For now, just provide feedback
    Serial.println("  -> Games functionality not yet implemented");
}

void MainMenuScreen::onSettingsSelected() {
    Serial.println("MainMenuScreen: Settings selected");
    
    // Demonstrate theme cycling
    cycleTheme();
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

// Private helper methods

void MainMenuScreen::createMenuItems() {
    // Clear existing items
    mainMenu->clear();
    
    // Add menu items using the MenuContainer interface
    mainMenu->addMenuItem("Alerts", 1, alertsCallback);
    mainMenu->addMenuItem("Games", 2, gamesCallback);
    mainMenu->addMenuItem("Settings", 3, settingsCallback);
    
    // Auto-layout the menu
    mainMenu->autoLayout();
    
    Serial.println("Created MainMenuScreen menu items");
}

void MainMenuScreen::updateTitle() {
    // Title is drawn in draw() method
    // This could update dynamic title content if needed
}
