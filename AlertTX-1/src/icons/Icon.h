#ifndef ICON_H
#define ICON_H

#include <Arduino.h>

// Simple icon descriptor for RGB565 PROGMEM bitmaps
// Field names (w/h) match existing code usage in DisplayUtils and screens
struct Icon {
	int16_t x;
	int16_t y;
	uint16_t w;
	uint16_t h;
	const uint16_t* data; // PROGMEM pointer to pixel data
};

#endif // ICON_H


