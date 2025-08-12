#ifndef THEME_H
#define THEME_H

#include <stdint.h>

// RGB565 color helpers for monochrome themes
#define RGB565_BLACK     0x0000
#define RGB565_WHITE     0xFFFF
#define RGB565_GREEN     0x07E0
#define RGB565_AMBER     0xFD20
#define RGB565_GRAY      0x8410

// FlipperZero-style colors (black text on amber background)
#define RGB565_FLIPPER_AMBER    0xFD20  // Bright amber/orange background
#define RGB565_FLIPPER_BG       0x0000  // Pure black text

// Pip-Boy style colors (bright green on very dark green)
#define RGB565_PIPBOY_GREEN     0x07E0  // Bright green (close to white)
#define RGB565_PIPBOY_BG        0x0200  // Very dark green (nearly black)

struct Theme {
    uint16_t background;      // Main background color
    uint16_t foreground;      // Text and icon color
    uint16_t accent;          // Selection/highlight color
    uint16_t secondary;       // Secondary text color
    uint16_t border;          // Border/divider color
};

// Predefined themes
extern const Theme THEME_MONOCHROME;    // Black background, white text
extern const Theme THEME_GREEN;         // Very dark green background, bright green text (Pip-Boy style)
extern const Theme THEME_AMBER;         // Amber background, black text (FlipperZero style)
extern const Theme THEME_INVERTED;      // White background, black text

#endif // THEME_H