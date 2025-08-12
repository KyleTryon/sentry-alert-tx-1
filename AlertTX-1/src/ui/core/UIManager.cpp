#include "UIManager.h"
#include <Arduino.h>

UIManager::UIManager() 
    : display(nullptr), buttonManager(nullptr), currentScreen(nullptr), 
      lastFrameTime(0), frameCount(0), fps(0.0f) {
    
    // Initialize screens array
    for (int i = 0; i < 4; i++) {
        screens[i] = nullptr;
    }
    
    // Set default theme
    currentTheme = THEME_AMBER;  // FlipperZero style
}

UIManager::~UIManager() {
    // Cleanup if needed
}

void UIManager::begin(Adafruit_ST7789* disp, ButtonManager* btnManager) {
    display = disp;
    buttonManager = btnManager;
    
    // Initialize subsystems
    renderer.begin(display);
    renderer.setTheme(currentTheme);
    
    // Performance tracking
    lastFrameTime = millis();
    frameCount = 0;
    fps = 0.0f;
    lastUpdateTime = 0;
    
    Serial.println("UIManager initialized");
}

void UIManager::update() {
    unsigned long currentTime = millis();
    
    // Frame rate limiting
    if ((currentTime - lastUpdateTime) < FRAME_RATE_LIMIT) {
        return;
    }
    lastUpdateTime = currentTime;
    
    // 1. Update button states and generate events
    if (buttonManager) {
        buttonManager->update();
    }
    
    // 2. Process all pending events
    processEvents();
    
    // 3. Update current screen
    updateScreen();
    
    // 4. Render frame if needed
    if (needsRedraw || (currentScreen && currentScreen->needsRedraw())) {
        renderFrame();
        needsRedraw = false;
        if (currentScreen) {
            currentScreen->clearRedrawFlag();
        }
    }
    
    // 5. Calculate performance metrics
    calculateFPS();
}

void UIManager::setTheme(const Theme& theme) {
    currentTheme = theme;
    renderer.setTheme(theme);
    
    // Update current screen theme
    if (currentScreen) {
        currentScreen->setTheme(theme);
        currentScreen->markForRedraw();
    }
    
    needsRedraw = true;
}

void UIManager::addScreen(Screen* screen) {
    if (screenCount < 4 && screen) {
        screens[screenCount] = screen;
        screen->setDisplay(display);
        screen->setRenderer(&renderer);
        screen->setTheme(currentTheme);
        screenCount++;
        
        // If this is the first screen, make it current
        if (!currentScreen) {
            currentScreen = screen;
            currentScreen->begin();
        }
    }
}

void UIManager::switchScreen(Screen* screen) {
    if (screen && screen != currentScreen) {
        currentScreen = screen;
        currentScreen->setDisplay(display);
        currentScreen->setRenderer(&renderer);
        currentScreen->setTheme(currentTheme);
        currentScreen->begin();
        currentScreen->markForRedraw();
        needsRedraw = true;
        
        // Generate screen change event
        eventSystem.pushEvent(UIEventType::SCREEN_CHANGE, 0, screen);
    }
}

void UIManager::switchScreen(int screenIndex) {
    if (screenIndex >= 0 && screenIndex < screenCount && screens[screenIndex]) {
        switchScreen(screens[screenIndex]);
    }
}

void UIManager::processEvents() {
    UIEvent event;
    
    while (eventSystem.popEvent(event)) {
        // Handle system-level events
        switch (event.type) {
            case UIEventType::THEME_CHANGE:
                if (event.data) {
                    setTheme(*(Theme*)event.data);
                }
                break;
                
            case UIEventType::SCREEN_CHANGE:
                // Screen change already handled in switchScreen()
                break;
                
            case UIEventType::RENDER_REQUEST:
                needsRedraw = true;
                break;
                
            case UIEventType::BUTTON_PRESS:
            case UIEventType::BUTTON_RELEASE:
            case UIEventType::BUTTON_LONG_PRESS:
                Serial.printf("UIManager: Processing button event - Type: %d, Button: %d\n", 
                            (int)event.type, event.buttonId);
                // Translate button events to actions and pass to current screen
                if (currentScreen) {
                    ButtonAction action = translateButtonEvent(event);
                    Serial.printf("  - Translated to action: %d\n", (int)action);
                    if (action != ACTION_NONE) {
                        currentScreen->handleInput(action);
                    }
                    currentScreen->handleEvent(event);
                }
                break;
                
            default:
                // Pass other events to current screen
                if (currentScreen) {
                    currentScreen->handleEvent(event);
                }
                break;
        }
    }
}

void UIManager::updateScreen() {
    if (currentScreen) {
        currentScreen->update();
    }
}

void UIManager::renderFrame() {
    Serial.println("UIManager: Rendering frame...");
    
    // Clear canvas with theme background
    Serial.println("  - Clearing canvas...");
    renderer.clear(currentTheme);
    
    // Render current screen
    if (currentScreen) {
        Serial.println("  - Drawing current screen...");
        currentScreen->draw();
    } else {
        Serial.println("  - No current screen to draw!");
    }
    
    // Flush to display
    Serial.println("  - Flushing to display...");
    renderer.flush();
    
    frameCount++;
    Serial.println("UIManager: Frame rendered successfully");
}

void UIManager::calculateFPS() {
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - lastFrameTime;
    
    if (elapsed >= 1000) { // Every second
        fps = (float)frameCount * 1000.0f / elapsed;
        frameCount = 0;
        lastFrameTime = currentTime;
    }
}

ButtonAction UIManager::translateButtonEvent(const UIEvent& event) {
    if (event.type != UIEventType::BUTTON_PRESS && 
        event.type != UIEventType::BUTTON_LONG_PRESS) {
        return ACTION_NONE;
    }
    
    // Navigation mapping (FlipperZero style)
    // Button A (0): Select/Enter
    // Button B (1): Up/Previous  
    // Button C (2): Down/Next
    switch (event.buttonId) {
        case 0: // Button A
            return (event.type == UIEventType::BUTTON_LONG_PRESS) ? ACTION_BACK : ACTION_SELECT;
        case 1: // Button B
            return ACTION_UP;
        case 2: // Button C
            return ACTION_DOWN;
        default:
            return ACTION_NONE;
    }
}

void UIManager::printStats() {
    Serial.println("=== UIManager Stats ===");
    Serial.print("FPS: ");
    Serial.println(fps);
    Serial.print("Current Screen: ");
    Serial.println(currentScreen ? currentScreen->getName() : "None");
    Serial.print("Screen Count: ");
    Serial.println(screenCount);
    Serial.print("Event Queue Size: ");
    Serial.println(eventSystem.getQueueSize());
    renderer.printStats();
    Serial.println("=====================");
}
