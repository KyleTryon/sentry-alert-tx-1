#ifndef DISPLAY_RENDERER_H
#define DISPLAY_RENDERER_H

#include "../core/Theme.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Display dimensions (240x135 for ESP32-S3 Reverse TFT Feather)
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 135

struct Rectangle {
    int x, y, w, h;
};

class DisplayRenderer {
private:
    Adafruit_ST7789* display;
    bool needsFullRedraw = true;
    Rectangle dirtyAreas[8];
    int dirtyAreaCount = 0;
    Theme currentTheme;
    
public:
    void begin(Adafruit_ST7789* display);
    void setTheme(const Theme& theme);
    
    // Display access
    Adafruit_ST7789* getDisplay() { return display; }
    
    // Dirty area management
    void markDirty(int x, int y, int w, int h);
    void clearDirtyAreas();
    void markFullRedraw() { needsFullRedraw = true; }
    
    // Rendering
    void clear(const Theme& theme);
    void render();
    void flush();
    
    // Helper methods (direct pass-through to display)
    void fillScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    
    // Text rendering
    void setTextColor(uint16_t color);
    void setTextSize(uint8_t size);
    void setCursor(int16_t x, int16_t y);
    void print(const char* text);
    void print(int value);
    
    // Performance monitoring
    void printStats();
};

#endif // DISPLAY_RENDERER_H