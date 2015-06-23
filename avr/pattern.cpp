
#include <math.h>
#include <stdio.h>
#include <bitset>
#include "pattern.h"

#include <iostream>

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdlib.h>

double random(double max)
{
    return max * drand48();
}

double random(double min, double max)
{
    return min + (max - min) * drand48();
}
#endif

void rainbow(double dt, rgb *out)
{
    static double phase = 0;

    phase += 3.0 * dt;

    for (int i = 0; i < NLIGHTS; i++) {
        double r, g, b;
        float pos = (float)i / (float)NLIGHTS;
        rgb_from_hue(frac(pos + phase), &r, &g, &b);
        out[i] = (rgb){ r, g, b };
    }

    phase = frac(phase);
}

void alternating(double _dt, rgb out[])
{
    rgb red = { 1, 0, 0 };
    rgb green = { 0, 1, 0 };
    rgb blue = { 0, 0, 1 };

    for (int i = 0; i < NLIGHTS; i++) {
        switch (i % 3) {
            case 0: out[i] = red; break;
            case 1: out[i] = green; break;
            case 2: out[i] = blue; break;
        }
    }
}

void pattern0(double dt, rgb out[])
{
  static float time = 0;

  time += 2.0 * dt;

  for (int i = 0; i < NLIGHTS; i++) {
    double pos = (double)i / (double)NLIGHTS;
    double lum = 0.99 * hsin(time * 3 * (0.0 + 1.0 * pos));
    //lum = 0.05 + 0.95 * lum;
    //int ilum = int(255 * (1.0 - lum));
    double r, g, b;
    //double hue = frac(pos + 0.2 * hsin(time / 15.0));
    double hue = frac(0.3 * pos + time / 30.0);
    rgb_from_hue(hue, &r, &g, &b);
    out[i] = (rgb){ lum * r, lum * g, lum * b };
  }
}

void pattern1(double dt, rgb out[])
{
    static float time = 0;

    time += dt;

    for (int i = 0; i < NLIGHTS; i++) {
        double pos = (double)i / (double)NLIGHTS;
        double centre = hsin(time / 5.0);
        //double lum = 1.0 - pow(centre - pos, 10.0);
        //double lum = 1.0 / pow(fabs(pos - centre), 0.1);
        double d = pos - centre;
        //double lum = exp(pow(d * 100.0, 2.0));
        double lum = 1.0 - fabs(5.0 + 5.0 * hsin(time) * d);
        lum = fmin(fmax(0, lum), 1);
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
        out[i] = (rgb){ lum * r, lum * g, lum * b };
    }
}

void parabola(double dt, rgb out[])
{
    static float x = 0;
    static float hue = 0;
    float height = 1 - pow(x - 1, 2);

    x += dt;

    if (height < 0) {
        x = 0;
        height = 0;
        hue = frac(hue + 0.1);
    }

    for (int i = 0; i < NLIGHTS; i++) {
        float lum = 1.0 - clamp(0.0, 1.0, fabs(pow(i - height * (NLIGHTS - 1), 4.0)));
        double r, g, b;
        rgb_from_hue(hue, &r, &g, &b);
        out[i] = (rgb){ lum * r, lum * g, lum * b };
    }
}

void twinkle(double dt, rgb out[])
{
    static double hue[NLIGHTS] = { 0, };
    static double lum[NLIGHTS] = { 0, };

    for (int i = 0; i < NLIGHTS; i++) {
        lum[i] *= (1.0 - 0.5 * dt);

        if (lum[i] < 0.01 && random(1, 10000) < 200 * dt) {
            hue[i] = random(0, 255.0) / 255.0;
            lum[i] = 1.0;
        }

        double r, g, b;
        rgb_from_hue(hue[i], &r, &g, &b);
        out[i] = (rgb){ lum[i] * r, lum[i] * g, lum[i] * b };
    }
}

