#ifndef ICON_H
#define ICON_H

#include <Arduino.h>

// Icon structure for storing icon data
struct Icon {
    int16_t x_offset;      // X offset for positioning
    int16_t y_offset;      // Y offset for positioning
    uint16_t width;        // Icon width in pixels
    uint16_t height;       // Icon height in pixels
    const uint16_t* data;  // Pointer to RGB565 pixel data
};

#endif // ICON_H

