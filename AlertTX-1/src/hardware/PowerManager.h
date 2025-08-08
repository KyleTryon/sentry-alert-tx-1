#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>

// Power states for the AlertTX-1 - focused on actual power consumption
enum class PowerState {
    DEEP_SLEEP,     // Lowest power, wake on button press only
    SLEEP,          // Reduced power, quick wake
    ACTIVE,         // Full functionality, normal operation
    POLLING         // Sleep but wake periodically to check for alerts
};

// Battery level indicators
enum class BatteryLevel {
    CRITICAL,       // < 10%
    LOW,           // 10-25%
    MEDIUM,        // 25-75%
    HIGH,          // 75-100%
    CHARGING       // USB connected
};

// Power management configuration
struct PowerConfig {
    unsigned long inactivityTimeout = 60000;      // 60 seconds before sleep
    unsigned long deepSleepTimeout = 300000;      // 5 minutes before deep sleep
    unsigned long pollingInterval = 30000;        // 30 seconds between alert checks
    uint8_t sleepBrightness = 50;                 // Display brightness in sleep
    uint8_t activeBrightness = 100;               // Display brightness when active
    bool enableWiFiInPolling = true;              // Keep WiFi on in polling mode
    bool enableMQTTInPolling = true;              // Keep MQTT connected in polling
};

class PowerManager {
private:
    PowerState currentState = PowerState::DEEP_SLEEP;
    PowerState previousState = PowerState::DEEP_SLEEP;
    unsigned long lastActivityTime = 0;
    unsigned long stateStartTime = 0;
    unsigned long lastBatteryCheck = 0;
    unsigned long lastPollingCheck = 0;
    unsigned long batteryCheckInterval = 30000;   // Check battery every 30 seconds
    
    PowerConfig config;
    
    // Battery monitoring
    float batteryVoltage = 0.0f;
    BatteryLevel batteryLevel = BatteryLevel::HIGH;
    bool isCharging = false;
    
    // Wake sources
    bool wakeFromDeepSleep = false;
    esp_sleep_wakeup_cause_t wakeCause = ESP_SLEEP_WAKEUP_UNDEFINED;
    
    // Callback functions
    std::function<void(PowerState, PowerState)> stateChangeCallback = nullptr;
    std::function<void(BatteryLevel)> batteryLevelCallback = nullptr;
    std::function<void()> enterSleepCallback = nullptr;
    std::function<void()> wakeUpCallback = nullptr;
    std::function<void()> pollingCallback = nullptr;

public:
    PowerManager();
    ~PowerManager() = default;
    
    // Initialization
    void begin(const PowerConfig& config = PowerConfig{});
    void setupWakeSources();
    
    // Main update loop
    void update();
    
    // State management
    PowerState getCurrentState() const { return currentState; }
    PowerState getPreviousState() const { return previousState; }
    void setState(PowerState newState);
    bool isInSleepMode() const;
    bool isInDeepSleep() const;
    bool isInPollingMode() const;
    
    // Activity tracking
    void recordActivity();
    unsigned long getInactivityTime() const;
    void resetInactivityTimer();
    
    // Battery management
    float getBatteryVoltage() const { return batteryVoltage; }
    BatteryLevel getBatteryLevel() const { return batteryLevel; }
    bool isBatteryCharging() const { return isCharging; }
    void updateBatteryStatus();
    
    // Sleep management
    void enterSleepMode();
    void enterDeepSleep();
    void enterPollingMode();
    void wakeUp();
    void forceWakeUp();
    
    // Wake source handling
    esp_sleep_wakeup_cause_t getWakeCause() const { return wakeCause; }
    bool wasWakeFromDeepSleep() const { return wakeFromDeepSleep; }
    void clearWakeFlags();
    
    // Configuration
    void setConfig(const PowerConfig& config);
    PowerConfig getConfig() const { return config; }
    
    // Callbacks
    void setStateChangeCallback(std::function<void(PowerState, PowerState)> callback);
    void setBatteryLevelCallback(std::function<void(BatteryLevel)> callback);
    void setEnterSleepCallback(std::function<void()> callback);
    void setWakeUpCallback(std::function<void()> callback);
    void setPollingCallback(std::function<void()> callback);
    
    // Utility functions
    void printStatus() const;
    const char* getStateString(PowerState state) const;
    const char* getBatteryLevelString(BatteryLevel level) const;

private:
    // Internal state management
    void handleStateTransition(PowerState newState);
    void checkStateTransitions();
    void checkInactivityTimeout();
    void checkBatteryTimeout();
    void checkPollingTimeout();
    
    // Hardware-specific power management
    void configureDisplayPower(bool enable, uint8_t brightness);
    void configureWiFiPower(bool enable);
    void configureLEDPower(bool enable);
    void configureAudioPower(bool enable);
    
    // Battery voltage reading
    float readBatteryVoltage();
    BatteryLevel calculateBatteryLevel(float voltage);
    
    // Sleep configuration
    void configureDeepSleepWakeSources();
    void configureLightSleepWakeSources();
};

// Global power manager instance
extern PowerManager powerManager;

#endif // POWER_MANAGER_H 