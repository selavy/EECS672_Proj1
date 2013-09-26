// project1.c++

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/freeglut.h>

#include "Controller.h"
#include "ModelView.h"

#include "CSVReader.h"

int main(int argc, char* argv[])
{
	// One-time initialization of the glut
	glutInit(&argc, argv);

	Controller c(argv[0]);
	c.reportVersions(std::cout);
	std::cout << std::endl;

	try
	  {
	    /* hardcoding filename for now */
	    CSVReader * reader = new CSVReader("ExchangeRates.csv");
	    c.addModel( new ModelView( reader->getData(0) ) );
	    c.addModel( new ModelView( reader->getData(1) ) );

	    glClearColor(1.0, 1.0, 1.0, 1.0);

	    // Start glut looking for events:
	    glutMainLoop();
	  }
	catch( const std::exception& e )
	  {
	    std::cerr << e.what() << std::endl;
	  }

	return 0;
}
