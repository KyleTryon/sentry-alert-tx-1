#ifndef ICON_STRUCT_H
#define ICON_STRUCT_H

#include <Arduino.h>

struct Icon {
    int x;
    int y;
    int w;
    int h;
    const uint16_t* data; // RGB565 pixel data in PROGMEM
};

#endif // ICON_STRUCT_H

