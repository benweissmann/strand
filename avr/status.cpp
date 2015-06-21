
#include <Arduino.h>
#include "status.h"

#define BLINK_LEN 500

Status::Status(int pin_)
{
    pin = pin_;
    blink_level = -1;
    level = 0;
    pinMode(pin, OUTPUT);
}

void Status::set(float level_)
{
    level = level_;
}

void Status::blink(float level_)
{
    blink_t = millis();
    blink_level = level_;
}

void Status::update()
{
    if (blink_level >= 0) {
        if (blink_t + BLINK_LEN < millis()) {
            analogWrite(pin, 255 * blink_level);
            return;
        } else {
            blink_level = -1;
        } 
    }

    analogWrite(pin, 255 * level);
}
