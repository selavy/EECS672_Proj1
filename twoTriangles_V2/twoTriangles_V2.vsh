#version 420 core

in vec2 wcPosition; // vertex position; "wc" stands for "world coordinates"
uniform vec4 scaleTrans; // for mapping coordinates into Logical Device Space

// send the LDS coordinates to the fragment shader so it can create colors based
// on position
out float ldsX, ldsY;

void main()
{
	ldsX = scaleTrans[0]*wcPosition.x + scaleTrans[1];
	ldsY = scaleTrans[2]*wcPosition.y + scaleTrans[3];
	gl_Position = vec4(ldsX, ldsY, 0, 1);
}

