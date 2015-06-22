
#include <math.h>
#include <Arduino.h>

#include "button.h"
#include "common.h"
#include "pattern.h"
#include "ws28xx.h"

#define NLIGHTS 50

static int mode = 0;
static double p0;
static double p1;
static double p2;
static Button *b;

// XXX: scale sublinearly
//int brightness(double x) { return (int)(1.0 + 254.0 * x); }
//int brightness(double x) { return (int)(1.0 + 254.0 * pow(p0 * x, 2.5)); }
int brightness(double x) { return 255.0 * p0 * x; }

double timedelta()
{
    static unsigned long prev = 0;

    if (prev == 0) {
        prev = micros();
        return 0;
    } else {
        unsigned long now = micros();
        unsigned long dt = now - prev;
        prev = now;
        return dt / 1.0e6;
    }
}

void read_params() {
    p0 = analogRead(0) / 1023.0;
    p1 = analogRead(1) / 1023.0;
    p2 = analogRead(2) / 1023.0;
#ifdef DEBUG
    Serial.print("p0 = ");
    Serial.println(p0);
    Serial.print("p1 = ");
    Serial.println(p1);
    Serial.print("p2 = ");
    Serial.println(p2);
#endif
}

void setup() {
    Serial.begin(9600);
    b = new Button(4);
}

static void (*patterns[])(double dt, rgb *out) = {
    pattern0,
    pattern1,
    rainbow,
    parabola,
    twinkle,
    climb2,
    NULL
};

void switch_mode()
{
    mode++;

    if (!patterns[mode])
        mode = 0;
}

void loop()
{
    double dt = p1 * timedelta();
    rgb cols[NLIGHTS];

    if (b->read())
        switch_mode();

    read_params();
    patterns[mode](dt, cols);

    begin();

    for (int i = 0; i < NLIGHTS; i++) {
        put_colour(
            brightness(cols[i].r),
            brightness(cols[i].g),
            brightness(cols[i].b));
    }

    end();
}
