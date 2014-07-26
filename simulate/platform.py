# OpenGL live code wrapper
# $ python platform.py SCRIPT.py

import os
import sys

import glut

class Platform(glut.GLUTApp):
    def __init__(self, path):
        self.path = path
        self.ts = None
        self.draw = None

    def _load(self):
        new_ts = os.path.getctime(self.path)

        if self.ts is None or new_ts > self.ts:
            print 'load'
            self.ts = new_ts
            self.draw = load(self.path)
            self.postRedisplay()

        self.setTimer(1000, self._load)

    def _redisplay(self):
        self.postRedisplay()
        self.setTimer(30, self._redisplay)

    def setup(self):
        self._load()
        self.setTimer(30, self._redisplay)
        self.setTimer(1000, self._load)

    def display(self):
        if self.draw is not None:
            self.draw()

def load(path):
    src = file(path).read()
    code = compile(src, path, 'exec')
    env = {}
    exec code in env, env
    draw = env.get('draw')

    if draw is None:
        raise RuntimeError("no draw")

    return draw

def main():
    (path,) = sys.argv[1:]
    Platform(path).run()

if __name__ == '__main__':
    main()
