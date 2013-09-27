#version 420 core

// Replace the placeholder implementation here...
uniform int color;

out vec4 fragmentColor;

vec4 colorChoices[6] =
{
	{ 1.0, 1.0, 0.0, 1.0 },
	{ 1.0, 0.0, 1.0, 1.0 },
	{ 0.0, 1.0, 1.0, 1.0 },
	{ 0.0, 0.0, 1.0, 1.0 },
	{ 0.0, 0.0, 0.0, 1.0 }, // for x & y - axes
	{ 0.5, 0.5, 0.5, 0.5 }
};


void main()
{
	if( color > 4 )
	    fragmentColor = colorChoices[5];

	else
	    fragmentColor = colorChoices[color];	
}

