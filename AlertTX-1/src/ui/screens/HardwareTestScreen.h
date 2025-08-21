#ifndef HARDWARE_TEST_SCREEN_H
#define HARDWARE_TEST_SCREEN_H

#include "../core/Screen.h"
#include "../core/DisplayUtils.h"
#include "../components/MenuContainer.h"
#include "../../hardware/LED.h"

/**
 * @brief Hardware Test Screen for testing buzzer and LED functionality
 * 
 * This screen provides a simple interface to test the hardware components:
 * - LED test: Turns on the LED for 1 second
 * - Buzzer test: Plays a simple tone sequence
 * 
 * Useful for debugging hardware connections and verifying pin assignments.
 */
class HardwareTestScreen : public Screen {
private:
    MenuContainer* menu;
    
    // Hardware reference
    LED* ledRef;
    
    // Test state
    bool ledTestActive;
    unsigned long ledTestStartTime;
    bool buzzerTestActive;
    unsigned long buzzerTestStartTime;
    int buzzerTestStep;
    
    // Static instance for callbacks
    static HardwareTestScreen* instance;
    
public:
    HardwareTestScreen(Adafruit_ST7789* display, LED* led);
    virtual ~HardwareTestScreen();
    
    // Screen interface implementation
    void enter() override;
    void exit() override;
    void update() override;
    void draw() override;
    void handleButtonPress(int button) override;
    
    // Hardware test methods
    void onLEDTestSelected();
    void onBuzzerTestSelected();
    void onBackSelected();
    
    // Static callback wrappers
    static void ledTestCallback();
    static void buzzerTestCallback();
    static void backCallback();
    
private:
    void setupMenu();
    void updateLEDTest();
    void updateBuzzerTest();
    
    // Centralized rendering methods
    void drawStaticContent();
    void drawDynamicContent();
};

#endif // HARDWARE_TEST_SCREEN_H
