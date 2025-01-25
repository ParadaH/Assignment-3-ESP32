#include "Led.h"

Led::Led(int greenPin, int redPin) {
    this->greenPin = greenPin;
    this->redPin = redPin;
}

void Led::init() {
    pinMode(greenPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    turnOff(); // Upewniamy się, że diody są wyłączone na początku
}

void Led::setGreen() {
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
}

void Led::setRed() {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
}

void Led::turnOff() {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, LOW);
}