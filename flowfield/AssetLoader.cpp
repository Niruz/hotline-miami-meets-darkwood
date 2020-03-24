
Sound sound;

static int
LoadAssets(Window* window)
{
	if (!LoadWaveFile(&sound, &directSound, "Audio/Stay_Closer.wav"))
	{
		return 0;
	}

	if (!InitShader(SIMPLE_SHADER, "Shaders/simpleVertex.glsl", "Shaders/simpleFragment.glsl"))
	{
		return 0;
	}
	if (!InitShader(FORWARD_SHADER, "Shaders/forwardVertex.glsl", "Shaders/fragmentVertex.glsl"))
	{
		return 0;
	}
	if (!InitShader(QUAD_SHADER, "Shaders/quadvertex.glsl", "Shaders/quadfragment.glsl"))
	{
		return 0;
	}
	if (!InitShader(SHADOW_SHADER, "Shaders/shadowvertex.glsl", "Shaders/shadowfragment.glsl")) 
	{
		return 0;
	}
	if (!InitShader(CIRCLE_SHADER, "Shaders/circleVertex.glsl", "Shaders/circleFragment.glsl"))
	{
		return 0;
	}
	//TODO: ugly 
	InitUniforms();

	if (!LoadFont("Fonts/Adventurer.ttf", ADVENTURER_128, 128))
	{
		return 0;
	}

	if (!LoadFont("Fonts/Adventurer.ttf", ADVENTURER_28, 28))
	{
		return 0;
	}

	if (!LoadFont("Fonts/Adventurer.ttf", ADVENTURER_14, 14))
	{
		return 0;
	}

	if (!LoadTexture("Textures/test.png", TEST1))
	{
		return 0;
	}

	return 1;
}