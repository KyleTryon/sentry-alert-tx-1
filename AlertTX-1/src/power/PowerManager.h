#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "esp_sleep.h"

class PowerManager {
public:
    enum PowerState {
        ACTIVE,
        IDLE_DIM,
        DEEP_SLEEP_CYCLE
    };

    // Lifecycle
    static void begin();
    // Returns true if we should skip splash (wake from deep sleep or user button wake)
    static bool onWake();
    static void update(unsigned long nowMs);

    // Activity notifications
    static void notifyActivity();

    // User actions
    static void requestSleepNow();
    static void requestPowerOff();

    // Battery metrics
    static float getBatteryVoltage();
    static int getBatteryPercent();

    // State
    static PowerState getCurrentState();

    // Wake result helpers
    static bool lastWakeWasFromSleep();
    static bool hasNewMessagesOnWake();

private:
    // State
    static volatile PowerState currentState;
    static volatile unsigned long lastActivityMs;
    static volatile bool backlightEnabled;

    // Battery
    static float batteryVoltage;
    static int batteryPercent;
    static float voltageEMA;
    static constexpr float EMA_ALPHA = 0.2f;

    // Wake flags
    static bool s_lastWakeWasFromSleep;
    static bool s_hasNewMessagesOnWake;

    // Internals
    static void initMAX17048();
    static void updateBattery();

    static void setBacklight(bool enabled);
    static void configureSleepWakeSources(bool enableTimerWake);
    static void enterDeepSleep();

    // MAX17048 helpers
    static bool readRegister16(uint8_t reg, uint16_t &out);
    static float readMAX17048Voltage();
    static int readMAX17048SOC();

    // Background periodic wake flow
    static void handlePeriodicWakeBackground();
};