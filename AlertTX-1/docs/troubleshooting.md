# Troubleshooting Guide

Common issues and solutions for the AlertTX-1 project.

## 🖥️ Display Issues

### Problem: Display Only Shows Backlight
**Symptoms**: Bright backlight but no graphics or text

**Solution**:
```cpp
// CRITICAL: Enable TFT power first
pinMode(TFT_I2C_POWER, OUTPUT);
digitalWrite(TFT_I2C_POWER, HIGH);
delay(10);

// Then backlight
pinMode(TFT_BACKLITE, OUTPUT);
digitalWrite(TFT_BACKLITE, HIGH);

// Finally initialize
tft.init(135, 240);
tft.setRotation(3);
```

**See**: [Display Troubleshooting Guide](setup/display-troubleshooting.md) for detailed fixes.

### Problem: Display Flickers or Shows Artifacts
**Symptoms**: Random pixels, flickering, or corrupted graphics

**Causes**:
- Insufficient power supply
- Loose connections
- Incorrect SPI speed

**Solutions**:
1. Use quality USB cable
2. Check all connections
3. Add capacitor across power pins
4. Reduce SPI speed in initialization

## 🔊 Audio Issues

### Problem: No Sound from Buzzer
**Symptoms**: Buzzer doesn't make any sound

**Checklist**:
1. Verify wiring: Buzzer (+) to GPIO14 (A4), (-) to GND
2. Check buzzer type: Must be passive (not active)
3. Test with simple tone:
   ```cpp
   tone(BUZZER_PIN, 1000, 500); // 1kHz for 500ms
   ```
4. Verify pin definition in `settings.h`:
   ```cpp
   const int BUZZER_PIN = 14;
   ```

### Problem: Buzzer Volume Too Low
**Symptoms**: Sound is barely audible

**Solutions**:
1. Direct connection provides ~50% volume due to GPIO current limits
2. For full volume, use transistor driver circuit
3. Check buzzer specifications (should be 2.5-4.5V rated)

## 🔌 Connection Issues

### Problem: Device Not Detected
**Symptoms**: Arduino IDE/CLI can't find the device

**Solutions**:
1. **Check USB cable**: Use data cable, not charge-only
2. **Install drivers**: 
   - Windows: Install CP2104 drivers
   - macOS/Linux: Usually automatic
3. **Check port**:
   ```bash
   arduino-cli board list
   # or
   ls /dev/tty* | grep -E "ACM|USB"
   ```
4. **Reset device**: Hold BOOT + press RESET

### Problem: Upload Fails
**Symptoms**: "Failed to connect" or timeout errors

**Solutions**:
1. **Enter bootloader mode**:
   - Hold BOOT button
   - Press and release RESET
   - Release BOOT after 1 second
2. **Reduce upload speed**:
   ```bash
   make UPLOAD_SPEED=115200 upload
   ```
3. **Check board selection**:
   - Must be "Adafruit Feather ESP32-S3 Reverse TFT"

## 🏗️ Build Errors

### Problem: Library Not Found
**Symptoms**: Compilation fails with missing library errors

**Solution**:
```bash
# Install all required libraries
make libraries

# Or manually install
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "Adafruit ST7789"
arduino-cli lib install "ArduinoJson"
arduino-cli lib install "PubSubClient"
```

### Problem: Compilation Errors
**Symptoms**: Syntax errors or undefined references

**Solutions**:
1. **Clean and rebuild**:
   ```bash
   make clean
   make build
   ```
2. **Check board configuration**:
   ```bash
   make info
   ```
3. **Update board definitions**:
   ```bash
   arduino-cli core update-index
   arduino-cli core upgrade esp32:esp32
   ```

### Problem: Out of Memory
**Symptoms**: "Sketch too big" or RAM overflow

**Solutions**:
1. **Check memory usage**:
   ```bash
   make size
   ```
2. **Optimize code**:
   - Use PROGMEM for constants
   - Reduce string literals
   - Disable unused features
3. **Change partition scheme**:
   - Use "Minimal SPIFFS" partition

## 📡 MQTT/Network Issues

