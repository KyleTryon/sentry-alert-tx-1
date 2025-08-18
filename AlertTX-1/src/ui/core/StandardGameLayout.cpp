#include "StandardGameLayout.h"

bool StandardGameLayout::isHeaderDirty = true;
bool StandardGameLayout::isPlayAreaDirty = true;

static void drawCenteredTitle(Adafruit_ST7789* display, const char* title, int y) {
    if (!display || !title) return;
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(2);
    int16_t x1, y1; uint16_t w, h;
    display->getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    int x = (DISPLAY_WIDTH - (int)w) / 2;
    display->setCursor(x, y);
    display->print(title);
}

void StandardGameLayout::drawGameHeader(Adafruit_ST7789* display, const char* title, int score, const char* scoreLabel) {
    if (!display) return;
    uint16_t bg = ThemeManager::getBackground();
    display->fillRect(0, 0, DISPLAY_WIDTH, HEADER_HEIGHT, bg);

    // Title centered near top of header
    drawCenteredTitle(display, title ? title : "", 2);

    // Optional score line near bottom of header
    if (score >= 0) {
        display->setTextColor(ThemeManager::getPrimaryText());
        display->setTextSize(1);
        int y = HEADER_HEIGHT - 7;
        display->setCursor(10, y);
        display->print(scoreLabel ? scoreLabel : "Score");
        display->print(": ");
        display->print(score);
    }

    isHeaderDirty = false;
}

void StandardGameLayout::clearHeader(Adafruit_ST7789* display) {
    if (!display) return;
    display->fillRect(0, 0, DISPLAY_WIDTH, HEADER_HEIGHT, ThemeManager::getBackground());
    isHeaderDirty = false;
}

void StandardGameLayout::drawPlayAreaBorder(Adafruit_ST7789* display, uint16_t borderColor) {
    if (!display) return;
    int w = PLAY_AREA_RIGHT - PLAY_AREA_LEFT;
    int h = PLAY_AREA_BOTTOM - PLAY_AREA_TOP;
    display->drawRect(PLAY_AREA_LEFT, PLAY_AREA_TOP, w, h, borderColor);
}

void StandardGameLayout::clearPlayArea(Adafruit_ST7789* display, uint16_t bgColor) {
    if (!display) return;
    int w = PLAY_AREA_RIGHT - PLAY_AREA_LEFT;
    int h = PLAY_AREA_BOTTOM - PLAY_AREA_TOP;
    display->fillRect(PLAY_AREA_LEFT, PLAY_AREA_TOP, w, h, bgColor);
    isPlayAreaDirty = false;
}

bool StandardGameLayout::isInPlayArea(int x, int y) {
    return (x >= PLAY_AREA_LEFT && x < PLAY_AREA_RIGHT && y >= PLAY_AREA_TOP && y < PLAY_AREA_BOTTOM);
}

void StandardGameLayout::clampToPlayArea(int& x, int& y) {
    if (x < PLAY_AREA_LEFT) x = PLAY_AREA_LEFT;
    if (x >= PLAY_AREA_RIGHT) x = PLAY_AREA_RIGHT - 1;
    if (y < PLAY_AREA_TOP) y = PLAY_AREA_TOP;
    if (y >= PLAY_AREA_BOTTOM) y = PLAY_AREA_BOTTOM - 1;
}
