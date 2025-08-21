# Flashlight Mode

## Overview

The Alert TX-1 now includes a flashlight mode that allows the LED to be used as a constant light source. This feature is accessible through the Settings menu and persists across device restarts.

## Usage

### Enabling Flashlight Mode

1. Navigate to **Settings** from the main menu
2. Select **Flashlight** (will show current state)
3. Press button C to toggle between ON/OFF
4. The LED will immediately turn on/off
5. The menu item updates to show "Flashlight [ON]" or "Flashlight [OFF]"

### Features

- **Persistent State**: Flashlight mode setting is saved to non-volatile storage
- **Auto-restore**: If flashlight was ON when device powered off, it will turn ON automatically at startup
- **Ringtone Conflict Prevention**: When flashlight is ON, LED sync with ringtones is automatically disabled
- **State Indication**: Menu shows current flashlight state clearly

## Technical Details

### Implementation

The flashlight mode is implemented across several components:

1. **SettingsManager**: Stores flashlight state persistently using ESP32 Preferences
   - Key: `flash_on` (boolean)
   - Default: false (OFF)

2. **LED Control**: Uses the existing LED class
   - `statusLed.on()` - Turns LED on constantly
   - `statusLed.off()` - Turns LED off

3. **Ringtone Integration**: 
   - When flashlight is ON, `ringtonePlayer.setLedSyncEnabled(false)` prevents ringtones from blinking the LED
   - When flashlight is OFF, LED sync is re-enabled for visual alerts

### Code Structure

```cpp
// In SettingsScreen.cpp
void SettingsScreen::onFlashlightSelected() {
    // Toggle state
    bool newState = !SettingsManager::getFlashlightEnabled();
    
    // Save to persistent storage
    SettingsManager::setFlashlightEnabled(newState);
    
    // Apply to hardware
    if (newState) {
        statusLed.on();
        ringtonePlayer.setLedSyncEnabled(false);
    } else {
        statusLed.off();
        ringtonePlayer.setLedSyncEnabled(true);
    }
}
```

### Startup Behavior

During device initialization:
```cpp
// Restore flashlight state if enabled
if (SettingsManager::getFlashlightEnabled()) {
    statusLed.on();
    ringtonePlayer.setLedSyncEnabled(false);
}
```

## Use Cases

1. **Emergency Light**: Quick access to bright LED for navigation in dark
2. **Notification**: Leave device with flashlight ON as a visual indicator
3. **Testing**: Verify LED hardware functionality
4. **General Illumination**: Use as a small flashlight when needed

## Power Considerations

When flashlight mode is enabled:
- LED draws continuous current (typically 10-20mA)
- Battery life will be reduced
- Consider turning off when not needed to conserve power

## Future Enhancements

Potential improvements:
- Brightness control (PWM)
- Strobe/blink patterns
- Auto-off timer
- Low battery auto-disable
