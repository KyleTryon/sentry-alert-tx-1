# Power Management System

## Overview

The Alert TX-1 implements a focused power management system designed to maximize battery life while maintaining responsive user interaction and reliable alert functionality. The system uses the ESP32-S3's advanced sleep modes and separates power management from application state management.

## Power States

The power management system focuses on **actual power consumption levels**, not UI states:

### 1. Deep Sleep (`DEEP_SLEEP`)
- **Power Consumption**: ~10μA
- **Wake Sources**: Button presses, timer
- **Functionality**: Minimal - only wake detection active
- **Duration**: Until button press or timer expires
- **Use Case**: Long-term storage, maximum battery life

### 2. Sleep (`SLEEP`)
- **Power Consumption**: ~50-100μA
- **Wake Sources**: Button presses
- **Functionality**: Reduced display brightness, minimal processing
- **Duration**: Until activity or timeout
- **Use Case**: Short-term power saving, quick wake response

### 3. Active (`ACTIVE`)
- **Power Consumption**: ~50-200mA
- **Wake Sources**: All normal sources
- **Functionality**: Full system operation
- **Duration**: Until inactivity timeout
- **Use Case**: Normal operation, user interaction

### 4. Polling (`POLLING`)
- **Power Consumption**: ~10-50μA (with periodic WiFi wake)
- **Wake Sources**: Timer (every 30 seconds), button presses
- **Functionality**: Sleep but wake periodically to check for alerts
- **Duration**: Until alert received or button pressed
- **Use Case**: Alert monitoring while conserving power

## Application States (Separate from Power)

Application states are managed separately by the `AppStateManager`:

- **IDLE**: No specific UI active
- **MENU**: Menu navigation active
- **GAME**: Beeper Hero game active
- **ALERT**: Alert notification active
- **SETTINGS**: Settings screen active
- **RINGTONES**: Ringtones selection active
- **STATUS**: Status/connection screen active

## State Transitions

```
┌─────────────────┐
│   Deep Sleep    │ ← Wake on button press
└─────────┬───────┘
          │ Button Press
          ▼
┌─────────────────┐
│   Active Mode   │ ← Inactivity timeout
└─────────┬───────┘
          │ Inactivity timeout
          ▼
┌─────────────────┐
│   Sleep Mode    │ ← Deep sleep timeout
└─────────┬───────┘
          │ Deep sleep timeout
          ▼
┌─────────────────┐
│   Deep Sleep    │
└─────────────────┘

┌─────────────────┐
│   Polling Mode  │ ← Alternative to deep sleep
└─────────┬───────┘
          │ Timer (30s)
          ▼
┌─────────────────┐
│ Check for Alerts│
└─────────┬───────┘
          │ No alerts
          ▼
┌─────────────────┐
│   Polling Mode  │
└─────────────────┘
```

### Transition Rules

1. **Activity Detection**: Any button press or user interaction
2. **Inactivity Timeout**: Configurable (default: 60 seconds)
3. **Deep Sleep Timeout**: Configurable (default: 5 minutes)
4. **Polling Interval**: Configurable (default: 30 seconds)
5. **Alert Override**: Alerts can wake from any sleep state
6. **Manual Control**: Long press can force deep sleep

## Battery Management

### Battery Level Monitoring

The system continuously monitors battery voltage and provides real-time status:

| Level | Voltage Range | LED Indicator | Action |
|-------|---------------|---------------|---------|
| **CRITICAL** | < 3.5V | Fast red blink | Immediate sleep, warning |
| **LOW** | 3.5V - 3.7V | Slow red blink | Reduce functionality |
| **MEDIUM** | 3.7V - 4.0V | Yellow solid | Normal operation |
| **HIGH** | > 4.0V | Green solid | Full functionality |
| **CHARGING** | USB connected | Blue solid | Normal operation |

### Battery Voltage Reading

```cpp
// Battery voltage is read from ADC pin 8 (A5)
float voltage = readBatteryVoltage();

// Voltage divider correction applied
// LiPo battery typically uses 2:1 voltage divider
voltage *= 2.0; // Adjust based on your hardware
```

### Charging Detection

The system detects USB charging through ADC readings and adjusts behavior accordingly:

- **Charging**: Full functionality, blue LED indicator
- **Not Charging**: Normal power management applies

