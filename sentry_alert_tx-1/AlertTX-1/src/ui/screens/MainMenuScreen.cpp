#include "MainMenuScreen.h"
#include <Arduino.h>
#include <time.h>

MainMenuScreen::MainMenuScreen() {
    initializeMenuItems();
}

void MainMenuScreen::begin() {
    // Initialize menu with items
    menu.setItems(menuItems, MENU_ITEM_COUNT);
    
    // Initialize status bar
    statusBar.updateBatteryLevel(100);
    statusBar.updateWiFiStatus(false);
    statusBar.updateBluetoothStatus(false);
    statusBar.updateTime("00:00:00");
    
    // Mark for initial redraw
    markForRedraw();
    
    Serial.println("MainMenuScreen initialized");
}

void MainMenuScreen::update() {
    unsigned long currentTime = millis();
    
    // Update status bar periodically
    if (currentTime - lastStatusUpdate >= STATUS_UPDATE_INTERVAL) {
        updateStatusBar();
        lastStatusUpdate = currentTime;
    }
    
    // Check if components need redraw
    if (statusBar.hasChanged() || menu.hasChanged()) {
        markForRedraw();
    }
}

void MainMenuScreen::draw() {
    if (!renderer || !needsRedraw()) return;
    
    // Clear screen with background color
    renderer->fillScreen(theme.background);
    
    // Draw status bar at top
    statusBar.draw(renderer, theme);
    
    // Draw menu below status bar
    int menuY = statusBar.getHeight();
    menu.draw(renderer, theme, menuY);
    
    clearRedrawFlag();
}

void MainMenuScreen::handleInput(ButtonAction action) {
    // Pass input to menu for navigation
    menu.handleInput(action);
    
    // Check if menu changed
    if (menu.hasChanged()) {
        markForRedraw();
    }
}

void MainMenuScreen::handleEvent(const UIEvent& event) {
    // Pass events to menu
    menu.handleEvent(event);
    
    // Handle additional screen-specific events
    switch (event.type) {
        case UIEventType::MENU_SELECT:
            Serial.print("Menu item selected: ");
            Serial.println(event.menuIndex);
            break;
        default:
            break;
    }
}

void MainMenuScreen::updateBatteryLevel(int percentage) {
    statusBar.updateBatteryLevel(percentage);
}

void MainMenuScreen::updateConnectivityStatus(bool wifi, bool bluetooth) {
    statusBar.updateWiFiStatus(wifi);
    statusBar.updateBluetoothStatus(bluetooth);
}

void MainMenuScreen::updateSystemTime(int hours, int minutes, int seconds) {
    statusBar.updateTime(hours, minutes, seconds);
}

void MainMenuScreen::initializeMenuItems() {
    // Initialize menu items with icons and callbacks
    menuItems[0] = MenuItem("Alerts", ICON_ALERT, 1, onAlertsSelected);
    menuItems[1] = MenuItem("Games", ICON_GAMES, 2, onGamesSelected);
    menuItems[2] = MenuItem("Settings", ICON_SETTINGS, 3, onSettingsSelected);
    menuItems[3] = MenuItem("Ringtones", ICON_MUSIC, 4, onRingtonesSelected);
    menuItems[4] = MenuItem("BeeperHero", ICON_PLAY, 5, onBeeperHeroSelected);
    menuItems[5] = MenuItem("System Info", ICON_MAIL, 6, onSystemInfoSelected); // Using mail as placeholder
}

void MainMenuScreen::updateStatusBar() {
    // Update time
    time_t now;
    struct tm* timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    
    if (timeinfo) {
        statusBar.updateTime(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    } else {
        // Fallback: use millis() for a simple clock
        unsigned long seconds = (millis() / 1000) % 86400; // Seconds in a day
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        statusBar.updateTime(hours, minutes, secs);
    }
    
    // TODO: Update actual battery level from hardware
    // statusBar.updateBatteryLevel(getBatteryLevel());
    
    // TODO: Update actual connectivity status
    // statusBar.updateWiFiStatus(WiFi.isConnected());
    // statusBar.updateBluetoothStatus(isBluetoothConnected());
}

void MainMenuScreen::drawContent() {
    // Additional content drawing if needed
    // Currently handled in main draw() method
}

// Static callback implementations
void MainMenuScreen::onAlertsSelected() {
    Serial.println("Alerts menu selected");
    // TODO: Switch to Alerts screen
}

void MainMenuScreen::onGamesSelected() {
    Serial.println("Games menu selected");
    // TODO: Switch to Games screen
}

void MainMenuScreen::onSettingsSelected() {
    Serial.println("Settings menu selected");
    // TODO: Switch to Settings screen
}

void MainMenuScreen::onRingtonesSelected() {
    Serial.println("Ringtones menu selected");
    // TODO: Switch to Ringtones screen
}

void MainMenuScreen::onBeeperHeroSelected() {
    Serial.println("BeeperHero menu selected");
    // TODO: Switch to BeeperHero screen/game
}

void MainMenuScreen::onSystemInfoSelected() {
    Serial.println("System Info menu selected");
    // TODO: Switch to System Info screen
}