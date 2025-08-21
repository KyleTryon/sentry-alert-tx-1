# Power Management Plan – Alert TX-1

## Goals
- Real battery reading with MAX17048 fuel gauge and smoothing
- Auto-dim/backlight-off on inactivity; deep sleep after grace period
- Periodic deep-sleep wake to poll for notifications; instant wake on button
- Settings to configure timeouts and a "Sleep now / Power off" option
- Skip splash on deep-sleep wake; preserve UX consistency

## Status Summary (Delta)
- MQTT client is now non-blocking and safe if offline. It retries opportunistically without blocking the UI or boot.
- Alerts flow is compatible with deep-sleep strategy. Background periodic wakes can reuse non-blocking MQTT to check for new messages.
- Ringtone should be triggered on a foreground wake when new messages are presented to the user (see Wake Behavior and Ringtone below).

## Hardware References
- **Pinout**: [Pinout Reference](../setup/pinout-reference.md) - Battery Monitor MAX17048 (I2C 0x36), VBAT, wake sources, TFT_BACKLITE (GPIO45)
- **Hardware Setup**: [Hardware Setup](../setup/hardware-setup.md) - Display power, pin assignments, LiPo 400mAh specs

## Architecture Overview
Introduce `PowerManager` to orchestrate power states. Integrate with `InputRouter` for inactivity tracking and with `ScreenManager` for skip-splash behavior on wake.

### New Module: `src/power/PowerManager.{h,cpp}`
**Responsibilities:**
- `begin()`: Initialize MAX17048, display power domain (TFT power GPIO), wake sources
- `onWake()`: Run at boot to handle splash skip and state carryover
- `update(nowMs)`: State machine transitions: Active → IdleDim → DeepSleepCycle
- `requestSleepNow()` / `requestPowerOff()`: Invoked from Settings to enter deep sleep
- `getBatteryVoltage()`, `getBatteryPercent()`: MAX17048-based readings with EMA smoothing
- Config getters read from `SettingsManager`

**Power State Machine:**
- **Active**
  - `InputRouter` calls `PowerManager::notifyActivity()` on any input
  - If `now - lastActivityMs >= inactivityTimeoutMs` → IdleDim (dim/off backlight)
  - If additionally `>= inactivityTimeoutMs + dimGraceMs` → DeepSleepCycle
- **IdleDim**
  - Display panel off: `PowerManager` must disable backlight AND avoid any GFX draws
  - Any input → restore panel power/backlight, return to Active
  - If grace period exceeded → DeepSleepCycle
- **DeepSleepCycle**
  - Configure timer wake: `esp_sleep_enable_timer_wakeup(sleepIntervalMs * 1000ULL)`
  - Configure button wake: ext0 (Button A, LOW) + ext1 (Button B/C, ANY_HIGH)
  - Enter `esp_deep_sleep_start()`
  - **Strict display-off rule**: During background wake checks, do not touch the display (no Adafruit_GFX calls, keep panel/backlight power off) unless we are transitioning to foreground due to a new alert

**Display/Panel Power Control:**
- Do not only turn the backlight off — ensure the entire panel is quiescent during sleep/background checks:
  - Backlight off: `digitalWrite(TFT_BACKLITE, LOW)`
  - Optionally disable TFT/IO power rail if available (e.g., `TFT_I2C_POWER`), but only after the panel is in a safe state
  - Absolutely avoid any GFX calls while sleeping/background wake to prevent re-enabling the bus implicitly

### Input Router Integration – `src/ui/core/InputRouter.h`
- Already centralizes all input handling
- **Add**: Call `PowerManager::notifyActivity()` on any press/release/long-press
- Preserve existing A/B auto-repeat and long-press back behavior

### Screen Manager Integration – `src/ui/core/ScreenManager.*`
- Clear screen before entering any screen (already implemented)
- **Add**: Skip splash on deep-sleep wake - `PowerManager::onWake()` determines initial screen

### Settings Integration – `src/config/SettingsManager.*`
**Add config keys with defaults:**
- `inactivityTimeoutMs` (default: 10000) - Time before dimming backlight
- `dimGraceMs` (default: 2000) - Grace period before deep sleep
- `deepSleepIntervalMs` (default: 60000) - Periodic wake interval
- `backlightDimMode` (default: "off") - Future: "off" | "dim"
- `backlightDimLevel` (default: 50) - Future: PWM level 0-100%

### Settings UI – `src/ui/screens/SettingsScreen.*`
**Add "Power" submenu with options:**
- "Sleep Now" → `PowerManager::requestSleepNow()` (timer wake enabled)
- "Power Off" → `PowerManager::requestPowerOff()` (no timer wake, button wake only)
- "Inactivity Timeout" → Off/10s/30s/60s
- "Sleep Check Interval" → 30s/60s/5m
- "Backlight Dim" → Off/On (future)
- "Dim Level" → 10-100% (future)

