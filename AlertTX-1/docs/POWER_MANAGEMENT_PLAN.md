# Power Management Plan – Alert TX-1

References:
- Adafruit ESP32-S3 Reverse TFT Feather Power Management [link](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/power-management)
- Adafruit ESP32-S3 Reverse TFT Feather Pinouts [link](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/pinouts)

## Goals
- Real battery reading with MAX17048 fuel gauge and smoothing
- Auto-dim/backlight-off on inactivity; deep sleep after grace period
- Periodic deep-sleep wake to poll for notifications; instant wake on button
- Settings to configure timeouts and a "Sleep now / Power off" option
- Skip splash on deep-sleep wake; preserve UX consistency
- Detect USB presence and prevent sleep while USB is connected; start inactivity timer once unplugged

## Hardware References
- **Pinout**: `docs/pinout-reference.md` - Battery Monitor MAX17048 (I2C 0x36), VBAT, wake sources, TFT_BACKLITE (GPIO45)
- **Hardware Setup**: `docs/hardware-setup.md` - Display power, pin assignments, LiPo 400mAh specs

## Architecture Overview
Introduce `PowerManager` to orchestrate power states. Integrate with `InputRouter` for inactivity tracking and with `ScreenManager` for skip-splash behavior on wake.

### New Module: `src/power/PowerManager.{h,cpp}`
**Responsibilities:**
- `begin()`: Initialize MAX17048, backlight control (GPIO45)
- `onWake()`: Run at boot to handle splash skip and state carryover
- `update(nowMs)`: State machine transitions: Active → IdleDim → DeepSleepCycle
- `requestSleepNow()` / `requestPowerOff()`: Invoked from Settings to enter deep sleep
- `getBatteryVoltage()`, `getBatteryPercent()`: MAX17048-based readings with EMA smoothing
- `isUsbPowered()`: USB presence detection; disable dim/sleep while true (voltage heuristic)
- Config getters read from `SettingsManager`

**Power State Machine:**
- **Active**
  - `InputRouter` calls `PowerManager::notifyActivity()` on any input
  - If `now - lastActivityMs >= inactivityTimeoutMs` → IdleDim (dim/off backlight)
  - If additionally `>= inactivityTimeoutMs + dimGraceMs` → DeepSleepCycle
  - If `isUsbPowered()` → remain Active and reset inactivity timer
- **IdleDim**
  - Backlight off: `digitalWrite(TFT_BACKLITE, LOW)`
  - Any input → restore backlight, return to Active
  - If grace period exceeded → DeepSleepCycle
  - If `isUsbPowered()` → force Active and backlight on
- **DeepSleepCycle**
  - Configure timer wake: `esp_sleep_enable_timer_wakeup(sleepIntervalMs * 1000ULL)`
  - Configure button wake: ext0 (Button A, LOW) + ext1 (Button B/C, ANY_HIGH)
  - Enter `esp_deep_sleep_start()`
  - **On wake**: skip splash, reconnect Wi‑Fi, check MQTT for new messages
    - If new messages: navigate to Messages screen (future)
    - Else: re-enter DeepSleepCycle

**USB Detection:**
- Heuristic: treat as USB present if `batteryVoltage > 4.0V` (approximate)
- Behavior: While USB present, do not dim or sleep; inactivity timer is held/reset
- When USB disconnects, `lastActivityMs` is reset to start normal inactivity countdown

**Backlight Control:**
- Off mode: `digitalWrite(TFT_BACKLITE, LOW)` (immediate power savings)
- Future: PWM dimming with configurable levels

### Input Router Integration – `src/ui/core/InputRouter.h`
- Already centralizes all input handling
- Add: Call `PowerManager::notifyActivity()` on any press/release/long-press
- Preserve existing A/B auto-repeat and long-press back behavior

### Screen Manager Integration – `src/ui/core/ScreenManager.*`
- Clear screen before entering any screen (already implemented)
- Add: Skip splash on deep-sleep wake - `PowerManager::onWake()` determines initial screen

