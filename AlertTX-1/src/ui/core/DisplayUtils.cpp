#include "DisplayUtils.h"

// =============================================================================
// TEXT RENDERING UTILITIES
// =============================================================================

void DisplayUtils::centerText(Adafruit_ST7789* display, const char* text, int textSize, int y) {
    // Calculate text bounds for centering
    int16_t x1, y1;
    uint16_t w, h;
    
    display->setTextSize(textSize);
    display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    // Center horizontally using display config
    int x = CENTER_X(w);
    
    display->setCursor(x, y);
    display->println(text);
}

void DisplayUtils::centerTextWithColor(Adafruit_ST7789* display, const char* text, int textSize, int y, uint16_t color) {
    display->setTextColor(color);
    centerText(display, text, textSize, y);
}

void DisplayUtils::drawTitle(Adafruit_ST7789* display, const char* title) {
    display->setTextColor(ThemeManager::getPrimaryText());
    centerText(display, title, TEXT_SIZE_TITLE, TITLE_Y);
}

int DisplayUtils::getTextWidth(Adafruit_ST7789* display, const char* text, int textSize) {
    int16_t x1, y1;
    uint16_t w, h;
    
    display->setTextSize(textSize);
    display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    return w;
}

// =============================================================================
// SHAPE AND DRAWING UTILITIES  
// =============================================================================

void DisplayUtils::drawCenteredRect(Adafruit_ST7789* display, int width, int height, uint16_t color) {
    int x = CENTER_X(width);
    int y = CENTER_Y(height);
    
    display->fillRect(x, y, width, height, color);
}

void DisplayUtils::drawSeparatorLine(Adafruit_ST7789* display, int y, uint16_t color) {
    display->drawFastHLine(0, y, DISPLAY_WIDTH, color);
}

// =============================================================================
// LAYOUT UTILITIES
// =============================================================================

bool DisplayUtils::isOnScreen(int x, int y) {
    return IS_ON_SCREEN(x, y);
}

bool DisplayUtils::fitsOnScreen(int x, int y, int width, int height) {
    return (x >= 0 && y >= 0 && 
            (x + width) <= DISPLAY_WIDTH && 
            (y + height) <= DISPLAY_HEIGHT);
}

void DisplayUtils::clampToScreen(int& x, int& y) {
    x = CLAMP_X(x);
    y = CLAMP_Y(y);
}

// =============================================================================
// DEBUG UTILITIES
// =============================================================================

void DisplayUtils::debugScreenEnter(const char* screenName) {
    #ifdef DEBUG_SCREENS
    Serial.printf("=== ENTERED %s ===\n", screenName);
    #endif
}

void DisplayUtils::debugScreenExit(const char* screenName) {
    #ifdef DEBUG_SCREENS
    Serial.printf("=== EXITED %s ===\n", screenName);
    #endif
}

void DisplayUtils::debugScreenAction(const char* screenName, const char* action) {
    #ifdef DEBUG_SCREENS
    Serial.printf("%s: %s\n", screenName, action);
    #endif
}

void DisplayUtils::drawDebugOverlay(Adafruit_ST7789* display) {
    #ifdef DEBUG_DISPLAY
    uint16_t debugColor = 0xF800; // Red
    
    // Draw screen boundary
    display->drawRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, debugColor);
    
    // Draw safe area
    display->drawRect(MARGIN_MEDIUM, MARGIN_MEDIUM, 
                     SAFE_AREA_WIDTH - 2*MARGIN_MEDIUM, 
                     SAFE_AREA_HEIGHT - 2*MARGIN_MEDIUM, debugColor);
    
    // Draw center crosshairs
    display->drawFastVLine(DISPLAY_WIDTH/2, 0, DISPLAY_HEIGHT, debugColor);
    display->drawFastHLine(0, DISPLAY_HEIGHT/2, DISPLAY_WIDTH, debugColor);
    
    // Display dimensions text
    display->setTextColor(debugColor);
    display->setTextSize(1);
    display->setCursor(5, 5);
    display->printf("%dx%d", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    #endif
}

#if __has_include("../../icons/Icon.h")
#include "../../icons/Icon.h"
#include <pgmspace.h>
static inline void drawIconLine(Adafruit_ST7789* display, const uint16_t* data, int x, int y, int w) {
    display->drawRGBBitmap(x, y, data, w, 1);
}

static void drawIconInternal(Adafruit_ST7789* display, const Icon& icon, int x, int y) {
    if (!display) return;
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
    int drawWidth = icon.w;
    int drawHeight = icon.h;
    if (drawWidth <= 0 || drawHeight <= 0) return;

    const int MAX_LINE = DISPLAY_WIDTH;
    static uint16_t line[MAX_LINE];
    int copyWidth = (drawWidth < MAX_LINE) ? drawWidth : MAX_LINE;
    for (int row = 0; row < drawHeight; ++row) {
        for (int col = 0; col < copyWidth; ++col) {
            line[col] = pgm_read_word(&(icon.data[row * icon.w + col]));
        }
        drawIconLine(display, line, x, y + row, copyWidth);
    }
}

void DisplayUtils::drawIcon(Adafruit_ST7789* display, const Icon& icon, int x, int y) {
    drawIconInternal(display, icon, x, y);
}
#endif