## Battery Monitoring Strategy

### MAX17048 Fuel Gauge (Built-in)
From `docs/pinout-reference.md`: **Battery Monitor MAX17048 (I2C 0x36)**

**Advantages:**
- Hardware fuel gauge with accurate SoC (State of Charge)
- Built-in voltage, current, and capacity tracking
- No external components needed (built into ESP32-S3 Feather)
- Temperature compensation and aging compensation
- Eliminates need for voltage dividers or ADC calibration

**Implementation:**
```cpp
// In PowerManager.cpp
#include <Wire.h>
// Use MAX17048 library or direct I2C commands

class PowerManager {
private:
    static float batteryVoltage;
    static int batteryPercent;
    static float voltageEMA;  // Exponential moving average for smoothing
    static const float EMA_ALPHA = 0.2f;
    
public:
    static void initMAX17048() {
        Wire.begin();
        // Initialize MAX17048 on I2C address 0x36
        // Configure alert thresholds, sleep mode, etc.
        // Set configuration for LiPo battery profile
    }
    
    static void updateBattery() {
        // Read from MAX17048 registers
        float newVoltage = readMAX17048Voltage();
        int newPercent = readMAX17048SOC();
        
        // Apply EMA smoothing to voltage for display stability
        voltageEMA = (EMA_ALPHA * newVoltage) + ((1.0f - EMA_ALPHA) * voltageEMA);
        
        batteryVoltage = voltageEMA;
        batteryPercent = newPercent; // SOC already accurate from fuel gauge
    }
    
private:
    static float readMAX17048Voltage() {
        // Read VCELL register (0x02) - 16-bit value
        // Formula: Voltage = (register_value * 78.125) / 1000000 V
    }
    
    static int readMAX17048SOC() {
        // Read SOC register (0x04) - 16-bit value  
        // Formula: SOC = register_value / 256 %
    }
};
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
        
        // Check if we have new messages/alerts using non-blocking MQTT
        bool hasNewMessages = PowerManager::hasNewMessagesOnWake();
        
        if (hasNewMessages) {
            // IMPORTANT: Only now initialize/restore display power and draw
            PowerManager::enableDisplayPanel();
            // Navigate directly to Alerts screen (or Messages screen when implemented)
            screenManager->pushScreen(alertsScreen /* or messagesScreen */);
            // Trigger ringtone on foreground wake
            ringtonePlayer.playRingtoneByIndex(SettingsManager::getRingtoneIndex());
        } else {
            // Normal wake - go straight to main menu without touching the display during background checks
            screenManager->pushScreen(mainMenuScreen);
        }
    } else {
        // Normal cold boot - show splash screen
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
            // Background check path: do NOT initialize the display, keep panel/backlight OFF
            lastWakeWasFromSleep = true;
            handlePeriodicWakeBackground();  // Non-blocking MQTT check, screen stays off
            return true;  // Skip splash (if new messages found) but allow logic above to decide
            
        case ESP_SLEEP_WAKEUP_EXT0: // Button A
        case ESP_SLEEP_WAKEUP_EXT1: // Button B/C
            lastWakeWasFromSleep = true;
            return true;  // Skip splash - user wants immediate access
            
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            lastWakeWasFromSleep = false;
            return false;  // Show splash - first boot experience
    }
}
```

### Wake Behavior and Ringtone
- During timer-based background wake: keep display completely off (no power, no draws). Use non-blocking MQTT to check for new alerts. If none, go back to deep sleep immediately.
- If new alerts are found: transition to foreground wake by enabling the display panel and backlight, navigate to the Alerts view, and immediately trigger the ringtone using the user-selected index. LED sync will blink with notes when active.

### User Experience Impact
- **Cold Boot**: Shows splash for 2s (branding). 
- **Wake from Deep Sleep**: Immediate functionality; splash skipped.
- **Timer Wake (Background)**: Invisible when no new alerts; foreground wake + ringtone when there is something to show.

## Technical Implementation Details

**Splash Screen Lifecycle Impact:**
- SplashScreen constructor/enter unchanged; only initial selection differs.

**Memory Efficiency:**
- On sleep wake, SplashScreen may not be created; saves RAM and time.

**State Preservation:**
- Deep sleep clears RAM; settings persist in NVS. Theme and ringtone index are persisted.

