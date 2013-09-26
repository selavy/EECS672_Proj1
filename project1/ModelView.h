// ModelView.h - a basic combined Model and View for OpenGL

#ifndef MODELVIEW_H
#define MODELVIEW_H

class ShaderIF;

#include <string>
#include <GL/gl.h>
#include <GL/glext.h>
#include <vector>

// for interfacing to common GLSL data types
typedef float vec4[4]; 

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
	// VAO(s), VBO(s), and other relevant instance variable declarations here...
	GLuint vbo_dataPoints;
	GLuint vao;

	// we assume all instances share the same shader program:
	static ShaderIF* shaderIF;
	static int numInstances;
	static GLuint shaderProgram;
	// "pvaLoc_" prefix means shader program location of an "attribute" variable
	// "ppuLoc_" prefix means shader program location of an "uniform" variable
	
	// uniform and attribute variable locations go here...
	static GLint ppuLoc_scaleTrans;
	static GLint ppuLoc_color;
	static GLint pvaLoc_wcPosition;

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

	static void fetchGLSLVariableLocations();
	void defineModel();
	int _points;
	std::vector<double> _datapts;
};

#endif
