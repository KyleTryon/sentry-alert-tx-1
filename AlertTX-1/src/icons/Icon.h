#ifndef ICON_H
#define ICON_H

#include <Arduino.h>

/**
 * Icon Structure
 * 
 * Represents a bitmap icon with position, dimensions, and RGB565 pixel data.
 * Used by the icon generation system for efficient icon rendering.
 */

struct Icon {
    int x;                          // X position (can be set when drawing)
    int y;                          // Y position (can be set when drawing)
    int width;                      // Icon width in pixels
    int height;                     // Icon height in pixels
    const uint16_t* data;           // RGB565 pixel data array (stored in PROGMEM)
};

#endif // ICON_H
