/*
 * AnyRtttl Integration Test
 * 
 * This sketch tests the AnyRtttl library integration with the Alert TX-1 project.
 * It plays a simple RTTTL melody and demonstrates the non-blocking functionality.
 */

#include <Arduino.h>
#include "src/config/settings.h"
#include "lib/AnyRtttl/src/anyrtttl.h"

// Test RTTTL melody (Mario theme)
const char* testMelody = "Mario:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p";

// Test states
enum TestState {
    IDLE,
    PLAYING,
    PAUSED,
    FINISHED
};

TestState currentState = IDLE;
unsigned long testStartTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("AnyRtttl Integration Test");
    Serial.println("=========================");
    
    // Initialize buzzer pin
    pinMode(BUZZER_PIN, OUTPUT);
    
    Serial.println("Press any button to start test melody");
    Serial.printf("Test melody: %s\n", testMelody);
}

void loop() {
    // Check for button press to start test
    if (currentState == IDLE) {
        if (digitalRead(BUTTON_A_PIN) == LOW || 
            digitalRead(BUTTON_B_PIN) == LOW || 
            digitalRead(BUTTON_C_PIN) == LOW) {
            
            Serial.println("Starting melody playback...");
            currentState = PLAYING;
            testStartTime = millis();
            
            // Start AnyRtttl playback
            anyrtttl::nonblocking::begin(BUZZER_PIN, testMelody);
        }
    }
    
    // Update AnyRtttl if playing
    if (currentState == PLAYING) {
        anyrtttl::nonblocking::play();
        
        // Check if still playing
        if (!anyrtttl::nonblocking::isPlaying()) {
            if (anyrtttl::nonblocking::done()) {
                Serial.println("Melody finished!");
                currentState = FINISHED;
            }
        }
        
        // Print status every second
        static unsigned long lastStatus = 0;
        if (millis() - lastStatus >= 1000) {
            lastStatus = millis();
            unsigned long elapsed = millis() - testStartTime;
            Serial.printf("Playing... Elapsed: %lu ms\n", elapsed);
        }
    }
    
    // Check for button press to restart
    if (currentState == FINISHED) {
        if (digitalRead(BUTTON_A_PIN) == LOW || 
            digitalRead(BUTTON_B_PIN) == LOW || 
            digitalRead(BUTTON_C_PIN) == LOW) {
            
            Serial.println("Restarting test...");
            currentState = IDLE;
            delay(1000); // Debounce
        }
    }
    
    // Small delay for stability
    delay(10);
} 