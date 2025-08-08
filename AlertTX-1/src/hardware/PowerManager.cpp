#include "PowerManager.h"
#include "src/config/settings.h"
#include <WiFi.h>
#include <esp_adc_cal.h>

// Global power manager instance
PowerManager powerManager;

PowerManager::PowerManager() {
    // Initialize with default values
    currentState = PowerState::DEEP_SLEEP;
    previousState = PowerState::DEEP_SLEEP;
    lastActivityTime = millis();
    stateStartTime = millis();
    lastBatteryCheck = millis();
    lastPollingCheck = millis();
    
    // Initialize debouncers
    batteryVoltageDebouncer = AnalogDebounceUtility(500, 0.05f); // 500ms, 0.05V threshold
    chargingStateDebouncer = DebounceUtility(100); // 100ms debounce for charging detection
}

void PowerManager::begin(const PowerConfig& config) {
    this->config = config;
    
    // Check if we're waking from deep sleep
    wakeCause = esp_sleep_get_wakeup_cause();
    if (wakeCause != ESP_SLEEP_WAKEUP_UNDEFINED) {
        wakeFromDeepSleep = true;
        currentState = PowerState::ACTIVE;
        Serial.println("Woke from deep sleep");
    }
    
    // Initialize battery monitoring
    updateBatteryStatus();
    
    // Setup wake sources for future sleep cycles
    setupWakeSources();
    
    Serial.println("PowerManager initialized");
}

void PowerManager::setupWakeSources() {
    // Configure GPIO wake sources
    // Button A (D0) - GPIO0 - Built-in button
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // Wake on LOW (button press)
    
    // Buttons B and C (D1, D2) - GPIO1, GPIO2 - Built-in buttons
    // These are normally LOW, so we wake on HIGH (button press)
    esp_sleep_enable_ext1_wakeup(0x06, ESP_EXT1_WAKEUP_ANY_HIGH); // GPIO1 and GPIO2
    
    // Enable timer wake-up for polling mode
    esp_sleep_enable_timer_wakeup(config.pollingInterval * 1000ULL); // Convert to microseconds
    
    Serial.println("Wake sources configured");
}

void PowerManager::update() {
    unsigned long currentTime = millis();
    
    // Check battery status periodically
    if (currentTime - lastBatteryCheck >= batteryCheckInterval) {
        updateBatteryStatus();
        lastBatteryCheck = currentTime;
    }
    
    // Check for state transitions based on inactivity
    checkStateTransitions();
    
    // Check polling timeout if in polling mode
    if (currentState == PowerState::POLLING) {
        checkPollingTimeout();
    }
    
    // Handle wake from deep sleep
    if (wakeFromDeepSleep) {
        handleWakeFromDeepSleep();
        wakeFromDeepSleep = false;
    }
}

void PowerManager::setState(PowerState newState) {
    if (newState != currentState) {
        handleStateTransition(newState);
    }
}

void PowerManager::handleStateTransition(PowerState newState) {
    previousState = currentState;
    currentState = newState;
    stateStartTime = millis();
    
    Serial.printf("Power state transition: %s -> %s\n", 
                  getStateString(previousState), 
                  getStateString(currentState));
    
    // Configure hardware based on new state
    switch (currentState) {
        case PowerState::DEEP_SLEEP:
            configureDisplayPower(false, 0);
            configureWiFiPower(false);
            configureLEDPower(false);
            configureAudioPower(false);
            break;
            
        case PowerState::SLEEP:
            configureDisplayPower(true, config.sleepBrightness);
            configureWiFiPower(false);
            configureLEDPower(false);
            configureAudioPower(false);
            break;
            
        case PowerState::POLLING:
            configureDisplayPower(false, 0);
            configureWiFiPower(config.enableWiFiInPolling);
            configureLEDPower(false);
            configureAudioPower(false);
            break;
            
        case PowerState::ACTIVE:
            configureDisplayPower(true, config.activeBrightness);
            configureWiFiPower(true);
            configureLEDPower(true);
            configureAudioPower(true);
            break;
    }
    
    // Call state change callback if set
    if (stateChangeCallback) {
        stateChangeCallback(previousState, currentState);
    }
}

void PowerManager::checkStateTransitions() {
    unsigned long currentTime = millis();
    unsigned long inactivityTime = currentTime - lastActivityTime;
    
    // Only check transitions if we're not in deep sleep
    if (currentState == PowerState::DEEP_SLEEP) {
        return;
    }
    
    // Check for inactivity timeout
    if (inactivityTime >= config.inactivityTimeout) {
        if (currentState == PowerState::ACTIVE) {
            setState(PowerState::SLEEP);
        }
    }
    
    // Check for deep sleep timeout
    if (inactivityTime >= config.deepSleepTimeout) {
        if (currentState != PowerState::DEEP_SLEEP) {
            enterDeepSleep();
        }
    }
}

