#ifndef COMPONENT_H
#define COMPONENT_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include "Theme.h"

/**
 * Base Component Class
 * 
 * Foundation for all UI components in the Alert TX-1 framework.
 * Provides common functionality for layout management, theme integration,
 * dirty tracking for efficient rendering, and basic component lifecycle.
 * 
 * Memory Efficient Design:
 * - Uses references to avoid copying large objects
 * - Minimal memory footprint per component
 * - PROGMEM theme storage
 */

class Component {
protected:
    Adafruit_ST7789* display;
    
    // Layout properties
    int x, y, width, height;
    bool visible = true;
    bool needsRedraw = true;
    
    // Component identification for debugging
    const char* componentName;
    
public:
    Component(Adafruit_ST7789* display, const char* name = "Component");
    virtual ~Component() = default;
    
    // Core component interface - must be implemented by subclasses
    virtual void draw() = 0;
    virtual void update() {}  // Optional update logic for animations, etc.
    
    // Layout management
    void setBounds(int x, int y, int w, int h);
    void setPosition(int x, int y);
    void setSize(int w, int h);
    void setVisible(bool visible);
    
    // Getters for layout properties
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isVisible() const { return visible; }
    
    // Bounds checking utilities
    bool containsPoint(int px, int py) const;
    bool intersects(int rx, int ry, int rw, int rh) const;
    
    // Dirty tracking for efficient rendering
    void markDirty() { needsRedraw = true; }
    bool isDirty() const { return needsRedraw; }
    void clearDirty() { needsRedraw = false; }
    
    // Theme access (uses global ThemeManager)
    uint16_t getThemeColor(const char* colorType) const;
    
    // Debug utilities
    const char* getName() const { return componentName; }
    void printBounds() const;
    
    // Component validation
    virtual bool validate() const;
    
protected:
    // Helper methods for subclasses
    void drawRect(int x, int y, int w, int h, uint16_t color);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void drawText(const char* text, int x, int y, uint16_t color, int size = 1);
    
    // Layout validation helpers
    bool isOnScreen() const;
    bool hasValidBounds() const;
};

#endif // COMPONENT_H
