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
	GLuint vao[2]; // we will create two individual objects
	GLuint vbo[2]; // each object has its own coordinate buffer ...

	// interactive display options
	int colorMode[2]; // flag to tell fragment shader how to assign colors
	bool visible[2]; // i.e., whether we actually currently draw it

	// we assume all instances share the same shader program:
	static ShaderIF* shaderIF;
	static int numInstances;
	static GLuint shaderProgram;
	// "pvaLoc_" prefix means shader program location of an "attribute" variable
	// "ppuLoc_" prefix means shader program location of an "uniform" variable
	static GLint ppuLoc_colorMode; // a uniform variable (per-primitive)
	static GLint ppuLoc_scaleTrans; // a uniform used to scale MC to LDS
	static GLint pvaLoc_wcPosition; // an attribute variable (per-vertex)

	// "pp": "per-primitive"; "pv": "per-vertex"
	static GLint ppUniformLocation(GLuint glslProgram, const std::string& name);
	static GLint pvAttribLocation(GLuint glslProgram, const std::string& name);

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

	void defineTriangle();
	void deleteObject(int which);
	static void fetchGLSLVariableLocations();
};

#endif
