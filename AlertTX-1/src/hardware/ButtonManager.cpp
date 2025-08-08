#include "ButtonManager.h"

ButtonManager::ButtonManager() 
  : _debounceTime(50), _longPressTime(LONG_PRESS_THRESHOLD_MS),
    _eventHead(0), _eventTail(0), _eventCount(0), _inputCallback(nullptr) {
}

void ButtonManager::begin() {
  // Configure button pins as inputs with pull-up resistors
  pinMode(BUTTON_LEFT_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_MID_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  
  // Initialize button states
  for (int i = 0; i < BUTTON_COUNT; i++) {
    _buttons[i] = ButtonState();
  }
  
  // Clear event queue
  clearEvents();
}

void ButtonManager::update() {
  // Update all buttons
  updateButton(BUTTON_LEFT_UP);
  updateButton(BUTTON_LEFT_MID);
  updateButton(BUTTON_LEFT_DOWN);
  updateButton(BUTTON_RIGHT);
  
  // Process events
  while (hasPendingEvents()) {
    InputEvent event = getNextEvent();
    if (_inputCallback) {
      _inputCallback(event);
    }
  }
}

void ButtonManager::updateButton(PhysicalButton button) {
  ButtonState& state = _buttons[button];
  bool currentReading = !readButtonState(button); // Invert because of pull-up
  
  // Reset just pressed/released flags
  state.justPressed = false;
  state.justReleased = false;
  
  // Debounce logic
  if (currentReading != state.lastReading) {
    state.lastDebounceTime = millis();
  }
  
  if ((millis() - state.lastDebounceTime) > _debounceTime) {
    // Button state has stabilized
    
    if (currentReading && !state.pressed) {
      // Button was just pressed
      state.pressed = true;
      state.justPressed = true;
      state.pressStartTime = millis();
      state.longPressed = false;
      
      triggerButtonPress(button);
    } else if (!currentReading && state.pressed) {
      // Button was just released
      state.pressed = false;
      state.justReleased = true;
      state.longPressed = false;
      
      triggerButtonRelease(button);
    }
    
    // Check for long press
    if (state.pressed && !state.longPressed) {
      if ((millis() - state.pressStartTime) > _longPressTime) {
        state.longPressed = true;
        triggerLongPress(button);
      }
    }
  }
  
  state.lastReading = currentReading;
}

bool ButtonManager::readButtonState(PhysicalButton button) const {
  int pin = getButtonPin(button);
  return digitalRead(pin);
}

bool ButtonManager::isButtonPressed(PhysicalButton button) const {
  if (button >= 0 && button < BUTTON_COUNT) {
    return _buttons[button].pressed;
  }
  return false;
}

bool ButtonManager::isButtonLongPressed(PhysicalButton button) const {
  if (button >= 0 && button < BUTTON_COUNT) {
    return _buttons[button].longPressed;
  }
  return false;
}

bool ButtonManager::wasButtonJustPressed(PhysicalButton button) const {
  if (button >= 0 && button < BUTTON_COUNT) {
    return _buttons[button].justPressed;
  }
  return false;
}

bool ButtonManager::wasButtonJustReleased(PhysicalButton button) const {
  if (button >= 0 && button < BUTTON_COUNT) {
    return _buttons[button].justReleased;
  }
  return false;
}

ButtonAction ButtonManager::getButtonAction(PhysicalButton button) const {
  switch (button) {
    case BUTTON_LEFT_UP:
      return ACTION_UP;
    case BUTTON_LEFT_DOWN:
      return ACTION_DOWN;
    case BUTTON_LEFT_MID:
      return ACTION_SELECT;
    case BUTTON_RIGHT:
      return ACTION_BACK;
    default:
      return ACTION_NONE;
  }
}

PhysicalButton ButtonManager::getButtonForAction(ButtonAction action) const {
  switch (action) {
    case ACTION_UP:
      return BUTTON_LEFT_UP;
    case ACTION_DOWN:
      return BUTTON_LEFT_DOWN;
    case ACTION_SELECT:
      return BUTTON_LEFT_MID;
    case ACTION_BACK:
      return BUTTON_RIGHT;
    default:
      return BUTTON_LEFT_UP; // Default fallback
  }
}

void ButtonManager::setInputCallback(void (*callback)(const InputEvent&)) {
  _inputCallback = callback;
}

bool ButtonManager::hasPendingEvents() const {
  return _eventCount > 0;
}

InputEvent ButtonManager::getNextEvent() {
  if (_eventCount == 0) {
    return InputEvent(); // Return empty event
  }
  
  InputEvent event = _eventQueue[_eventHead];
  _eventHead = (_eventHead + 1) % MAX_EVENTS;
  _eventCount--;
  
  return event;
}

int ButtonManager::getButtonPin(PhysicalButton button) const {
  switch (button) {
    case BUTTON_LEFT_UP:
      return BUTTON_LEFT_UP_PIN;
    case BUTTON_LEFT_MID:
      return BUTTON_LEFT_MID_PIN;
    case BUTTON_LEFT_DOWN:
      return BUTTON_LEFT_DOWN_PIN;
    case BUTTON_RIGHT:
      return BUTTON_RIGHT_PIN;
    default:
      return -1;
  }
}

void ButtonManager::addEvent(const InputEvent& event) {
  if (_eventCount < MAX_EVENTS) {
    _eventQueue[_eventTail] = event;
    _eventTail = (_eventTail + 1) % MAX_EVENTS;
    _eventCount++;
  }
}

void ButtonManager::clearEvents() {
  _eventHead = 0;
  _eventTail = 0;
  _eventCount = 0;
}

void ButtonManager::triggerButtonPress(PhysicalButton button) {
  ButtonAction action = getButtonAction(button);
  InputEvent event(button, action, false);
  addEvent(event);
}

void ButtonManager::triggerButtonRelease(PhysicalButton button) {
  // Button release events are typically not needed for UI
  // but could be useful for some applications
}

void ButtonManager::triggerLongPress(PhysicalButton button) {
  ButtonAction action = getButtonAction(button);
  InputEvent event(button, action, true);
  addEvent(event);
} 