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
uniform vec2 shit;
uniform vec2 shit2;
uniform vec3 color1;
uniform vec3 color2;

uniform int activeLights;
uniform int currentActiveOutput;
uniform vec3 lightArray[16];
uniform vec3 colorArray[16];

uniform float lightZPosition; //not technically a light radius, its the z value to move the light up and down


uniform usampler2D stencilTexturino;
uniform int shouldLightBeRestrictedrino;

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

	// scale and bias attenuation such that:
	//   attenuation == 0 at extent of max influence
	//   attenuation == 1 when d == 0
	attenuation = (attenuation - cutoff) / (1 - cutoff);
	attenuation = max(attenuation, 0);

	float dot = max(dot(L, N), 0);
	return lightColour * dot * attenuation;
}

void main()
{
/*	float intensity = 50.0f / length(fs_in.pos.xy - shit);

	float constant = 0.2f;
	float linear = 0.0009f;
	float quadratic = 0.0032f;

	float distance = length(fs_in.pos.xy - shit);

	float attenuation = 1.0 / (constant + linear * distance +
		quadratic * (distance * distance));


	float radius = 200.0f;
	float att = clamp(1.0 - distance * distance / (radius*radius), 0.0, 1.0); 
	att *= att;


	//attenuation
	float radiuser = 10000.0f;
	float test = max(0, 1 - (distance / radiuser));
	test *= test;*/

				 //DirectIllumination(vec3 P, vec3 N, vec3 lightCentre, float lightRadius, vec3 lightColour, float cutoff)
	//vec3 colorTest = DirectIllumination(vec3(gl_FragCoord.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(shit.xy, 30.0f), 200.0f, vec3(0.0f, 1.0f, 0.0f), 0.05f);
	//vec3 colorTest = DirectIllumination(vec3(fs_in.pos.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(shit.xy, 30.0f), 200.0f, vec3(0.0f, 1.0f, 0.0f), 0.05f);
	//vec3 colorTest = DirectIllumination(vec3(fs_in.pos.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(shit.xy, 30.0f), 200.0f, color1, 0.05f);
	//vec3 colorTest2 = DirectIllumination(vec3(fs_in.pos.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(shit2.xy, 30.0f), 200.0f, color2, 0.05f);

	//color *= attenuation;

	//vec4 texColor = fs_in.col;
	//color = texColor * intensity;
	//color = texColor * attenuation;
	//color = vec4(colorTest + colorTest2, 1.0f);
	/*if (currentActiveOutput == 0)
		colorOut0 = vec4(colorTest, 0.5f);
	if (currentActiveOutput == 1)
		colorOut1 = vec4(colorTest, 0.5f);*/





	//vec3 colorTest = DirectIllumination(vec3(fs_in.pos.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(shit.xy, 30.0f), 200.0f, color1, 0.05f);
	uint stencil1 = texelFetch(stencilTexturino, ivec2(gl_FragCoord.xy), 0).r;
	//uint stencil1 = texture(stencilTexturino, UV).r;
	vec3 colorTest = DirectIllumination(vec3(fs_in.pos.xy, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(shit.xy, lightZPosition), 200.0f, color1, 0.05f);
	if (shouldLightBeRestrictedrino == 1)
	{
		colorTest = colorTest * stencil1;
	}
	colorOut0 = vec4(colorTest, 1.0f);
	//colorOut0 = vec4(stencil1, stencil1, stencil1, 1.0f);
	stencil = 1;





	//color = texColor * test;
	//color = texColor * att;
	//color = vec4(1.0f, 0.0f, 0.0f, 0.6f) * intensity;
	/*if (lightPos.x > 0)
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (lightPos.x == 0)
		color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	else 
		color = vec4(0.0f, 1.0f, 0.0f, 1.0f);*/
	//color = vec4(lightPos.x, 0.0f, 0.0f, 1.0f);
}