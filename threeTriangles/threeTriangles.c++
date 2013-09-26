// threeTriangles.c++: An OpenGL program that draws three colored triangles

#include <GL/gl.h>
#include <GL/freeglut.h>

#include "Controller.h"
#include "ModelView.h"

int main(int argc, char* argv[])
{
	// One-time initialization of the glut
	glutInit(&argc, argv);

	Controller c(argv[0]);
	c.addModel( new ModelView() );

	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Off to the glut event handling loop:
	glutMainLoop();

	return 0;
}
