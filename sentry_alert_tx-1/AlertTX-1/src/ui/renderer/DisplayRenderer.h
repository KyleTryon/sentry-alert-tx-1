#ifndef DISPLAY_RENDERER_H
#define DISPLAY_RENDERER_H

#include "../core/Theme.h"
#include <Arduino_GFX_Library.h>
#include <Arduino_Canvas.h>

// Display dimensions (from settings.h)
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 135

struct Rectangle {
    int x, y, w, h;
};

class DisplayRenderer {
private:
    Arduino_Canvas* canvas;
    Arduino_GFX* display;
    bool needsFullRedraw = true;
    Rectangle dirtyAreas[8];
    int dirtyAreaCount = 0;
    Theme currentTheme;
    
public:
    void begin(Arduino_GFX* display);
    void setTheme(const Theme& theme);
    
    // Canvas access
    Arduino_Canvas* getCanvas() { return canvas; }
    
    // Dirty area management
    void markDirty(int x, int y, int w, int h);
    void clearDirtyAreas();
    void markFullRedraw() { needsFullRedraw = true; }
    
    // Rendering
    void clear(const Theme& theme);
    void render();
    void flush();
    
    // Helper methods
    void fillScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    
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