#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include "Screen.h"

/**
 * ScreenManager
 * 
 * Manages navigation between different screens in the Alert TX-1 UI framework.
 * Provides a simple stack-based navigation system with screen transitions.
 * 
 * Features:
 * - Stack-based navigation (push/pop screens)
 * - Automatic screen lifecycle management
 * - Memory efficient (fixed arrays)
 * - Transition support
 * - Global input routing
 */

class ScreenManager {
private:
    Adafruit_ST7789* display;
    
    // Screen stack (fixed array for memory efficiency)
    static const int MAX_SCREEN_STACK = 8;  // Maximum navigation depth
    Screen* screenStack[MAX_SCREEN_STACK];
    bool ownedStack[MAX_SCREEN_STACK];
    int stackSize = 0;
    
    // Current screen state
    Screen* currentScreen = nullptr;
    bool currentOwned = false;
    bool needsRedraw = true;
    
    // Transition management
    bool inTransition = false;
    unsigned long transitionStartTime = 0;
    static const unsigned long TRANSITION_DURATION = 200;  // 200ms transitions
    // Global input cooldown after navigation to prevent stale presses
    unsigned long inputCooldownUntilMs = 0;
    static const unsigned long INPUT_COOLDOWN_MS = 300;
    
    // Performance tracking
    unsigned long lastUpdateTime = 0;
    unsigned long lastDrawTime = 0;
    static const unsigned long UPDATE_INTERVAL = 16;  // ~60 FPS
    
public:
    ScreenManager(Adafruit_ST7789* display);
    ~ScreenManager();
    
    // Core update loop (call from main Arduino loop)
    void update();
    void draw();
    
    // Screen navigation
    bool pushScreen(Screen* screen, bool takeOwnership = false); // Navigate to new screen
    bool popScreen();                         // Go back to previous screen
    bool switchToScreen(Screen* screen);      // Replace current screen
    void clearStack();                        // Clear all screens
    
    // Screen management
    Screen* getCurrentScreen() const { return currentScreen; }
    Screen* getPreviousScreen() const;
    int getStackSize() const { return stackSize; }
    bool isEmpty() const { return stackSize == 0; }
    
    // Input handling - routes to current screen
    void handleButtonPress(int button);
    void handleButtonLongPress(int button);
    
    // Force redraw
    void invalidate() { needsRedraw = true; }
    
    // Transition control
    bool isInTransition() const { return inTransition; }
    void setTransitionDuration(unsigned long duration);
    
    // Performance monitoring
    void printStackState() const;
    void printPerformanceStats() const;
    
    // Validation
    bool validate() const;
    
private:
    // Stack management helpers
    bool pushToStack(Screen* screen, bool owned);
    Screen* popFromStack(bool& ownedOut);
    void setCurrentScreen(Screen* screen, bool owned);
    
    // Transition management
    void startTransition();
    void updateTransition();
    bool isTransitionComplete() const;
    
    // Drawing helpers
    void drawTransition();
    void drawScreen(Screen* screen);
    
    // Performance helpers
    bool shouldUpdate() const;
    bool shouldDraw() const;
    
    // Validation helpers
    bool isValidScreen(Screen* screen) const;
    void validateStack() const;
};

/**
 * Global ScreenManager Instance
 * 
 * Provides global access to the screen manager for easy navigation
 * from any component or screen.
 */
class GlobalScreenManager {
private:
    static ScreenManager* instance;
    
public:
    static void setInstance(ScreenManager* manager) { instance = manager; }
    static ScreenManager* getInstance() { return instance; }
    
    // Convenience methods for global access
    static bool navigateTo(Screen* screen) {
        return instance ? instance->pushScreen(screen) : false;
    }
    
    static bool goBack() {
        return instance ? instance->popScreen() : false;
    }
    
    static Screen* getCurrentScreen() {
        return instance ? instance->getCurrentScreen() : nullptr;
    }
};

#endif // SCREENMANAGER_H
