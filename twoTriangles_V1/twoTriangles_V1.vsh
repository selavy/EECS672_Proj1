#version 420 core

in vec2 wcPosition; // vertex position; "wc" stands for "world coordinates"
uniform vec4 scaleTrans; // for mapping coordinates into Logical Device Space

void main()
{
	float ldsX = scaleTrans[0]*wcPosition.x + scaleTrans[1];
	float ldsY = scaleTrans[2]*wcPosition.y + scaleTrans[3];
	gl_Position = vec4(ldsX, ldsY, 0, 1);
}

