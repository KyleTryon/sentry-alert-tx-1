// Main Arduino sketch for Alert TX-1
#include <Arduino.h>

// Include custom headers
#include "src/config/settings.h"
#include "src/hardware/PowerManager.h"
#include "src/ui/AppStateManager.h"
#include "src/ui/UIManager.h"
#include "src/mqtt/MQTTClient.h"
#include "src/hardware/ButtonHandler.h"
#include "src/hardware/Buzzer.h"
#include "src/hardware/LED.h"
#include "src/ringtones/RingtonePlayer.h"
#include "src/game/BeeperHeroGame.h"
#include "lib/DebounceUtility.h"

// Global instances
UIManager uiManager;
MQTTClient mqttClient;
ButtonHandler buttonHandler;
Buzzer buzzer;
LED led;
RingtonePlayer ringtonePlayer;
BeeperHeroGame beeperHeroGame(&uiManager, &buttonHandler, &ringtonePlayer);

// Power management configuration
PowerConfig powerConfig;

// Debounce utilities
MultiDebounceUtility buttonDebouncers(3, 50); // 3 buttons, 50ms debounce
StateChangeDebounceUtility powerStateDebouncer(200); // 200ms for power state changes
StateChangeDebounceUtility appStateDebouncer(100); // 100ms for app state changes
DebounceUtility mqttMessageDebouncer(1000); // 1 second for MQTT message handling

// Timing variables for non-blocking operations
unsigned long lastUIUpdate = 0;
unsigned long lastMQTTUpdate = 0;
unsigned long lastButtonUpdate = 0;
unsigned long lastStatusUpdate = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Alert TX-1 Starting...");
    
    // Configure power management
    powerConfig.inactivityTimeout = INACTIVITY_TIMEOUT_MS;
    powerConfig.deepSleepTimeout = 300000; // 5 minutes
    powerConfig.pollingInterval = 30000;   // 30 seconds between alert checks
    powerConfig.sleepBrightness = 50;
    powerConfig.activeBrightness = 100;
    powerConfig.enableWiFiInPolling = true;
    powerConfig.enableMQTTInPolling = true;
    
    // Initialize power manager first
    powerManager.begin(powerConfig);
    
    // Set up power manager callbacks
    powerManager.setStateChangeCallback([](PowerState oldState, PowerState newState) {
        // Debounce power state changes
        if (powerStateDebouncer.update(static_cast<int>(newState))) {
            Serial.printf("Power state changed: %s -> %s\n", 
                          powerManager.getStateString(oldState),
                          powerManager.getStateString(newState));
        }
    });
    
    powerManager.setBatteryLevelCallback([](BatteryLevel level) {
        Serial.printf("Battery level: %s\n", powerManager.getBatteryLevelString(level));
        // Update UI with battery status
        if (level == BatteryLevel::CRITICAL) {
            // Show low battery warning
        }
    });
    
    powerManager.setEnterSleepCallback([]() {
        Serial.println("Entering sleep mode - saving state");
        // Save any necessary state before sleep
    });
    
    powerManager.setWakeUpCallback([]() {
        Serial.println("Waking up from sleep");
        // Restore state after wake
    });
    
    powerManager.setPollingCallback([]() {
        Serial.println("Polling for alerts...");
        // Check for new MQTT messages
        mqttClient.checkForMessages();
    });
    
    // Set up app state manager callbacks
    appStateManager.setStateChangeCallback([](AppState oldState, AppState newState) {
        // Debounce app state changes
        if (appStateDebouncer.update(static_cast<int>(newState))) {
            Serial.printf("App state changed: %s -> %s\n", 
                          appStateManager.getStateString(oldState),
                          appStateManager.getStateString(newState));
        }
    });
    
    // Initialize hardware components
    led.begin(13); // GPIO13 - onboard LED
    buzzer.begin(BUZZER_PIN);
    buttonHandler.begin();
    
    // Initialize UI system
    uiManager.begin();
    
    // Initialize MQTT client
    mqttClient.begin(WIFI_SSID, WIFI_PASSWORD, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID);
    
    // Initialize game system
    beeperHeroGame.begin();
    
    // Initialize ringtone player
    ringtonePlayer.begin();
    
    Serial.println("Alert TX-1 Initialized Successfully");
    powerManager.printStatus();
    appStateManager.printStatus();
}

