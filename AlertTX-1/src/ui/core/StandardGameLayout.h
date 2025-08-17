#ifndef STANDARD_GAME_LAYOUT_H
#define STANDARD_GAME_LAYOUT_H

#include <Adafruit_ST7789.h>
#include "Theme.h"
#include "../../config/DisplayConfig.h"

class StandardGameLayout {
public:
    static const int HEADER_HEIGHT = 29;        // y = 0..28
    static const int PLAY_AREA_TOP = 30;         // y = 30
    static const int PLAY_AREA_LEFT = 6;         // x = 6
    static const int PLAY_AREA_RIGHT = 234;      // x = 234
    static const int PLAY_AREA_BOTTOM = 125;     // y = 125
    static const int FOOTER_TOP = 126;           // y >= 126

    static const int PLAY_AREA_WIDTH = PLAY_AREA_RIGHT - PLAY_AREA_LEFT;  // 228px
    static const int PLAY_AREA_HEIGHT = PLAY_AREA_BOTTOM - PLAY_AREA_TOP; // 95px

    static void drawGameHeader(Adafruit_ST7789* display, const char* title, int score = -1, const char* scoreLabel = "Score");
    static void clearHeader(Adafruit_ST7789* display);

    static void drawPlayAreaBorder(Adafruit_ST7789* display, uint16_t borderColor);
    static void clearPlayArea(Adafruit_ST7789* display, uint16_t bgColor);

    static bool isInPlayArea(int x, int y);
    static void clampToPlayArea(int& x, int& y);

    static bool isHeaderDirty;
    static bool isPlayAreaDirty;
};

#endif // STANDARD_GAME_LAYOUT_H