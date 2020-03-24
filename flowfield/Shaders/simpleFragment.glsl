#version 440 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
	vec4 texColor = texture(texture1,TexCoord);
	//FragColor =texColor;
	//FragColor = vec4(0,0,0,texColor.r);

	FragColor = vec4(texColor.x,texColor.y,texColor.z,texColor.w);
	//FragColor = vec4(1.0,0.0,0.0,1.0f);
}
