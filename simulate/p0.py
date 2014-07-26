
import time

import numpy

import lights

def hsin(x):
    return 0.5 + 0.5 * numpy.sin(x)

def draw():
    t = time.time()
    x = numpy.linspace(0.25, 0.75, 10)
    p = (t / 2) % 1
    r = 2 / (1 + numpy.exp(20 * (x - p) ** 2.0))
    print x
    print p
    print (x - p)
    print r
    print
    rgb = numpy.zeros((10, 3), numpy.float32)
    rgb[:,0] = r
    lights.draw_lights(rgb)
