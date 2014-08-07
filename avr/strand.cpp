
#include <math.h>
#include <Arduino.h>

#include "ws28xx.h"

#define NLIGHTS 50

void blink() {
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  delay(100);
}

void write() {
  for (int i = 0; i < NLIGHTS; i++) {
    colour(0, 0, 0);
  }
}

void setup() {
}

double hsin(double x) { return 0.5 + 0.5 * sin(x); }

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV

void rgb(double hue, double *r, double *g, double *b) {
  double s = 1.0;
  double v = 1.0;
  double c = v * s;
  double h = hue * 6.0;
  double x = c * (1 - fabs(fmod(h, 2.0) - 1));
  double m = v - c;
  double r_, g_, b_;

  switch ((int) trunc(h)) {
  case 0: r_ = c; g_ = x; b_ = 0; break;
  case 1: r_ = x; g_ = c; b_ = 0; break;
  case 2: r_ = 0; g_ = c; b_ = x; break;
  case 3: r_ = 0; g_ = x; b_ = c; break;
  case 4: r_ = x; g_ = 0; b_ = c; break;
  case 5: r_ = c; g_ = 0; b_ = x; break;
  }

  *r = r_ + m;
  *g = g_ + m;
  *b = b_ + m;
}

void rainbow() {
  for (int i = 0; i < NLIGHTS; i++) {
    double r, g, b;
    rgb((float)i / (float)NLIGHTS, &r, &g, &b);
    colour(255 * r, 255 * g, 255 * b);
  }
}

double timedelta() {
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

void alternating() {
  for (int i = 0; i < NLIGHTS; i++) {
    switch (i % 3) {
    case 0: colour(0, 0, 255); break;
    case 1: colour(0, 255, 0); break;
    case 2: colour(255, 0, 0); break;
    }
  }
}

//int brightness(double x) { return (int)(1.0 + 254.0 * x); }
int brightness(double x) { return 255 * x; }

void pattern0() {
  float time = millis() / 1000.0;
  Serial.println(time);

  // ???
  colour(0, 0, 0);

  for (int i = 0; i < NLIGHTS; i++) {
    double pos = (double)i / (double)NLIGHTS;
    double lum = 0.99 * hsin(time * 3 * (0.0 + 1.0 * pos));
    //lum = 0.05 + 0.95 * lum;
    //int ilum = int(255 * (1.0 - lum));
    double r, g, b;
    //double hue = fmod(pos + 0.2 * hsin(time / 15.0), 1.0);
    double hue = fmod(0.3 * pos + time / 30.0, 1.0);
    rgb(hue, &r, &g, &b);
    // colour(255 * lum * r, 255 * lum * g, 255 * lum * b);
    // colour(255 * lum, 255 * lum, 255 * lum);
    colour(brightness(lum * r), brightness(lum * g), brightness(lum * b));
  }
}

void pattern1() {
  float time = millis() / 1000.0;

  for (int i = 0; i < NLIGHTS; i++) {
    double pos = (double)i / (double)NLIGHTS;
    double centre = hsin(time / 5.0);
    //double lum = 1.0 - pow(centre - pos, 10.0);
    //double lum = 1.0 / pow(abs(pos - centre), 0.1);
    double d = pos - centre;
    //double lum = exp(pow(d * 100.0, 2.0));
    double lum = 1.0 - abs(5.0 + 5.0 * hsin(time) * d);
    lum = min(max(0, lum), 1);
    // Serial.print(i);
    // Serial.print(" ");
    // Serial.print(pos);
    // Serial.print(" ");
    // Serial.println(lum);
    // Serial.println(brightness(lum));


    // if (i == 25) {
    //   colour(255, 0, 255);
    //   // colour(0, 0, 32 * hsin(time));
    // } else {
    //   //colour(0, brightness(lum), brightness(1.0 - lum));
    //   //colour(0, 0, 64 * hsin(time));
    //   //colour(0, 0, 64 * (1.0 - lum));
    //   colour(0, brightness(lum), 0);
    // }

    //colour(0, brightness(lum) >> 2, 0);

    double r, g, b;
    rgb(d, &r, &g, &b);
    colour(brightness(lum * r), brightness(lum * g), brightness(lum * b));
  }
}

void loop() {
  // Serial.begin(9600);
  begin();
  // rainbow();
  // alternating();
  pattern0();
  // pattern1();
  end();
}
