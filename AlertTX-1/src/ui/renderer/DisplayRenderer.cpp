#include "DisplayRenderer.h"
#include <Arduino.h>

void DisplayRenderer::begin(Adafruit_ST7789* disp) {
    display = disp;
    
    // Initialize state
    needsFullRedraw = true;
    dirtyAreaCount = 0;
    
    Serial.println("DisplayRenderer initialized with Adafruit_ST7789");
}

void DisplayRenderer::setTheme(const Theme& theme) {
    currentTheme = theme;
    needsFullRedraw = true;
}

void DisplayRenderer::markDirty(int x, int y, int w, int h) {
    if (dirtyAreaCount < 8) {
        dirtyAreas[dirtyAreaCount] = {x, y, w, h};
        dirtyAreaCount++;
    } else {
        // Too many dirty areas, force full redraw
        needsFullRedraw = true;
    }
}

void DisplayRenderer::clearDirtyAreas() {
    dirtyAreaCount = 0;
}

void DisplayRenderer::clear(const Theme& theme) {
    if (display) {
        display->fillScreen(theme.background);
    }
}

void DisplayRenderer::render() {
    if (!display) return;
    
    if (needsFullRedraw) {
        // Full screen redraw
        display->fillScreen(currentTheme.background);
        needsFullRedraw = false;
    } else {
        // Partial redraw of dirty areas
        for (int i = 0; i < dirtyAreaCount; i++) {
            Rectangle& area = dirtyAreas[i];
            display->fillRect(area.x, area.y, area.w, area.h, currentTheme.background);
        }
    }
    
    clearDirtyAreas();
}

void DisplayRenderer::flush() {
    // No explicit flush needed for Adafruit_ST7789
    // Drawing operations are immediately sent to display
}

// Pass-through methods to Adafruit_ST7789
void DisplayRenderer::fillScreen(uint16_t color) {
    if (display) {
        display->fillScreen(color);
    }
}

void DisplayRenderer::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (display) {
        display->drawPixel(x, y, color);
    }
}

void DisplayRenderer::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (display) {
        display->drawLine(x0, y0, x1, y1, color);
    }
}

void DisplayRenderer::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (display) {
        display->drawRect(x, y, w, h, color);
    }
}

void DisplayRenderer::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (display) {
        display->fillRect(x, y, w, h, color);
    }
}

void DisplayRenderer::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (display) {
        display->drawCircle(x0, y0, r, color);
    }
}

void DisplayRenderer::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (display) {
        display->fillCircle(x0, y0, r, color);
    }
}

void DisplayRenderer::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    if (display) {
        display->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    }
}

void DisplayRenderer::setTextColor(uint16_t color) {
    if (display) {
        display->setTextColor(color);
    }
}

void DisplayRenderer::setTextSize(uint8_t size) {
    if (display) {
        display->setTextSize(size);
    }
}

void DisplayRenderer::setCursor(int16_t x, int16_t y) {
    if (display) {
        display->setCursor(x, y);
    }
}

void DisplayRenderer::print(const char* text) {
    if (display) {
        display->print(text);
    }
}

void DisplayRenderer::print(int value) {
    if (display) {
        display->print(value);
    }
}

void DisplayRenderer::printStats() {
    Serial.print("Dirty areas: ");
    Serial.println(dirtyAreaCount);
    Serial.print("Full redraw: ");
    Serial.println(needsFullRedraw ? "true" : "false");
}