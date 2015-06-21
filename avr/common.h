
#define NLIGHTS 50

struct rgb
{
    double r;
    double g;
    double b;
};

typedef void (*pattern_update)(double detla, struct rgb *out);

struct pattern
{
    const char *name;
    void (*init)();
    void (*update)(double delta, struct rgb *out);
};

extern "C" {

double frac(double x);

void rgb(double hue, double *r, double *g, double *b);

}
