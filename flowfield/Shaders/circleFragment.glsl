#version 440 core

layout(location = 0) out uint stencil;

in DATA
{
	vec4 pos;
	vec4 col;
	float tid;
	vec2 texCoords;
} fs_in;


void main()
{
	stencil = 1;
}