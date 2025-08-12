#ifndef SCREEN_H
#define SCREEN_H

#include "Theme.h"
#include "EventSystem.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Forward declarations
class DisplayRenderer;

class Screen {
protected:
    Adafruit_ST7789* display;
    DisplayRenderer* renderer;
    Theme theme;
    bool needsRedrawFlag = true;
    
public:
    virtual ~Screen() = default;
    
    // Core screen lifecycle
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void handleInput(ButtonAction action) = 0;
    virtual void handleEvent(const UIEvent& event) = 0;
    
    // Configuration
    void setDisplay(Adafruit_ST7789* d) { display = d; }
    void setRenderer(DisplayRenderer* r) { renderer = r; }
    void setTheme(const Theme& t) { theme = t; }
    
    // State management
    bool needsRedraw() const { return needsRedrawFlag; }
    void markForRedraw() { needsRedrawFlag = true; }
    void clearRedrawFlag() { needsRedrawFlag = false; }
    
    // Screen identification
    virtual const char* getName() const = 0;
};

#endif // SCREEN_H
