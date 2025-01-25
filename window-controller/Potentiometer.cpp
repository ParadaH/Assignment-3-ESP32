#include "Potentiometer.h"

Potentiometer::Potentiometer(int pin) : pin(pin) {
    pinMode(pin, INPUT);
}

int Potentiometer::read() {
    int manualValue = analogRead(pin);
    return map(manualValue, 0, 1023, 0, 100);
}
