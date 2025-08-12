/*
 * Advanced Icon Usage Example
 * 
 * This example demonstrates how to implement the actual drawing function
 * with scaling support for the generated icon headers.
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

// Example display buffer (replace with your actual display library)
uint16_t displayBuffer[240][320]; // Example 240x320 display

// Function to set a pixel in the display buffer
void setPixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < 320 && y >= 0 && y < 240) {
        displayBuffer[y][x] = color;
    }
}

// Advanced drawing function with scaling support
void drawIcon(const Icon& icon, int x, int y, int scale = 1) {
    for (int py = 0; py < icon.h; py++) {
        for (int px = 0; px < icon.w; px++) {
            // Read pixel from PROGMEM
            uint16_t pixel = pgm_read_word(&icon.data[py * icon.w + px]);
            
            // Skip transparent pixels (black in our case)
            if (pixel == 0x0000) continue;
            
            // Draw scaled pixel
            for (int sy = 0; sy < scale; sy++) {
                for (int sx = 0; sx < scale; sx++) {
                    int drawX = x + (px * scale) + sx;
                    int drawY = y + (py * scale) + sy;
                    setPixel(drawX, drawY, pixel);
                }
            }
        }
    }
}

// Function to draw an icon with a background color
void drawIconWithBackground(const Icon& icon, int x, int y, int scale = 1, uint16_t bgColor = 0x0000) {
    int scaledWidth = icon.w * scale;
    int scaledHeight = icon.h * scale;
    
    // Fill background
    for (int sy = 0; sy < scaledHeight; sy++) {
        for (int sx = 0; sx < scaledWidth; sx++) {
            setPixel(x + sx, y + sy, bgColor);
        }
    }
    
    // Draw icon on top
    drawIcon(icon, x, y, scale);
}

// Function to center an icon on screen
void drawIconCentered(const Icon& icon, int scale = 1) {
    int screenWidth = 320;  // Replace with your display width
    int screenHeight = 240; // Replace with your display height
    int scaledWidth = icon.w * scale;
    int scaledHeight = icon.h * scale;
    
    int x = (screenWidth - scaledWidth) / 2;
    int y = (screenHeight - scaledHeight) / 2;
    
    drawIcon(icon, x, y, scale);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Advanced Icon Usage Example");
    Serial.println("============================");
    
    // Clear display buffer
    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            displayBuffer[y][x] = 0x0000; // Black background
        }
    }
    
    // Example 1: Draw icons at different scales
    Serial.println("Drawing icons at different scales...");
    drawIcon(alert, 10, 10);           // 16x16
    drawIcon(battery_full, 50, 10, 2); // 32x32
    drawIcon(play, 10, 50, 3);         // 48x48
    drawIcon(pause, 50, 50, 1);        // 16x16
    
    // Example 2: Draw icons with backgrounds
    Serial.println("Drawing icons with backgrounds...");
    drawIconWithBackground(alert, 150, 10, 2, 0x8410); // Gray background
    drawIconWithBackground(battery_full, 150, 50, 3, 0xF800); // Red background
    
    // Example 3: Center an icon
    Serial.println("Centering an icon...");
    drawIconCentered(play, 4); // 64x64 centered icon
    
    Serial.println("Advanced icon drawing complete!");
    Serial.println("Check your display buffer for the results.");
}

void loop() {
    // Nothing to do in loop for this example
    delay(1000);
}
