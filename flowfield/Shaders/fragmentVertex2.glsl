#version 440 core

layout(location = 0) out vec4 color;
//out vec4 color;

in DATA
{
	vec4 pos;
	vec4 col;
	float tid;
	vec2 texCoords;
} fs_in;

//uniform vec2 lightPos;
//uniform sampler2D texture2d;
uniform sampler2D textureArray[16];
uniform usampler2D stencilTexturinoChar;
void main()
{
	//float intensity = 50.0f / length(fs_in.pos.xy - lightPos);
	uint stencil1 = texelFetch(stencilTexturinoChar, ivec2(gl_FragCoord.xy), 0).r;
	vec4 texColor = fs_in.col;
	if (fs_in.tid > 0.0)
	{
		int tid = int(fs_in.tid - 0.5);
		texColor = fs_in.col *texture(textureArray[tid], fs_in.texCoords);
		texColor = texColor.xyzw;
		//texColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//	texColor = vec4(1.0f,1.0f,1.0f,1.0f);

	}
	/*if (fs_in.tid > 0.0)
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	if (fs_in.tid < 0.0)
		color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	if (fs_in.tid == 0.0)
		color = vec4(0.0f, 1.0f, 0.0f, 1.0f);*/
		//texColor.xyz *= intensity;
		//vec4 diffuse = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	color = texColor * stencil1;// *diffuse;// * intensity;
	//color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}