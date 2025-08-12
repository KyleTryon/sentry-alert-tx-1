/**
 * Simple Screen Test for ESP32-S3 Reverse TFT Feather
 * 
 * This is a minimal test to verify the screen is working with 
 * the correct Adafruit GFX library stack.
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

// Display pin definitions (from Adafruit documentation)
#define TFT_CS        7
#define TFT_RST       40 
#define TFT_DC        39
#define TFT_BACKLIGHT 45

// Initialize display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    Serial.println("=== ESP32-S3 Reverse TFT Feather Screen Test ===");
    
    // Turn on backlight
    pinMode(TFT_BACKLIGHT, OUTPUT);
    digitalWrite(TFT_BACKLIGHT, HIGH);
    
    // Initialize display
    Serial.println("Initializing display...");
    tft.init(135, 240);  // Initialize with the dimensions (portrait)
    tft.setRotation(3);  // Rotate to landscape (240x135)
    
    // Clear screen
    tft.fillScreen(ST77XX_BLACK);
    Serial.println("Screen cleared to black");
    
    // Draw "Hello, World!" text
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(30, 40);
    tft.println("Hello, World!");
    Serial.println("Drew 'Hello, World!' text");
    
    // Draw a colored rectangle
    tft.fillRect(30, 80, 180, 30, ST77XX_GREEN);
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(1);
    tft.setCursor(35, 90);
    tft.println("Screen test successful!");
    Serial.println("Drew green rectangle with text");
    
    // Draw some additional test elements
    tft.drawCircle(200, 100, 20, ST77XX_RED);
    tft.fillCircle(50, 100, 15, ST77XX_BLUE);
    
    Serial.println("=== Screen test complete! ===");
    Serial.println("If you can see text and shapes on the screen, it's working correctly.");
}

void loop() {
    // Blink the built-in LED to show the sketch is running
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
    if (millis() - lastBlink > 1000) {
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
        lastBlink = millis();
        
        Serial.println("Sketch running... (LED blink)");
    }
}
