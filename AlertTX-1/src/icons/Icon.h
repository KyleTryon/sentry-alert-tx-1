#ifndef ICON_H
#define ICON_H

#include <stdint.h>

// Icon structure for 16x16 RGB565 icons
struct Icon {
    int x;                    // X offset (usually 0)
    int y;                    // Y offset (usually 0)
    int width;                // Width in pixels (should be 16)
    int height;               // Height in pixels (should be 16)
    const uint16_t* data;     // RGB565 pixel data in PROGMEM
};

#endif // ICON_H