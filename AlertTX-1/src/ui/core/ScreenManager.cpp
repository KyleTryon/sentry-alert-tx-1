#include "ScreenManager.h"
#include "Theme.h"

// Initialize static member
ScreenManager* GlobalScreenManager::instance = nullptr;

ScreenManager::ScreenManager(Adafruit_ST7789* display) : display(display) {
    // Initialize screen stack
    for (int i = 0; i < MAX_SCREEN_STACK; i++) {
        screenStack[i] = nullptr;
        ownedStack[i] = false;
    }
    
    if (!display) {
        Serial.println("ERROR: ScreenManager created with null display!");
    }
    
    // Set as global instance
    GlobalScreenManager::setInstance(this);
    
    Serial.println("ScreenManager initialized");
}

ScreenManager::~ScreenManager() {
    clearStack();
    GlobalScreenManager::setInstance(nullptr);
    Serial.println("ScreenManager destroyed");
}

void ScreenManager::update() {
    if (!shouldUpdate()) return;
    
    lastUpdateTime = millis();
    
    // Update transition if in progress
    if (inTransition) {
        updateTransition();
    }
    
    // Update current screen
    if (currentScreen && !inTransition) {
        currentScreen->update();
    }
}

void ScreenManager::draw() {
    if (!shouldDraw()) return;
    
    lastDrawTime = millis();
    
    if (inTransition) {
        drawTransition();
    } else if (currentScreen) {
        drawScreen(currentScreen);
    }
    
    needsRedraw = false;
}

bool ScreenManager::pushScreen(Screen* screen, bool takeOwnership) {
    if (!isValidScreen(screen)) {
        Serial.println("ERROR: Cannot push invalid screen");
        return false;
    }
    
    if (stackSize >= MAX_SCREEN_STACK) {
        Serial.printf("ERROR: Screen stack overflow! Max depth: %d\n", MAX_SCREEN_STACK);
        return false;
    }
    
    // Exit current screen if any
    if (currentScreen) {
        currentScreen->exit();
    }
    
    // Push current screen to stack (if any)
    if (currentScreen && !pushToStack(currentScreen, currentOwned)) {
        Serial.println("ERROR: Failed to push current screen to stack");
        return false;
    }
    
    // Set new current screen
    setCurrentScreen(screen, takeOwnership);
    startTransition();
    
    Serial.printf("Pushed screen '%s' (stack size: %d)\n", screen->getName(), stackSize);
    return true;
}

bool ScreenManager::popScreen() {
    if (stackSize == 0) {
        Serial.println("WARNING: Cannot pop screen - stack is empty");
        return false;
    }
    
    // Exit current screen
    if (currentScreen) {
        currentScreen->exit();
        if (currentOwned) {
            delete currentScreen;
        }
    }
    
    // Pop previous screen from stack
    bool ownedPrev = false;
    Screen* previousScreen = popFromStack(ownedPrev);
    if (!previousScreen) {
        Serial.println("ERROR: Failed to pop screen from stack");
        return false;
    }
    
    // Set previous screen as current
    setCurrentScreen(previousScreen, ownedPrev);
    startTransition();
    
    Serial.printf("Popped to screen '%s' (stack size: %d)\n", 
                 previousScreen->getName(), stackSize);
    return true;
}

bool ScreenManager::switchToScreen(Screen* screen) {
    if (!isValidScreen(screen)) {
        Serial.println("ERROR: Cannot switch to invalid screen");
        return false;
    }
    
    // Exit current screen
    if (currentScreen) {
        currentScreen->exit();
        if (currentOwned) {
            delete currentScreen;
            currentOwned = false;
        }
    }
    
    // Don't push to stack - just replace
    setCurrentScreen(screen, false /*owned*/);
    startTransition();
    
    Serial.printf("Switched to screen '%s'\n", screen->getName());
    return true;
}

void ScreenManager::clearStack() {
    Serial.printf("Clearing screen stack (%d screens)\n", stackSize);
    
    // Exit and delete current screen if owned
    if (currentScreen) {
        currentScreen->exit();
        if (currentOwned) {
            delete currentScreen;
            currentOwned = false;
        }
        currentScreen = nullptr;
    }
    
    // Clear stack, deleting owned screens
    for (int i = 0; i < stackSize; i++) {
        if (screenStack[i]) {
            if (ownedStack[i]) {
                screenStack[i]->exit();
                delete screenStack[i];
            }
            screenStack[i] = nullptr;
            ownedStack[i] = false;
        }
    }
    stackSize = 0;
}

Screen* ScreenManager::getPreviousScreen() const {
    if (stackSize > 0) {
        return screenStack[stackSize - 1];
    }
    return nullptr;
}

void ScreenManager::handleButtonPress(int button) {
    unsigned long now = millis();
    if (inTransition || now < inputCooldownUntilMs) {
        // Ignore input during transitions
        return;
    }
    
    if (currentScreen) {
        currentScreen->handleButtonPress(button);
    }
}

