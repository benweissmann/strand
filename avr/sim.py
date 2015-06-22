
from __future__ import division

import cffi
import math
import time
import sys

import cairo
import numpy
from gi.repository import GLib, Gtk

def vbox(*ws):
    #box = Gtk.Box(Gtk.Orientation.HORIZONTAL, 0)
    box = Gtk.VBox(0)

    for (e, w) in ws:
        box.pack_start(w, e, True, 0)

    return box

class Lights(Gtk.DrawingArea):
    def __init__(self):
        Gtk.DrawingArea.__init__(self)
        self.nlights = 50
        self.colours = numpy.zeros(
            (self.nlights, 3), dtype=numpy.double)

    def do_draw(self, cr, *a):
        #print 'draw', a
        w = self.get_allocated_width()
        h = self.get_allocated_height()
        cr.rectangle(0, 0, w, h)
        cr.set_source_rgb(0, 0, 0)
        cr.fill()

        margin = w / 20
        ew = w - 2 * margin
        eh = h - 2 * margin

        #cr.set_source_rgb(1, 0, 0)
        #cr.rectangle(margin, margin, ew, eh)
        #cr.stroke()

        for i in xrange(self.nlights):
            cr.arc(
                margin + float(i) * ew / (self.nlights - 1),
                h / 2, 8, 0, 2 * math.pi)
            cr.set_source_rgb(0.5, 0.5, 0.5)
            cr.stroke_preserve()
            cr.set_source_rgb(*self.colours[i])
            cr.fill()

class Pattern:
    def reset(self):
        pass

    def update(self, dt, out):
        pass

class Library:
    _def = '''
        typedef struct {
            double r;
            double g;
            double b;
        } rgb;

        typedef struct
        {
            const char *name;
            void (*update)(double delta, rgb *out);
        } pattern;
        '''

    def __init__(self, path):
        self.ffi = cffi.FFI()
        self.ffi.cdef(self._def)
        self.lib = self.ffi.dlopen(path)

    def get_pattern(self, name):
        #self.ffi.cdef('pattern {0};'.format(name))
        self.ffi.cdef('void {0}(double, rgb *);'.format(name))
        #cpat = getattr(self.lib, name)
        #pat = Pattern()
        #pat.reset = cpat.init
        #pat.update = lambda dt, out: \
        #    cpat.update(dt, self.ffi.cast('rgb *', out.ctypes.data))
        update = getattr(self.lib, name)
        pat = Pattern()
        pat.reset = lambda: None
        pat.update = lambda dt, out: \
            update(dt, self.ffi.cast('rgb *', out.ctypes.data))
        return pat

def update(pat, speed_adj, lights):
    t = time.time()
    dt = speed_adj.get_value() * (t - lights.t)
    lights.t = t
    pat.update(dt, lights.colours)
    lights.queue_draw()
    return True

def main():
    args = sys.argv[1:]
    pat_name = args[0] if args else 'rainbow'

    lib = Library('./strand.so')
    pat = lib.get_pattern(pat_name)
    pat.reset()
    lights = Lights()
    lights.t = time.time()

    adj = Gtk.Adjustment(0.5, 0, 1, 0.01)
    scale = Gtk.Scale.new(Gtk.Orientation.HORIZONTAL, adj)
    scale.props.draw_value = 0
    #scale.props.digits = 2

    GLib.timeout_add(30, lambda: update(pat, adj, lights))

    win = Gtk.Window()
    win.add(vbox(
        (True, lights),
        (False, scale)))
    win.connect('destroy', lambda w: Gtk.main_quit())
    win.show_all()
    Gtk.main()

if __name__ == '__main__':
    main()
