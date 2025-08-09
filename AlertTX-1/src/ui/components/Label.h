#ifndef LABEL_H
#define LABEL_H

#include "../core/StatefulComponent.h"
#include "../renderer/Renderer.h"
#include <String.h>

/**
 * Label component for displaying text in the Alert TX-1 UI.
 */
class Label : public StatefulComponent {
private:
    String text;
    uint16_t textColor = COLOR_WHITE;
    uint16_t backgroundColor = COLOR_BLACK;
    bool drawBackground = false;
    int textSize = 1;
    bool centerAlign = false;
    bool wordWrap = false;

public:
    Label(int x, int y, const String& text = "", uint16_t color = COLOR_WHITE)
        : StatefulComponent(x, y, 0, 0), text(text), textColor(color) {
        calculateSize();
    }
    
    Label(int x, int y, int w, int h, const String& text = "", uint16_t color = COLOR_WHITE)
        : StatefulComponent(x, y, w, h), text(text), textColor(color) {}

    /**
     * Set the label text.
     */
    void setText(const String& newText) {
        if (text != newText) {
            setState([this, &newText]() {
                text = newText;
                if (w == 0 || h == 0) {
                    calculateSize();
                }
            });
        }
    }
    
    /**
     * Get the current text.
     */
    const String& getText() const {
        return text;
    }
    
    /**
     * Set the text color.
     */
    void setTextColor(uint16_t color) {
        if (textColor != color) {
            setState([this, color]() {
                textColor = color;
            });
        }
    }
    
    /**
     * Set the background color and enable/disable background drawing.
     */
    void setBackgroundColor(uint16_t color, bool draw = true) {
        setState([this, color, draw]() {
            backgroundColor = color;
            drawBackground = draw;
        });
    }
    
    /**
     * Set the text size (1-4).
     */
    void setTextSize(int size) {
        if (textSize != size && size >= 1 && size <= 4) {
            setState([this, size]() {
                textSize = size;
                calculateSize();
            });
        }
    }
    
    /**
     * Enable/disable center alignment.
     */
    void setCenterAlign(bool center) {
        if (centerAlign != center) {
            setState([this, center]() {
                centerAlign = center;
            });
        }
    }
    
    /**
     * Enable/disable word wrapping.
     */
    void setWordWrap(bool wrap) {
        if (wordWrap != wrap) {
            setState([this, wrap]() {
                wordWrap = wrap;
            });
        }
    }
    
    /**
     * Render the label.
     */
    void render(Adafruit_GFX& gfx) override {
        if (!isVisible() || text.isEmpty()) return;
        
        // Set text properties
        gfx.setTextSize(textSize);
        gfx.setTextColor(textColor);
        gfx.setTextWrap(wordWrap);
        
        // Draw background if enabled
        if (drawBackground && w > 0 && h > 0) {
            gfx.fillRect(x, y, w, h, backgroundColor);
        }
        
        // Calculate text position
        int textX = x;
        int textY = y;
        
        if (centerAlign && w > 0 && h > 0) {
            // Use renderer's centered text function
            Renderer::drawTextCentered(gfx, x, y, w, h, text.c_str(), textColor);
        } else {
            // Simple left-aligned text
            gfx.setCursor(textX, textY);
            gfx.print(text);
        }
    }

private:
    /**
     * Calculate the size of the text for auto-sizing.
     */
    void calculateSize() {
        if (text.isEmpty()) {
            w = 0;
            h = 0;
            return;
        }
        
        // Approximate text size calculation
        // This is a rough estimate; for exact sizing, you'd need the actual GFX context
        int charWidth = 6 * textSize;  // Default font is 6 pixels wide
        int charHeight = 8 * textSize; // Default font is 8 pixels tall
        
        w = text.length() * charWidth;
        h = charHeight;
        
        // If we have word wrap and a fixed width, estimate height
        if (wordWrap && w > 0) {
            int lines = (text.length() * charWidth) / w + 1;
            h = lines * charHeight;
        }
    }
};

#endif // LABEL_H