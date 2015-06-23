
#include "common.h"

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
PATTERN(automaton146)
PATTERN(automaton154)
PATTERN(automaton22)
}
