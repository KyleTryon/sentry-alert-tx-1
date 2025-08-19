#include "LED.h"

LED::LED() : _pin(-1), _blinkStartTime(0), _blinkDuration(0), _isBlinking(false) {}

void LED::begin(int pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
  off();
}

void LED::on() {
  digitalWrite(_pin, HIGH);
  _isBlinking = false;
}

void LED::off() {
  digitalWrite(_pin, LOW);
  _isBlinking = false;
}

void LED::blink(unsigned long durationMs) {
  _blinkStartTime = millis();
  _blinkDuration = durationMs;
  _isBlinking = true;
  on(); // Start blink by turning on
}

void LED::update() {
  if (_isBlinking && (millis() - _blinkStartTime >= _blinkDuration)) {
    off();
    _isBlinking = false;
  }
}
