#include "Servomotor.h"

Servomotor::Servomotor() : pin(-1) {}

void Servomotor::attach(int pin) {
    this->pin = pin;
    motor.attach(pin);
}

void Servomotor::write(int angle) {
    if (pin != -1) {
        motor.write(angle);
    }
}