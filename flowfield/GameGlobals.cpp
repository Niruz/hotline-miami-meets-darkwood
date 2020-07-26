
namespace Game
{
	bool running = true;
	mat4x4 ortho = setOrthoFrustum(-640.0f, 640.0f, -360.0f, 360.0f, -100.0f, 100.0f);
	vec2<float> windowSize;
	/*Used everywhere to render at correct places and determine tilemaps etc... should be able to be changed to 8.0f to get 16x16 tiles
	for example quads are and other objects are rendered from the mid position while some world calcuilations like shadows are from edges to edges
	so we need to offset stuff when we render.

	and anothe rexample is the tilemap where we want to be able to sepecify 16x16 or 32x32 and just have the tilemap and shadowing work automatically
	*/
	//const vec2<float> tileHalfWidth = V2(16.0f, 16.0f); 
	//const vec2<float> tileFullWidth = V2(32.0f, 32.0f);
	const vec2<float> tileHalfWidth = V2(8.0f, 8.0f);
	const vec2<float> tileFullWidth = V2(16.0f, 16.0f);
}