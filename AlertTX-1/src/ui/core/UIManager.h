#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "Screen.h"
#include "EventSystem.h"
#include "Theme.h"
#include "../renderer/DisplayRenderer.h"
#include "../../hardware/ButtonManager.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

class UIManager {
private:
    Adafruit_ST7789* display;
    ButtonManager* buttonManager;
    EventSystem eventSystem;
    DisplayRenderer renderer;
    Screen* currentScreen;
    Screen* screens[4];  // Array to hold different screens
    int screenCount = 0;
    Theme currentTheme;
    
    // Performance tracking
    unsigned long lastFrameTime;
    unsigned long frameCount;
    float fps;
    
    // State management
    bool needsRedraw = true;
    unsigned long lastUpdateTime = 0;
    static const unsigned long FRAME_RATE_LIMIT = 33; // ~30 FPS (33ms per frame)
    
public:
    UIManager();
    ~UIManager();
    
    // Initialization
    void begin(Adafruit_ST7789* display, ButtonManager* btnManager);
    
    // Main update loop
    void update();  // Call in main loop
    
    // Theme management
    void setTheme(const Theme& theme);
    const Theme& getTheme() const { return currentTheme; }
    
    // Screen management
    void addScreen(Screen* screen);
    void switchScreen(Screen* screen);
    void switchScreen(int screenIndex);
    Screen* getCurrentScreen() { return currentScreen; }
    
    // Performance monitoring
    float getFPS() const { return fps; }
    void printStats();
    
    // Event system access
    EventSystem* getEventSystem() { return &eventSystem; }
    
private:
    void processEvents();
    void updateScreen();
    void renderFrame();
    void calculateFPS();
    ButtonAction translateButtonEvent(const UIEvent& event);
};

#endif // UI_MANAGER_H
