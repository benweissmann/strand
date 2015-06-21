
#include <math.h>

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

void rgb(double hue, double *r, double *g, double *b)
{
  double s = 1.0;
  double v = 1.0;
  double c = v * s;
  double h = hue * 6.0;
  double x = c * (1 - fabs(fmod(h, 2.0) - 1));
  double m = v - c;
  double r_ = 0, g_ = 0, b_ = 0;

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