### Settings Integration – `src/config/SettingsManager.*`
Add config keys with defaults:
- `inactivityTimeoutMs` (default: 10000) - Time before dimming backlight
- `dimGraceMs` (default: 2000) - Grace period before deep sleep
- `deepSleepIntervalMs` (default: 60000) - Periodic wake interval
- Future: `backlightDimMode`/`backlightDimLevel`

### Settings UI – `src/ui/screens/SettingsScreen.*`
- Add "Power" item
  - MVP: triggers `PowerManager::requestSleepNow()`
  - Future: Power submenu: Sleep Now, Power Off, set timeouts/interval

## Battery Monitoring Strategy

### MAX17048 Fuel Gauge (Built-in)
From `docs/pinout-reference.md`: **Battery Monitor MAX17048 (I2C 0x36)**

Advantages:
- Hardware fuel gauge with accurate SoC (State of Charge)
- Built-in voltage, current, and capacity tracking
- No external components needed (built into ESP32-S3 Feather)
- Temperature compensation and aging compensation
- Eliminates need for voltage dividers or ADC calibration

Implementation:
```cpp
// In PowerManager.cpp
#include <Wire.h>
// Use MAX17048 library or direct I2C commands
```

## Deep Sleep Wake and Splash Skip

### Current Boot Flow (Normal Power-On)
Looking at `AlertTX-1.ino`, the current boot sequence is:
1. Hardware initialization (backlight, TFT power, display init)
2. Settings Manager and Theme initialization  
3. Component framework setup (ScreenManager, screens, InputRouter)
4. **Push SplashScreen** - shows "SENTRY" logo for 2 seconds
5. SplashScreen auto-transitions to MainMenuScreen after timeout or button press

### Wake-from-Sleep Flow (Modified Boot)
When waking from deep sleep, we want to bypass the branding splash and get straight to functionality:

**Modified Boot Sequence:**
```cpp
void setup() {
    // ... existing hardware init (steps 1-6) ...
    
    // STEP 7: Initialize Power Manager FIRST
    PowerManager::begin(&tft);
    
    // STEP 8: Check wake cause and determine initial screen
    bool skipSplash = PowerManager::onWake();  // Returns true if waking from deep sleep
    
    // ... existing component framework init (steps 8-9) ...
    
    // STEP 10: Choose initial screen based on wake cause
    if (skipSplash) {
        Serial.println("Wake from deep sleep detected - skipping splash");
        
        // Check if we have new messages/alerts (future feature)
        bool hasNewMessages = PowerManager::hasNewMessagesOnWake();
        
        if (hasNewMessages) {
            // Navigate directly to Messages/Alerts screen
            Serial.println("New messages detected - going to Messages screen");
            // messagesScreen = new MessagesScreen(&tft);  // Future
            // screenManager->pushScreen(messagesScreen);
            screenManager->pushScreen(mainMenuScreen);  // Fallback for now
        } else {
            // Normal wake - go straight to main menu
            Serial.println("No new messages - going to Main Menu");
            screenManager->pushScreen(mainMenuScreen);
        }
    } else {
        // Normal cold boot - show splash screen
        Serial.println("Cold boot detected - showing splash screen");
        screenManager->pushScreen(splashScreen);
    }
}
```

### Wake Source Detection
The `PowerManager::onWake()` function determines how the device started:

```cpp
bool PowerManager::onWake() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wake: Timer (periodic MQTT check)");
            lastWakeWasFromSleep = true;
            handlePeriodicWakeBackground();  // Check messages, SCREEN STAYS OFF
            return true;  // Skip splash (if new messages found)
            
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wake: Button A pressed");
            lastWakeWasFromSleep = true;
            return true;  // Skip splash - user wants immediate access
            
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wake: Button B or C pressed");
            lastWakeWasFromSleep = true;
            return true;  // Skip splash - user wants immediate access
            
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            Serial.println("Wake: Cold boot (power on/reset)");
            lastWakeWasFromSleep = false;
            return false;  // Show splash - first boot experience
    }
}
```

### User Experience Impact

**Cold Boot (Power On/Reset):**
- Shows "SENTRY" branding splash for 2 seconds
- Professional, polished first impression
- Time to mentally context-switch to "beeper mode"

