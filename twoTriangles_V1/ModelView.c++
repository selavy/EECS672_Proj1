// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"

ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;
GLuint ModelView::shaderProgram = 0;
GLint ModelView::ppuLoc_color = -1; // uniform variable (per-primitive)
GLint ModelView::ppuLoc_scaleTrans = -1;
GLint ModelView::pvaLoc_wcPosition = -1; // attribute variable (per-vertex)

static const int numVerticesInTriangle = 3; // same as in Hello, OpenGL

ModelView::ModelView()
{
	if (ModelView::shaderProgram == 0)
	{
		// Create the common shader program:
		ModelView::shaderIF = new ShaderIF("twoTriangles_V1.vsh", "twoTriangles_V1.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
		fetchGLSLVariableLocations();
	}

	defineTriangle();
	ModelView::numInstances++;
}

ModelView::~ModelView()
{
	// delete the vertex array objects and buffers, if they have not
	// already been deleted
	for (int i=0 ; i<2 ; i++)
		deleteObject(i);
	if (--ModelView::numInstances == 0)
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

void ModelView::defineTriangle()
{
	typedef float vec2[2];
	// define vertex coordinates for the two triangles
	vec2 vertexPositions[][numVerticesInTriangle] =
	{
		// triangle 1:
		{ { -6.0, 137.0 }, { 2.0, 137.0 }, { -2.0, 145.0 } },
		// triangle 2:
		{ { -6.0, 135.0 }, { 2.0, 135.0 }, { -2.0, 127.0 } }
	};

	// NOTE: "useColor", "drawInBlack", and "visible" are defined here along with
	//       the geometry with which they will be used, BUT they are NOT sent to
	//       the GPU. Instead we will use them in the "render" method below.

	// first triangle color is dark green
	useColor[0][0] = 0.0; useColor[0][1] = 0.5; useColor[0][2] = 0.0; useColor[0][3] = 1.0;
	drawInBlack[0] = false;
	visible[0] = true;
	// second triangle color is dark red
	useColor[1][0] = 0.5; useColor[1][1] = 0.0; useColor[1][2] = 0.0; useColor[1][3] = 1.0;
	drawInBlack[1] = false;
	visible[1] = true;

	// Send the actual geometry to the GPU:

	// Get TWO new, previously unused VAO names (one for each of the two triangles):
	glGenVertexArrays(2, vao);
	// Create TWO new, previously unused names for the coordinate buffers (again, one
	// for each of the two triangles. Recall each buffer object will be associated with
	// a VAO.
	glGenBuffers(2, vbo);

	for (int i=0 ; i<2 ; i++)
	{
		// (initialize and) make active the i-th VAO
		glBindVertexArray(vao[i]);
		// (initialize and) make active the corresponding i-th vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);

		// Allocate space for AND copy the vertex data for the i-th triangle to the GPU buffer
		int numBytesInBuffer = numVerticesInTriangle * sizeof(vec2);
		glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, vertexPositions[i], GL_STATIC_DRAW);
		glVertexAttribPointer(ModelView::pvaLoc_wcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(ModelView::pvaLoc_wcPosition);
	}
}

void ModelView::deleteObject(int which)
{
	if (vao[which] > 0) // hasn't already been deleted
	{
		glDeleteBuffers(1, &vbo[which]);
		glDeleteVertexArrays(1, &vao[which]);
		vao[which] = vbo[which] = 0;
	}
}

void ModelView::fetchGLSLVariableLocations()
{
	if (ModelView::shaderProgram > 0)
	{
		ModelView::ppuLoc_color = ppUniformLocation(shaderProgram, "color");
		ModelView::ppuLoc_scaleTrans = ppUniformLocation(shaderProgram, "scaleTrans");
		// "wc" in "wcPosition" refers to "world coordinates"
		ModelView::pvaLoc_wcPosition = pvAttribLocation(shaderProgram, "wcPosition");
	}
}

// xyzLimits: {wcXmin, wcXmax, wcYmin, wcYmax, wcZmin, wcZmax}
void ModelView::getWCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] =  -6.0; xyzLimits[1] =   2.0; // (xmin, xmax)
	xyzLimits[2] = 127.0; xyzLimits[3] = 145.0; // (ymin, ymax)
	xyzLimits[4] =  -1.0; xyzLimits[5] =   1.0; // (zmin, zmax) (really 0..0)
}

void ModelView::handleCommand(unsigned char key, double ldsX, double ldsY)
{
	// determine which triangle was "picked" based on which half of the
	// window contains the cursor
	int which;
	if (ldsY >= 0.0)
		which = 0;
	else
		which = 1;

	// Now apply the current operation to it:
	if (key == 'v')
		// toggle visibility
		visible[which] = !visible[which];
	else if (key == 'd')
		deleteObject(which);
	else // assume toggle drawing in black
		drawInBlack[which] = !drawInBlack[which];
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
	
	float black[] = { 0.0, 0.0, 0.0, 1.0 };

	for (int i=0 ; i<2 ; i++)
	{
		if ((vao[i] == 0)  || // if it has been deleted
		    (!visible[i]))    // if it has been temporarily blanked
			continue;
		// set the desired triangle (primitive) color
		if (drawInBlack[i])
			glUniform4fv(ModelView::ppuLoc_color, 1, black);
		else
			glUniform4fv(ModelView::ppuLoc_color, 1, useColor[i]);

		// Bind the i-th triangle.
		// IMPORTANT NOTES:
		// Binding a VAO automatically binds all buffers associated with it. It also
		// effectively reestablishes all associated glVertexAttribPointer settings
		// as well as the associated gl[En|Dis]ableVertexAttribArray flags.
		glBindVertexArray(vao[i]);

		// Draw the i-th triangle.
		glDrawArrays(GL_TRIANGLES, 0, numVerticesInTriangle);
	}

	// restore the previous program
	glUseProgram(pgm);
}
