#include "Theme.h"

// Theme implementations
const Theme THEME_MONOCHROME = {
    .background = RGB565_BLACK,
    .foreground = RGB565_WHITE,
    .accent = RGB565_GRAY,
    .secondary = RGB565_GRAY,
    .border = RGB565_GRAY
};

const Theme THEME_GREEN = {
    .background = RGB565_PIPBOY_BG,      // Very dark green (nearly black)
    .foreground = RGB565_PIPBOY_GREEN,   // Bright green text
    .accent = RGB565_WHITE,              // White for highlights
    .secondary = RGB565_GRAY,            // Gray for secondary text
    .border = RGB565_PIPBOY_GREEN        // Green borders
};

const Theme THEME_AMBER = {
    .background = RGB565_FLIPPER_AMBER,  // Bright amber background
    .foreground = RGB565_FLIPPER_BG,     // Pure black text
    .accent = RGB565_BLACK,              // Black for highlights
    .secondary = RGB565_GRAY,            // Gray for secondary text
    .border = RGB565_BLACK               // Black borders
};

const Theme THEME_INVERTED = {
    .background = RGB565_WHITE,
    .foreground = RGB565_BLACK,
    .accent = RGB565_GRAY,
    .secondary = RGB565_GRAY,
    .border = RGB565_GRAY
};