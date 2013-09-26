#version 420 core

// replace the placeholder implementation here
in vec2 wcPosition; // vertex position; in world coords
uniform vec4 scaleTrans;

void main()
{
	float ldsX = scaleTrans[0]*wcPosition.x + scaleTrans[1];
     	float ldsY = scaleTrans[2]*wcPosition.y + scaleTrans[3];
	gl_Position = vec4(ldsX, ldsY, 0, 1);
}

