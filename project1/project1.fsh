#version 420 core

// Replace the placeholder implementation here...
uniform int color;

out vec4 fragmentColor;

vec4 colorChoices[4] =
{
	{ 1.0, 1.0, 0.0, 1.0 },
	{ 1.0, 0.0, 1.0, 1.0 },
	{ 0.0, 1.0, 1.0, 1.0 },
	{ 0.0, 0.0, 1.0, 1.0 }
};


void main()
{
	if( color > 3 )
	    fragmentColor = colorChoices[0];
	else
	    fragmentColor = colorChoices[color];	
}

