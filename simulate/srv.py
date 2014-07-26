
from __future__ import division

import select
import socket

import numpy

import glut
import lights

class Listen(object):
    def __init__(self, path):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
        self.sock.bind(path)

    def poll(self, timeout=None):
        (rfd, _, _) = select.select([self.sock], [], [], timeout)

        if rfd:
            return self.sock.recv(65536)
        else:
            return None

class Srv(glut.GLUTApp):
    def __init__(self):
        glut.GLUTApp.__init__(self)
        self.rgb = numpy.zeros((10, 3), numpy.float32)
        self.listen = Listen('.socket')

    def setup(self):
        self.setTimer(30, self._redisplay)
        self.setTimer(1000, self._poll)

    def _poll(self):
        msg = self.listen.poll(0)

        if msg is not None:
            msglen = 30
            msg = numpy.fromstring(msg + '\0' * msglen, numpy.uint8)[:msglen]
            self.rgb[:,:] = (msg / 255.0).reshape((-1, 3))
            print self.rgb

        self.setTimer(1000, self._poll)

    def _redisplay(self):
        self.postRedisplay()
        self.setTimer(30, self._redisplay)

    def display(self):
        lights.draw_lights(self.rgb)

def main():
    # listen = Listen('.socket')

    # while True:
    #     print repr(listen.poll())

    Srv().run()

if __name__ == '__main__':
    main()
