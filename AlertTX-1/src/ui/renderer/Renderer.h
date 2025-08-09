#ifndef RENDERER_H
#define RENDERER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Color definitions for 16-bit RGB565 format
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_CYAN        0x07FF
#define COLOR_MAGENTA     0xF81F
#define COLOR_ORANGE      0xFD20
#define COLOR_PURPLE      0x8010
#define COLOR_GRAY        0x8410
#define COLOR_DARK_GRAY   0x4208
#define COLOR_LIGHT_GRAY  0xC618

/**
 * Enhanced renderer for the Alert TX-1 ESP32-S3 TFT display.
 * Provides high-level drawing functions and UI helpers.
 */
class Renderer {
private:
    static constexpr int DISPLAY_WIDTH = 240;
    static constexpr int DISPLAY_HEIGHT = 135;

public:
    /**
     * Draw text centered within a rectangle.
     */
    static void drawTextCentered(Adafruit_GFX& gfx, int x, int y, int w, int h, 
                                const char* text, uint16_t color = COLOR_WHITE) {
        int16_t x1, y1;
        uint16_t textW, textH;
        gfx.getTextBounds(text, 0, 0, &x1, &y1, &textW, &textH);
        
        int centerX = x + (w - textW) / 2;
        int centerY = y + (h - textH) / 2;
        
        gfx.setTextColor(color);
        gfx.setCursor(centerX, centerY);
        gfx.print(text);
    }
    
    /**
     * Draw a button with optional rounded corners.
     */
    static void drawButton(Adafruit_GFX& gfx, int x, int y, int w, int h,
                          const char* text, bool pressed = false, 
                          uint16_t bgColor = COLOR_DARK_GRAY,
                          uint16_t textColor = COLOR_WHITE,
                          uint16_t borderColor = COLOR_GRAY) {
        
        // Draw button background
        uint16_t fillColor = pressed ? borderColor : bgColor;
        gfx.fillRect(x + 1, y + 1, w - 2, h - 2, fillColor);
        
        // Draw border
        gfx.drawRect(x, y, w, h, borderColor);
        
        // Draw text
        drawTextCentered(gfx, x, y, w, h, text, textColor);
    }
    
    /**
     * Draw a rounded rectangle (simple version using multiple rects).
     */
    static void drawRoundedRect(Adafruit_GFX& gfx, int x, int y, int w, int h, 
                               int radius, uint16_t color) {
        // For simplicity, we'll draw a regular rectangle
        // A full rounded rect implementation would require more complex drawing
        gfx.drawRect(x, y, w, h, color);
    }
    
    /**
     * Draw a filled rounded rectangle.
     */
    static void fillRoundedRect(Adafruit_GFX& gfx, int x, int y, int w, int h, 
                               int radius, uint16_t color) {
        gfx.fillRect(x, y, w, h, color);
    }
    
    /**
     * Draw a progress bar.
     */
    static void drawProgressBar(Adafruit_GFX& gfx, int x, int y, int w, int h,
                               float progress, uint16_t fillColor = COLOR_GREEN,
                               uint16_t bgColor = COLOR_DARK_GRAY,
                               uint16_t borderColor = COLOR_GRAY) {
        // Clamp progress to 0.0-1.0
        progress = constrain(progress, 0.0f, 1.0f);
        
        // Draw background
        gfx.fillRect(x + 1, y + 1, w - 2, h - 2, bgColor);
        
        // Draw progress fill
        int fillWidth = (int)((w - 2) * progress);
        if (fillWidth > 0) {
            gfx.fillRect(x + 1, y + 1, fillWidth, h - 2, fillColor);
        }
        
        // Draw border
        gfx.drawRect(x, y, w, h, borderColor);
    }
    
    /**
     * Draw a simple icon using basic shapes.
     */
    static void drawIcon(Adafruit_GFX& gfx, int x, int y, int size, 
                        const char* iconType, uint16_t color = COLOR_WHITE) {
        if (strcmp(iconType, "play") == 0) {
            // Draw play triangle
            gfx.fillTriangle(x, y, x, y + size, x + size, y + size/2, color);
        } else if (strcmp(iconType, "pause") == 0) {
            // Draw pause bars
            int barWidth = size / 4;
            gfx.fillRect(x, y, barWidth, size, color);
            gfx.fillRect(x + size - barWidth, y, barWidth, size, color);
        } else if (strcmp(iconType, "stop") == 0) {
            // Draw stop square
            gfx.fillRect(x, y, size, size, color);
        } else if (strcmp(iconType, "up") == 0) {
            // Draw up arrow
            gfx.fillTriangle(x + size/2, y, x, y + size, x + size, y + size, color);
        } else if (strcmp(iconType, "down") == 0) {
            // Draw down arrow
            gfx.fillTriangle(x, y, x + size, y, x + size/2, y + size, color);
        }
    }
    
    /**
     * Draw a status indicator (dot).
     */
    static void drawStatusDot(Adafruit_GFX& gfx, int x, int y, int radius,
                             uint16_t color = COLOR_GREEN) {
        gfx.fillCircle(x, y, radius, color);
    }
    
    /**
     * Draw a menu item with selection indicator.
     */
    static void drawMenuItem(Adafruit_GFX& gfx, int x, int y, int w, int h,
                            const char* text, bool selected = false,
                            uint16_t textColor = COLOR_WHITE,
                            uint16_t selectedColor = COLOR_BLUE) {
        
        if (selected) {
            // Draw selection background
            gfx.fillRect(x, y, w, h, selectedColor);
            textColor = COLOR_WHITE;
        }
        
        // Draw text with padding
        gfx.setTextColor(textColor);
        gfx.setCursor(x + 4, y + 4);
        gfx.print(text);
        
        if (selected) {
            // Draw selection indicator arrow
            drawIcon(gfx, x + w - 12, y + h/2 - 3, 6, "right", COLOR_WHITE);
        }
    }
    
    /**
     * Clear a specific region of the display.
     */
    static void clearRegion(Adafruit_GFX& gfx, int x, int y, int w, int h,
                           uint16_t color = COLOR_BLACK) {
        gfx.fillRect(x, y, w, h, color);
    }
    
    /**
     * Get display dimensions.
     */
    static int getDisplayWidth() { return DISPLAY_WIDTH; }
    static int getDisplayHeight() { return DISPLAY_HEIGHT; }
    
    /**
     * Convert RGB values to 16-bit color.
     */
    static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }
    
    /**
     * Create a color with alpha blending (simple version).
     */
    static uint16_t blendColor(uint16_t fg, uint16_t bg, float alpha) {
        alpha = constrain(alpha, 0.0f, 1.0f);
        
        // Extract RGB components
        uint8_t fgR = (fg >> 11) & 0x1F;
        uint8_t fgG = (fg >> 5) & 0x3F;
        uint8_t fgB = fg & 0x1F;
        
        uint8_t bgR = (bg >> 11) & 0x1F;
        uint8_t bgG = (bg >> 5) & 0x3F;
        uint8_t bgB = bg & 0x1F;
        
        // Blend
        uint8_t r = (uint8_t)(fgR * alpha + bgR * (1.0f - alpha));
        uint8_t g = (uint8_t)(fgG * alpha + bgG * (1.0f - alpha));
        uint8_t b = (uint8_t)(fgB * alpha + bgB * (1.0f - alpha));
        
        return (r << 11) | (g << 5) | b;
    }
};

#endif // RENDERER_H