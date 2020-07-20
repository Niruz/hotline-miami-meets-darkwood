
namespace Game
{
	bool running = true;
	mat4x4 ortho = setOrthoFrustum(-640.0f, 640.0f, -360.0f, 360.0f, -100.0f, 100.0f);
	vec2<float> windowSize;
}