void PowerManager::checkPollingTimeout() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastPollingCheck >= config.pollingInterval) {
        lastPollingCheck = currentTime;
        
        Serial.println("Polling for alerts...");
        
        // Call polling callback to check for alerts
        if (pollingCallback) {
            pollingCallback();
        }
        
        // Return to sleep after polling
        enterSleepMode();
    }
}

void PowerManager::recordActivity() {
    lastActivityTime = millis();
    
    // Wake up from sleep modes on activity
    if (currentState == PowerState::SLEEP || currentState == PowerState::POLLING) {
        setState(PowerState::ACTIVE);
    }
}

unsigned long PowerManager::getInactivityTime() const {
    return millis() - lastActivityTime;
}

void PowerManager::resetInactivityTimer() {
    lastActivityTime = millis();
}

void PowerManager::updateBatteryStatus() {
    // Read raw battery voltage
    float rawVoltage = readBatteryVoltage();
    
    // Update debounced battery voltage
    if (batteryVoltageDebouncer.update(rawVoltage)) {
        batteryVoltage = batteryVoltageDebouncer.getValue();
        BatteryLevel newLevel = calculateBatteryLevel(batteryVoltage);
        
        // Only update battery level if it actually changed
        if (newLevel != batteryLevel) {
            batteryLevel = newLevel;
            
            // Call battery level callback if set
            if (batteryLevelCallback) {
                batteryLevelCallback(batteryLevel);
            }
            
            Serial.printf("Battery level changed to: %s (%.2fV)\n", 
                          getBatteryLevelString(batteryLevel), batteryVoltage);
        }
    }
    
    // Update charging state with debouncing
    bool rawChargingState = (analogRead(8) > 2000); // ADC reading on A5 pin
    if (chargingStateDebouncer.update(rawChargingState)) {
        bool newChargingState = chargingStateDebouncer.getState();
        if (newChargingState != isCharging) {
            isCharging = newChargingState;
            Serial.printf("Charging state changed: %s\n", isCharging ? "Charging" : "Not charging");
        }
    }
    
    // Print status periodically (but not on every update to avoid spam)
    static unsigned long lastStatusPrint = 0;
    if (millis() - lastStatusPrint >= 60000) { // Print every minute
        Serial.printf("Battery: %.2fV, Level: %s, Charging: %s\n", 
                      batteryVoltage, 
                      getBatteryLevelString(batteryLevel),
                      isCharging ? "Yes" : "No");
        lastStatusPrint = millis();
    }
}

float PowerManager::readBatteryVoltage() {
    // Read battery voltage from ADC
    // ESP32-S3 has ADC1_CH4 on GPIO8 (A5)
    const int adcPin = 8;
    const int samples = 10;
    
    uint32_t adcReading = 0;
    for (int i = 0; i < samples; i++) {
        adcReading += analogRead(adcPin);
        delay(1);
    }
    adcReading /= samples;
    
    // Convert ADC reading to voltage
    // ESP32-S3 ADC reference voltage is 3.3V
    // ADC resolution is 12-bit (0-4095)
    float voltage = (adcReading * 3.3) / 4095.0;
    
    // Apply voltage divider correction if needed
    // LiPo battery voltage divider typically reduces voltage by half
    voltage *= 2.0; // Adjust based on your voltage divider
    
    return voltage;
}

BatteryLevel PowerManager::calculateBatteryLevel(float voltage) {
    // LiPo battery voltage levels
    if (voltage >= 4.0) return BatteryLevel::HIGH;
    if (voltage >= 3.7) return BatteryLevel::MEDIUM;
    if (voltage >= 3.5) return BatteryLevel::LOW;
    return BatteryLevel::CRITICAL;
}

void PowerManager::enterSleepMode() {
    Serial.println("Entering sleep mode");
    
    if (enterSleepCallback) {
        enterSleepCallback();
    }
    
    setState(PowerState::SLEEP);
}

void PowerManager::enterDeepSleep() {
    Serial.println("Entering deep sleep mode");
    
    if (enterSleepCallback) {
        enterSleepCallback();
    }
    
    setState(PowerState::DEEP_SLEEP);
    
    // Configure wake sources
    configureDeepSleepWakeSources();
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void PowerManager::enterPollingMode() {
    Serial.println("Entering polling mode");
    
    if (enterSleepCallback) {
        enterSleepCallback();
    }
    
    setState(PowerState::POLLING);
    
    // Configure timer wake-up for polling
    esp_sleep_enable_timer_wakeup(config.pollingInterval * 1000ULL);
    
    // Enter light sleep (can be woken by timer or buttons)
    esp_light_sleep_start();
}

void PowerManager::wakeUp() {
    if (currentState == PowerState::SLEEP || currentState == PowerState::POLLING) {
        setState(PowerState::ACTIVE);
        
        if (wakeUpCallback) {
            wakeUpCallback();
        }
    }
}

void PowerManager::forceWakeUp() {
    if (currentState == PowerState::DEEP_SLEEP) {
        // This would require external interrupt or timer
        // For now, just transition to active state
        setState(PowerState::ACTIVE);
    } else {
        wakeUp();
    }
}

void PowerManager::handleWakeFromDeepSleep() {
    Serial.printf("Wake cause: %d\n", wakeCause);
    
    // Determine wake source
    switch (wakeCause) {
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Woke from Button A (D0)");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            Serial.println("Woke from Button B/C (D1/D2)");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Woke from timer (polling)");
            // If we woke from timer, enter polling mode
            setState(PowerState::POLLING);
            return;
        default:
            Serial.println("Unknown wake cause");
            break;
    }
    
    // Transition to active state
    setState(PowerState::ACTIVE);
    
    if (wakeUpCallback) {
        wakeUpCallback();
    }
}

