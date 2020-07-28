
#version 440 core
//out vec4 fragColor;
layout(location = 0) out vec4 fragColor;

in vec2 UV;

uniform sampler2D colorTexture;
uniform sampler2D shadowTexture;
uniform sampler2D shadowTextureCharacters;
//uniform usampler2D stencilTexture;
uniform usampler2D stencilTexture1;
uniform usampler2D stencilTexture2;
uniform sampler2D colorTextureCharacters;
uniform usampler2D stencilTextureCharacters;
uniform int shouldLightBeRestricted;

void main()
{

	uint stencil = texture(stencilTexture1, UV).r;
	uint stencil2 = texture(stencilTexture2, UV).r;
	uint stencil3 = texture(stencilTextureCharacters, UV).r;
	fragColor = texture(shadowTexture, UV);
	vec4 color = texture(colorTexture, UV);
	vec4 shadow = texture(shadowTexture, UV);
	vec4 characters = texture(colorTextureCharacters, UV);
	vec4 playerShadow = texture(shadowTextureCharacters, UV);

	vec4 finalColor = (vec4(shadow.xyz, 1.0f) + playerShadow + color + characters * stencil3);

	if (stencil3 == 0)
	{
		finalColor = vec4(finalColor.r * 0.3 + finalColor.g * 0.59 + finalColor.b * 0.11);
		finalColor.w = 1.0f;
	}

	fragColor = finalColor;


}