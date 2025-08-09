#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H

#include <Adafruit_GFX.h>
#include <functional>

/**
 * Base class for all UI components in the Alert TX-1 React-like framework.
 * Provides the fundamental interface for positioning, rendering, and event handling.
 */
class UIComponent {
protected:
    int x, y, w, h;
    bool visible = true;
    bool enabled = true;

public:
    UIComponent(int x = 0, int y = 0, int w = 0, int h = 0) 
        : x(x), y(y), w(w), h(h) {}
    
    virtual ~UIComponent() = default;

    // Core rendering interface
    virtual void render(Adafruit_GFX& gfx) = 0;
    virtual void update() {}
    
    // Event handling
    virtual void handleEvent(int eventType, int eventData = 0) {}
    virtual bool handleTouch(int touchX, int touchY) { return false; }
    
    // Dirty tracking for optimization
    virtual bool isDirty() { return true; }
    virtual void clearDirty() {}
    
    // Position and size
    virtual void setPosition(int newX, int newY) { x = newX; y = newY; }
    virtual void setSize(int newW, int newH) { w = newW; h = newH; }
    virtual void setBounds(int newX, int newY, int newW, int newH) {
        x = newX; y = newY; w = newW; h = newH;
    }
    
    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return w; }
    int getHeight() const { return h; }
    
    // Visibility and interaction
    virtual void setVisible(bool vis) { visible = vis; }
    virtual bool isVisible() const { return visible; }
    virtual void setEnabled(bool en) { enabled = en; }
    virtual bool isEnabled() const { return enabled; }
    
    // Hit testing for touch/button navigation
    virtual bool contains(int pointX, int pointY) const {
        return pointX >= x && pointX < x + w && 
               pointY >= y && pointY < y + h;
    }
};

#endif // UI_COMPONENT_H