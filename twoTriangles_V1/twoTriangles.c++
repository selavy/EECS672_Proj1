// twoTriangles.c++ (V1): Adding client-side specification of color via uniform variables

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/freeglut.h>

#include "Controller.h"
#include "ModelView.h"

int main(int argc, char* argv[])
{
	// One-time initialization of the glut
	glutInit(&argc, argv);

	Controller c(argv[0]);
	Controller::reportVersions(std::cout);
	c.addModel( new ModelView() );

	glClearColor(1.0, 1.0, 1.0, 1.0);

	// Start glut looking for events:
	glutMainLoop();

	return 0;
}
