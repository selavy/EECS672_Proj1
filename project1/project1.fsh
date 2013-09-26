#version 420 core

// Replace the placeholder implementation here...
uniform int color;
out vec4 fragmentColor;

vec4 colorChoices[2] = {
     vec4(0.4, 0.7, 0.0, 1.0),
     vec4(0.7, 0.7, 0.7, 1.0)
};

void main()
{
	if( color == 0 )
	{
	    fragmentColor = colorChoices[0];
	}
	else
        {
	    fragmentColor = colorChoices[1];	
	}
}

