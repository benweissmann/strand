# $ python platform.py hue.py

import colorsys
import time

import numpy

import gl
import lights

start = time.time()
# lights = numpy.random.uniform(0, 1, (10, 3)).astype(numpy.float32)
# lights = numpy.linspace(0, 1, 10).astype(numpy.float32)
rgb = numpy.zeros((10, 3), numpy.float32)
hsv_to_rgb = numpy.vectorize(colorsys.hsv_to_rgb)

def draw():
    gl.clear((0, 1, 0, 1))

    t = time.time() - start
    (r, g, b) = hsv_to_rgb(
        ((((t / 5.0) % 1.0 - numpy.linspace(0, 1, 10)) % 1)),
          numpy.repeat(0.5, 10),
          numpy.repeat(1.0, 10))
    rgb[:,0] = r
    rgb[:,1] = g
    rgb[:,2] = b
    lights.draw_lights(rgb)
