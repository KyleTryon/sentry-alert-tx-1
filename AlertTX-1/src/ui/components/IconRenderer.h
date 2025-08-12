#ifndef ICON_RENDERER_H
#define ICON_RENDERER_H

#include "../core/Theme.h"
#include "../icons/IconDefinitions.h"
#include "../renderer/DisplayRenderer.h"

class IconRenderer {
public:
    // Draw icon at position with theme colors
    static void drawIcon(DisplayRenderer* renderer, const Icon& icon, 
                        int x, int y, const Theme& theme);
    
    // Draw icon with custom foreground color
    static void drawIcon(DisplayRenderer* renderer, const Icon& icon,
                        int x, int y, uint16_t color);
    
    // Draw icon with transparency support (replaces black pixels with background)
    static void drawIconTransparent(DisplayRenderer* renderer, const Icon& icon,
                                   int x, int y, uint16_t foregroundColor, uint16_t backgroundColor);
    
    // Draw icon with color replacement (replace one color with another)
    static void drawIconWithColorReplacement(DisplayRenderer* renderer, const Icon& icon,
                                            int x, int y, uint16_t fromColor, uint16_t toColor);

    // Utility functions
    static bool isValidIcon(const Icon* icon);
    static void drawIconBounds(DisplayRenderer* renderer, const Icon& icon, 
                              int x, int y, uint16_t color);  // For debugging
};

#endif // ICON_RENDERER_H
