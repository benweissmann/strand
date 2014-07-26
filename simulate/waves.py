
import time

import numpy

import lights

def hsin(x):
    return 0.5 + 0.5 * numpy.sin(x)

def draw():
    t = time.time()
    x = numpy.linspace(0, 1, 10)
    lum = hsin(1 + numpy.exp(10 * x))
    print lum
    rgb = numpy.zeros((10, 3), numpy.float32)
    rgb[:,2] = lum
    lights.draw_lights(rgb)
