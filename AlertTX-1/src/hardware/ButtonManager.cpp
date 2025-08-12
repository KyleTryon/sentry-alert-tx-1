#include "ButtonManager.h"
#include <esp_sleep.h>

void ButtonManager::begin(LED* led, Buzzer* bz) {
    statusLED = led;
    buzzer = bz;
    
    // Initialize button pins
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);  // D0/BOOT has built-in pullup
    pinMode(BUTTON_B_PIN, INPUT_PULLDOWN); // D1 needs pulldown
    pinMode(BUTTON_C_PIN, INPUT_PULLDOWN); // D2 needs pulldown
    
    // Initialize button states
    for (int i = 0; i < 3; i++) {
        buttons[i].currentState = false;
        buttons[i].lastState = false;
        buttons[i].pressed = false;
        buttons[i].released = false;
        buttons[i].longPressed = false;
        buttons[i].longPressTriggered = false;
        buttons[i].lastDebounceTime = 0;
        buttons[i].pressStartTime = 0;
        buttons[i].lastRepeatTime = 0;
        buttons[i].repeatCount = 0;
    }
    
    setupWakeSources();
}

void ButtonManager::update() {
    unsigned long currentTime = millis();
    
    for (int i = 0; i < 3; i++) {
        ButtonState& btn = buttons[i];
        
        // Read current button state
        bool reading = readButtonState(i);
        
        // Debouncing
        if (reading != btn.lastState) {
            btn.lastDebounceTime = currentTime;
        }
        
        if ((currentTime - btn.lastDebounceTime) > DEBOUNCE_DELAY) {
            if (reading != btn.currentState) {
                btn.currentState = reading;
                
                if (btn.currentState) {
                    // Button pressed
                    btn.pressed = true;
                    btn.pressStartTime = currentTime;
                    btn.repeatCount = 0;
                    
                    // Button press detected (no event system needed)
                    
                    // Provide feedback
                    provideFeedback(i);
                    
                } else {
                    // Button released
                    btn.released = true;
                    btn.longPressTriggered = false;
                    
                    // Button release detected (no event system needed)
                }
            }
        }
        
        // Long press detection
        if (btn.currentState && !btn.longPressTriggered) {
            if ((currentTime - btn.pressStartTime) > LONG_PRESS_DELAY) {
                btn.longPressed = true;
                btn.longPressTriggered = true;
                
                // Long press detected (no event system needed)
            }
        }
        
        // Button repeat (for held buttons)
        if (btn.currentState && btn.longPressTriggered) {
            if ((currentTime - btn.lastRepeatTime) > REPEAT_DELAY) {
                btn.lastRepeatTime = currentTime;
                btn.repeatCount++;
                
                // Button repeat detected (no event system needed)
            }
        }
        
        btn.lastState = reading;
    }
}

bool ButtonManager::readButtonState(int buttonIndex) {
    switch (buttonIndex) {
        case 0: // Button A (D0/BOOT) - pulled HIGH, goes LOW when pressed
            return (digitalRead(BUTTON_A_PIN) == LOW);
        case 1: // Button B (D1) - pulled LOW, goes HIGH when pressed
            return (digitalRead(BUTTON_B_PIN) == HIGH);
        case 2: // Button C (D2) - pulled LOW, goes HIGH when pressed
            return (digitalRead(BUTTON_C_PIN) == HIGH);
        default:
            return false;
    }
}

void ButtonManager::setupWakeSources() {
    // Configure buttons for deep sleep wake-up
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);  // D0/BOOT - wake on LOW
    esp_sleep_enable_ext1_wakeup(
        ((1ULL << GPIO_NUM_1) | (1ULL << GPIO_NUM_2)), 
        ESP_EXT1_WAKEUP_ANY_HIGH  // D1/D2 - wake on HIGH
    );
}

bool ButtonManager::isPressed(int buttonIndex) {
    if (buttonIndex < 0 || buttonIndex >= 3) return false;
    return buttons[buttonIndex].currentState;
}

bool ButtonManager::wasPressed(int buttonIndex) {
    if (buttonIndex < 0 || buttonIndex >= 3) return false;
    bool result = buttons[buttonIndex].pressed;
    buttons[buttonIndex].pressed = false;  // Clear flag
    return result;
}

bool ButtonManager::isLongPressed(int buttonIndex) {
    if (buttonIndex < 0 || buttonIndex >= 3) return false;
    bool result = buttons[buttonIndex].longPressed;
    buttons[buttonIndex].longPressed = false;  // Clear flag
    return result;
}

void ButtonManager::provideFeedback(int buttonId) {
    if (buzzer) {
        buzzer->playTone(800, 50);  // Short beep
    }
    if (statusLED) {
        statusLED->blink(100);      // Brief LED flash
    }
}
