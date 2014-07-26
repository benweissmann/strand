
import gl

vertices = gl.ArrayBuffer([[0, 0], [0, 1], [1, 1], [1, 0]])
quad = gl.ElementArrayBuffer([0, 1, 3, 2])

prog = gl.Program(
    gl.VertexShader(
        """
        attribute vec2 pos;
        varying vec2 xy;

        void main() {
            gl_Position = vec4(vec2(2.0, 2.0) * (pos - 0.5), 0, 1);
            xy = pos;
        }
        """),
    gl.FragmentShader(
        """
        varying vec2 xy;
        uniform vec3 lights[10];
        int n_lights = 10;
        float m = 0.075;
        float w = 1.0 - 2.0 * m;

        void main() {
            vec3 c = vec3(0.0);

            for (int i = 0; i < n_lights; i++) {
              vec2 lxy = vec2(m + w * float(i) / float(n_lights - 1), 0.5);
              vec2 dxy = abs(lxy - xy);
              float d = pow(pow(dxy.x / 1.3, 2.0) + pow(dxy.y / 5.0, 2.0), 0.5);
              c += 1.0 * lights[i] * pow(1.0 - d, 40.0);
            }

            gl_FragColor = vec4(c, 1.0);
        }
        """))

def draw_lights(lights):
    prog.bind()

    with prog.vertexAttrib("pos", vertices):
        #prog.setUniform1f('t', time.time() - start)
        prog.setUniform3fv('lights', lights)
        quad.bind()
        quad.drawElements(gl._DrawMode.TRIANGLE_STRIP)