### Future Message Handling – Background Check (Non-Blocking)
```cpp
void PowerManager::handlePeriodicWakeBackground() {
    // Screen OFF during background wake
    // Reconnect WiFi if needed (non-blocking) and pump MQTT client update()
    // Example: use existing non-blocking MQTTClient::update() loops
    bool hasNewMessages = MQTTClient::peekForNewMessagesNonBlocking(); // pseudo
    if (!hasNewMessages) {
        enterDeepSleep();
        return;
    }
    // Messages exist: foreground wake will init display, navigate, and play ringtone
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
    enum PowerState { ACTIVE, IDLE_DIM, DEEP_SLEEP_CYCLE };
    static void begin();
    static bool onWake();
    static void update(unsigned long nowMs);
    static void notifyActivity();
    static void requestSleepNow();
    static void requestPowerOff();
    static float getBatteryVoltage();
    static int getBatteryPercent();
    static PowerState getCurrentState();

    // Display/power helpers
    static void enableDisplayPanel();
    static void disableDisplayPanel();
    static bool hasNewMessagesOnWake();

private:
    static PowerState currentState;
    static unsigned long lastActivityMs;
    static void enterDeepSleep();
};
```

### 2. Wire into Main Loop
**AlertTX-1.ino changes:**
```cpp
#include "src/power/PowerManager.h"

void setup() {
    // ... existing setup ...
    PowerManager::begin();
    bool skipSplash = PowerManager::onWake();
    // select initial screen per logic above
}

void loop() {
    inputRouter->update();
    PowerManager::update(millis());
    screenManager->update();
    screenManager->draw();
}
```

**InputRouter.h integration:**
```cpp
// In InputRouter::update()
if (buttons->wasPressed(...) || buttons->wasReleased(...)) {
    PowerManager::notifyActivity();
}
```

### 3. Update SystemInfoScreen
Use `PowerManager` for battery readings.

### 4. Settings Configuration
Add setters/getters for timeouts.

### 5. MQTT Integration Placeholder
Use the existing non-blocking MQTT client to check messages in background wakes safely with the display OFF.

---

This update clarifies: (1) MQTT is non-blocking and compatible with sleep/wake. (2) The display must remain completely untouched during sleep/background wake (not just backlight off). (3) When new messages are detected on a periodic wake, the device should fully wake the UI and play the selected ringtone as part of the foreground transition.

## Refinements and Safeguards (Required)

### Display Gating and ScreenManager
- Add `PowerManager::isDisplayEnabled()`.
- Add `PowerManager::enableDisplayPanel()` and `disableDisplayPanel()` as the only API to change panel state.
- Modify `ScreenManager::draw()` to early-return if `!PowerManager::isDisplayEnabled()` so no Adafruit_GFX calls occur while the panel is off.
- Call `disableDisplayPanel()` upon entering IdleDim and before DeepSleep; only call `enableDisplayPanel()` on foreground wake.

### Alerts Ingestion and Ringtone Policy
- Background (timer) ingestion MUST pass `playTone = false` to `AlertsScreen::addMessage(...)`.
- On foreground wake due to new messages, after `enableDisplayPanel()`, navigate to alerts and call `ringtonePlayer.playRingtoneByIndex(SettingsManager::getRingtoneIndex())` exactly once.

### RTC vs NVS for Wake Context
- Use RTC memory for volatile flags (e.g., `lastWakeWasFromSleep`, `hasNewMessagesPending`).
- Persist last-seen message id and MQTT session hints in NVS to quickly determine “new content” on wake without rendering.
- Background wake flow: compare retained/last-seen id → set `hasNewMessagesPending` (RTC) if newer → go foreground path; else return to sleep.

### Wake Initialization: Audio and Timing
- On foreground wake, reinitialize audio timing sources to avoid drift after `millis()` reset:
  - Ensure `ringtonePlayer.begin(BUZZER_PIN)` has been called since boot.
  - Ensure LED state is reset (`statusLed.off()`).
  - Trigger ringtone only in foreground after display is enabled.

### Boot Director and Screen Lifecycle
- Introduce a lightweight "boot director" in `setup()` that:
  - Computes initial screen per wake cause and background checks.
  - Ensures any required screens (e.g., `AlertsScreen`) are constructed before `pushScreen(...)` (do not rely on `MainMenuScreen::initializeScreens()` in skip‑splash path).
  - Defers pushing any screen until after enabling the panel on foreground wake.

### Sleep Entry Checklist (Race-Free)
Before calling `esp_deep_sleep_start()`:
- Stop/pause audio: `ringtonePlayer.stop()`.
- Ensure `statusLed.off()`.
- Ensure MQTT client is not in the middle of a publish; if needed, skip/flush.
- Disable display panel: `PowerManager::disableDisplayPanel()` and avoid further draws.

### Ring-Once UX on Wake
- Aggregate multiple new messages during sleep; play ringtone once when entering foreground.
- Consider a small visual badge (e.g., "3 new") and optional LED blink pattern instead of multiple consecutive ringtones.

### Adaptive and Non-Blocking Connectivity (Recap)
- If SSID/broker/clientId are empty → skip connect attempts.
- Use non-blocking Wi‑Fi/MQTT update loops with backoff and strict attempt caps during background wake.
- Consider QoS ≥ 1 or retained “last-message-id” pointer topic to improve reliability while sleeping.
