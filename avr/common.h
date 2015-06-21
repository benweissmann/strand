
#define NLIGHTS 50

typedef struct
{
    double r;
    double g;
    double b;
} colour;

typedef void (*pattern_update)(double delta, colour *out);

struct pattern
{
    const char *name;
    void (*init)();
    void (*update)(double delta, colour *out);
};

extern "C" {

double frac(double x);

void rgb(double hue, double *r, double *g, double *b);

}
