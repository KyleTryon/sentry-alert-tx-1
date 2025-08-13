#include "Buzzer.h"

Buzzer::Buzzer() : _pin(-1) {}

void Buzzer::begin(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
}

void Buzzer::playTone(unsigned int frequency, unsigned long duration) {
  if (_pin != -1) {
    tone(_pin, frequency, duration);
  }
}

void Buzzer::noTone() {
  if (_pin != -1) {
    ::noTone(_pin);
  }
}
