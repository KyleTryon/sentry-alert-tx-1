#include "IconRenderer.h"
#include <Arduino.h>

void IconRenderer::drawIcon(DisplayRenderer* renderer, const Icon& icon, 
                           int x, int y, const Theme& theme) {
    drawIcon(renderer, icon, x, y, theme.foreground);
}

void IconRenderer::drawIcon(DisplayRenderer* renderer, const Icon& icon,
                           int x, int y, uint16_t color) {
    if (!renderer || !isValidIcon(&icon)) {
        return;
    }
    
    // Draw icon pixel by pixel
    for (int py = 0; py < icon.height; py++) {
        for (int px = 0; px < icon.width; px++) {
            uint16_t pixelColor = pgm_read_word(&icon.data[py * icon.width + px]);
            
            // Only draw non-black pixels (treating black as transparent)
            if (pixelColor != 0x0000) {
                renderer->drawPixel(x + px, y + py, color);
            }
        }
    }
}

void IconRenderer::drawIconTransparent(DisplayRenderer* renderer, const Icon& icon,
                                      int x, int y, uint16_t foregroundColor, uint16_t backgroundColor) {
    if (!renderer || !isValidIcon(&icon)) {
        return;
    }
    
    // Draw icon pixel by pixel with transparency
    for (int py = 0; py < icon.height; py++) {
        for (int px = 0; px < icon.width; px++) {
            uint16_t pixelColor = pgm_read_word(&icon.data[py * icon.width + px]);
            
            if (pixelColor == 0x0000) {
                // Black pixels become background
                renderer->drawPixel(x + px, y + py, backgroundColor);
            } else {
                // Non-black pixels become foreground
                renderer->drawPixel(x + px, y + py, foregroundColor);
            }
        }
    }
}

void IconRenderer::drawIconWithColorReplacement(DisplayRenderer* renderer, const Icon& icon,
                                               int x, int y, uint16_t fromColor, uint16_t toColor) {
    if (!renderer || !isValidIcon(&icon)) {
        return;
    }
    
    // Draw icon with color replacement
    for (int py = 0; py < icon.height; py++) {
        for (int px = 0; px < icon.width; px++) {
            uint16_t pixelColor = pgm_read_word(&icon.data[py * icon.width + px]);
            
            if (pixelColor == fromColor) {
                renderer->drawPixel(x + px, y + py, toColor);
            } else if (pixelColor != 0x0000) {
                // Draw original color if not black and not the replacement color
                renderer->drawPixel(x + px, y + py, pixelColor);
            }
        }
    }
}

bool IconRenderer::isValidIcon(const Icon* icon) {
    return (icon != nullptr && 
            icon->data != nullptr && 
            icon->width > 0 && 
            icon->height > 0 &&
            icon->width <= 32 &&  // Reasonable size limits
            icon->height <= 32);
}

void IconRenderer::drawIconBounds(DisplayRenderer* renderer, const Icon& icon, 
                                 int x, int y, uint16_t color) {
    if (!renderer || !isValidIcon(&icon)) {
        return;
    }
    
    // Draw a rectangle around the icon for debugging
    renderer->drawRect(x - 1, y - 1, icon.width + 2, icon.height + 2, color);
}