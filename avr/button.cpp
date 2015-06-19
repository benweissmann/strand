
#include <Arduino.h>

#include "button.h"

Button::Button(int pin_)
{
    pin = pin_;
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

int Button::read(void)
{
     int state = 0;

    for (int i = 0; i < 5; i++) {
        state += !digitalRead(pin);
        delayMicroseconds(10);
    }

    state = state >= 3;
    int edge = state && !pressed;
    pressed = state;
    return edge;
}

