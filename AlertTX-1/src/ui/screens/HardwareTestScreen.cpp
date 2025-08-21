#include "HardwareTestScreen.h"
#include "../../config/settings.h"
#include "../core/ScreenManager.h"
#include <Arduino.h>

// Pin definitions for testing
const int TEST_BUZZER_PIN_A4 = 14;  // A4 = GPIO14 (user's wiring)
const int TEST_BUZZER_PIN_A3 = 15;  // A3 = GPIO15 (configured in settings.h)
const int TEST_LED_PIN_A0 = 18;     // A0 = GPIO18

// Static instance
HardwareTestScreen* HardwareTestScreen::instance = nullptr;

HardwareTestScreen::HardwareTestScreen(Adafruit_ST7789* display, LED* led) 
    : Screen(display, "HardwareTest", 2), ledRef(led),
      ledTestActive(false), ledTestStartTime(0),
      buzzerTestActive(false), buzzerTestStartTime(0), buzzerTestStep(0) {
    
    instance = this;
    
    // Create menu container
    menu = new MenuContainer(display, 10, 50);
    
    // Add menu to screen components
    addComponent(menu);
    
    // Setup menu items
    setupMenu();
    
    Serial.println("HardwareTestScreen created");
}

HardwareTestScreen::~HardwareTestScreen() {
    instance = nullptr;
    Serial.println("HardwareTestScreen destroyed");
}

void HardwareTestScreen::enter() {
    Screen::enter();
    Serial.println("Entered HardwareTestScreen");
    
    // Reset test states
    ledTestActive = false;
    buzzerTestActive = false;
    buzzerTestStep = 0;
    
    // Reset menu selection
    menu->setSelectedIndex(0);
}

void HardwareTestScreen::exit() {
    // Make sure to stop any active tests
    if (buzzerTestActive) {
        noTone(TEST_BUZZER_PIN_A4);
        noTone(TEST_BUZZER_PIN_A3);
    }
    if (ledTestActive && ledRef) {
        ledRef->off();
    }
    
    Screen::exit();
    Serial.println("Exited HardwareTestScreen");
}

void HardwareTestScreen::update() {
    Screen::update();
    
    updateLEDTest();
    updateBuzzerTest();
}

void HardwareTestScreen::draw() {
    Screen::draw();
    
    // Draw title
    drawTitle("Hardware Test");
    
    // Draw pin info
    display->setTextColor(ThemeManager::getPrimaryText());
    display->setTextSize(1);
    display->setCursor(10, 35);
    display->print("LED: A0 (GPIO18)");
    display->setCursor(10, 45);
    display->print("Buzzer: A4 (GPIO14)");
    
    // Show test status
    if (ledTestActive) {
        display->setTextColor(ST77XX_GREEN);
        display->setCursor(150, 35);
        display->print("ACTIVE!");
    }
    
    if (buzzerTestActive) {
        display->setTextColor(ST77XX_YELLOW);
        display->setCursor(150, 45);
        display->print("ACTIVE!");
    }
}

void HardwareTestScreen::handleButtonPress(int button) {
    // Let the menu handle navigation
    if (menu) {
        menu->handleButtonPress(button);
    }
}

void HardwareTestScreen::setupMenu() {
    if (!menu) return;
    
    // Clear existing items
    menu->clear();
    
    // Add menu items
    menu->addMenuItem("Test LED (A0)", 1, ledTestCallback);
    menu->addMenuItem("Test Buzzer (A4)", 2, buzzerTestCallback);
    menu->addMenuItem("Back", 3, backCallback);
    
    // Auto-layout the menu
    menu->autoLayout();
    
    Serial.println("HardwareTestScreen menu setup complete");
}

// Hardware test implementations

