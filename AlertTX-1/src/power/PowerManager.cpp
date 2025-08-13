#include "PowerManager.h"
#include <WiFi.h>
#include "../config/SettingsManager.h"

// Some cores use TFT_BACKLIGHT, others expose TFT_BACKLITE. Prefer TFT_BACKLIGHT if defined.
#if defined(TFT_BACKLIGHT)
  #define ALERTTX_BACKLIGHT_PIN TFT_BACKLIGHT
#elif defined(TFT_BACKLITE)
  #define ALERTTX_BACKLIGHT_PIN TFT_BACKLITE
#else
  // Fallback for ESP32-S3 Reverse TFT Feather
  #define ALERTTX_BACKLIGHT_PIN 45
#endif

// Defaults until configurable via SettingsManager
static const uint32_t DEFAULT_INACTIVITY_TIMEOUT_MS = 10000;  // 10s before dim
static const uint32_t DEFAULT_DIM_GRACE_MS = 2000;            // 2s before sleep
static const uint32_t DEFAULT_DEEP_SLEEP_INTERVAL_MS = 60000; // 60s periodic wake

// Static members
volatile PowerManager::PowerState PowerManager::currentState = PowerManager::ACTIVE;
volatile unsigned long PowerManager::lastActivityMs = 0;
volatile bool PowerManager::backlightEnabled = true;

float PowerManager::batteryVoltage = 0.0f;
int PowerManager::batteryPercent = 0;
float PowerManager::voltageEMA = 0.0f;

bool PowerManager::s_lastWakeWasFromSleep = false;
bool PowerManager::s_hasNewMessagesOnWake = false;

void PowerManager::begin() {
    pinMode(ALERTTX_BACKLIGHT_PIN, OUTPUT);
    setBacklight(true);

    Wire.begin();
    initMAX17048();

    lastActivityMs = millis();
    currentState = ACTIVE;
}

bool PowerManager::onWake() {
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wake: Timer (periodic MQTT check)");
            s_lastWakeWasFromSleep = true;
            // Keep screen OFF until we know there's work
            s_hasNewMessagesOnWake = false;
            handlePeriodicWakeBackground();
            // Skip splash if we are going to show something; otherwise we will go back to sleep
            return s_hasNewMessagesOnWake;
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wake: Button A pressed");
            s_lastWakeWasFromSleep = true;
            return true;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Wake: Button B or C pressed");
            s_lastWakeWasFromSleep = true;
            return true;
        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            Serial.println("Wake: Cold boot (power on/reset)");
            s_lastWakeWasFromSleep = false;
            return false;
    }
}

void PowerManager::update(unsigned long nowMs) {
    updateBattery();

    // Simple state machine driven by inactivity
    uint32_t inactivityTimeoutMs = SettingsManager::getInactivityTimeoutMs();
    uint32_t dimGraceMs = SettingsManager::getDimGraceMs();

    switch (currentState) {
        case ACTIVE: {
            if (nowMs - lastActivityMs >= inactivityTimeoutMs) {
                // Dim/off backlight
                setBacklight(false);
                currentState = IDLE_DIM;
            }
            break;
        }
        case IDLE_DIM: {
            if (nowMs - lastActivityMs >= inactivityTimeoutMs + dimGraceMs) {
                currentState = DEEP_SLEEP_CYCLE;
                // Configure timer + button wake and sleep
                configureSleepWakeSources(true);
                enterDeepSleep();
            }
            break;
        }
        case DEEP_SLEEP_CYCLE: {
            // Should not run; we go to deep sleep immediately
            break;
        }
    }
}

void PowerManager::notifyActivity() {
    lastActivityMs = millis();
    if (!backlightEnabled) {
        setBacklight(true);
    }
    if (currentState != ACTIVE) {
        currentState = ACTIVE;
    }
}

void PowerManager::requestSleepNow() {
    Serial.println("PowerManager: Sleep Now requested");
    configureSleepWakeSources(true);
    enterDeepSleep();
}

