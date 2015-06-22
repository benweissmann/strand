
from __future__ import division

import cffi
import math
import time
import sys

import cairo
import numpy
from gi.repository import GLib, Gtk

def pack_box(box, ws):
    for (e, w) in ws:
        box.pack_start(w, e, True, 0)

def hbox(*ws):
    box = Gtk.HBox(0)
    pack_box(box, ws)
    return box

def vbox(*ws):
    #box = Gtk.Box(Gtk.Orientation.HORIZONTAL, 0)
    box = Gtk.VBox(0)
    pack_box(box, ws)
    return box

class Lights(Gtk.DrawingArea):
    def __init__(self):
        Gtk.DrawingArea.__init__(self)
        self.nlights = 50
        self.colours = numpy.zeros(
            (self.nlights, 3), dtype=numpy.double)
        self._draw_lights = self._draw_linear

    def _draw_light(self, cr, x, y, colour):
        cr.arc(x, y, 8, 0, 2 * math.pi)
        cr.set_source_rgb(0.5, 0.5, 0.5)
        cr.stroke_preserve()
        cr.set_source_rgb(*colour)
        cr.fill()

    def _draw_linear(self, w, h, cr):
        margin = w / 20
        ew = w - 2 * margin
        eh = h - 2 * margin

        #cr.set_source_rgb(1, 0, 0)
        #cr.rectangle(margin, margin, ew, eh)
        #cr.stroke()

        for i in xrange(self.nlights):
            x = margin + float(i) * ew / (self.nlights - 1)
            y = h / 2
            self._draw_light(cr, x, y, self.colours[i])

    def _draw_circular(self, w, h, cr):
        r = 0.9 * (min(w, h) / 2)

        for i in xrange(self.nlights):
            theta = float(i) / self.nlights * -2 * math.pi
            x = w / 2 + r * math.sin(theta)
            y = h / 2 + r * math.cos(theta)
            self._draw_light(cr, x, y, self.colours[i])

    def do_draw(self, cr):
        w = self.get_allocated_width()
        h = self.get_allocated_height()

        cr.rectangle(0, 0, w, h)
        cr.set_source_rgb(0, 0, 0)
        cr.fill()

        self._draw_lights(w, h, cr)

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

def mk_layout_radio(lights):
    def lin_cb(r):
        if r.get_active():
            lights._draw_lights = lights._draw_linear

    def circ_cb(r):
        if r.get_active():
            lights._draw_lights = lights._draw_circular

    lin = Gtk.RadioButton.new_with_label(None, 'linear')
    lin.connect('toggled', lin_cb)
    circ = Gtk.RadioButton.new_with_label_from_widget(lin, 'circular')
    circ.connect('toggled', circ_cb)

    return hbox(
        (True, lin),
        (True, circ))

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
        (False, mk_layout_radio(lights)),
        (False, scale)))
    win.connect('destroy', lambda w: Gtk.main_quit())
    win.show_all()
    Gtk.main()

if __name__ == '__main__':
    main()
