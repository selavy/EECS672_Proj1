// Controller.c++: a basic Controller (in Model-View-Controller sense)

#include <GL/gl.h>
#include <GL/freeglut.h>

#include "Controller.h"
#include "ModelView.h"

Controller* Controller::curController = NULL;

Controller::Controller(const std::string& name) :
	model(NULL), vpWidth(-1), vpHeight(-1)
{
	Controller::curController = this;

	// indicate we do not yet have any models by setting min to +1 and max to -1:
	overallWCBoundingBox[0] = overallWCBoundingBox[2] = overallWCBoundingBox[4] = 1.0;
	overallWCBoundingBox[1] = overallWCBoundingBox[3] = overallWCBoundingBox[5] = -1.0;

	// First create the window and its Rendering Context (RC)
	int windowID = createWindow(name);

	// Then initialize the newly created OpenGL RC
	establishInitialCallbacksForRC(); // the callbacks for this RC
}

Controller::~Controller()
{
	if (this == curController)
		curController = NULL;
}

void Controller::addModel(ModelView* m)
{
	// for now, we just assume we only have one model:
	if (m != NULL)
	{
		model = m;
		m->getWCBoundingBox(overallWCBoundingBox);
	}
}

bool Controller::checkForErrors(std::ostream& os, const std::string& context)
	// CLASS METHOD
{
	bool hadError = false;
	GLenum e = glGetError();
	while (e != GL_NO_ERROR)
	{
		os << "CheckForErrors (context: " <<  context
		   << "): " << (char*)gluErrorString(e) << std::endl;
		e = glGetError();
		hadError = true;
	}
	return hadError;
}

int Controller::createWindow(const std::string& windowTitle) // CLASS METHOD
{
	// The following calls enforce use of only non-deprecated functionality.
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Now create the window and its Rendering Context.
	int windowID = glutCreateWindow(windowTitle.c_str());
	return windowID;
}

void Controller::displayCB() // CLASS METHOD
{
	if (curController != NULL)
		curController->handleDisplay();
}

void Controller::establishInitialCallbacksForRC()
{
	glutDisplayFunc(Controller::displayCB);
	glutKeyboardFunc(Controller::keyboardCB);
	glutReshapeFunc(Controller::reshapeCB);
}

double Controller::getViewportAspectRatio() const
{
	return static_cast<double>(vpHeight) / static_cast<double>(vpWidth);
}

// For now, we assume the "region of interest" is just the whole model.
void Controller::getWCRegionOfInterest(double* xyzLimits) const
{
	for (int i=0 ; i<6 ; i++)
		xyzLimits[i] = overallWCBoundingBox[i];
}

void Controller::handleDisplay()
{
	// clear the frame buffer
	glClear(GL_COLOR_BUFFER_BIT);

	if (model != NULL)
		model->render();

	// force execution of queued OpenGL commands
	glFlush();

	checkForErrors(std::cout, "Controller::handleDisplay");
}

void Controller::handleKeyboard(unsigned char key, int x, int y)
{
	const unsigned char ESC = 27;
	if (key == ESC)
		exit(0);

	// No other key is currently used by the Controller, so we just send
	// the event information to the model.

	// The only coordinate system known to both the Controller and the ModelView
	// is OpenGL's Logical Device Space (LDS) in which x and y vary -1..+1.
	// We convert the coordinates before passing them on to the ModelView.
	if (model != NULL)
	{
		double ldsX, ldsY;
		screenXYToLDS(x, y, ldsX, ldsY);
		model->handleCommand(key, ldsX, ldsY);
	}

	glutPostRedisplay();
}

void Controller::handleReshape()
{
	glViewport(0, 0, vpWidth, vpHeight);
}

void Controller::keyboardCB(unsigned char key, int x, int y)
{
	if (Controller::curController != NULL)
		Controller::curController->handleKeyboard(key, x, y);
}

void Controller::reportVersions(std::ostream& os)
{
	const char* glVer = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const char* glslVer = reinterpret_cast<const char*>
			(glGetString(GL_SHADING_LANGUAGE_VERSION));
	// glGetString can return NULL if no rendering context has been created
	os << "VERSIONS: GL: ";
	if (glVer == NULL)
		os << "NULL (has RC been created?)\n";
	else
		os << glVer << '\n';
	os << "        GLSL: ";
	if (glslVer == NULL)
		os << "NULL (has RC been created?)\n";
	else
		os << glslVer << '\n';
	if ((glVer == NULL) || (glslVer == NULL))
		return;
	int glutVer = glutGet(GLUT_VERSION);
	int glutVerMajor = glutVer/10000;
	int glutVerMinor = (glutVer % 10000) / 100;
	int glutVerPatch = glutVer % 100;
	os << "        GLUT: " << glutVerMajor << '.';
	if (glutVerMinor < 10)
		os << '0';
	os << glutVerMinor << '.';
	if (glutVerPatch < 10)
		os << '0';
	os << glutVerPatch << " (" << glutVer << ")\n";
#ifdef __GLEW_H__
	os << "        GLEW: " << glewGetString(GLEW_VERSION) << '\n';
#endif
}

void Controller::reshapeCB(int width, int height)
{
	if (Controller::curController != NULL)
	{
		Controller::curController->vpWidth = width;
		Controller::curController->vpHeight = height;
		Controller::curController->handleReshape();
	}
}

void Controller::screenXYToLDS(int x, int y, double& ldsX, double& ldsY)
{
	ldsX = 2.0 * static_cast<double>(x) / static_cast<double>(vpWidth) - 1.0;
	// The glut reports pixel coordinates assuming y=0 is at the top of the
	// window. The main OpenGL API assumes y=0 is at the bottom, so note we
	// account for that here.
	ldsY = 2.0 * static_cast<double>(vpHeight - y) / static_cast<double>(vpHeight) - 1.0;
}