void loop() {
    unsigned long currentTime = millis();
    
    // Update power manager (handles state transitions and battery monitoring)
    powerManager.update();
    
    // Only run active components based on power state
    PowerState currentPowerState = powerManager.getCurrentState();
    
    switch (currentPowerState) {
        case PowerState::DEEP_SLEEP:
            // Should not reach here - deep sleep is handled by ESP32
            break;
            
        case PowerState::SLEEP:
            // Minimal updates in sleep mode
            if (currentTime - lastButtonUpdate >= 100) {
                updateButtonHandling();
                lastButtonUpdate = currentTime;
            }
            break;
            
        case PowerState::POLLING:
            // Check for alerts, then return to sleep
            if (currentTime - lastMQTTUpdate >= 1000) {
                mqttClient.update();
                lastMQTTUpdate = currentTime;
            }
            break;
            
        case PowerState::ACTIVE:
            // Full functionality in active state
            updateActiveComponents(currentTime);
            break;
    }
    
    // Small delay for stability
    delay(10); // 100Hz main loop
}

void updateActiveComponents(unsigned long currentTime) {
    // Update button handling (every 10ms)
    if (currentTime - lastButtonUpdate >= 10) {
        updateButtonHandling();
        lastButtonUpdate = currentTime;
    }
    
    // Update UI system (every 16ms = 60 FPS)
    if (currentTime - lastUIUpdate >= 16) {
        uiManager.update();
        lastUIUpdate = currentTime;
    }
    
    // Update MQTT client (every 100ms)
    if (currentTime - lastMQTTUpdate >= 100) {
        mqttClient.update();
        lastMQTTUpdate = currentTime;
    }
    
    // Update status display (every 1 second)
    if (currentTime - lastStatusUpdate >= 1000) {
        updateStatusDisplay();
        lastStatusUpdate = currentTime;
    }
    
    // Update audio system (non-blocking)
    ringtonePlayer.update();
    
    // Update game system if in game state
    if (appStateManager.isInGame()) {
        beeperHeroGame.update();
    }
}

void updateButtonHandling() {
    // Update button handler
    buttonHandler.update();
    
    // Record activity for power management if any button activity
    if (buttonHandler.hasActivity()) {
        powerManager.recordActivity();
    }
    
    // Get button events and handle them with debouncing
    ButtonEvent event = buttonHandler.getEvent();
    if (event != BUTTON_NONE) {
        handleButtonEvent(event);
    }
}

void updateStatusDisplay() {
    // Update battery status on display
    BatteryLevel batteryLevel = powerManager.getBatteryLevel();
    float batteryVoltage = powerManager.getBatteryVoltage();
    
    // Update LED based on battery level
    switch (batteryLevel) {
        case BatteryLevel::CRITICAL:
            led.setBlinkPattern(100, 100); // Fast blink
            break;
        case BatteryLevel::LOW:
            led.setBlinkPattern(500, 500); // Slow blink
            break;
        case BatteryLevel::MEDIUM:
            led.setSolidColor(255, 255, 0); // Yellow
            break;
        case BatteryLevel::HIGH:
            led.setSolidColor(0, 255, 0); // Green
            break;
        case BatteryLevel::CHARGING:
            led.setSolidColor(0, 0, 255); // Blue
            break;
    }
    
    // Print status to serial for debugging
    if (Serial.availableForWrite()) {
        Serial.printf("Status - Battery: %.2fV (%s), Power: %s, App: %s\n",
                      batteryVoltage,
                      powerManager.getBatteryLevelString(batteryLevel),
                      powerManager.getStateString(powerManager.getCurrentState()),
                      appStateManager.getStateString(appStateManager.getCurrentState()));
    }
}