void HardwareTestScreen::onLEDTestSelected() {
    if (!ledTestActive) {
        ledTestActive = true;
        ledTestStartTime = millis();
        
        // Directly control the LED pin for testing
        pinMode(TEST_LED_PIN_A0, OUTPUT);
        digitalWrite(TEST_LED_PIN_A0, HIGH);
        
        // Also try using the LED class if available
        if (ledRef) {
            ledRef->on();
        }
        
        Serial.println("LED Test: ON (Pin A0/GPIO18)");
        
        // Force redraw to show status
        markForFullRedraw();
    }
}

void HardwareTestScreen::onBuzzerTestSelected() {
    if (!buzzerTestActive) {
        buzzerTestActive = true;
        buzzerTestStartTime = millis();
        buzzerTestStep = 0;
        
        // Setup pins for testing
        pinMode(TEST_BUZZER_PIN_A4, OUTPUT);
        pinMode(TEST_BUZZER_PIN_A3, OUTPUT);
        
        Serial.println("Buzzer Test: Starting (Testing A4/GPIO14)");
        
        // Force redraw to show status
        markForFullRedraw();
    }
}

void HardwareTestScreen::onBackSelected() {
    Serial.println("HardwareTestScreen: Back selected");
    
    // Navigate back
    ScreenManager* manager = GlobalScreenManager::getInstance();
    if (manager) {
        manager->popScreen();
    }
}

// Update methods

void HardwareTestScreen::updateLEDTest() {
    if (ledTestActive && (millis() - ledTestStartTime >= 1000)) {
        // Turn off LED after 1 second
        digitalWrite(TEST_LED_PIN_A0, LOW);
        if (ledRef) {
            ledRef->off();
        }
        
        ledTestActive = false;
        Serial.println("LED Test: OFF");
        
        // Force redraw to update status
        markForFullRedraw();
    }
}

void HardwareTestScreen::updateBuzzerTest() {
    if (!buzzerTestActive) return;
    
    unsigned long elapsed = millis() - buzzerTestStartTime;
    
    // Play different tones in sequence on A4 (user's pin)
    switch (buzzerTestStep) {
        case 0: // First tone
            if (elapsed < 200) {
                tone(TEST_BUZZER_PIN_A4, 1000); // 1kHz
                if (elapsed == 0) Serial.println("Buzzer A4: 1000Hz");
            } else {
                noTone(TEST_BUZZER_PIN_A4);
                buzzerTestStep++;
                buzzerTestStartTime = millis();
            }
            break;
            
        case 1: // Pause
            if (elapsed >= 100) {
                buzzerTestStep++;
                buzzerTestStartTime = millis();
            }
            break;
            
        case 2: // Second tone
            if (elapsed < 200) {
                tone(TEST_BUZZER_PIN_A4, 1500); // 1.5kHz
                if (elapsed == 0) Serial.println("Buzzer A4: 1500Hz");
            } else {
                noTone(TEST_BUZZER_PIN_A4);
                buzzerTestStep++;
                buzzerTestStartTime = millis();
            }
            break;
            
        case 3: // Pause
            if (elapsed >= 100) {
                buzzerTestStep++;
                buzzerTestStartTime = millis();
            }
            break;
            
        case 4: // Third tone
            if (elapsed < 200) {
                tone(TEST_BUZZER_PIN_A4, 2000); // 2kHz
                if (elapsed == 0) Serial.println("Buzzer A4: 2000Hz");
            } else {
                noTone(TEST_BUZZER_PIN_A4);
                buzzerTestStep++;
                buzzerTestStartTime = millis();
            }
            break;
            
        case 5: // Final cleanup
            if (elapsed >= 100) {
                buzzerTestActive = false;
                buzzerTestStep = 0;
                Serial.println("Buzzer Test: Complete");
                
                // Force redraw to update status
                markForFullRedraw();
            }
            break;
    }
}

// Static callback wrappers

void HardwareTestScreen::ledTestCallback() {
    if (instance) {
        instance->onLEDTestSelected();
    }
}

void HardwareTestScreen::buzzerTestCallback() {
    if (instance) {
        instance->onBuzzerTestSelected();
    }
}

void HardwareTestScreen::backCallback() {
    if (instance) {
        instance->onBackSelected();
    }
}