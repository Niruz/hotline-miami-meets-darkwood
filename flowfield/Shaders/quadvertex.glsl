
#version 440 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 inUV;

out vec2 UV;

void main()
{
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0); 
	UV = inUV;
}