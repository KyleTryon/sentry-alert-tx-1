#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include "../core/StatefulComponent.h"
#include "../renderer/Renderer.h"
#include <String.h>

/**
 * ProgressBar component for showing progress indicators in the Alert TX-1 UI.
 */
class ProgressBar : public StatefulComponent {
private:
    float progress = 0.0f;  // 0.0 to 1.0
    float minValue = 0.0f;
    float maxValue = 1.0f;
    uint16_t fillColor = COLOR_GREEN;
    uint16_t backgroundColor = COLOR_DARK_GRAY;
    uint16_t borderColor = COLOR_GRAY;
    bool showPercentage = false;
    bool showBorder = true;
    String customText = "";

public:
    ProgressBar(int x, int y, int w, int h, float initial = 0.0f)
        : StatefulComponent(x, y, w, h), progress(initial) {}

    /**
     * Set the progress value (0.0 to 1.0).
     */
    void setProgress(float value) {
        value = constrain(value, 0.0f, 1.0f);
        if (progress != value) {
            setState([this, value]() {
                progress = value;
            });
        }
    }
    
    /**
     * Set the progress using min/max values.
     */
    void setValue(float value, float min = 0.0f, float max = 1.0f) {
        setState([this, value, min, max]() {
            minValue = min;
            maxValue = max;
            progress = (value - min) / (max - min);
            progress = constrain(progress, 0.0f, 1.0f);
        });
    }
    
    /**
     * Get the current progress (0.0 to 1.0).
     */
    float getProgress() const {
        return progress;
    }
    
    /**
     * Get the current value based on min/max range.
     */
    float getValue() const {
        return minValue + (progress * (maxValue - minValue));
    }
    
    /**
     * Set the colors for the progress bar.
     */
    void setColors(uint16_t fill, uint16_t background = COLOR_DARK_GRAY, uint16_t border = COLOR_GRAY) {
        setState([this, fill, background, border]() {
            fillColor = fill;
            backgroundColor = background;
            borderColor = border;
        });
    }
    
    /**
     * Enable/disable percentage text display.
     */
    void setShowPercentage(bool show) {
        if (showPercentage != show) {
            setState([this, show]() {
                showPercentage = show;
            });
        }
    }
    
    /**
     * Set custom text to display on the progress bar.
     */
    void setCustomText(const String& text) {
        if (customText != text) {
            setState([this, &text]() {
                customText = text;
            });
        }
    }
    
    /**
     * Enable/disable border drawing.
     */
    void setShowBorder(bool show) {
        if (showBorder != show) {
            setState([this, show]() {
                showBorder = show;
            });
        }
    }
    
    /**
     * Animate progress to a target value over time.
     */
    void animateToProgress(float target, unsigned long duration = 1000) {
        // Simple animation implementation
        // In a more sophisticated system, this would use a tween system
        target = constrain(target, 0.0f, 1.0f);
        
        // For now, just set directly
        // TODO: Implement smooth animation
        setProgress(target);
    }
    
    /**
     * Render the progress bar.
     */
    void render(Adafruit_GFX& gfx) override {
        if (!isVisible()) return;
        
        // Use renderer to draw the progress bar
        Renderer::drawProgressBar(gfx, x, y, w, h, progress, fillColor, backgroundColor, borderColor);
        
        // Draw text overlay if enabled
        if (showPercentage || !customText.isEmpty()) {
            String displayText = customText;
            if (displayText.isEmpty() && showPercentage) {
                displayText = String((int)(progress * 100)) + "%";
            }
            
            if (!displayText.isEmpty()) {
                // Draw text with outline for better visibility
                gfx.setTextSize(1);
                
                // Black outline
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx != 0 || dy != 0) {
                            Renderer::drawTextCentered(gfx, x + dx, y + dy, w, h, 
                                                     displayText.c_str(), COLOR_BLACK);
                        }
                    }
                }
                
                // White text
                Renderer::drawTextCentered(gfx, x, y, w, h, displayText.c_str(), COLOR_WHITE);
            }
        }
    }
};

#endif // PROGRESS_BAR_H