void PowerManager::requestPowerOff() {
    Serial.println("PowerManager: Power Off requested (button wake only)");
    configureSleepWakeSources(false);
    enterDeepSleep();
}

float PowerManager::getBatteryVoltage() { return batteryVoltage; }
int PowerManager::getBatteryPercent() { return batteryPercent; }
PowerManager::PowerState PowerManager::getCurrentState() { return currentState; }
bool PowerManager::lastWakeWasFromSleep() { return s_lastWakeWasFromSleep; }
bool PowerManager::hasNewMessagesOnWake() { return s_hasNewMessagesOnWake; }

void PowerManager::setBacklight(bool enabled) {
    digitalWrite(ALERTTX_BACKLIGHT_PIN, enabled ? HIGH : LOW);
    backlightEnabled = enabled;
}

void PowerManager::configureSleepWakeSources(bool enableTimerWake) {
    // Button A: GPIO0 - wake on LOW
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);

    // Buttons B/C: GPIO1/2 - ANY_HIGH
    uint64_t ext1_mask = (1ULL << GPIO_NUM_1) | (1ULL << GPIO_NUM_2);
    esp_sleep_enable_ext1_wakeup(ext1_mask, ESP_EXT1_WAKEUP_ANY_HIGH);

    if (enableTimerWake) {
        esp_sleep_enable_timer_wakeup((uint64_t)SettingsManager::getDeepSleepIntervalMs() * 1000ULL);
    }
}

void PowerManager::enterDeepSleep() {
    // Ensure backlight off before sleeping
    setBacklight(false);
    Serial.println("PowerManager: Entering deep sleep...");
    delay(50);
    esp_deep_sleep_start();
}

void PowerManager::initMAX17048() {
    // MAX17048 typically works out of the box; no special init required for basic reads.
    // Could verify communication by reading a known register (Version or Config) later.
}

bool PowerManager::readRegister16(uint8_t reg, uint16_t &out) {
    Wire.beginTransmission(0x36);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }
    if (Wire.requestFrom(0x36, (uint8_t)2) != 2) {
        return false;
    }
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    out = ((uint16_t)msb << 8) | lsb;
    return true;
}

float PowerManager::readMAX17048Voltage() {
    uint16_t raw;
    if (!readRegister16(0x02, raw)) {
        return batteryVoltage > 0.0f ? batteryVoltage : 3.90f;
    }
    // VCELL: 12-bit fixed point 14,2? Datasheet: VCELL LSB = 78.125 uV
    // Voltage (V) = raw * 78.125e-6
    float v = (float)raw * 78.125f / 1000000.0f;
    return v;
}

int PowerManager::readMAX17048SOC() {
    uint16_t raw;
    if (!readRegister16(0x04, raw)) {
        return batteryPercent > 0 ? batteryPercent : 50;
    }
    // SOC register: value/256 = percent
    int percent = (int)((raw / 256.0f) + 0.5f);
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;
    return percent;
}

void PowerManager::updateBattery() {
    float newVoltage = readMAX17048Voltage();
    int newPercent = readMAX17048SOC();

    if (voltageEMA <= 0.0f) {
        voltageEMA = newVoltage;
    } else {
        voltageEMA = (EMA_ALPHA * newVoltage) + ((1.0f - EMA_ALPHA) * voltageEMA);
    }

    batteryVoltage = voltageEMA;
    batteryPercent = newPercent;
}

void PowerManager::handlePeriodicWakeBackground() {
    // Keep screen OFF; do minimal work
    // Attempt brief WiFi connect if not already connected
    if (WiFi.status() != WL_CONNECTED) {
        // Credentials expected via generated_secrets / SettingsManager; skip for now
        // Keep a tight timeout to save power (placeholder)
        // In current phase, we do not attempt connection.
    }

    // Placeholder: no MQTT check yet
    bool hasNewMessages = false;

    if (hasNewMessages) {
        setBacklight(true);
        s_hasNewMessagesOnWake = true;
    } else {
        // No work; go right back to sleep
        configureSleepWakeSources(true);
        enterDeepSleep();
    }
}