void PowerManager::clearWakeFlags() {
    wakeFromDeepSleep = false;
    wakeCause = ESP_SLEEP_WAKEUP_UNDEFINED;
}

void PowerManager::setConfig(const PowerConfig& config) {
    this->config = config;
}

void PowerManager::setStateChangeCallback(std::function<void(PowerState, PowerState)> callback) {
    stateChangeCallback = callback;
}

void PowerManager::setBatteryLevelCallback(std::function<void(BatteryLevel)> callback) {
    batteryLevelCallback = callback;
}

void PowerManager::setEnterSleepCallback(std::function<void()> callback) {
    enterSleepCallback = callback;
}

void PowerManager::setWakeUpCallback(std::function<void()> callback) {
    wakeUpCallback = callback;
}

void PowerManager::setPollingCallback(std::function<void()> callback) {
    pollingCallback = callback;
}

bool PowerManager::isInSleepMode() const {
    return (currentState == PowerState::SLEEP || currentState == PowerState::DEEP_SLEEP);
}

bool PowerManager::isInDeepSleep() const {
    return (currentState == PowerState::DEEP_SLEEP);
}

bool PowerManager::isInPollingMode() const {
    return (currentState == PowerState::POLLING);
}

void PowerManager::configureDisplayPower(bool enable, uint8_t brightness) {
    // Configure TFT display power and brightness
    // This would interface with the display manager
    Serial.printf("Display power: %s, brightness: %d\n", 
                  enable ? "ON" : "OFF", brightness);
}

void PowerManager::configureWiFiPower(bool enable) {
    if (enable) {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin();
        }
    } else {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
    Serial.printf("WiFi power: %s\n", enable ? "ON" : "OFF");
}

void PowerManager::configureLEDPower(bool enable) {
    // Configure NeoPixel LED power
    // This would interface with the LED manager
    Serial.printf("LED power: %s\n", enable ? "ON" : "OFF");
}

void PowerManager::configureAudioPower(bool enable) {
    // Configure buzzer/audio power
    // This would interface with the audio manager
    Serial.printf("Audio power: %s\n", enable ? "ON" : "OFF");
}

void PowerManager::configureDeepSleepWakeSources() {
    // Configure wake sources for deep sleep
    // This is already done in setupWakeSources()
}

void PowerManager::configureLightSleepWakeSources() {
    // Configure wake sources for light sleep
    // Similar to deep sleep but with different timing
}

void PowerManager::printStatus() const {
    Serial.println("=== Power Manager Status ===");
    Serial.printf("Current State: %s\n", getStateString(currentState));
    Serial.printf("Previous State: %s\n", getStateString(previousState));
    Serial.printf("Inactivity Time: %lu ms\n", getInactivityTime());
    Serial.printf("Battery Voltage: %.2fV\n", batteryVoltage);
    Serial.printf("Battery Level: %s\n", getBatteryLevelString(batteryLevel));
    Serial.printf("Charging: %s\n", isCharging ? "Yes" : "No");
    Serial.printf("Wake from Deep Sleep: %s\n", wakeFromDeepSleep ? "Yes" : "No");
    Serial.printf("Polling Interval: %lu ms\n", config.pollingInterval);
    Serial.println("===========================");
}

const char* PowerManager::getStateString(PowerState state) const {
    switch (state) {
        case PowerState::DEEP_SLEEP: return "DEEP_SLEEP";
        case PowerState::SLEEP: return "SLEEP";
        case PowerState::ACTIVE: return "ACTIVE";
        case PowerState::POLLING: return "POLLING";
        default: return "UNKNOWN";
    }
}

const char* PowerManager::getBatteryLevelString(BatteryLevel level) const {
    switch (level) {
        case BatteryLevel::CRITICAL: return "CRITICAL";
        case BatteryLevel::LOW: return "LOW";
        case BatteryLevel::MEDIUM: return "MEDIUM";
        case BatteryLevel::HIGH: return "HIGH";
        case BatteryLevel::CHARGING: return "CHARGING";
        default: return "UNKNOWN";
    }
} 