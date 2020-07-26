
namespace Game
{
	bool running = true;
	mat4x4 ortho = setOrthoFrustum(-640.0f, 640.0f, -360.0f, 360.0f, -100.0f, 100.0f);
	vec2<float> windowSize;
	vec2<float> tileHalfWidth = V2(16.0f, 16.0f); //Used everywhere to render at correct places and determine tilemaps etc... should be able to be changed to 8.0f to get 16x16 tiles
}