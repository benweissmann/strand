
import time

import numpy

import lights

def s(ta, tb):
    return 0.5 + 0.5 * numpy.sin(numpy.linspace(ta, tb, 10))

def draw():
    t = time.time()
    r = s(t, t + 10)
    g = s(t + 3, t + 10)
    b = s(t + 6, t + 10)
    rgb = numpy.dstack((r, g, b))[0]
    lights.draw_lights(rgb.astype(numpy.float32))