// Button event handlers
void handleButtonEvent(ButtonEvent event) {
    // Record activity for power management
    powerManager.recordActivity();
    
    switch (event) {
        case BUTTON_A_PRESS:
            handleButtonAPress();
            break;
        case BUTTON_A_LONG_PRESS:
            handleButtonALongPress();
            break;
        case BUTTON_B_PRESS:
            handleButtonBPress();
            break;
        case BUTTON_B_LONG_PRESS:
            handleButtonBLongPress();
            break;
        case BUTTON_C_PRESS:
            handleButtonCPress();
            break;
        case BUTTON_C_LONG_PRESS:
            handleButtonCLongPress();
            break;
        default:
            break;
    }
}

void handleButtonAPress() {
    PowerState currentPowerState = powerManager.getCurrentState();
    AppState currentAppState = appStateManager.getCurrentState();
    
    // Handle power state transitions first
    if (currentPowerState == PowerState::SLEEP || currentPowerState == PowerState::POLLING) {
        powerManager.wakeUp();
        return;
    }
    
    // Handle app state transitions
    switch (currentAppState) {
        case AppState::IDLE:
            appStateManager.setState(AppState::MENU);
            break;
        case AppState::MENU:
            // Navigate back or select
            uiManager.handleButtonA();
            break;
        case AppState::GAME:
            beeperHeroGame.handleButtonA();
            break;
        case AppState::ALERT:
            // Dismiss alert
            appStateManager.setState(AppState::IDLE);
            break;
        default:
            // Handle other app states
            break;
    }
}

void handleButtonALongPress() {
    // Force wake from deep sleep or enter deep sleep
    if (powerManager.getCurrentState() == PowerState::DEEP_SLEEP) {
        powerManager.forceWakeUp();
    } else {
        // Enter deep sleep
        powerManager.enterDeepSleep();
    }
}

void handleButtonBPress() {
    AppState currentAppState = appStateManager.getCurrentState();
    
    switch (currentAppState) {
        case AppState::MENU:
            uiManager.handleButtonB();
            break;
        case AppState::GAME:
            beeperHeroGame.handleButtonB();
            break;
        default:
            break;
    }
}

void handleButtonBLongPress() {
    // Custom long press action for button B
    Serial.println("Button B long press");
}

void handleButtonCPress() {
    AppState currentAppState = appStateManager.getCurrentState();
    
    switch (currentAppState) {
        case AppState::MENU:
            uiManager.handleButtonC();
            break;
        case AppState::GAME:
            beeperHeroGame.handleButtonC();
            break;
        default:
            break;
    }
}

void handleButtonCLongPress() {
    // Custom long press action for button C
    Serial.println("Button C long press");
}

// MQTT message handler
void handleMQTTMessage(const char* topic, const char* message) {
    if (strcmp(topic, MQTT_TOPIC_SUBSCRIBE) == 0) {
        // Debounce MQTT message handling to prevent rapid state changes
        if (mqttMessageDebouncer.update(true)) {
            // Parse alert message
            Serial.printf("Received alert: %s\n", message);
            
            // Wake up if in sleep mode
            if (powerManager.isInSleepMode()) {
                powerManager.wakeUp();
            }
            
            // Transition to alert state
            appStateManager.setState(AppState::ALERT);
            
            // Start alert sequence
            startAlertSequence(message);
        }
    }
}

void startAlertSequence(const char* message) {
    // Play alert sound
    ringtonePlayer.playRingtone("alert");
    
    // Flash LED
    led.setBlinkPattern(200, 200);
    
    // Display alert on screen
    uiManager.showAlert(message);
    
    // Return to idle state after alert
    delay(5000); // Show alert for 5 seconds
    appStateManager.setState(AppState::IDLE);
}