## Configuration

### Power Configuration Structure

```cpp
struct PowerConfig {
    unsigned long inactivityTimeout = 60000;      // 60 seconds
    unsigned long deepSleepTimeout = 300000;      // 5 minutes
    unsigned long pollingInterval = 30000;        // 30 seconds between alert checks
    uint8_t sleepBrightness = 50;                 // Display brightness in sleep
    uint8_t activeBrightness = 100;               // Display brightness when active
    bool enableWiFiInPolling = true;              // Keep WiFi on in polling mode
    bool enableMQTTInPolling = true;              // Keep MQTT connected in polling
};
```

### Configuration Examples

#### Maximum Battery Life
```cpp
PowerConfig config;
config.inactivityTimeout = 30000;     // 30 seconds
config.deepSleepTimeout = 120000;     // 2 minutes
config.pollingInterval = 60000;       // 1 minute
config.enableWiFiInPolling = false;
config.enableMQTTInPolling = false;
config.sleepBrightness = 25;
```

#### Responsive Mode
```cpp
PowerConfig config;
config.inactivityTimeout = 120000;    // 2 minutes
config.deepSleepTimeout = 600000;     // 10 minutes
config.pollingInterval = 15000;       // 15 seconds
config.enableWiFiInPolling = true;
config.enableMQTTInPolling = true;
config.sleepBrightness = 75;
```

#### Alert-Focused Mode
```cpp
PowerConfig config;
config.inactivityTimeout = 300000;    // 5 minutes
config.deepSleepTimeout = 1800000;    // 30 minutes
config.pollingInterval = 30000;       // 30 seconds
config.enableWiFiInPolling = true;
config.enableMQTTInPolling = true;
config.sleepBrightness = 50;
```

## Wake Sources

### GPIO Wake Sources

The ESP32-S3 is configured with multiple wake sources:

```cpp
// Button A (D0) - GPIO0 - Built-in button
esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); // Wake on LOW

// Buttons B and C (D1, D2) - GPIO1, GPIO2 - Built-in buttons
esp_sleep_enable_ext1_wakeup(0x06, ESP_EXT1_WAKEUP_ANY_HIGH); // GPIO1 and GPIO2

// Timer wake-up for polling mode
esp_sleep_enable_timer_wakeup(30000000ULL); // 30 seconds
```

### Wake Source Behavior

| Button | Default State | Wake Trigger | Use Case |
|--------|---------------|--------------|----------|
| **A (D0)** | HIGH | Goes LOW | Primary wake button |
| **B (D1)** | LOW | Goes HIGH | Secondary wake |
| **C (D2)** | LOW | Goes HIGH | Secondary wake |

## Hardware Power Management

### Display Power Control

```cpp
void configureDisplayPower(bool enable, uint8_t brightness) {
    if (enable) {
        // Turn on display with specified brightness
        display.setBrightness(brightness);
    } else {
        // Turn off display completely
        display.powerOff();
    }
}
```

### WiFi Power Control

```cpp
void configureWiFiPower(bool enable) {
    if (enable) {
        WiFi.begin();
    } else {
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
    }
}
```

### LED Power Control

```cpp
void configureLEDPower(bool enable) {
    if (enable) {
        // Enable NeoPixel LED
        neopixel.setPower(true);
    } else {
        // Disable NeoPixel LED
        neopixel.setPower(false);
    }
}
```

### Audio Power Control

```cpp
void configureAudioPower(bool enable) {
    if (enable) {
        // Enable buzzer/audio system
        buzzer.enable();
    } else {
        // Disable buzzer/audio system
        buzzer.disable();
    }
}
```

## Callback System

The PowerManager provides a comprehensive callback system for integration with other components:

### State Change Callback
```cpp
powerManager.setStateChangeCallback([](PowerState oldState, PowerState newState) {
    Serial.printf("Power state: %s -> %s\n", 
                  powerManager.getStateString(oldState),
                  powerManager.getStateString(newState));
});
```

### Battery Level Callback
```cpp
powerManager.setBatteryLevelCallback([](BatteryLevel level) {
    if (level == BatteryLevel::CRITICAL) {
        // Show low battery warning
        uiManager.showLowBatteryWarning();
    }
});
```

