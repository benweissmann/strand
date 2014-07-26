
import time

import numpy

import lights

# rgb = numpy.zeros((10, 3), numpy.float32)
start = time.time()

def draw():
    # global rgb
    now = time.time()
    dt = now - start
    phase = 2 * abs(0.5 - (dt / 3) % 1)
    # add = (1 - 10 * abs(numpy.linspace(0, 1, 10) - phase)).clip(0, 1)
    # rgb[:] = ((rgb * 0.8) + add.repeat(3).reshape((10, 3))).clip(0, 1)
    # rgb[:] = add.repeat(3).reshape((10, 3)).clip(0, 1)
    #print add
    #print
    x = numpy.linspace(0, 1, 10)
    lum = 2 / (1 + numpy.exp(60 * (x - phase) ** 2))
    rgb = lum.repeat(3).reshape((-1, 3))
    lights.draw_lights(rgb.astype(numpy.float32))
