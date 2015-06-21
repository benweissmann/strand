
#define NLIGHTS 50

typedef struct
{
    double r;
    double g;
    double b;
} rgb;

typedef void (*pattern_update)(double delta, rgb *out);

struct pattern
{
    const char *name;
    void (*init)();
    void (*update)(double delta, rgb *out);
};

extern "C" {

double clamp(double dmin, double dmax, double x);
double frac(double x);
double hsin(double x);

void rgb_from_hsv(
    double h, double s, double v,
    double *r, double *g, double *b);
void rgb_from_hue(double hue, double *r, double *g, double *b);

}
