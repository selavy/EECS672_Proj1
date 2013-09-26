#version 420 core

in vec4 colorToFS;
in float temperatureToFS;

// colorMode:
//   1 ==> use interpolated vertex color (colorToFS)
//   2 ==> use temperature to build a color ramp
uniform int colorMode;

out vec4 fragmentColor;

const vec4 VERY_HOT  = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 VERY_COLD = vec4(0.6, 0.6, 1.0, 1.0);

void main()
{
	if (colorMode == 1)
		fragmentColor = colorToFS;
	else
	{
		// assume temperatures are in -32 <= temperature <= 120
		if (temperatureToFS < -32.0)
			fragmentColor = VERY_COLD;
		else if (temperatureToFS > 120)
			fragmentColor = VERY_HOT;
		else // ramp from VERY_COLD to VERY_HOT
		{
			// f is fraction of the way from coldest (-32) to hottest (120)
			float f = (temperatureToFS + 32) / 152;
			fragmentColor = mix(VERY_COLD, VERY_HOT, f);
		}
	}
}
