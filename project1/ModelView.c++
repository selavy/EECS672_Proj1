// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"

#define __DEBUG__

ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;
GLuint ModelView::shaderProgram = 0;

/* initialize static member variables */
GLint ModelView::ppuLoc_scaleTrans;
GLint ModelView::ppuLoc_color;
GLint ModelView::pvaLoc_wcPosition;

ModelView::ModelView()
  :
  _points(2)
{
	if (ModelView::shaderProgram == 0)
	{
		// Create the common shader program:
		ModelView::shaderIF = new ShaderIF("project1.vsh", "project1.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
		fetchGLSLVariableLocations();
	}

	// TODO: define and call method(s) to initialize your model and send data to GPU
	defineModel();
	ModelView::numInstances++;
}

ModelView::~ModelView()
{
	// TODO: delete the vertex array objects and buffers here
        glDeleteBuffers(1, &vbo_dataPoints);
        glDeleteVertexArrays(1, &vao);

	if (--numInstances == 0)
	{
		ModelView::shaderIF->destroy();
		delete ModelView::shaderIF;
		ModelView::shaderIF = NULL;
		ModelView::shaderProgram = 0;
	}
}

// computeScaleTrans determines the current world coordinate region of
// interest and then uses linearMap to determine how to map coordinates
// in the region of interest to their proper location in Logical Device
// Space. (Returns float[] because glUniform currently favors float[].)
void ModelView::computeScaleTrans(float* scaleTransF) // CLASS METHOD
{
	double xyzLimits[6];
	Controller::getCurrentController()->getWCRegionOfInterest(xyzLimits);

	// We are only concerned with the xy extents for now
	// Map the desired limits to the -1..+1 Logical Device Space:
	double scaleTrans[4];
	linearMap(xyzLimits[0], xyzLimits[1], -1.0, 1.0, scaleTrans[0], scaleTrans[1]);
	linearMap(xyzLimits[2], xyzLimits[3], -1.0, 1.0, scaleTrans[2], scaleTrans[3]);
	for (int i=0 ; i<4 ; i++)
		scaleTransF[i] = static_cast<float>(scaleTrans[i]);
}

void ModelView::fetchGLSLVariableLocations()
{
	if (ModelView::shaderProgram > 0)
	{
		// TODO: Set GLSL variable locations here
	  ModelView::ppuLoc_color = ppUniformLocation(shaderProgram, "color");
	  ModelView::ppuLoc_scaleTrans = ppUniformLocation(shaderProgram, "scaleTrans");
	  ModelView::pvaLoc_wcPosition = pvAttribLocation(shaderProgram, "wcPosition");
	}
}

// xyzLimits: {wcXmin, wcXmax, wcYmin, wcYmax, wcZmin, wcZmax}
void ModelView::getWCBoundingBox(double* xyzLimits) const
{
	// TODO:
	// put xmin and xmax in xyzLimits[0] and xyzLimits[1], respectively
	// put ymin and ymax in xyzLimits[2] and xyzLimits[3], respectively
	// put zmin and zmax (-1 .. +1 is OK for 2D) in xyzLimits[4] and xyzLimits[5], respectively
  double xmin, xmax, ymin, ymax, zmin, zmax;

  xmin = -1.0;
  xmax = 1.0;
  ymin = -1.0;
  ymax  = 1.0;
  zmin = -1.0;
  zmax = 1.0;

  xyzLimits[0] = xmin; xyzLimits[1] = xmax;
  xyzLimits[1] = ymin; xyzLimits[2] = ymax;
  xyzLimits[3] = zmin; xyzLimits[4] = zmax;
}

void ModelView::handleCommand(unsigned char key, double ldsX, double ldsY)
{
}

// linearMap determines the scale and translate parameters needed in
// order to map a value, f (fromMin <= f <= fromMax) to its corresponding
// value, t (toMin <= t <= toMax). Specifically: t = scale*f + trans.
void ModelView::linearMap(double fromMin, double fromMax, double toMin, double toMax,
					  double& scale, double& trans) // CLASS METHOD
{
	scale = (toMax - toMin) / (fromMax - fromMin);
	trans = toMin - scale*fromMin;
}

GLint ModelView::ppUniformLocation(GLuint glslProgram, const std::string& name)
{
	GLint loc = glGetUniformLocation(glslProgram, name.c_str());
	if (loc < 0)
		std::cerr << "Could not locate per-primitive uniform: '" << name << "'\n";
	return loc;
}

GLint ModelView::pvAttribLocation(GLuint glslProgram, const std::string& name)
{
	GLint loc = glGetAttribLocation(glslProgram, name.c_str());
	if (loc < 0)
		std::cerr << "Could not locate per-vertex attribute: '" << name << "'\n";
	return loc;
}

void ModelView::render() const
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);

	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderProgram);

	// define the mapping from WC to -1..+1 Logical Device Space:
	// TODO: Uncomment when missing pieces completed...
	float scaleTrans[4];
	computeScaleTrans(scaleTrans);
	// TODO: get rid of code below
	scaleTrans[0] = 1.0;
	scaleTrans[1] = 0.0;
	scaleTrans[2] = 1.0;
	scaleTrans[3] = 0.0;
	scaleTrans[4] = 1.0;
	scaleTrans[5] = 0.0;
	glUniform4fv(ModelView::ppuLoc_scaleTrans, 1, scaleTrans);


#ifdef __DEBUG__
	std::cout << "scaleTrans: " << scaleTrans[0] << ", " << scaleTrans[1] << ", " << scaleTrans[2] << ", " << scaleTrans[3] << std::endl;
#endif

	// TODO: ACTUAL MODEL RENDERING HERE (OR CALL ANOTHER METHOD FROM HERE)

	glBindVertexArray(vao);

	int color = 0;

	glUniform1i( ppuLoc_color, color );
	glDrawArrays( GL_LINE_STRIP, 0, _points );

	// restore the previous program
	glUseProgram(pgm);
}

void ModelView::defineModel()
{
  typedef float vec2[2];

  vec2 dataPoints[2] = { {-0.9, 0.0}, {0.9, 0.0} };
  //vec2* dataPoints = new vec2[_points];
  /*
  dataPoints[0][0] = -0.9;
  dataPoints[0][1] = 0.0;
  dataPoints[1][0] = 0.9;
  dataPoints[1][1] = 0.0;
  */

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo_dataPoints);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_dataPoints);
  int numBytesInBuffer = _points * sizeof( vec2 );
  glBufferData( GL_ARRAY_BUFFER, numBytesInBuffer, dataPoints, GL_STATIC_DRAW );
  glVertexAttribPointer( ModelView::pvaLoc_wcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray( ModelView::pvaLoc_wcPosition ); 

  //delete [] dataPoints;
}