**Wake from Deep Sleep:**
- **Immediate functionality** - no delay, no branding
- Responsive feel - press button, get menu instantly
- Preserves "always-on" device illusion
- Important for emergency/urgent alert scenarios

**Timer Wake (Background):**
- Device wakes every 60s to check for messages
- **Screen stays OFF** during background wake (backlight remains LOW)
- If no new messages: immediately return to deep sleep (user completely unaware)
- If new messages: turn screen ON, navigate to Messages screen (future)
- User experience: device "magically" wakes when there's something important

### Technical Implementation Details

**Splash Screen Lifecycle Impact:**
- SplashScreen constructor and enter() logic remains unchanged
- Only the initial screen selection in `setup()` changes
- SplashScreen can still be manually accessed later if needed

**Memory Efficiency:**
- On sleep wake, SplashScreen object may not be created at all
- Saves ~1KB of RAM and faster boot time
- More important when waking frequently (every 60s)

**State Preservation:**
- Deep sleep loses all RAM contents
- Settings/preferences preserved in NVS
- Theme selection stored in NVS via SettingsManager (see THEME_SELECTION_SYSTEM.md)
- WiFi credentials injected at build-time via generated_secrets.h
- Fresh app state each wake (like closing/reopening apps)

### Future Message Handling
When MQTT message functionality is implemented:

```cpp
void PowerManager::handlePeriodicWakeBackground() {
    // CRITICAL: Keep screen OFF during background wake
    // Do NOT call setBacklight(true) here
    
    // Reconnect WiFi if needed (no visual feedback)
    if (!WiFi.isConnected()) {
        WiFi.begin(/* credentials from generated_secrets.h */);
        // Wait for connection with timeout (5-10 seconds max)
    }
    
    // Check MQTT for new messages since last wake
    bool hasNewMessages = MQTTClient::hasUnreadMessages();
    
    if (hasNewMessages) {
        // Turn screen ON only if new messages found
        setBacklight(true);
        hasNewMessagesOnWake = true;
        // setup() will navigate to Messages screen
    } else {
        // No new messages - go back to sleep immediately
        // Screen never turned on, user unaware of wake
        enterDeepSleep();
    }
}
```

This creates a smart wake behavior:
- **Button wake**: Straight to main menu (user-initiated)
- **Timer wake + new messages**: Straight to Messages screen (urgent)
- **Timer wake + no messages**: Return to deep sleep (invisible to user)
- **Cold boot**: Show splash (branding/professional experience)

**Wake Source Configuration:**
```cpp
void configureSleepWakeSources() {
    // Button A: GPIO0, pulled HIGH, goes LOW when pressed
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // Wake on LOW
    
    // Buttons B/C: GPIO1/2, pulled LOW, go HIGH when pressed  
    uint64_t ext1_mask = (1ULL << GPIO_NUM_1) | (1ULL << GPIO_NUM_2);
    esp_sleep_enable_ext1_wakeup(ext1_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
    
    // Timer wake for periodic MQTT checks
    esp_sleep_enable_timer_wakeup(deepSleepIntervalMs * 1000ULL);
}
```

## Implementation Steps

### 1. Create PowerManager Module
```cpp
// src/power/PowerManager.h
#pragma once
#include <Arduino.h>
#include "esp_sleep.h"

class PowerManager {
public:
    enum PowerState {
        ACTIVE,
        IDLE_DIM,
        DEEP_SLEEP_CYCLE
    };
    
    static void begin();
    static void onWake();
    static void update(unsigned long nowMs);
    static void notifyActivity();
    static void requestSleepNow();
    static void requestPowerOff();
    static float getBatteryVoltage();
    static int getBatteryPercent();
    static PowerState getCurrentState();
    
private:
    static PowerState currentState;
    static unsigned long lastActivityMs;

    static float batteryVoltage;
    static int batteryPercent;
    
    static void initMAX17048();
    static void updateBattery();
    static void setBacklight(bool enabled);
    static void enterDeepSleep();
    static void configureSleepWakeSources();
};
```

