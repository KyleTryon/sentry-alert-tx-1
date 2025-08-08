#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "../config/settings.h"

// Forward declarations
class UIManager;

// Button definitions that map to physical hardware
enum PhysicalButton {
  BUTTON_LEFT_UP,    // Maps to BUTTON_LEFT_UP_PIN
  BUTTON_LEFT_MID,   // Maps to BUTTON_LEFT_MID_PIN  
  BUTTON_LEFT_DOWN,  // Maps to BUTTON_LEFT_DOWN_PIN
  BUTTON_RIGHT,      // Maps to BUTTON_RIGHT_PIN
  BUTTON_COUNT
};

// Logical button actions for UI
enum ButtonAction {
  ACTION_UP,
  ACTION_DOWN, 
  ACTION_SELECT,
  ACTION_BACK,
  ACTION_NONE
};

// Button state tracking
struct ButtonState {
  bool pressed;
  bool longPressed;
  bool justPressed;
  bool justReleased;
  unsigned long pressStartTime;
  unsigned long lastDebounceTime;
  bool lastReading;
  
  ButtonState() : pressed(false), longPressed(false), justPressed(false), 
                  justReleased(false), pressStartTime(0), lastDebounceTime(0), 
                  lastReading(false) {}
};

// Input event structure
struct InputEvent {
  PhysicalButton button;
  ButtonAction action;
  bool isLongPress;
  unsigned long timestamp;
  
  InputEvent() : button(BUTTON_LEFT_UP), action(ACTION_NONE), 
                 isLongPress(false), timestamp(0) {}
  InputEvent(PhysicalButton b, ButtonAction a, bool longPress = false) 
    : button(b), action(a), isLongPress(longPress), timestamp(millis()) {}
};

class ButtonManager {
public:
  ButtonManager();
  ~ButtonManager() = default;
  
  // Core input processing
  void begin();
  void update();
  
  // Button state queries
  bool isButtonPressed(PhysicalButton button) const;
  bool isButtonLongPressed(PhysicalButton button) const;
  bool wasButtonJustPressed(PhysicalButton button) const;
  bool wasButtonJustReleased(PhysicalButton button) const;
  
  // Action mapping
  ButtonAction getButtonAction(PhysicalButton button) const;
  PhysicalButton getButtonForAction(ButtonAction action) const;
  
  // Event handling
  void setInputCallback(void (*callback)(const InputEvent&));
  bool hasPendingEvents() const;
  InputEvent getNextEvent();
  
  // Configuration
  void setDebounceTime(unsigned long time) { _debounceTime = time; }
  void setLongPressTime(unsigned long time) { _longPressTime = time; }
  unsigned long getDebounceTime() const { return _debounceTime; }
  unsigned long getLongPressTime() const { return _longPressTime; }
  
  // Hardware access
  int getButtonPin(PhysicalButton button) const;
  bool readButtonState(PhysicalButton button) const;
  
private:
  ButtonState _buttons[BUTTON_COUNT];
  
  // Timing configuration
  unsigned long _debounceTime;
  unsigned long _longPressTime;
  
  // Event queue
  static const int MAX_EVENTS = 8;
  InputEvent _eventQueue[MAX_EVENTS];
  int _eventHead;
  int _eventTail;
  int _eventCount;
  
  // Callback
  void (*_inputCallback)(const InputEvent&);
  
  // Private methods
  void updateButton(PhysicalButton button);
  void addEvent(const InputEvent& event);
  void clearEvents();
  void triggerButtonPress(PhysicalButton button);
  void triggerButtonRelease(PhysicalButton button);
  void triggerLongPress(PhysicalButton button);
};

#endif // BUTTON_MANAGER_H 