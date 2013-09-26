#version 420 core

in vec2 wcPosition;
in float vertexTemperature;
in vec4 vertexColor;

uniform vec4 scaleTrans;
out vec4 colorToFS;
out float temperatureToFS;

void main()
{
	colorToFS = vertexColor;
	temperatureToFS = vertexTemperature;
	float ldsX = scaleTrans[0]*wcPosition.x + scaleTrans[1];
	float ldsY = scaleTrans[2]*wcPosition.y + scaleTrans[3];
	gl_Position = vec4(ldsX, ldsY, 0, 1);
}
