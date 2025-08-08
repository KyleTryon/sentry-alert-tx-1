#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <Adafruit_GFX.h>
#include "../config/settings.h"

// Display configuration class that handles different display types
class DisplayConfig {
public:
    static Adafruit_GFX* createDisplay();
    static void initializeDisplay(Adafruit_GFX* display);
    static int getWidth() { return DISPLAY_WIDTH; }
    static int getHeight() { return DISPLAY_HEIGHT; }
    
private:
    static void setupPins();
};

#endif // DISPLAY_CONFIG_H 