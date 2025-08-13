#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../components/MenuContainer.h"

// Forward declaration for navigation
class ThemeSelectionScreen;

/**
 * SettingsScreen
 * 
 * Settings menu with navigable options. Uses the MenuContainer
 * component for consistent navigation experience.
 * 
 * Menu Options:
 * - Ringtone: Select and preview ringtones
 * - Themes: Navigate to dedicated theme selection screen
 * - System Info: Display system information
 * 
 * Features:
 * - MenuContainer-based navigation
 * - Long press for back navigation
 * - Navigation to ThemeSelectionScreen
 * - System information display
 */

class SettingsScreen : public Screen {
private:
    MenuContainer* settingsMenu;
    
    // Current settings state
    int currentRingtoneIndex;
    static const int MAX_RINGTONES = 5;
    static const char* RINGTONE_NAMES[];
    
    // Child screen for theme selection
    ThemeSelectionScreen* themeSelectionScreen;
    class SystemInfoScreen* systemInfoScreen;
    
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
    void onThemesSelected();  // Navigate to theme selection screen
    void onSystemInfoSelected();
    void onPowerSelected();
    static void powerCallback();
    
    // Static callback wrappers
    static void ringtoneCallback();
    static void themeCallback();
    static void systemInfoCallback();
    
    // Future: navigate to Power settings screen
    void navigateToPowerSettings();
    
private:
    // Helper methods
    void setupMenu();
    void createMenuItems();
    void cycleRingtone();
    void navigateToThemeSelection();
    void navigateToSystemInfo();
    void showSystemInfo();
    
    // Screen management
    void initializeChildScreens();
    void cleanupChildScreens();
    
    // Display helpers
    void drawSystemInfo();
    void drawRingtoneInfo();
};

#endif // SETTINGSSCREEN_H
