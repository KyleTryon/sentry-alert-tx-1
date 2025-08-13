#include "AlertsScreen.h"
#include "../core/ScreenManager.h"

// Static content definitions
const char* AlertsScreen::SCREEN_TITLE = "Alerts";

const char* AlertsScreen::INFO_TEXT[] = {
    "System Status: Active",
    "MQTT: Disconnected", 
    "Last Alert: None",
    "",
    "Features:",
    "- Remote notifications",
    "- Custom ringtones",
    "- Visual indicators",
    "",
    "Long press any button",
    "to go back"
};

const int AlertsScreen::INFO_LINE_COUNT = 11;

AlertsScreen::AlertsScreen(Adafruit_ST7789* display)
    : Screen(display, "Alerts", 1) {
    Serial.println("AlertsScreen created");
}

AlertsScreen::~AlertsScreen() {
    Serial.println("AlertsScreen destroyed");
}

void AlertsScreen::enter() {
    Screen::enter();
    DisplayUtils::debugScreenEnter("ALERTS");
    Serial.println("Entered AlertsScreen");
}

void AlertsScreen::exit() {
    Screen::exit();
    DisplayUtils::debugScreenExit("ALERTS");
    Serial.println("Exited AlertsScreen");
}

void AlertsScreen::update() {
    Screen::update();
    // AlertsScreen doesn't need special update logic
    // Could add real-time status updates here
}

void AlertsScreen::draw() {
    Screen::draw();
    
    // Draw title
    DisplayUtils::drawTitle(display, SCREEN_TITLE);
    
    // Draw content
    drawContent();
}

void AlertsScreen::handleButtonPress(int button) {
    Serial.printf("AlertsScreen: Button %d pressed\n", button);
    
    // Currently no specific button actions for AlertsScreen
    // Long press back navigation is handled globally in main loop
    // Future: Could add specific alert management actions here
    
    Serial.println("AlertsScreen: No specific action for this button");
}

// Private helper methods

void AlertsScreen::drawContent() {
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(1);
    
    // Draw each info line
    for (int i = 0; i < INFO_LINE_COUNT; i++) {
        drawInfoLine(INFO_TEXT[i], i);
    }
}

void AlertsScreen::drawInfoLine(const char* text, int lineNumber) {
    if (!text) return;
    
    int y = START_Y + (lineNumber * LINE_HEIGHT);
    
    // Skip empty lines but maintain spacing
    if (strlen(text) == 0) return;
    
    // Different colors for different types of content
    if (strstr(text, "Status:") || strstr(text, "MQTT:") || strstr(text, "Last Alert:")) {
        display->setTextColor(ThemeManager::getSecondaryText());
    } else if (strstr(text, "Features:")) {
        display->setTextColor(ThemeManager::getAccent());
    } else if (strstr(text, "Long press")) {
        display->setTextColor(ThemeManager::getSecondaryText());
    } else {
        display->setTextColor(ThemeManager::getPrimaryText());
    }
    
    display->setCursor(15, y);  // 15px left margin
    display->println(text);
}
