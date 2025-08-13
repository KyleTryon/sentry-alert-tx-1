#include "SplashScreen.h"
#include "../core/ScreenManager.h"

SplashScreen::SplashScreen(Adafruit_ST7789* display, MainMenuScreen* mainMenu)
    : Screen(display, "Splash", 0), mainMenuScreen(mainMenu) {
    
    Serial.println("SplashScreen created");
}

SplashScreen::~SplashScreen() {
    Serial.println("SplashScreen destroyed");
}

void SplashScreen::enter() {
    Screen::enter();
    
    startTimer();
    hasStarted = true;
    hasDrawn = false;  // Reset draw flag
    
    DisplayUtils::debugScreenEnter("SPLASH");
}

void SplashScreen::exit() {
    Screen::exit();
    hasStarted = false;
    Serial.println("Exited SplashScreen");
}

void SplashScreen::update() {
    Screen::update();
    
    // Check if it's time to transition
    if (hasStarted && shouldTransition()) {
        Serial.printf("=== SPLASH TIMEOUT === Elapsed: %lu ms\n", getElapsedTime());
        transitionToMainMenu();
    }
}

void SplashScreen::draw() {
    // Only draw once to prevent flickering from constant redraws
    if (hasDrawn) return;
    
    // Clear screen with theme background
    display->fillScreen(ThemeManager::getBackground());
    
    // Draw the splash content
    drawLogo();
    drawSubtitle();
    
    // DON'T call Screen::draw() - splash screen has no components and it might interfere
    
    // Force display update and add small delay to ensure rendering completes
    delay(10);  // Small delay to ensure display has time to render
    
    // Mark as drawn to prevent redrawing
    hasDrawn = true;
    
    #ifdef DEBUG_SPLASH
    Serial.println("SplashScreen: Content rendered");
    #endif
}

void SplashScreen::handleButtonPress(int button) {
    // Any button press skips the splash screen
    Serial.printf("Button %d pressed - skipping splash screen\n", button);
    transitionToMainMenu();
}

void SplashScreen::drawLogo() {
    // Display "SENTRY" in large, centered text
    display->setTextColor(ThemeManager::getPrimaryText());
    DisplayUtils::centerText(display, "SENTRY", LOGO_TEXT_SIZE, 60);
}

void SplashScreen::drawSubtitle() {
    // Display subtitle below the logo
    display->setTextColor(ThemeManager::getSecondaryText());
    DisplayUtils::centerText(display, "Alert TX-1", SUBTITLE_TEXT_SIZE, 90);
}

bool SplashScreen::shouldTransition() const {
    return hasStarted && (getElapsedTime() >= SPLASH_DURATION);
}

void SplashScreen::transitionToMainMenu() {
    if (!mainMenuScreen) {
        Serial.println("ERROR: No main menu screen set for transition!");
        return;
    }
    
    Serial.println("Transitioning from splash to main menu...");
    
    // Use the global screen manager to navigate
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->switchToScreen(mainMenuScreen);
    } else {
        Serial.println("ERROR: No global screen manager available!");
    }
}

void SplashScreen::startTimer() {
    splashStartTime = millis();
}

unsigned long SplashScreen::getElapsedTime() const {
    if (!hasStarted) return 0;
    return millis() - splashStartTime;
}
