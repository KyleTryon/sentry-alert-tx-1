#include "ButtonHandler.h"

ButtonHandler::ButtonHandler() {
  for (int i = 0; i < 4; i++) {
    _lastPressTime[i] = 0;
    _buttonState[i] = HIGH; // Assuming pull-up resistors
  }
}

void ButtonHandler::begin() {
  for (int i = 0; i < 4; i++) {
    pinMode(_buttonPins[i], INPUT_PULLUP);
  }
}

ButtonEvent ButtonHandler::getButtonEvent() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(_buttonPins[i]);
    if (reading != _buttonState[i]) {
      // Debounce delay
      delay(50);
      reading = digitalRead(_buttonPins[i]);
      if (reading != _buttonState[i]) {
        _buttonState[i] = reading;
        if (_buttonState[i] == LOW) { // Button pressed
          _lastPressTime[i] = millis();
        } else { // Button released
          long pressDuration = millis() - _lastPressTime[i];
          if (pressDuration >= LONG_PRESS_THRESHOLD_MS) {
            switch (i) {
              case 0: return LONG_PRESS_LEFT_UP;
              case 1: return LONG_PRESS_LEFT_MID;
              case 2: return LONG_PRESS_LEFT_DOWN;
              case 3: return LONG_PRESS_RIGHT;
            }
          } else {
            switch (i) {
              case 0: return SHORT_PRESS_LEFT_UP;
              case 1: return SHORT_PRESS_LEFT_MID;
              case 2: return SHORT_PRESS_LEFT_DOWN;
              case 3: return SHORT_PRESS_RIGHT;
            }
          }
        }
      }
    }
  }
  return NONE;
}
