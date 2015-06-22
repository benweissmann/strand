
#if 0
struct pattern
{
    void (*init)();
    void (*update)(double delta, struct rgb *out);
};
#endif

#include "common.h"

#if 0
pattern_update rainbow;
pattern_update alternating;
pattern_update pattern0;
pattern_update pattern1;
pattern_update climb;
pattern_update climb2;
pattern_update parabola;
pattern_update pulse;
pattern_update twinkle;
#endif

#define PATTERN(X) \
    void X(double dt, rgb out[]);

extern "C" {
PATTERN(rainbow)
PATTERN(alternating)
PATTERN(pattern0)
PATTERN(pattern1)
PATTERN(climb)
PATTERN(climb2)
PATTERN(parabola)
PATTERN(pulse)
PATTERN(twinkle)
PATTERN(flash)
}
