// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"

ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;
GLuint ModelView::shaderProgram = 0;
GLint ModelView::pvaLoc_vertexColor = -1;
GLint ModelView::pvaLoc_vertexTemperature = -1;
GLint ModelView::pvaLoc_wcPosition = -1;
GLint ModelView::ppuLoc_colorMode = -1;
GLint ModelView::ppuLoc_scaleTrans = -1;

ModelView::ModelView()
{
	if (ModelView::shaderProgram == 0)
	{
		// The first time an instance of this class is generated, create the common shader program:
		ModelView::shaderIF = new ShaderIF("threeTriangles.vsh", "threeTriangles.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
		fetchGLSLVariableLocations();
	}

	// Now define the geometry
	defineTriangles();
	// colorMode: 1->use per-vertex colors; 2->use per-vertex temperatures
	colorMode[0] = 1;
	colorMode[1] = 2;
	colorMode[2] = 1;
	numInstances++;
}

ModelView::~ModelView()
{
	glDeleteBuffers(3, vbo);
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
	Controller* c = Controller::getCurrentController();
	double xyzLimits[6];
	c->getWCRegionOfInterest(xyzLimits);

	// preserve aspect ratio. Make "region of interest" wider or taller to
	// match the Controller's viewport aspect ratio.
	double vAR = c->getViewportAspectRatio();
	double xmin = xyzLimits[0], xmax = xyzLimits[1];
	double ymin = xyzLimits[2], ymax = xyzLimits[3];
	if (vAR > 0.0)
	{
		double wHeight = xyzLimits[3] - xyzLimits[2];
		double wWidth = xyzLimits[1] - xyzLimits[0];
		double wAR = wHeight / wWidth;
		if (wAR > vAR)
		{
			// make window wider
			wWidth = wHeight / vAR;
			double xmid = 0.5 * (xmin + xmax);
			xmin = xmid - 0.5*wWidth;
			xmax = xmid + 0.5*wWidth;
		}
		else
		{
			// make window taller
			wHeight = wWidth * vAR;
			double ymid = 0.5 * (ymin + ymax);
			ymin = ymid - 0.5*wHeight;
			ymax = ymid + 0.5*wHeight;
		}
	}

    // We are only concerned with the xy extents for now, hence we will
    // ignore xyzLimits[4] and xyzLimits[5].
    // Map the overall limits to the -1..+1 range expected by the OpenGL engine:
	double scaleTrans[4];
	linearMap(xmin, xmax, -1.0, 1.0, scaleTrans[0], scaleTrans[1]);
	linearMap(ymin, ymax, -1.0, 1.0, scaleTrans[2], scaleTrans[3]);
	for (int i=0 ; i<4 ; i++)
		scaleTransF[i] = static_cast<float>(scaleTrans[i]);
}

void ModelView::defineTriangles()
{
	typedef float vec2[2]; // for (x,y) coordinates
	typedef float vec4[4]; // for (r,g,b,a) color 4-tuples

	const int numTriangles = 3;
	const int numVerticesPerTriangle = 3;

	// define per-vertex colors
	// (Whether these will actually be used depends on the current "colorMode".)
	vec4 vertexColors[] =
	{
		// if colorMode==1, then first triangle would be grayish-green
		{ 0.4, 0.5, 0.4, 1.0 }, { 0.4, 0.5, 0.4, 1.0 }, { 0.4, 0.5, 0.4, 1.0 },
		// if colorMode==1, then second triangle would be BLUE
		{ 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 },
		// if colorMode==1, then third triangle would be a blend
		{ 1.0, 0.0, 0.0, 1.0 }, { 0.0, 1.0, 0.0, 1.0 }, { 0.0, 0.0, 1.0, 1.0 }
	};

	// Define the per-vertex temperatures
	float vertexTemps[] =
	{
		0.0, 10.1, 120.0,   -25.3, 10.6, 98.5,   -30.4, 92.5, -25.8
	};

	// define vertex coordinates
	vec2 vertexPositions[] =
	{
		{  0.5, 0.5 }, { 1.0, -0.5 }, {  0.0, -0.5 },
		{  0.0, 0.3 }, { 1.0,  0.3 }, {  0.5, -0.7 },
		{ -0.5, 0.5 }, { 0.0, -0.5 }, { -1.0, -0.5 }
	};

	// SOMETHING A BIT DIFFERENT: Here we are choosing to put all three triangles
	// into one VAO. Since we want each triangle's coloring mode to be independent,
	// we will issue three glDrawArrays calls in ModelView::render, each with a
	// different offset.
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// create per-vertex attribute buffers for the three triangles
	// [0] - color buffer; [1] - temperature; [2] - vertex coordinate buffer
	glGenBuffers(3, vbo);

	// Allocate space for and copy CPU color information to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	int numBytesColors = numTriangles * numVerticesPerTriangle * sizeof(vec4);
	glBufferData(GL_ARRAY_BUFFER, numBytesColors, vertexColors, GL_STATIC_DRAW);
	glVertexAttribPointer(ModelView::pvaLoc_vertexColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ModelView::pvaLoc_vertexColor);

	// Allocate space for and copy CPU temperature information to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	int numBytesTemperatureData = numTriangles * numVerticesPerTriangle * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, numBytesTemperatureData, vertexTemps, GL_STATIC_DRAW);
	glVertexAttribPointer(ModelView::pvaLoc_vertexTemperature, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ModelView::pvaLoc_vertexTemperature);

	// Allocate space for and copy CPU coordinate data to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	int numBytesCoordinateData = numTriangles * numVerticesPerTriangle * sizeof(vec2);
	glBufferData(GL_ARRAY_BUFFER, numBytesCoordinateData, vertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(ModelView::pvaLoc_wcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ModelView::pvaLoc_wcPosition);

	// IMPORTANT FINAL NOTE:
	// All three array buffers here - along with their memory layout information and
	// enable statuses - will be reestablished in ModelView::render with the single
	// call to glBindVertexArray(va0).
}

void ModelView::fetchGLSLVariableLocations()
{
	if (ModelView::shaderProgram > 0)
	{
		ModelView::pvaLoc_vertexColor = pvAttribLocation(shaderProgram, "vertexColor");
		ModelView::pvaLoc_vertexTemperature = pvAttribLocation(shaderProgram, "vertexTemperature");
		ModelView::pvaLoc_wcPosition = pvAttribLocation(shaderProgram, "wcPosition");
		ModelView::ppuLoc_scaleTrans = ppUniformLocation(shaderProgram, "scaleTrans");
		ModelView::ppuLoc_colorMode = ppUniformLocation(shaderProgram, "colorMode");
	}
}

// xyzLimits: {wcXmin, wcXmax, wcYmin, wcYmax, wcZmin, wcZmax}
void ModelView::getWCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = -1.0; xyzLimits[1] = 1.0; // (xmin, xmax)
	xyzLimits[2] = -1.0; xyzLimits[3] = 1.0; // (ymin, ymax)
	xyzLimits[4] = -1.0; xyzLimits[5] = 1.0; // (zmin, zmax) (really 0..0)
}

void ModelView::handleCommand(unsigned char key, double ldsX, double ldsY)
{
	if ((key >= '0') && (key <= '2'))
	{
		int which = static_cast<int>(key) - static_cast<int>('0');
		colorMode[which] = 3 - colorMode[which];
	}
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
	float scaleTrans[4];
	computeScaleTrans(scaleTrans);
	glUniform4fv(ModelView::ppuLoc_scaleTrans, 1, scaleTrans);

	glBindVertexArray(vao); // reestablishes all buffer settings as noted above
	
	// draw the three triangles separately so that each can use a different
	// colorMode
	for (int i=0 ; i<3 ; i++)
	{
		glUniform1i(ModelView::ppuLoc_colorMode, colorMode[i]);
		glDrawArrays(GL_TRIANGLES, i*3, 3);
	}
	// restore the previous program
	glUseProgram(pgm);
}
