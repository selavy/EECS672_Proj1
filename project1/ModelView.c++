// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"

#include "CSVReader.h"

#define __DEBUG__

ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;
GLuint ModelView::shaderProgram = 0;

/* initialize static member variables */
GLint ModelView::ppuLoc_scaleTrans;
GLint ModelView::ppuLoc_color; /* color := [0,3] */
GLint ModelView::pvaLoc_wcPosition;

ModelView::ModelView( std::vector<double> usd, std::vector<double> aud, std::vector<double> brl, std::vector<double> czk )
  :
  _usd( usd ),
  _aud( aud ),
  _brl( brl ),
  _czk( czk )
{
	if (ModelView::shaderProgram == 0)
	{
		// Create the common shader program:
		ModelView::shaderIF = new ShaderIF("project1.vsh", "project1.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
		fetchGLSLVariableLocations();
	}

	// TODO: define and call method(s) to initialize your model and send data to GPU
	// Assume that data has same number of points for each category
	_points = _usd.size();

	generateBuffers();

	for( int i = 0; i < 4; ++i )
	  defineModel( i );

	for( int i = 0; i < 1; ++i )
	  drawAxes( i );

	ModelView::numInstances++;
}

ModelView::~ModelView()
{
	// TODO: delete the vertex array objects and buffers here
  for( int i = 0; i < 5; ++i )
    deleteObject( i );

	if (--numInstances == 0)
	{
		ModelView::shaderIF->destroy();
		delete ModelView::shaderIF;
		ModelView::shaderIF = NULL;
		ModelView::shaderProgram = 0;
	}
}

/* function taken from twoTriangles_V1 by Dr. Miller */
void ModelView::deleteObject( int i )
{
  if( vao[i] > 0 ) // hasn't already been deleted
    {
      glDeleteBuffers(1, &vbo_dataPoints[i]);
      glDeleteVertexArrays(1, &vao[i]);
      vao[i] = vbo_dataPoints[i] = 0;
    }
} /* end ModelView::deleteObject() */

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
  ymin = -0.1; /* allow room for the x-axis */
  ymax  = 0;
  zmin = -1.0;
  zmax = 1.0;

  std::vector<double> datapts;
  datapts.insert( datapts.end(), _usd.begin(), _usd.end() );
  datapts.insert( datapts.end(), _aud.begin(), _aud.end() );
  datapts.insert( datapts.end(), _brl.begin(), _brl.end() );
  datapts.insert( datapts.end(), _czk.begin(), _czk.end() );
  
  for( std::vector<double>::const_iterator it = datapts.begin(); it != datapts.end(); ++it )
    {
      if( *it > ymax )
	ymax = *it;
      else if( *it < ymin )
	ymin = *it;
    }

  xyzLimits[0] = xmin; xyzLimits[1] = xmax;
  xyzLimits[2] = ymin; xyzLimits[3] = ymax;
  xyzLimits[4] = zmin; xyzLimits[5] = zmax;
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
	//scaleTrans[2] = 1.0;
	//scaleTrans[3] = 0.0;
	scaleTrans[4] = 1.0;
	scaleTrans[5] = 0.0;

	glUniform4fv(ModelView::ppuLoc_scaleTrans, 1, scaleTrans);

	// TODO: ACTUAL MODEL RENDERING HERE (OR CALL ANOTHER METHOD FROM HERE)

	for( int i = 0; i < 5; ++i )
	  {
	    if( vao[i] == 0 )
	      continue;
	    glUniform1i( ppuLoc_color, i );

	    if( i > 3 )
	      {
		glLineWidth(2.0);
	      }
	    glBindVertexArray(vao[i]);
	    glDrawArrays( GL_LINE_STRIP, 0, _points );
	  }

	glLineWidth(1.0);

	// restore the previous program
	glUseProgram(pgm);
}

void ModelView::generateBuffers()
{
  glGenVertexArrays(5, vao);
  glGenBuffers(5, vbo_dataPoints);
}

void ModelView::defineModel( int i )
{
  std::vector<double> data;

  switch( i )
    {
    case 0:
      data = _usd;
      break;
    case 1:
      data = _aud;
      break;
    case 2:
      data = _brl;
      break;
    case 3:
      data = _czk;
      break;
    default:
      data = _usd; 
   }

  typedef float vec2[2];

  vec2 * dataPoints = new vec2[_points];

  float t = -1.0; float dt = 2.0 / (_points - 1);

  for( int n = 0; n < _points; ++n, t += dt )
    {
      dataPoints[n][1] = data.at(n);
      dataPoints[n][0] = t;
    }

  glBindVertexArray(vao[i]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_dataPoints[i]);
  int numBytesInBuffer = _points * sizeof( vec2 );
  glBufferData( GL_ARRAY_BUFFER, numBytesInBuffer, dataPoints, GL_STATIC_DRAW );
  glVertexAttribPointer( ModelView::pvaLoc_wcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray( ModelView::pvaLoc_wcPosition ); 

  delete [] dataPoints;
}

void ModelView::drawAxes( int i )
{
  typedef float vec2[2];

  vec2 * axes = new vec2[2];

  float scaleTrans[4];
  computeScaleTrans(scaleTrans);

  axes[0][0] = -1.0;                                
  // y*scaleTrans[2] + scaleTrans[3] = 0
  // => y = -scaleTrans[3] / scaleTrans[2]
  axes[0][1] = -1.0 * scaleTrans[3] / scaleTrans[2];
  axes[1][0] = 1.0;
  axes[1][1] = axes[0][1];

  glBindVertexArray(vao[4+i]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_dataPoints[4+i]);
  int numBytesInBuffer = 2 * sizeof( vec2 );
  glBufferData( GL_ARRAY_BUFFER, numBytesInBuffer, axes, GL_STATIC_DRAW );
  glVertexAttribPointer( ModelView::pvaLoc_wcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0 );
  glEnableVertexAttribArray( ModelView::pvaLoc_wcPosition );

  delete [] axes;
}