### 2. Wire into Main Loop
**AlertTX-1.ino changes:**
```cpp
#include "src/power/PowerManager.h"

void setup() {
    // ... existing setup ...
    
    PowerManager::begin();
    PowerManager::onWake();  // Handle wake logic before splash
    
    // ... rest of setup ...
}

void loop() {
    inputRouter->update();
    PowerManager::update(millis());  // Add power management
    screenManager->update();
    screenManager->draw();
    
    // ... existing loop ...
}
```

**InputRouter.h integration:**
```cpp
// In InputRouter::update()
void InputRouter::update() {
    // ... existing button handling ...
    
    // Notify power manager of any activity
    if (buttons->wasPressed(ButtonManager::BUTTON_A) ||
        buttons->wasPressed(ButtonManager::BUTTON_B) ||
        buttons->wasPressed(ButtonManager::BUTTON_C) ||
        buttons->wasReleased(ButtonManager::BUTTON_A) ||
        buttons->wasReleased(ButtonManager::BUTTON_B) ||
        buttons->wasReleased(ButtonManager::BUTTON_C)) {
        PowerManager::notifyActivity();
    }
    
    // ... rest of existing logic ...
}
```

### 3. Update SystemInfoScreen
**Replace battery placeholder in `src/ui/screens/SystemInfoScreen.cpp`:**
```cpp
#include "../../power/PowerManager.h"

void SystemInfoScreen::refreshMetrics() {
    // Replace placeholder with PowerManager calls
    batteryVoltage = PowerManager::getBatteryVoltage();
    batteryPercent = PowerManager::getBatteryPercent();
}

// Remove local readBatteryVoltage() and estimateBatteryPercent() methods
```

### 4. Settings Configuration
**SettingsManager.h additions:**
```cpp
// Add to SettingsManager class
static uint32_t getInactivityTimeoutMs();
static void setInactivityTimeoutMs(uint32_t timeoutMs);
static uint32_t getDimGraceMs();
static void setDimGraceMs(uint32_t graceMs);
static uint32_t getDeepSleepIntervalMs();
static void setDeepSleepIntervalMs(uint32_t intervalMs);
```

**SettingsScreen.cpp additions:**
```cpp
// Add "Power" menu item in constructor
menuContainer->addMenuItem(new MenuItem("Power", [this]() { onPowerSelected(); }));

void SettingsScreen::onPowerSelected() {
    // Navigate to PowerSettingsScreen (new screen)
}
```

### 5. MQTT Integration Placeholder
**In PowerManager wake cycle:**
```cpp
void PowerManager::handlePeriodicWake() {
    // TODO: Reconnect WiFi if disconnected
    // WiFi.begin(ssid, password);
    
    // TODO: Connect to MQTT and check for new messages
    // MQTTClient::checkForNewMessages();
    
    // Placeholder: assume no new messages for now
    bool hasNewMessages = false;
    
    if (hasNewMessages) {
        // TODO: Navigate to Messages screen when implemented
        // ScreenManager::switchToScreen(new MessagesScreen());
    } else {
        // Go back to deep sleep
        enterDeepSleep();
    }
}
```

## Hardware Implementation Notes

**From pinout-reference.md:**
- **Button A (GPIO0)**: Pulled HIGH, goes LOW when pressed → `esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0)`
- **Button B (GPIO1)**: Pulled LOW, goes HIGH when pressed → ext1 wake ANY_HIGH
- **Button C (GPIO2)**: Pulled LOW, goes HIGH when pressed → ext1 wake ANY_HIGH  
- **TFT Backlight (GPIO45)**: `digitalWrite(TFT_BACKLITE, LOW/HIGH)` for power control
- **MAX17048**: I2C address 0x36, SDA=GPIO3, SCL=GPIO4 (built into board)

**Power Consumption Targets:**
- Active: ~50-200mA (WiFi on, display on)
- IdleDim: ~30-100mA (WiFi on, display off)
- DeepSleep: <1mA (only RTC and wake circuits active)

---

This plan leverages the MAX17048 fuel gauge for accurate battery monitoring, implements a robust power state machine with user-configurable timeouts, and integrates cleanly with the existing UI framework while minimizing code changes to other modules.
