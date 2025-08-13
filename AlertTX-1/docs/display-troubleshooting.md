# ESP32-S3 Reverse TFT Feather Display Troubleshooting Guide

## ⚠️ Common Issue: Display Shows Only Backlight

### Problem Description
When using the Adafruit ESP32-S3 Reverse TFT Feather, the most common issue is that the display backlight turns on but no graphics are displayed. This manifests as:

- Bright backlight visible
- No text, colors, or graphics appear
- Code appears to run normally (serial output works)
- Display initialization functions complete without errors

### Root Cause
The ESP32-S3 Reverse TFT Feather requires **explicit power enabling** for the TFT display subsystem via the `TFT_I2C_POWER` pin. This is **mandatory** and different from many other TFT displays.

### ✅ Solution: Correct Initialization Sequence

**Use this exact initialization order:**

```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Use hardware-defined pins (predefined by board variant)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    
    // STEP 1: Enable TFT power supply (CRITICAL!)
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);  // Allow power to stabilize
    
    // STEP 2: Enable backlight
    pinMode(TFT_BACKLITE, OUTPUT);  // Note: TFT_BACKLITE not TFT_BACKLIGHT
    digitalWrite(TFT_BACKLITE, HIGH);
    
    // STEP 3: Initialize display
    tft.init(135, 240);  // Portrait dimensions
    tft.setRotation(3);  // Rotate to landscape (240x135)
    
    // STEP 4: Test - should show red screen
    tft.fillScreen(ST77XX_RED);
}
```

### 🔑 Critical Notes

1. **`TFT_I2C_POWER` is mandatory** - Without this, only backlight will work
2. **Use `TFT_BACKLITE`** - This is the correct define for ESP32-S3 Reverse TFT Feather (not `TFT_BACKLIGHT`)
3. **Order matters** - Power → Backlight → Init → Use
4. **Both pins are predefined** - No need to define pin numbers manually
5. **10ms delay** - Allows power supply to stabilize

### ❌ Common Mistakes

```cpp
// WRONG - Missing TFT_I2C_POWER
pinMode(TFT_BACKLIGHT, OUTPUT);  // Wrong pin name too
digitalWrite(TFT_BACKLIGHT, HIGH);
tft.init(135, 240);

// WRONG - Wrong order
tft.init(135, 240);
pinMode(TFT_I2C_POWER, OUTPUT);
digitalWrite(TFT_I2C_POWER, HIGH);

// WRONG - Missing delay
pinMode(TFT_I2C_POWER, OUTPUT);
digitalWrite(TFT_I2C_POWER, HIGH);  // No delay
pinMode(TFT_BACKLITE, OUTPUT);
```

### 🧪 Quick Test Code

Use this minimal test to verify your display is working:

```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("ESP32-S3 Reverse TFT Test");
    
    // Correct initialization sequence
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);
    
    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);
    
    tft.init(135, 240);
    tft.setRotation(3);
    
    // Test sequence
    tft.fillScreen(ST77XX_RED);
    delay(1000);
    tft.fillScreen(ST77XX_GREEN);
    delay(1000);
    tft.fillScreen(ST77XX_BLUE);
    delay(1000);
    
    // Text test
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(20, 60);
    tft.println("SUCCESS!");
    
    Serial.println("Display test complete!");
}

void loop() {
    // Color cycling test
    static uint32_t lastChange = 0;
    static int color = 0;
    
    if (millis() - lastChange > 2000) {
        uint16_t colors[] = {ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE, ST77XX_WHITE};
        tft.fillScreen(colors[color]);
        color = (color + 1) % 4;
        lastChange = millis();
    }
}
```

### 📋 Verification Checklist

✅ **Working Display:**
- Colors appear immediately after init
- Text is clearly visible
- Graphics draw correctly
- No "blank backlight" period

❌ **Broken Display:**
- Only backlight visible (bright white/colored glow)
- No graphics appear
- Serial output shows "success" but screen is blank

### 📚 References

- [Official ESP32-S3 Reverse TFT Feather Guide](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/built-in-tft)
- [Adafruit GFX Graphics Library](https://learn.adafruit.com/adafruit-gfx-graphics-library)
- [Product Page](https://www.adafruit.com/product/5691)

### 🛠️ If Still Not Working

1. **Verify board selection** - Use "Adafruit Feather ESP32-S3 Reverse TFT"
2. **Check library versions** - Use latest Adafruit GFX and ST7789 libraries
3. **Test with official examples** - Try Adafruit's graphics test first
4. **Hardware check** - Ensure no physical damage to display
5. **Power supply** - Make sure USB provides adequate power

---

**Last Updated:** August 2024  
**Status:** ✅ Verified Working with ESP32-S3 Reverse TFT Feather
