#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "../config/settings.h"
#include "LED.h"
#include "Buzzer.h"
#include <Arduino.h>

class ButtonManager {
private:
    // Button pins (from settings.h and official documentation)
    static const int BUTTON_A_PIN = 0;  // GPIO0 - Built-in D0/BOOT button
    static const int BUTTON_B_PIN = 1;  // GPIO1 - Built-in D1 button  
    static const int BUTTON_C_PIN = 2;  // GPIO2 - Built-in D2 button
    
    // Button states (from official ESP32-S3 Reverse TFT Feather documentation)
    // Button A (D0/BOOT): Pulled HIGH by default, goes LOW when pressed
    // Button B (D1): Pulled LOW by default, goes HIGH when pressed  
    // Button C (D2): Pulled LOW by default, goes HIGH when pressed
    
    // Debouncing configuration (from settings.h)
    static const unsigned long DEBOUNCE_DELAY = 50;  // 50ms debounce
    static const unsigned long LONG_PRESS_DELAY = 1500; // 1.5 seconds for long press (back navigation)
    static const unsigned long REPEAT_DELAY = 200;  // For held buttons
    
    struct ButtonState {
        bool currentState;
        bool lastState;
        bool pressed;
        bool released;
        bool shortClicked;
        bool longPressed;
        bool longPressTriggered;
        unsigned long lastDebounceTime;
        unsigned long pressStartTime;
        unsigned long lastRepeatTime;
        int repeatCount;
    };
    
    ButtonState buttons[3];  // A, B, C
    LED* statusLED;
    Buzzer* buzzer;
    
    // Helper methods
    bool readButtonState(int buttonIndex);
    void setupWakeSources();
    
public:
    void begin(LED* led = nullptr, Buzzer* bz = nullptr);
    void update();  // Call in main loop
    
    // Button state queries
    bool isPressed(int buttonIndex);
    bool wasPressed(int buttonIndex);
    bool wasReleased(int buttonIndex);
    bool wasShortClick(int buttonIndex);
    bool isLongPressed(int buttonIndex);
    
    // Feedback
    void provideFeedback(int buttonId);
    
    // Button constants
    static const int BUTTON_A = 0;
    static const int BUTTON_B = 1;
    static const int BUTTON_C = 2;
};

#endif // BUTTON_MANAGER_H
