#ifndef MAIN_MENU_SCREEN_H
#define MAIN_MENU_SCREEN_H

#include "../core/Screen.h"
#include "../components/StatusBar.h"
#include "../components/Menu.h"
#include "../components/MenuItem.h"
#include "../icons/IconDefinitions.h"

class MainMenuScreen : public Screen {
private:
    StatusBar statusBar;
    Menu menu;
    
    // Menu items array
    static const int MENU_ITEM_COUNT = 6;
    MenuItem menuItems[MENU_ITEM_COUNT];
    
    // Update timing
    unsigned long lastStatusUpdate = 0;
    static const unsigned long STATUS_UPDATE_INTERVAL = 1000; // Update status every second
    
    // Action callbacks
    static void onAlertsSelected();
    static void onGamesSelected();
    static void onSettingsSelected();
    static void onRingtonesSelected();
    static void onBeeperHeroSelected();
    static void onSystemInfoSelected();
    
public:
    MainMenuScreen();
    
    // Screen interface implementation
    void begin() override;
    void update() override;
    void draw() override;
    void handleInput(ButtonAction action) override;
    void handleEvent(const UIEvent& event) override;
    const char* getName() const override { return "MainMenu"; }
    
    // Status updates
    void updateBatteryLevel(int percentage);
    void updateConnectivityStatus(bool wifi, bool bluetooth);
    void updateSystemTime(int hours, int minutes, int seconds);
    
private:
    void initializeMenuItems();
    void updateStatusBar();
    void drawContent();
};

#endif // MAIN_MENU_SCREEN_H