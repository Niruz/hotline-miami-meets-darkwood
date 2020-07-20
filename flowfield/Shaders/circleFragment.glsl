#version 440 core

layout(location = 0) out uint stencil;

uniform sampler2D shadowMapTexture;

in DATA
{
	vec4 pos;
	vec4 col;
	float tid;
	vec2 texCoords;
} fs_in;


void main()
{
	//stencil = 1;
	uint green = uint(texture(shadowMapTexture, fs_in.texCoords).x);
	stencil = green;
}