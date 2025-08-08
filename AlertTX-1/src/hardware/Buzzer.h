#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>
#include "../config/settings.h"

class Buzzer {
public:
  Buzzer();
  void begin(int pin);
  void playTone(unsigned int frequency, unsigned long duration);
  void noTone();
private:
  int _pin;
};
#endif // BUZZER_H
