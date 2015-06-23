
#include <math.h>
#include <stdlib.h>

#include "common.h"

double clamp(double dmin, double dmax, double x)
{
    return fmax(dmin, fmin(dmax, x));
}

double frac(double x)
{
    return fmod(x, 1.0);
}

double hsin(double x)
{
    return 0.5 + 0.5 * sin(x);
}

// https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV

void rgb_from_hsv(
    double h, double s, double v,
    double *r, double *g, double *b)
{
  double c = v * s;
  double h_ = h * 6.0;
  double x = c * (1 - fabs(fmod(h_, 2.0) - 1));
  double m = v - c;
  double r_ = 0, g_ = 0, b_ = 0;

  switch ((int) trunc(h_)) {
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

void rgb_from_hue(double hue, double *r, double *g, double *b)
{
    rgb_from_hsv(hue, 1.0, 1.0, r, g, b);
}

void rgb_mul(double x, rgb *c)
{
    c->r *= x;
    c->g *= x;
    c->b *= x;
}

int euclidean_mod(int n, int m) {
    int i = ((n%m) + m) % m;

    if(i < 0) {
        return i + abs(m);
    } else {
        return i;
    }
}
