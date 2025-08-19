#ifndef LED_H
#define LED_H
#include <Arduino.h>
#include "../config/settings.h"

class LED {
public:
  LED();
  void begin(int pin);
  void on();
  void off();
  void blink(unsigned long durationMs);
  void update(); // call repeatedly to service blink
private:
  int _pin;
  unsigned long _blinkStartTime;
  unsigned long _blinkDuration;
  bool _isBlinking;
};
#endif // LED_H
