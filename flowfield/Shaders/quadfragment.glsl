
#version 440 core
//out vec4 fragColor;
layout (location = 0) out vec4 fragColor;

in vec2 UV;

uniform sampler2D colorTexture;
uniform sampler2D shadowTexture;
//uniform usampler2D stencilTexture;
uniform usampler2D stencilTexture1;
uniform usampler2D stencilTexture2;
uniform int shouldLightBeRestricted;

void main()
{
	//fragColor = texture(screenTexture, inUV);
	//vec3 col = texture(screenTexture, inUV).rgb;

	uint stencil = texture(stencilTexture1, UV).r;
	uint stencil2 = texture(stencilTexture2, UV).r;
	fragColor = texture(shadowTexture, UV);
	vec4 color  = texture(colorTexture, UV);
	vec4 shadow = texture(shadowTexture, UV);
	/*fragColor = color * shadow;*/
	//fragColor = vec4(stencil, stencil, stencil, 1.0f);
	if (stencil == 0 )
		fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (stencil == 1) 
		fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

	vec4 diffuse = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	//color *= diffuse; //uncomment this
/*	unsigned int multiplier = 0;
	if (shouldLightBeRestricted == 1)
		multiplier = (stencil + stencil2) == 2 ? 1 : 0;
	else
		multiplier = stencil;*/

	vec4 finalColor = (stencil * shadow + color);
	finalColor = vec4(finalColor.r * 0.3 + finalColor.g * 0.59 + finalColor.b * 0.11);
	fragColor = finalColor;
	//fragColor = (stencil * shadow + color);

	//fragColor = vec4(stencil, stencil2, stencil , 1.0f);

	//fragColor = vec4(stencil, stencil, stencil, 1.0f);

	//fragColor = vec4(stencil, stencil, stencil, 1.0f);

	//fragColor = vec4(stencil, stencil, stencil, 1.0f);
	//fragColor = mix(texture(colorTexture, UV), texture(shadowTexture, UV), 0.2);

	//fragColor = vec4(inUV.x, inUV.y, 0.0, 1.0);
	//fragColor = vec4(col, 1.0);
	//fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);

}