void ScreenManager::handleButtonLongPress(int button) {
    unsigned long now = millis();
    if (inTransition || now < inputCooldownUntilMs) {
        return;
    }
    
    // Global long press handling - could implement global actions here
    // For now, just pass to current screen
    if (currentScreen) {
        currentScreen->handleButtonPress(button);  // Screens can differentiate
    }
}

void ScreenManager::setTransitionDuration(unsigned long duration) {
    // Could make TRANSITION_DURATION non-const for this to work
    // For now, just log the request
    Serial.printf("Transition duration change requested: %lu ms\n", duration);
}

void ScreenManager::printStackState() const {
    Serial.printf("ScreenManager state:\n");
    Serial.printf("  Current: %s\n", currentScreen ? currentScreen->getName() : "NULL");
    Serial.printf("  Stack size: %d/%d\n", stackSize, MAX_SCREEN_STACK);
    Serial.printf("  In transition: %s\n", inTransition ? "true" : "false");
    
    for (int i = stackSize - 1; i >= 0; i--) {
        Serial.printf("  [%d] %s\n", i, screenStack[i] ? screenStack[i]->getName() : "NULL");
    }
}

void ScreenManager::printPerformanceStats() const {
    unsigned long now = millis();
    Serial.printf("ScreenManager performance:\n");
    Serial.printf("  Last update: %lu ms ago\n", now - lastUpdateTime);
    Serial.printf("  Last draw: %lu ms ago\n", now - lastDrawTime);
    Serial.printf("  Update interval: %lu ms\n", UPDATE_INTERVAL);
    Serial.printf("  In transition: %s\n", inTransition ? "true" : "false");
}

bool ScreenManager::validate() const {
    if (!display) {
        Serial.println("ERROR: ScreenManager has null display");
        return false;
    }
    
    // Validate stack integrity
    validateStack();
    
    // Validate current screen
    if (currentScreen && !currentScreen->validate()) {
        Serial.printf("ERROR: Current screen '%s' failed validation\n", 
                     currentScreen->getName());
        return false;
    }
    
    return true;
}

// Private implementation methods

bool ScreenManager::pushToStack(Screen* screen, bool owned) {
    if (stackSize >= MAX_SCREEN_STACK) {
        return false;
    }
    
    screenStack[stackSize] = screen;
    ownedStack[stackSize] = owned;
    stackSize++;
    return true;
}

Screen* ScreenManager::popFromStack(bool& ownedOut) {
    if (stackSize == 0) {
        return nullptr;
    }
    
    stackSize--;
    Screen* screen = screenStack[stackSize];
    ownedOut = ownedStack[stackSize];
    screenStack[stackSize] = nullptr;
    return screen;
}

void ScreenManager::setCurrentScreen(Screen* screen, bool owned) {
    currentScreen = screen;
    currentOwned = owned;
    if (currentScreen) {
        // Clear display before entering the new screen to avoid remnants
        if (display) {
            display->fillScreen(ThemeManager::getBackground());
        }
        currentScreen->enter();
        needsRedraw = true;
    }
}

void ScreenManager::startTransition() {
    inTransition = true;
    transitionStartTime = millis();
    needsRedraw = true;
    // After transition completes, we will set a small input cooldown
    
    Serial.println("Started screen transition");
}

void ScreenManager::updateTransition() {
    if (isTransitionComplete()) {
        inTransition = false;
        needsRedraw = true;
        inputCooldownUntilMs = millis() + INPUT_COOLDOWN_MS;
        Serial.println("Completed screen transition");
    }
}

bool ScreenManager::isTransitionComplete() const {
    return (millis() - transitionStartTime) >= TRANSITION_DURATION;
}

void ScreenManager::drawTransition() {
    // Simple fade transition - just draw current screen
    // Could implement more sophisticated transitions here
    if (currentScreen) {
        drawScreen(currentScreen);
    }
}

void ScreenManager::drawScreen(Screen* screen) {
    if (screen && display) {
        screen->draw();
    }
}

bool ScreenManager::shouldUpdate() const {
    unsigned long now = millis();
    return (now - lastUpdateTime) >= UPDATE_INTERVAL;
}

bool ScreenManager::shouldDraw() const {
    return needsRedraw || inTransition || 
           (currentScreen && currentScreen->isActive());
}

bool ScreenManager::isValidScreen(Screen* screen) const {
    if (!screen) {
        Serial.println("ERROR: Screen is null");
        return false;
    }
    
    if (!screen->validate()) {
        Serial.printf("ERROR: Screen '%s' failed validation\n", screen->getName());
        return false;
    }
    
    return true;
}

void ScreenManager::validateStack() const {
    for (int i = 0; i < stackSize; i++) {
        if (!screenStack[i]) {
            Serial.printf("ERROR: Null screen at stack position %d\n", i);
        }
    }
    
    if (stackSize < 0 || stackSize > MAX_SCREEN_STACK) {
        Serial.printf("ERROR: Invalid stack size %d\n", stackSize);
    }
}
