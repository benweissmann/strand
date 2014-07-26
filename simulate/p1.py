
import time

import numpy

import lights

#start = time.time()

def hsin(x):
    return 0.5 + 0.5 * numpy.sin(x)

def draw():
    t = time.time()
    cs = numpy.linspace(1, 3, 10)
    lum = hsin(t * cs)
    rgb = lum.repeat(3).reshape((10, 3))
    lights.draw_lights(rgb.astype(numpy.float32))