void climb(double dt, rgb out[])
{
    static double phase = 0;
    static double hue[NLIGHTS] = { 0, };
    static double lum[NLIGHTS] = { 0, };

    phase += 10 * dt;

    if (random(1000) < 200 * dt) {
        lum[0] = 1;
        hue[0] = random(0, 255) / 255.0;
    }

    for (int i = 0; i < NLIGHTS; i++) {
        double r, g, b;
        rgb_from_hue(hue[i], &r, &g, &b);
        out[i] = (rgb){ lum[i] * r, lum[i] * g, lum[i] * b };
    }

    if (phase > 1.0) {
        for (int i = NLIGHTS - 1; i > 0; i--) {
            lum[i] = lum[i - 1];
            hue[i] = hue[i - 1];
        }

        lum[0] = 0;
        phase = frac(phase);
    }
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

void climb2(double dt, rgb out[])
{
    static struct point points[POINTS] = { 0, };
    static rgb cols[NLIGHTS] = { 0, };

    for (int i = 0; i < NLIGHTS; i++) {
        double pos = (double)i / (double)NLIGHTS;
        double r = 0, g = 0, b = 0;

        for (int j = 0; j < POINTS; j++) {
            struct point *p = points + j;

            if (p->on) {
                //double d = 1000.0 * fabs(p->pos - pos);
                //double d = pow(0 * fabs(p->pos - pos), 5.0);
                double d = pow(20.0 * fabs(p->pos - pos), 5.0);
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

        //cols(fmin(1.0, r), fmin(1.0, g), fmin(1.0, b));
        cols[i].r = fmin(1.0, r);
        cols[i].g = fmin(1.0, g);
        cols[i].b = fmin(1.0, b);
    }

    for (int i = 0; i < NLIGHTS; i++) {
        out[i] = cols[i];
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
            p->pos += 2.0 * p->speed * dt;

            if (p->pos > 1.5) {
                p->on = false;
                p->pos = 0;
            }
        } else if (random(0, 10000) < 50.0 * dt) { // XXX: scale
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

void pulse(double dt, rgb out[])
{
    static double phase = 0.0;

    phase += dt / 2.0;

    for (int i = 0; i < NLIGHTS; i++) {
        double pos = (double)i / (double)NLIGHTS;
        double lum = 0.8 + 0.2 * sin(5.0 * (pos - phase));
        out[i] = (rgb){ lum, lum, lum };
    }
}

void flash(double dt, rgb out[])
{
    static double phase = 0.0;

    if (phase < 0.5) {
      for (int i = 0; i < NLIGHTS; i++) {
        double r, g, b;
        double pos = (double)i / (double)NLIGHTS;
        rgb_from_hue(pos, &r, &g, &b);
        out[i] = (rgb){ r, g, b };
      }
    } else {
      for (int i = 0; i < NLIGHTS; i++) {
        out[i] = (rgb){ 0, 0, 0 };
      }
    }

    phase = frac(phase + dt * 100);
}

void automaton(double dt, rgb out[], int wolframCode)
{
    // XXX: we should do this elsewhere
    srand48(time(NULL));

    static double phase = 1.0;

    // bitset representing alive cells
    static std::bitset<NLIGHTS> state;

    // hues for living cells
    static double hues [NLIGHTS];

    // number of generations since last resent
    static int numGenSinceReset = 0;


    phase += 10 * dt;

    if (phase >= 1.0) {
        // run a generation of the auotmaton

        std::bitset<NLIGHTS> newState;
        double newHues [NLIGHTS];
        if (state.none() || (numGenSinceReset > 250)) {
            // we re-initialize if all cells are dead, and also perodically
            for (int i = 0; i < 3; i++) {
                int light = random(0, NLIGHTS - 1);
                newState.set(light);
                newHues[light] = random(255) / 255;
            }

            numGenSinceReset = 0;
        }
        else {
            numGenSinceReset++;

            for (int i = 0; i < NLIGHTS; i++) {
                // we only want the light at i and the ones to the left and right
                int leftShift = NLIGHTS - i - 2;
                int rightShift = NLIGHTS - 3;

                if(leftShift < 0) {
                    rightShift -= leftShift;
                    leftShift = 0;
                }

                std::bitset<NLIGHTS> cellState  = (state << leftShift) >> rightShift;

                // edge conditions
                if(i == (NLIGHTS - 1)) {
                    // left edge, use the right edge as the top bit
                    cellState.set(2, state[0]);
                } else if (i == 0) {
                    // right edge, use the left edge as the bottom bit
                    cellState.set(0, state[NLIGHTS - 1]);
                }

                // we interpret the cell state as a 3-digit binary number and then
                // read that digit out of the wolfram code to determine whether the
                // cell is alive in the next interation
                if (wolframCode & (1 << cellState.to_ulong())) {
                    // cell lives
                    newState.set(i);

                    // we give it the average color of its parents
                    int nParents = 0;
                    double parentHueSum = 0;

                    for (int offset = -1; offset <= 1; offset++) {
                        int parentIdx = euclidean_mod(i + offset, NLIGHTS);

                        if (state[parentIdx]) {
                            nParents++;
                            parentHueSum += hues[parentIdx];
                        }
                    }

                    if (nParents) {
                        newHues[i] = parentHueSum / nParents;
                    } else {
                        newHues[i] = random(0, 255.0) / 255.0;
                    }
                }

            }
        }

        state = newState;

        for(int i = 0; i < NLIGHTS; i++) {
            hues[i] = newHues[i];
        }


        phase = frac(phase);
    }

    double lum = 0.5 + 0.5 * (1-phase);

    for( int i = 0; i < NLIGHTS; i++) {
        if (state[i]) {
            double r, g, b;
            rgb_from_hue(hues[i], &r, &g, &b);
            out[i] = (rgb){ r*lum, g*lum, b*lum };
        } else {
            out[i] = (rgb){ 0, 0, 0};
        }
    }

}

void automaton146(double dt, rgb out[]) { automaton(dt, out, 146); }
void automaton154(double dt, rgb out[]) { automaton(dt, out, 154); }
void automaton22(double dt, rgb out[]) { automaton(dt, out, 22); }

