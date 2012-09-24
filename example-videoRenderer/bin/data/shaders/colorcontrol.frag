//Graciously provided by
// http://mouaif.wordpress.com/2009/01/05/photoshop-math-with-glsl-shaders/

// For all settings: 1.0 = 100% 0.5=50% 1.5 = 150%
vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con)
{
	// Increase or decrease theese values to adjust r, g and b color channels seperately
	const float AvgLumR = 0.5;
	const float AvgLumG = 0.5;
	const float AvgLumB = 0.5;
	
	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
	
	vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
	vec3 brtColor = color * brt;
	vec3 intensity = vec3(dot(brtColor, LumCoeff));
	vec3 satColor = mix(intensity, brtColor, sat);
	vec3 conColor = mix(AvgLumin, satColor, con);
	return conColor;
}

uniform sampler2DRect tex;
uniform float brightness;
uniform float contrast;
uniform float saturation;
uniform int invert;
void main (void)
{
	//use the color from the image, but use the r channel of the mask as the alpha channel of our output
	vec3 color = ContrastSaturationBrightness(texture2DRect(tex,gl_TexCoord[0].st).rgb, brightness, saturation, contrast);
	if(invert == 1) color = vec3(1.0) - color;
	
	gl_FragData[0] = vec4(color, 1.0);  
}