### Problem: Won't Connect to WiFi
**Symptoms**: Stuck at "Connecting to WiFi"

**Solutions**:
1. **Check credentials** in main sketch:
   ```cpp
   const char* ssid = "your-ssid";
   const char* password = "your-password";
   ```
2. **Verify 2.4GHz network** (5GHz not supported)
3. **Check signal strength**
4. **Add debug output**:
   ```cpp
   Serial.print("WiFi Status: ");
   Serial.println(WiFi.status());
   ```

### Problem: MQTT Connection Fails
**Symptoms**: WiFi connects but MQTT doesn't

**Solutions**:
1. **Verify broker settings**:
   ```cpp
   const char* mqtt_server = "broker.address";
   const int mqtt_port = 1883;
   ```
2. **Check firewall/ports**
3. **Test with mosquitto_sub**:
   ```bash
   mosquitto_sub -h broker.address -t "alerts/beeper"
   ```
4. **Enable debug logging**:
   ```cpp
   #define MQTT_DEBUG 1
   ```

## 🎮 Game Performance

### Problem: Games Run Slowly
**Symptoms**: Low frame rate, choppy animation

**Solutions**:
1. **Check frame limiting**:
   ```cpp
   setTargetFPS(60); // In game constructor
   ```
2. **Optimize rendering**:
   - Use dirty region tracking
   - Avoid full screen redraws
   - See [Game Development Guide](development/game-development.md)

### Problem: Screen Flickers in Games
**Symptoms**: Visible flicker during gameplay

**Solutions**:
1. **Don't use `markForFullRedraw()`** in game loop
2. **Track previous positions**:
   ```cpp
   // Clear old position
   display->fillRect(prevX, prevY, size, size, bgColor);
   // Draw new position
   display->fillRect(x, y, size, size, color);
   ```
3. **Use partial updates** instead of full redraws

## 🔋 Power Issues

### Problem: Device Resets Randomly
**Symptoms**: Unexpected reboots

**Causes**:
- Insufficient power
- Brown-out detection
- Software bugs

**Solutions**:
1. **Use quality power supply**
2. **Check current draw**:
   - Display: ~50mA
   - WiFi: ~150mA peak
   - Total: ~200-250mA
3. **Add delays in setup**:
   ```cpp
   void setup() {
     delay(100); // Allow power to stabilize
     // Initialize components
   }
   ```

### Problem: Battery Drains Quickly
**Symptoms**: Poor battery life

**Solutions**:
1. **Implement sleep modes** (see [Power Management](features/power-management.md))
2. **Reduce display brightness**
3. **Disable WiFi when not needed**:
   ```cpp
   WiFi.disconnect();
   WiFi.mode(WIFI_OFF);
   ```

## 🐛 Debug Techniques

### Serial Monitor
```cpp
// Add debug output
Serial.begin(115200);
Serial.println("Debug: Reached checkpoint");
Serial.printf("Value: %d\n", variable);
```

### LED Debugging
```cpp
// Use built-in NeoPixel for status
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixel(1, 33, NEO_GRB + NEO_KHZ800);

void showStatus(uint32_t color) {
  pixel.setPixelColor(0, color);
  pixel.show();
}

// Usage
showStatus(pixel.Color(255, 0, 0));  // Red = error
showStatus(pixel.Color(0, 255, 0));  // Green = success
```

### Memory Monitoring
```cpp
void printMemory() {
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Largest block: %d bytes\n", ESP.getMaxAllocHeap());
}
```

## 🆘 Getting Help

### Before Asking for Help
1. Check this troubleshooting guide
2. Read relevant documentation
3. Search existing [GitHub Issues](https://github.com/techsquidtv/sentry_alert_tx-1/issues)
4. Try example code first

### When Reporting Issues
Include:
- **Device model**: ESP32-S3 Reverse TFT Feather
- **Error messages**: Complete output
- **Code snippet**: Minimal example
- **What you tried**: Steps taken
- **Environment**: OS, Arduino version

### Support Channels
- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions
- **Documentation**: This guide and others

---

**Still stuck?** Create a detailed [issue](https://github.com/techsquidtv/sentry_alert_tx-1/issues/new) with all the information above.
