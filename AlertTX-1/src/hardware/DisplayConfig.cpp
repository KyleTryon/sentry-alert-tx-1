#include "DisplayConfig.h"

#ifdef USE_ST7789_DISPLAY
#include <Adafruit_ST7789.h>
static Adafruit_ST7789* _display = nullptr;
#elif defined(USE_ILI9341_DISPLAY)
#include <Adafruit_ILI9341.h>
static Adafruit_ILI9341* _display = nullptr;
#else
#error "No display type defined in settings.h"
#endif

Adafruit_GFX* DisplayConfig::createDisplay() {
    setupPins();
    
#ifdef USE_ST7789_DISPLAY
    _display = new Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RESET);
    return _display;
#elif defined(USE_ILI9341_DISPLAY)
    _display = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RESET);
    return _display;
#endif
}

void DisplayConfig::initializeDisplay(Adafruit_GFX* display) {
#ifdef USE_ST7789_DISPLAY
    Adafruit_ST7789* st7789 = static_cast<Adafruit_ST7789*>(display);
    st7789->init(DISPLAY_HEIGHT, DISPLAY_WIDTH); // Note: init takes height, width
    st7789->setRotation(1); // Landscape mode
    
    // Initialize backlight if available
    #ifdef TFT_BACKLIGHT
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
    #endif
    
    st7789->fillScreen(ST77XX_BLACK);
    
#elif defined(USE_ILI9341_DISPLAY)
    Adafruit_ILI9341* ili9341 = static_cast<Adafruit_ILI9341*>(display);
    ili9341->begin();
    ili9341->setRotation(1); // Landscape mode
    ili9341->fillScreen(ILI9341_BLACK);
#endif
}

void DisplayConfig::setupPins() {
    // Pin setup is handled by the display library
    // This method can be used for any additional pin configuration if needed
} 