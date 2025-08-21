#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "../core/Screen.h"
#include "../core/ScreenManager.h"
#include "../core/DisplayUtils.h"
#include "MainMenuScreen.h"

/**
 * SplashScreen
 * 
 * Displays the "SENTRY" logo/text on boot for 2 seconds,
 * then automatically transitions to the main menu.
 * 
 * Features:
 * - Auto-transition after 2 seconds
 * - Centered text display
 * - Clean, branded appearance
 * - Skip on any button press
 */

class SplashScreen : public Screen {
private:
    // Timing
    unsigned long splashStartTime = 0;
    static const unsigned long SPLASH_DURATION = 5000;
    bool hasStarted = false;
    bool hasDrawn = false;  // Track if we've already drawn to prevent flickering
    
    // References
    MainMenuScreen* mainMenuScreen = nullptr;
    
    // Display configuration
    static const int LOGO_TEXT_SIZE = 3;  // Large text for "SENTRY"
    static const int SUBTITLE_TEXT_SIZE = 1;  // Small text for subtitle
    
public:
    SplashScreen(Adafruit_ST7789* display, MainMenuScreen* mainMenu);
    ~SplashScreen();
    
    // Screen lifecycle
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    
    // Input handling
    void handleButtonPress(int button) override;
    
    // Configuration
    void setMainMenuScreen(MainMenuScreen* mainMenu) { mainMenuScreen = mainMenu; }
    
private:
    // Drawing helpers
    void drawLogo();
    void drawSubtitle();
    
    // Transition helpers
    bool shouldTransition() const;
    void transitionToMainMenu();
    
    // Timing helpers
    void startTimer();
    unsigned long getElapsedTime() const;
};

#endif // SPLASHSCREEN_H
