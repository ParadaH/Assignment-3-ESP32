#include "Button.h"

Button::Button(int pin, bool *mode) : pin(pin), mode(mode), lastButtonState(LOW) {
    pinMode(pin, INPUT);
}

void Button::handleButton() {
    bool buttonState = digitalRead(pin);

    if (buttonState == HIGH && lastButtonState == LOW) {
        *mode = !(*mode);
    }
    lastButtonState = buttonState;
}
