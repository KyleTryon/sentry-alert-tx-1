#ifndef BUTTON_H
#define BUTTON_H

#include "../core/StatefulComponent.h"
#include "../renderer/Renderer.h"
#include <String.h>
#include <functional>

/**
 * Button component for the Alert TX-1 UI with click handling and visual feedback.
 */
class Button : public StatefulComponent {
private:
    String label;
    std::function<void()> onClickCallback;
    std::function<void()> onPressCallback;
    std::function<void()> onReleaseCallback;
    
    uint16_t normalColor = COLOR_DARK_GRAY;
    uint16_t pressedColor = COLOR_GRAY;
    uint16_t textColor = COLOR_WHITE;
    uint16_t borderColor = COLOR_GRAY;
    uint16_t disabledColor = COLOR_BLACK;
    
    bool pressed = false;
    bool highlighted = false;
    unsigned long pressStartTime = 0;
    static constexpr unsigned long DEBOUNCE_TIME = 50; // ms
    
public:
    Button(int x, int y, int w, int h, const String& text = "", 
           std::function<void()> onClick = nullptr)
        : StatefulComponent(x, y, w, h), label(text), onClickCallback(onClick) {}

    /**
     * Set the button label text.
     */
    void setLabel(const String& text) {
        if (label != text) {
            setState([this, &text]() {
                label = text;
            });
        }
    }
    
    /**
     * Get the button label.
     */
    const String& getLabel() const {
        return label;
    }
    
    /**
     * Set the click callback function.
     */
    void setOnClick(std::function<void()> callback) {
        onClickCallback = callback;
    }
    
    /**
     * Set the press callback (called when button is pressed down).
     */
    void setOnPress(std::function<void()> callback) {
        onPressCallback = callback;
    }
    
    /**
     * Set the release callback (called when button is released).
     */
    void setOnRelease(std::function<void()> callback) {
        onReleaseCallback = callback;
    }
    
    /**
     * Set button colors.
     */
    void setColors(uint16_t normal, uint16_t pressed, uint16_t text = COLOR_WHITE, uint16_t border = COLOR_GRAY) {
        setState([this, normal, pressed, text, border]() {
            normalColor = normal;
            pressedColor = pressed;
            textColor = text;
            borderColor = border;
        });
    }
    
    /**
     * Manually trigger button press (for programmatic activation).
     */
    void press() {
        if (!isEnabled()) return;
        
        setState([this]() {
            pressed = true;
            pressStartTime = millis();
        });
        
        if (onPressCallback) {
            onPressCallback();
        }
    }
    
    /**
     * Manually trigger button release.
     */
    void release() {
        if (!pressed) return;
        
        bool wasPressed = pressed;
        setState([this]() {
            pressed = false;
        });
        
        if (wasPressed && onReleaseCallback) {
            onReleaseCallback();
        }
        
        // Check for click (press + release)
        if (wasPressed && millis() - pressStartTime >= DEBOUNCE_TIME) {
            click();
        }
    }
    
    /**
     * Trigger a button click.
     */
    void click() {
        if (!isEnabled()) return;
        
        if (onClickCallback) {
            onClickCallback();
        }
    }
    
    /**
     * Set highlighted state (for keyboard/button navigation).
     */
    void setHighlighted(bool highlight) {
        if (highlighted != highlight) {
            setState([this, highlight]() {
                highlighted = highlight;
            });
        }
    }
    
    /**
     * Check if button is currently pressed.
     */
    bool isPressed() const {
        return pressed;
    }
    
    /**
     * Check if button is highlighted.
     */
    bool isHighlighted() const {
        return highlighted;
    }
    
    /**
     * Handle touch events.
     */
    bool handleTouch(int touchX, int touchY) override {
        if (!isEnabled() || !isVisible()) return false;
        
        if (contains(touchX, touchY)) {
            if (!pressed) {
                press();
            }
            return true;
        } else if (pressed) {
            // Touch moved outside button while pressed - cancel
            setState([this]() {
                pressed = false;
            });
        }
        
        return false;
    }
    
    /**
     * Handle button events (for physical buttons).
     */
    void handleEvent(int eventType, int eventData = 0) override {
        if (!isEnabled()) return;
        
        switch (eventType) {
            case 1: // Button press
                if (!pressed) {
                    press();
                }
                break;
                
            case 2: // Button release
                if (pressed) {
                    release();
                }
                break;
                
            case 3: // Button click (single event)
                click();
                break;
                
            case 4: // Highlight/select
                setHighlighted(true);
                break;
                
            case 5: // Unhighlight/deselect
                setHighlighted(false);
                break;
        }
    }
    
    /**
     * Update button state.
     */
    void update() override {
        // Auto-release if pressed too long (safety feature)
        if (pressed && millis() - pressStartTime > 5000) { // 5 second timeout
            release();
        }
    }
    
    /**
     * Render the button.
     */
    void render(Adafruit_GFX& gfx) override {
        if (!isVisible()) return;
        
        uint16_t bgColor = normalColor;
        uint16_t textCol = textColor;
        uint16_t borderCol = borderColor;
        
        if (!isEnabled()) {
            bgColor = disabledColor;
            textCol = COLOR_DARK_GRAY;
            borderCol = COLOR_DARK_GRAY;
        } else if (pressed) {
            bgColor = pressedColor;
        } else if (highlighted) {
            borderCol = COLOR_WHITE;
        }
        
        // Use renderer to draw the button
        Renderer::drawButton(gfx, x, y, w, h, label.c_str(), pressed, bgColor, textCol, borderCol);
        
        // Additional highlight indicator
        if (highlighted && isEnabled()) {
            gfx.drawRect(x - 1, y - 1, w + 2, h + 2, COLOR_WHITE);
        }
    }
};

#endif // BUTTON_H