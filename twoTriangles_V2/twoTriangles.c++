// twoTriangles.c++ (V2): Adding client-side specification of color via uniform variables

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

	// Start glut looking for events:
	glutMainLoop();

	return 0;
}
