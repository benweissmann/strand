# slightly more declarative OpenGL usage
# c.f. https://readmes.numm.org/glitch/
#   but somewhat more geared for "modern" OpenGL usage

import numpy
import OpenGL.GL as GL

_np2gl = {
    numpy.uint8: GL.GL_UNSIGNED_BYTE,
    numpy.uint16: GL.GL_UNSIGNED_SHORT,
    numpy.uint32: GL.GL_UNSIGNED_INT,
    numpy.float32: GL.GL_FLOAT }

def np2gl(t):
    try:
        return _np2gl[t]
    except KeyError:
        raise ValueError("bad array type %s" % (t,))

class _DrawMode(object):
    # XXX use enum34
    TRIANGLE_STRIP = GL.GL_TRIANGLE_STRIP
    TRIANGLES = GL.GL_TRIANGLES

class _ImageFormat(object):
    # XXX use enum34
    RGB = GL.GL_RGB
    RGBA = GL.GL_RGBA
    INTENSITY = GL.GL_INTENSITY

class Resource(object):
    def __init__(self):
        self._resource = None

    def realize(self):
        if self._resource is None:
            self._resource = self._create()

        return self._resource

    def delete(self):
        # XXX
        assert self._resource is not None
        raise NotImplementedError()

class Buffer(Resource):
    def __init__(self, target, usage, data):
        Resource.__init__(self)
        self.target = target
        self.usage = usage
        # XXX: constraints on shape?
        self.data = data
        self._et = np2gl(data.dtype.type)

    def _create(self):
        buf = GL.glGenBuffers(1)
        GL.glBindBuffer(self.target, buf)
        GL.glBufferData(self.target, self.data.nbytes, self.data, self.usage)
        return buf

    def bind(self):
        GL.glBindBuffer(self.target, self.realize())

class ArrayBuffer(Buffer):
    def __init__(self, data, usage=GL.GL_STATIC_DRAW):
        if not isinstance(data, numpy.ndarray):
            data = numpy.array(data, dtype=numpy.float32)

        Buffer.__init__(self, GL.GL_ARRAY_BUFFER, usage, data)

class ElementArrayBuffer(Buffer):
    def __init__(self, data, usage=GL.GL_STATIC_DRAW):
        if not isinstance(data, numpy.ndarray):
            data = numpy.array(data, dtype=numpy.uint32)

        Buffer.__init__(self, GL.GL_ELEMENT_ARRAY_BUFFER, usage, data)

    def drawElements(self, tp):
        GL.glDrawElements(tp, self.data.size, self._et, None)

class Texture2D(Resource):
    _target = GL.GL_TEXTURE_2D

    def __init__(self, data):
        Resource.__init__(self)
        self._check_shape(data.shape)
        self._t = np2gl(data.dtype.type)
        self.data = data
        self._dirty = True

    def _check_shape(self, shape):
        # https://www.opengl.org/wiki/Image_Formats

        if len(shape) == 3:
            assert shape[2] == 3
            self.format = GL.GL_RGB
        elif len(shape) == 2:
            self.format = GL.GL_RED
        else:
            raise ValueError("bad shape %r" % shape)

    def _create(self):
        tx = GL.glGenTextures(1)
        GL.glBindTexture(self._target, tx)

        for (param, value) in [
                (GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR),
                (GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR),
                (GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP_TO_EDGE),
                (GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP_TO_EDGE)]:
            GL.glTexParameteri(self._target, param, value)
        return tx

    def _upload(self):
        # XXX: try to cater to preferred pixel format?
        # http://www.opengl.org/registry/specs/ARB/internalformat_query2.txt
        (height, width) = self.data.shape[:2]
        GL.glTexImage2D(
            self._target, 0, self.format, width, height, 0,
            self.format, self._t, self.data)

    def realize(self):
        tx = Resource.realize(self)

        if self._dirty:
            GL.glBindTexture(self._target, tx)
            self._upload()
            self._dirty = False

        return tx

    def dirty(self):
        self._dirty = True

class Shader(Resource):
    def __init__(self, type, src):
        Resource.__init__(self)
        self.type = type
        self.src = src

    def _create(self):
        shader = GL.glCreateShader(self.type)
        GL.glShaderSource(shader, self.src)
        GL.glCompileShader(shader)

        if not GL.glGetShaderiv(shader, GL.GL_COMPILE_STATUS):
            log = GL.glGetShaderInfoLog(shader)
            GL.glDeleteShader(shader)
            raise RuntimeError(log)

        return shader

class VertexShader(Shader):
    def __init__(self, src):
        Shader.__init__(self, GL.GL_VERTEX_SHADER, src)

class FragmentShader(Shader):
    def __init__(self, src):
        Shader.__init__(self, GL.GL_FRAGMENT_SHADER, src)

class VertexAttribCtx(object):
    def __init__(self, prog, attrib, buf):
        self.prog = prog
        self.attrib = attrib
        self.buf = buf

    def __enter__(self):
        self.buf.bind()
        pos = self.prog.attribLocation(self.attrib)
        width = self.buf.data.shape[1]
        GL.glVertexAttribPointer(pos, width, self.buf._et, False, 0, None)
        GL.glEnableVertexAttribArray(pos)

    def __exit__(self, exc_t, ecc_v, exc_tb):
        pos = self.prog.attribLocation(self.attrib)
        GL.glDisableVertexAttribArray(pos)

class Program(Resource):
    def __init__(self, vshader, fshader):
        Resource.__init__(self)
        self.vshader = vshader
        self.fshader = fshader

    def _create(self):
        prog = GL.glCreateProgram()
        GL.glAttachShader(prog, self.vshader.realize())
        GL.glAttachShader(prog, self.fshader.realize())
        GL.glLinkProgram(prog)

        if not GL.glGetProgramiv(prog, GL.GL_LINK_STATUS):
            # XXX get log
            GL.glDeleteProgram(prog)
            raise RuntimeError()

        return prog

    def bind(self):
        GL.glUseProgram(self.realize())

    def vertexAttrib(self, attrib, buf):
        return VertexAttribCtx(self, attrib, buf)

    def bindTexture(self, name, unit, texture):
        # Ideally we would automatically allocate texture units.
        GL.glActiveTexture(GL.GL_TEXTURE0 + unit)
        GL.glBindTexture(GL.GL_TEXTURE_2D, texture.realize())
        GL.glUniform1i(self.uniformLocation(name), unit)

    def attribLocation(self, name):
        return GL.glGetAttribLocation(self.realize(), name)

    def uniformLocation(self, name):
        return GL.glGetUniformLocation(self.realize(), name)

    def setUniform1i(self, name, value):
        # XXX
        # raise NotImplementedError()
        GL.glUniform1i(self.uniformLocation(name), value)

    def setUniform1f(self, name, value):
        # raise NotImplementedError()
        GL.glUniform1f(self.uniformLocation(name), value)

    def setUniform1fv(self, name, value):
        assert value.dtype == numpy.float32
        GL.glUniform1fv(self.uniformLocation(name), len(value), value)

    def setUniform3fv(self, name, value):
        assert value.dtype == numpy.float32
        GL.glUniform3fv(self.uniformLocation(name), len(value), value)

    def setUniform2f(self, name, v1, v2):
        # raise NotImplementedError()
        GL.glUniform2f(self.uniformLocation(name), v1, v2)

def clear(color=None):
    if color is not None:
        GL.glClearColor(*color)

    GL.glClear(GL.GL_COLOR_BUFFER_BIT)
