#version 420 core

// Replace the placeholder implementation here...
uniform int color;

out vec4 fragmentColor;

vec4 colorChoices[5] =
{
	{ 1.0, 1.0, 0.0, 1.0 },
	{ 1.0, 0.0, 1.0, 1.0 },
	{ 0.0, 1.0, 1.0, 1.0 },
	{ 0.0, 0.0, 1.0, 1.0 },
	{ 0.0, 0.0, 0.0, 1.0 }, // for x & y - axes
};


void main()
{
	if( color > 3 )
	    fragmentColor = colorChoices[4];
	else
	    fragmentColor = colorChoices[color];	
}

