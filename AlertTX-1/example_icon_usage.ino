/*
 * Example: Using Generated Icons
 * 
 * This example demonstrates how to use the automatically generated
 * icon headers in your Arduino project.
 * 
 * To use this example:
 * 1. Run "make icons" to generate the icon headers
 * 2. Upload this sketch to your ESP32
 */

#include "src/icons/Icon.h"
#include "src/icons/alert.h"
#include "src/icons/battery_full.h"
#include "src/icons/play.h"
#include "src/icons/pause.h"

// Example function to draw an icon (you would implement this based on your display library)
void drawIcon(const Icon& icon, int x, int y, int scale = 1) {
    int scaledWidth = icon.w * scale;
    int scaledHeight = icon.h * scale;
    Serial.printf("Drawing icon at (%d, %d) with scale %dx (size: %dx%d)\n", x, y, scale, scaledWidth, scaledHeight);
    
    // This is a placeholder - you would implement actual drawing here
    // For example, with Adafruit_GFX:
    // for (int py = 0; py < icon.h; py++) {
    //     for (int px = 0; px < icon.w; px++) {
    //         uint16_t pixel = pgm_read_word(&icon.data[py * icon.w + px]);
    //         // Draw scaled pixel
    //         for (int sy = 0; sy < scale; sy++) {
    //             for (int sx = 0; sx < scale; sx++) {
    //                 display.drawPixel(x + (px * scale) + sx, y + (py * scale) + sy, pixel);
    //             }
    //         }
    //     }
    // }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Icon Usage Example");
    Serial.println("==================");
    
    // Demonstrate accessing icon properties
    Serial.printf("Alert icon: %dx%d pixels\n", ALERT_WIDTH, ALERT_HEIGHT);
    Serial.printf("Battery full icon: %dx%d pixels\n", BATTERY_FULL_WIDTH, BATTERY_FULL_HEIGHT);
    Serial.printf("Play icon: %dx%d pixels\n", PLAY_WIDTH, PLAY_HEIGHT);
    Serial.printf("Pause icon: %dx%d pixels\n", PAUSE_WIDTH, PAUSE_HEIGHT);
    
    // Note: All icons are 16x16 pixels
    Serial.println("Note: All icons are standardized to 16x16 pixels for consistency");
    
    // Demonstrate drawing icons with different scales
    Serial.println("\nDrawing icons:");
    drawIcon(alert, 10, 10);           // Default scale (1x)
    drawIcon(battery_full, 50, 10, 2); // 2x scale (32x32)
    drawIcon(play, 10, 50, 3);         // 3x scale (48x48)
    drawIcon(pause, 50, 50, 1);        // 1x scale (16x16)
    
    Serial.println("\nIcon usage example complete!");
}

void loop() {
    // Nothing to do in loop for this example
    delay(1000);
}