### Polling Callback
```cpp
powerManager.setPollingCallback([]() {
    // Check for new MQTT messages
    mqttClient.checkForMessages();
});
```

### Sleep/Wake Callbacks
```cpp
powerManager.setEnterSleepCallback([]() {
    // Save state before sleep
    saveSettings();
});

powerManager.setWakeUpCallback([]() {
    // Restore state after wake
    restoreSettings();
});
```

## Integration with Main Application

### Main Loop Integration

```cpp
void loop() {
    // Update power manager first
    powerManager.update();
    
    // Only run active components based on power state
    PowerState currentPowerState = powerManager.getCurrentState();
    
    switch (currentPowerState) {
        case PowerState::DEEP_SLEEP:
            // Should not reach here
            break;
            
        case PowerState::SLEEP:
            // Minimal updates
            buttonHandler.update();
            break;
            
        case PowerState::POLLING:
            // Check for alerts, then return to sleep
            mqttClient.update();
            break;
            
        case PowerState::ACTIVE:
            // Full functionality
            updateActiveComponents();
            break;
    }
}
```

### Activity Recording

```cpp
// Record activity for power management
void handleButtonPress() {
    powerManager.recordActivity();
    // Handle button press
}

void handleMQTTMessage() {
    powerManager.recordActivity();
    // Handle MQTT message
}
```

## Power Consumption Optimization

### Component Power Usage

| Component | Active Mode | Sleep Mode | Deep Sleep | Polling Mode |
|-----------|-------------|------------|------------|--------------|
| **ESP32-S3 Core** | 50-100mA | 10-50μA | 10μA | 10-50μA |
| **TFT Display** | 50-100mA | 10-20mA | 0mA | 0mA |
| **WiFi** | 50-100mA | 0mA | 0mA | 50-100mA* |
| **NeoPixel LED** | 20-60mA | 0mA | 0mA | 0mA |
| **Buzzer** | 10-50mA | 0mA | 0mA | 0mA |
| **Total** | 180-410mA | 10-50μA | 10μA | 60-150μA |

*WiFi only active during polling checks

### Optimization Strategies

1. **Display Brightness**: Reduce brightness in sleep mode
2. **WiFi Management**: Disable WiFi when not needed
3. **LED Control**: Turn off LEDs in sleep modes
4. **Audio Management**: Disable audio in sleep modes
5. **Update Frequency**: Reduce update frequency in sleep mode
6. **Polling Mode**: Use polling instead of deep sleep for alert monitoring

## Troubleshooting

### Common Issues

1. **Device Not Waking**
   - Check button connections
   - Verify wake source configuration
   - Test with different buttons

2. **Battery Draining Too Fast**
   - Check power state transitions
   - Verify WiFi is disabled in sleep
   - Monitor component power usage

3. **Incorrect Battery Level**
   - Calibrate voltage divider
   - Check ADC readings
   - Verify battery voltage calculation

4. **Sleep Mode Not Entering**
   - Check activity detection
   - Verify timeout configuration
   - Monitor activity recording

5. **Polling Not Working**
   - Check timer configuration
   - Verify MQTT connection
   - Monitor polling callback

### Debug Commands

```cpp
// Print power manager status
powerManager.printStatus();

// Print app state manager status
appStateManager.printStatus();

// Force wake up
powerManager.forceWakeUp();

// Enter sleep mode
powerManager.enterSleepMode();

// Enter polling mode
powerManager.enterPollingMode();

// Check battery status
float voltage = powerManager.getBatteryVoltage();
BatteryLevel level = powerManager.getBatteryLevel();
```

## Best Practices

1. **Always record activity** when user interaction occurs
2. **Use appropriate timeouts** for your use case
3. **Test wake sources** thoroughly before deployment
4. **Monitor battery levels** and provide user feedback
5. **Save important state** before entering sleep modes
6. **Use callbacks** for clean integration with other components
7. **Test power consumption** in all states
8. **Provide user control** over power management settings
9. **Separate power states from app states** for cleaner architecture
10. **Use polling mode** for alert monitoring when deep sleep isn't needed

This power management system ensures the Alert TX-1 can operate for extended periods on battery power while maintaining responsive user interaction and reliable alert functionality. The separation of power states from application states provides a cleaner, more maintainable architecture. 