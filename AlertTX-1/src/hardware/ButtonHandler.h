#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H
#include <Arduino.h>
#include "../config/settings.h"

enum ButtonEvent {
  NONE,
  SHORT_PRESS_LEFT_UP,
  SHORT_PRESS_LEFT_MID,
  SHORT_PRESS_LEFT_DOWN,
  SHORT_PRESS_RIGHT,
  LONG_PRESS_LEFT_UP,
  LONG_PRESS_LEFT_MID,
  LONG_PRESS_LEFT_DOWN,
  LONG_PRESS_RIGHT
};

class ButtonHandler {
public:
  ButtonHandler();
  void begin();
  ButtonEvent getButtonEvent();
private:
  long _lastPressTime[4];
  bool _buttonState[4];
  int _buttonPins[4] = {BUTTON_LEFT_UP_PIN, BUTTON_LEFT_MID_PIN, BUTTON_LEFT_DOWN_PIN, BUTTON_RIGHT_PIN};
};
#endif // BUTTONHANDLER_H
