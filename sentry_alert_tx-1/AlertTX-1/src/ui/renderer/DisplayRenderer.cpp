#include "DisplayRenderer.h"

void DisplayRenderer::begin(Arduino_GFX* disp) {
    display = disp;
    
    // Create canvas
    canvas = new Arduino_Canvas(DISPLAY_WIDTH, DISPLAY_HEIGHT, display);
    canvas->begin();
    canvas->setRotation(1);  // Landscape orientation
    
    // Initialize state
    needsFullRedraw = true;
    dirtyAreaCount = 0;
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
    if (canvas) {
        canvas->fillScreen(theme.background);
    }
}

void DisplayRenderer::render() {
    if (!canvas) return;
    
    if (needsFullRedraw) {
        // Full screen redraw
        canvas->fillScreen(currentTheme.background);
        needsFullRedraw = false;
    } else {
        // Partial redraw of dirty areas
        for (int i = 0; i < dirtyAreaCount; i++) {
            Rectangle& area = dirtyAreas[i];
            canvas->fillRect(area.x, area.y, area.w, area.h, currentTheme.background);
        }
    }
    
    clearDirtyAreas();
}

void DisplayRenderer::flush() {
    if (canvas) {
        canvas->flush();
    }
}

// Canvas wrapper methods
void DisplayRenderer::fillScreen(uint16_t color) {
    if (canvas) {
        canvas->fillScreen(color);
    }
}

void DisplayRenderer::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (canvas) {
        canvas->drawPixel(x, y, color);
    }
}

void DisplayRenderer::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (canvas) {
        canvas->drawLine(x0, y0, x1, y1, color);
    }
}

void DisplayRenderer::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (canvas) {
        canvas->drawRect(x, y, w, h, color);
    }
}

void DisplayRenderer::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (canvas) {
        canvas->fillRect(x, y, w, h, color);
    }
}

void DisplayRenderer::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (canvas) {
        canvas->drawCircle(x0, y0, r, color);
    }
}

void DisplayRenderer::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    if (canvas) {
        canvas->fillCircle(x0, y0, r, color);
    }
}

void DisplayRenderer::setTextColor(uint16_t color) {
    if (canvas) {
        canvas->setTextColor(color);
    }
}

void DisplayRenderer::setTextSize(uint8_t size) {
    if (canvas) {
        canvas->setTextSize(size);
    }
}

void DisplayRenderer::setCursor(int16_t x, int16_t y) {
    if (canvas) {
        canvas->setCursor(x, y);
    }
}

void DisplayRenderer::print(const char* text) {
    if (canvas) {
        canvas->print(text);
    }
}

void DisplayRenderer::print(int value) {
    if (canvas) {
        canvas->print(value);
    }
}

void DisplayRenderer::printStats() {
    Serial.print("Dirty areas: ");
    Serial.println(dirtyAreaCount);
    Serial.print("Full redraw: ");
    Serial.println(needsFullRedraw ? "true" : "false");
}