// ModelView.h - a basic combined Model and View for OpenGL

#ifndef MODELVIEW_H
#define MODELVIEW_H

class ShaderIF;

#include <string>
#include <GL/gl.h>

class ModelView
{
public:
	ModelView();
	virtual ~ModelView();

	// xyzLimits: {wcXmin, wcXmax, wcYmin, wcYmax, wcZmin, wcZmax}
	void getWCBoundingBox(double* xyzLimits) const;
	void handleCommand(unsigned char key, double ldsX, double ldsY);
	void render() const;
private:
	GLuint vao;
	GLuint vbo[3]; // 0: colors; 1: temperatures; 2: coordinates
	int colorMode[3]; // how is color determined for each of the three triangles

	// All instances share the same shader program:
	static ShaderIF* shaderIF;
	static int numInstances;
	static GLuint shaderProgram;
	static GLint pvaLoc_vertexColor, pvaLoc_vertexTemperature;
	static GLint pvaLoc_wcPosition;
	static GLint ppuLoc_colorMode;
	static GLint ppuLoc_scaleTrans;

	// Routines for computing parameters necessary to map from arbitrary
	// world coordinate ranges into OpenGL's -1..+1 Logical Device Space.
	// 1. linearMap determines the scale and translate parameters needed in
	//    order to map a value, f (fromMin <= f <= fromMax) to its corresponding
	//    value, t (toMin <= t <= toMax). Specifically: t = scale*f + trans.
	static void linearMap(double fromMin, double fromMax, double toMin, double toMax,
						  double& scale, double& trans);
	// 2. computeScaleTrans determines the current world coordinate region of
	//    interest and then uses linearMap to determine how to map coordinates
	//    in the region of interest to their proper location in Logical Device
	//    Space. (Returns float[] because glUniform currently allows only float[].)
	static void computeScaleTrans(float* scaleTrans);

	// "pp": "per-primitive"
	static GLint ppUniformLocation(GLuint glslProgram, const std::string& name);
	// "pv": "per-vertex"
	static GLint pvAttribLocation(GLuint glslProgram, const std::string& name);
	
	void defineTriangles();
	static void fetchGLSLVariableLocations();
};

#endif
