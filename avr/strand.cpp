
#include <math.h>
#include <Arduino.h>

#include "button.h"
#include "common.h"
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

void col(double r, double g, double b)
{
    put_colour(brightness(r), brightness(g), brightness(b));
}

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

void rainbow(double dt)
{
    // XXX: scale this logarithmically
    double speed = 3.0 * p1;
    static double phase = 0;

    phase += speed * dt;

    for (int i = 0; i < NLIGHTS; i++) {
        double r, g, b;
        float pos = (float)i / (float)NLIGHTS;
        rgb_from_hue(frac(pos + phase), &r, &g, &b);
        col(r, g, b);
    }

    phase = frac(phase);
}

void alternating(double _dt)
{
  for (int i = 0; i < NLIGHTS; i++) {
    switch (i % 3) {
    case 0: col(0, 0, 1); break;
    case 1: col(0, 1, 0); break;
    case 2: col(1, 0, 0); break;
    }
  }
}

void pattern0(double dt)
{
  float speed = 2.0 * p1;
  static float time = 0;

  time += dt * speed;

  for (int i = 0; i < NLIGHTS; i++) {
    double pos = (double)i / (double)NLIGHTS;
    double lum = 0.99 * hsin(time * 3 * (0.0 + 1.0 * pos));
    //lum = 0.05 + 0.95 * lum;
    //int ilum = int(255 * (1.0 - lum));
    double r, g, b;
    //double hue = frac(pos + 0.2 * hsin(time / 15.0));
    double hue = frac(0.3 * pos + time / 30.0);
    rgb_from_hue(hue, &r, &g, &b);
    //hue += 123.45;
    //rgb_from_hue(frac(hue), &r, &g, &b);
    // colour(255 * lum * r, 255 * lum * g, 255 * lum * b);
    // colour(255 * lum, 255 * lum, 255 * lum);
    col(lum * r, lum * g, lum * b);
  }
}

void pattern1(double _dt)
{
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
    rgb_from_hue(d, &r, &g, &b);
    col(lum * r, lum * g, lum * b);
  }
}

void parabola(double dt)
{
    static float x = 0;
    static float hue = 0;
    x += p1 * dt;
    float height = 1 - pow(x - 1, 2);

    if (height < 0) {
        x = 0;
        height = 0;
        hue = frac(hue + 0.1);
    }

    for (int i = 0; i < NLIGHTS; i++) {
        float lum = 1.0 - clamp(0.0, 1.0, abs(pow(i - height * (NLIGHTS - 1), 4.0)));
        double r, g, b;
        rgb_from_hue(hue, &r, &g, &b);
        col(lum * r, lum * g, lum * b);
    }
}

void twinkle(double dt)
{
    static double hue[NLIGHTS] = { 0, };
    static double lum[NLIGHTS] = { 0, };

    for (int i = 0; i < NLIGHTS; i++) {
        lum[i] *= 0.9;

        if (lum[i] < 0.01 && random(1, 10000) < 20) {
            hue[i] = random(0, 255.0) / 255.0;
            lum[i] = 1.0;
        }

        double r, g, b;
        rgb_from_hue(hue[i], &r, &g, &b);
        col(lum[i] * r, lum[i] * g, lum[i] * b);
    }

    // XXX
    delay(30);
}

void climb(double dt)
{
    static double hue[NLIGHTS] = { 0, };
    static double lum[NLIGHTS] = { 0, };

    if (random(1000) < 50) {
        lum[0] = 1;
        hue[0] = random(0, 255) / 255.0;
    } else {
        lum[0] = 0;
    }

    for (int i = 0; i < NLIGHTS; i++) {
        double r, g, b;
        rgb_from_hue(hue[i], &r, &g, &b);
        col(lum[i] * r, lum[i] * g, lum[i] * b);
    }

    for (int i = NLIGHTS - 1; i > 0; i--) {
        lum[i] = lum[i - 1];
        hue[i] = hue[i - 1];
    }

    // XXX
    delay(200);
}

struct point {
    bool on;
    double pos;
    double speed;
    double r;
    double g;
    double b;
};

#define POINTS 4

void climb2(double dt)
{
    static struct point points[POINTS] = { 0, };
    static rgb cols[NLIGHTS] = { 0, };
    double td = timedelta();

    for (int i = 0; i < NLIGHTS; i++) {
        double pos = (double)i / (double)NLIGHTS;
        double r = 0, g = 0, b = 0;

        for (int j = 0; j < POINTS; j++) {
            struct point *p = points + j;

            if (p->on) {
                //double d = 1000.0 * abs(p->pos - pos);
                //double d = pow(0 * abs(p->pos - pos), 5.0);
                double d = pow(20.0 * abs(p->pos - pos), 5.0);
#if 0
                Serial.print(j);
                Serial.print(" : ");
                Serial.print(p->pos);
                Serial.print(" - ");
                Serial.print(pos);
                Serial.print(" ; ");
                Serial.print(d);
                Serial.println();
                Serial.print(d);
                Serial.print(" ");
                Serial.print(p->r);
                Serial.print(" ");
                Serial.print(p->g);
                Serial.print(" ");
                Serial.print(p->b);
                Serial.println();
#endif
                r += p->r / d;
                g += p->g / d;
                b += p->b / d;
            }
        }

        //cols(min(1.0, r), min(1.0, g), min(1.0, b));
        cols[i].r = min(1.0, r);
        cols[i].g = min(1.0, g);
        cols[i].b = min(1.0, b);
    }

    for (int i = 0; i < NLIGHTS; i++) {
        rgb *c = cols + i;
        col(c->r, c->g, c->b);
    }

#if 0
    for (int j = 0; j < POINTS; j++) {
        struct point p = points[j];
        Serial.print(p.on);
        Serial.print(" ");
    }

    Serial.println();

    for (int j = 0; j < POINTS; j++) {
        struct point p = points[j];
        Serial.print(p.pos);
        Serial.print(" ");
    }

    Serial.println();
#endif

    for (int j = 0; j < POINTS; j++) {
        struct point *p = points + j;

        if (p->on) {
            p->pos += 2.0 * p1 * p->speed * td;

            if (p->pos > 1.5) {
                p->on = false;
                p->pos = 0;
            }
        } else if (random(0, 10000) < 50.0 * p1) {
            p->on = true;
            p->pos = 0;
            p->speed = random(3, 20) / 10.0;
            //double r, g, b;
            double hue = random(0, 255) / 255.0;
            rgb_from_hue(hue, &p->r, &p->g, &p->b);

#if 0
            Serial.print(p->r);
            Serial.print(" ");
            Serial.print(p->g);
            Serial.print(" ");
            Serial.print(p->b);
            Serial.println();
#endif
        }
    }
}

void pulse(double dt)
{
    static double phase = 0.0;

    //phase += p1 * dt / 2.0;
    phase += dt / 2.0;

    for (int i = 0; i < NLIGHTS; i++) {
        double pos = (double)i / (double)NLIGHTS;
        double lum = 0.8 + 0.2 * sin(5.0 * (pos - phase));
        col(lum, lum, lum);
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

void loop()
{
  double dt = timedelta();
  read_params();
  //p0 = 1.0;
  begin();

  if (b->read())
    mode++;

  switch (mode) {
     case 0: pattern0(dt); break;
     case 1: pattern1(dt); break;
     case 2: rainbow(dt); break;
     case 3: parabola(dt); break;
     case 4: twinkle(dt); break;
     case 5: climb2(dt); break;
     default: mode = 0; break;
  }

  end();
}
