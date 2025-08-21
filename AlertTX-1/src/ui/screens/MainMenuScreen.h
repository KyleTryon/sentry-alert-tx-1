#ifndef MAINMENUSCREEN_H
#define MAINMENUSCREEN_H

#include "../core/Screen.h"
#include "../components/MenuContainer.h"

// Forward declarations for screen navigation
class AlertsScreen;
class GamesScreen;
class SettingsScreen;
class HardwareTestScreen;

/**
 * MainMenuScreen
 * 
 * The main menu screen for the Alert TX-1 device. This demonstrates
 * the new component-based screen architecture using MenuContainer
 * and MenuItem components.
 * 
 * Features:
 * - Component-based architecture
 * - Theme switching demonstration
 * - Navigation to other screens
 * - Proper screen lifecycle management
 */

class MainMenuScreen : public Screen {
private:
    MenuContainer* mainMenu;
    
    // Theme cycling for demonstration
    static const int THEME_COUNT = 4;
    static const Theme* themes[THEME_COUNT];
    int currentThemeIndex = 0;
    
public:
    MainMenuScreen(Adafruit_ST7789* display);
    virtual ~MainMenuScreen();
    
    // Screen interface implementation
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    void handleButtonPress(int button) override;
    
    // MainMenuScreen specific
    void setupMenu();
    void cycleTheme();
    
    // Menu action callbacks
    void onAlertsSelected();
    void onGamesSelected();
    void onSettingsSelected();
    void onHardwareTestSelected();
    
    // Static callback wrappers for C-style callbacks
    static void alertsCallback();
    static void gamesCallback();
    static void settingsCallback();
    static void hardwareTestCallback();
    
private:
    // Screen instances for navigation
    AlertsScreen* alertsScreen;
    GamesScreen* gamesScreen;
    SettingsScreen* settingsScreen;
    HardwareTestScreen* hardwareTestScreen;
    
    // Helper methods
    void createMenuItems();
    void updateTitle();
    void initializeScreens();
    void cleanupScreens();
    
    // Static instance for callbacks
    static MainMenuScreen* instance;
};

#endif // MAINMENUSCREEN_H
