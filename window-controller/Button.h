#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <Arduino.h>

class Button {
private:
    int pin;                
    bool lastButtonState;   
    bool *mode;             

public:
    Button(int pin, bool *mode);
    void handleButton();
};

#endif
