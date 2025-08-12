#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../components/MenuContainer.h"

/**
 * SettingsScreen
 * 
 * Settings menu with navigable options. Uses the MenuContainer
 * component for consistent navigation experience.
 * 
 * Menu Options:
 * - Ringtone: Select and preview ringtones
 * - Theme: Cycle through available themes
 * - System Info: Display system information
 * 
 * Features:
 * - MenuContainer-based navigation
 * - Long press for back navigation
 * - Theme switching functionality
 * - System information display
 */

class SettingsScreen : public Screen {
private:
    MenuContainer* settingsMenu;
    
    // Current settings state
    int currentRingtoneIndex;
    static const int MAX_RINGTONES = 5;
    static const char* RINGTONE_NAMES[];
    
    // Theme management (from MainMenuScreen)
    static const int THEME_COUNT = 4;
    static const Theme* themes[THEME_COUNT];
    int currentThemeIndex = 0;
    
    // Static instance for callbacks
    static SettingsScreen* instance;
    
public:
    SettingsScreen(Adafruit_ST7789* display);
    ~SettingsScreen();
    
    // Screen lifecycle
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    
    // Input handling
    void handleButtonPress(int button) override;
    
    // Settings actions
    void onRingtoneSelected();
    void onThemeSelected();
    void onSystemInfoSelected();
    
    // Static callback wrappers
    static void ringtoneCallback();
    static void themeCallback();
    static void systemInfoCallback();
    
private:
    // Helper methods
    void setupMenu();
    void createMenuItems();
    void cycleRingtone();
    void cycleTheme();
    void showSystemInfo();
    
    // Display helpers
    void drawSystemInfo();
    void drawRingtoneInfo();
};

#endif // SETTINGSSCREEN_H
