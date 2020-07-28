#version 440 core

layout(location = 0) out vec4 colorOut0;
//layout(location = 1) out vec4 colorOut1;
layout(location = 2) out uint stencil;
//out vec4 color;

in DATA
{
	vec4 pos;
	vec4 col;
	float tid;
	vec2 texCoords;
} fs_in;

uniform vec2 lightPos;
uniform vec2 pos;
uniform vec2 pos2;
uniform vec3 color1;
uniform vec3 color2;

uniform int activeLights;
uniform int currentActiveOutput;
uniform vec3 lightArray[16];
uniform vec3 colorArray[16];

uniform float lightZPosition; //not technically a light radius, its the z value to move the light up and down


uniform usampler2D stencilTexturino;
uniform int shouldLightBeRestricted;

vec3 DirectIllumination(vec3 P, vec3 N, vec3 lightCentre, float lightRadius, vec3 lightColour, float cutoff)
{
	// calculate normalized light vector and distance to sphere light surface
	float r = lightRadius;
	vec3 L = lightCentre - P;
	float distance = length(L);
	float d = max(distance - r, 0);
	L /= distance;

	// calculate basic attenuation
	float denom = d / r + 1;
	float attenuation = 1 / (denom*denom);

	attenuation = (attenuation - cutoff) / (1 - cutoff);
	attenuation = max(attenuation, 0);

	float dot = max(dot(L, N), 0);
	return lightColour * dot * attenuation;
}

void main()
{

	uint stencil1 = texelFetch(stencilTexturino, ivec2(gl_FragCoord.xy), 0).r;
	//uint stencil1 = texture(stencilTexturino, UV).r;
	vec3 colorTest = DirectIllumination(vec3(fs_in.pos.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(pos.xy, lightZPosition), 200.0f, color1, 0.05f);
	if (shouldLightBeRestricted == 1)
	{
		colorTest = colorTest * stencil1;
		//stencil = 1 * stencil1;
	}
	colorOut0 = vec4(colorTest, 1.0f);
	//colorOut0 = vec4(stencil1, stencil1, stencil1, 1.0f);
	stencil = 1 * stencil1;


}