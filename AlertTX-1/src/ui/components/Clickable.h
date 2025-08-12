#ifndef CLICKABLE_H
#define CLICKABLE_H

#include <Arduino.h>
#include <functional>

/**
 * Clickable Interface
 * 
 * Abstract interface for components that can respond to button interactions.
 * This provides a standardized way to handle "clicking" via physical buttons
 * on the ESP32-S3 device (not touch screen).
 * 
 * Button Mapping:
 * - Button A (GPIO0): Select/Enter or Back (context dependent)
 * - Button B (GPIO1): Up/Previous navigation
 * - Button C (GPIO2): Down/Next navigation
 */

class Clickable {
public:
    virtual ~Clickable() = default;
    
    // Core click interface - must be implemented
    virtual void onClick() = 0;
    
    // Optional interaction feedback
    virtual void onPress() {}       // Called when button first pressed
    virtual void onRelease() {}     // Called when button released
    virtual void onLongPress() {}   // Called on long press (>1000ms)
    
    // State queries
    virtual bool isClickable() const { return true; }
    virtual bool isPressed() const { return false; }
};

/**
 * ClickableComponent
 * 
 * Mixin class that adds clickable behavior to any Component.
 * Uses modern C++ std::function for flexible callback handling.
 */

template<typename BaseComponent>
class ClickableComponent : public BaseComponent, public Clickable {
private:
    std::function<void()> onClickCallback;
    std::function<void()> onLongPressCallback;
    bool clickEnabled = true;
    bool currentlyPressed = false;
    
public:
    // Constructor forwards to base component
    template<typename... Args>
    ClickableComponent(Args&&... args) : BaseComponent(std::forward<Args>(args)...) {}
    
    // Clickable interface implementation
    void onClick() override {
        if (clickEnabled && onClickCallback) {
            onClickCallback();
        }
    }
    
    void onPress() override {
        currentlyPressed = true;
        // Optional visual feedback - mark for redraw
        this->markDirty();
    }
    
    void onRelease() override {
        currentlyPressed = false;
        // Clear visual feedback
        this->markDirty();
    }
    
    void onLongPress() override {
        if (clickEnabled && onLongPressCallback) {
            onLongPressCallback();
        }
    }
    
    // State queries
    bool isClickable() const override { return clickEnabled; }
    bool isPressed() const override { return currentlyPressed; }
    
    // Callback management
    void setOnClick(std::function<void()> callback) {
        onClickCallback = callback;
    }
    
    void setOnLongPress(std::function<void()> callback) {
        onLongPressCallback = callback;
    }
    
    void setClickEnabled(bool enabled) {
        clickEnabled = enabled;
        this->markDirty();  // Update visual state
    }
    
    // Helper for simple function pointer callbacks (C-style)
    void setOnClick(void (*callback)()) {
        if (callback) {
            onClickCallback = std::function<void()>(callback);
        } else {
            onClickCallback = nullptr;
        }
    }
};

/**
 * Input Event Structure
 * 
 * Represents button input events for processing by clickable components.
 */
struct InputEvent {
    enum Type {
        BUTTON_PRESS,
        BUTTON_RELEASE,
        BUTTON_LONG_PRESS
    };
    
    Type type;
    int button;              // Button index (0=A, 1=B, 2=C)
    unsigned long timestamp; // millis() when event occurred
    
    InputEvent(Type t, int b) : type(t), button(b), timestamp(millis()) {}
};

/**
 * Input Handler
 * 
 * Utility class for processing input events and dispatching them
 * to clickable components. Handles button-to-component mapping.
 */
class InputHandler {
private:
    Clickable* targetComponent = nullptr;
    
public:
    void setTarget(Clickable* component) {
        targetComponent = component;
    }
    
    void handleInput(const InputEvent& event) {
        if (!targetComponent) return;
        
        switch (event.type) {
            case InputEvent::BUTTON_PRESS:
                targetComponent->onPress();
                break;
                
            case InputEvent::BUTTON_RELEASE:
                targetComponent->onRelease();
                // Trigger click on release (standard UI pattern)
                targetComponent->onClick();
                break;
                
            case InputEvent::BUTTON_LONG_PRESS:
                targetComponent->onLongPress();
                break;
        }
    }
    
    void clearTarget() {
        targetComponent = nullptr;
    }
};

#endif // CLICKABLE_H
