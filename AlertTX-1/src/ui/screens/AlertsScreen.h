#ifndef ALERTSSCREEN_H
#define ALERTSSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"

/**
 * AlertsScreen
 * 
 * Demo screen for alerts functionality. Shows information about
 * the alert system and current status.
 * 
 * Features:
 * - Display system information
 * - Show alert status
 * - Long press for back navigation
 * - Simple informational display
 */

class AlertsScreen : public Screen {
private:
    // Display content
    static const char* SCREEN_TITLE;
    static const char* INFO_TEXT[];
    static const int INFO_LINE_COUNT;
    
    // Layout constants
    static const int LINE_HEIGHT = 12;
    static const int START_Y = 40;
    
public:
    AlertsScreen(Adafruit_ST7789* display);
    ~AlertsScreen();
    
    // Screen lifecycle
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    
    // Input handling
    void handleButtonPress(int button) override;
    
private:
    // Drawing helpers
    void drawContent();
    void drawInfoLine(const char* text, int lineNumber);
};

#endif // ALERTSSCREEN_H
