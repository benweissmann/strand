
import OpenGL.GLUT as GLUT

class GLUTApp(object):
    title = "glut"
    idle = None

    def run(self):
        def _display():
            self.display()
            # print '%15.3f: swap' % time.time()
            GLUT.glutSwapBuffers()

        GLUT.glutInit([])
        GLUT.glutInitDisplayMode(GLUT.GLUT_RGB | GLUT.GLUT_DOUBLE)
        GLUT.glutCreateWindow(self.title)
        self.setup()

        if self.idle is not None:
            GLUT.glutIdleFunc(self.idle)

        GLUT.glutDisplayFunc(_display)
        GLUT.glutMainLoop()

    def postRedisplay(self):
        GLUT.glutPostRedisplay()

    def setTimer(self, msec, cb):
        GLUT.glutTimerFunc(msec, lambda _: cb(), None)

    def setup(self):
        pass

    def display(self):
        pass
