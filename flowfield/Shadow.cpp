


struct Edge
{
	//float sx; float sy;
	//float ex; float ey;
	vec2<float> start;
	vec2<float> end;
};


struct TileCell
{
	int edge_id[4];
	int edge_exist_cell_exist = 0; //this is supposed to be a bit array for what is listed below
	//bool edge_exist[4];
	//bool exist = false;
};


TileCell* world;// = new Cell[20 * 20];
std::vector<Edge> edges;


struct Shadow
{
	enum SHADOW_TYPE
	{
		POINT,
		CONE
	};
	std::vector<vec3<float>> visibilityPolygonPoints;
	std::vector<float> xPositions;
	std::vector<float> yPositions;
	vec2<float> position;
	vec3<float> color;
	float radius;
	float zOffset;
	float attenuation;
	float shadowZPos = 30.0f;
	float left_ang = 0.0f;
	float right_ang = 0.0f;
	bool reversingNeeded = false;
	int index = -1;
	vec2<float> direction;
	SHADOW_TYPE type;
};

struct Shadows
{
	static const unsigned int MAX_SHADOWS = 10;
	unsigned int numberOfActiveShadows = 0;
	Shadow shadows[MAX_SHADOWS];
};

Shadows shadows;

static Shadow* 
CreateShadow(vec2<float> position, vec3<float> color, Shadow::SHADOW_TYPE type) 
{
	if (shadows.numberOfActiveShadows == shadows.MAX_SHADOWS)
		return nullptr;

	shadows.numberOfActiveShadows++;

	shadows.shadows[shadows.numberOfActiveShadows - 1].position = position;
	shadows.shadows[shadows.numberOfActiveShadows - 1].color = color;
	shadows.shadows[shadows.numberOfActiveShadows - 1].type = type;
	shadows.shadows[shadows.numberOfActiveShadows - 1].position = V2(0.0f, 0.0f);
	return &shadows.shadows[shadows.numberOfActiveShadows - 1];
}


struct Frustum
{
	std::vector<Edge> worldEdges;
	float width;
	float height;
	float halfWidth;
	float halfHeight;
};

Frustum frustum;

static Edge
EDGE(vec2<float> start, vec2<float> end)
{
	Edge edge;
	edge.start = start;
	edge.end = end;
	return edge;
}

static void
InitializeShadowEdges(float width, float height) 
{
	frustum.width = width;
	frustum.height = height;

	frustum.halfWidth = (width  -10.0f) / 2.0f;
	frustum.halfHeight = (height - 10.0f) / 2.0f;

	float halfWidth = frustum.halfWidth;
	float halfHeight = frustum.halfHeight;
	//Left
	/*frustum.worldEdges.push_back(EDGE(V2(-halfWidth, -halfHeight), V2(-halfWidth, halfHeight)));
	//Right
	frustum.worldEdges.push_back(EDGE(V2(halfWidth, -halfHeight), V2(halfWidth, halfHeight)));
	//Bottom
	frustum.worldEdges.push_back(EDGE(V2(-halfWidth, -halfHeight), V2(halfWidth, -halfHeight)));
	//Top
	frustum.worldEdges.push_back(EDGE(V2(-halfWidth, halfHeight), V2(halfWidth, halfHeight)));*/
}

static void 
UpdateShadowFrustum(vec2<float> player) 
{
	/*vec2<float> negatedPlayer = player;
	negatedPlayer.y = -negatedPlayer.y;
	int startIndex = edges.size() - 4;
	for (unsigned int i = 0; i < frustum.worldEdges.size(); i++)
	{
		Edge newEdge;
		newEdge.start = frustum.worldEdges[i].start + negatedPlayer;
		newEdge.end = frustum.worldEdges[i].end + negatedPlayer;
		edges[startIndex] = newEdge;
		startIndex++;
	}*/
}
//TODO Extract to some header
#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3
#define CELL_EXIST 4

float blockWidth = 32.0f;

//Again this is possible somehow with bit shift magic
static void
SetCell(Tilemap* tilemap, TileCell* cells, vec2<float> position)
{
	float remappedX = map(position.x, tilemap->worldXRange.x, tilemap->worldXRange.y, tilemap->tileXRange.x, tilemap->tileXRange.y);
	float remappedY = map(position.y, tilemap->worldYRange.x, tilemap->worldYRange.y, tilemap->tileYRange.x, tilemap->tileYRange.y);
	remappedX = Round(remappedX);
	remappedY = Round(remappedY);

	if (ValidIndex((int)remappedX, (int)remappedY, tilemap->width, tilemap->height))
	{
		if (TestBit(&cells[GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width)].edge_exist_cell_exist, CELL_EXIST))
		{
			ClearBit(&cells[GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width)].edge_exist_cell_exist, CELL_EXIST);
		}
		else 
		{
			SetBit(&cells[GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width)].edge_exist_cell_exist, CELL_EXIST);
		}
		//cells[GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width)].exist = !cells[GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width)].exist;
	}

}

static void
ConvertTileMapToPolyMap(int sx, int sy, int w, int h, float blockWidth, int pitch, vec2<float> camera) 
{
	//Clear "PolyMap"
	edges.clear();

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++) 
			for (int j = 0; j < 4; j++) {
				ClearBit(&world[(y + sy) * pitch + (x + sx)].edge_exist_cell_exist, j);
				//world[(y + sy) * pitch + (x + sx)].edge_exist[j] = false;
				world[(y + sy) * pitch + (x + sx)].edge_id[j] = 0;
			}

	for (int x = 1; x < w - 1; x++)
		for (int y = 1; y < h - 1; y++) {

			int i = (y + sy) * pitch + (x + sx); // This
			int n = (y + sy - 1) * pitch + (x + sx); //north
			int s = (y + sy + 1) * pitch + (x + sx); //south
			int w = (y + sy) * pitch + (x + sx - 1); // west
			int e = (y + sy) * pitch + (x + sx + 1); // east

			//If this cell exists, run algorithm
			//if (world[i].exist) {
			if (TestBit(&world[i].edge_exist_cell_exist, CELL_EXIST)) {

				//If this cell has no western neighbour, it needs a western edge
				//if (!world[w].exist) {
				if (!TestBit(&world[w].edge_exist_cell_exist, CELL_EXIST)) {

					//It can either extend its northern neighbours western edge or 
					//Or it can start a new one if neither of these had one
					//if (world[n].edge_exist[WEST]) {
					if (TestBit(&world[n].edge_exist_cell_exist, WEST)) {
						//Northern neighbour has a western edge, so we can extend it downward
						edges[world[n].edge_id[WEST]].end.y += blockWidth;
						world[i].edge_id[WEST] = world[n].edge_id[WEST];
						//world[i].edge_exist[WEST] = true;
						SetBit(&world[i].edge_exist_cell_exist, WEST);
					}
					else {
						Edge edge;
						edge.start.x = (sx + x) * blockWidth;
						edge.start.y = (sy + y) * blockWidth;
						edge.end.x = edge.start.x;
						edge.end.y = edge.start.y + blockWidth;

						int edge_id = edges.size();
						edges.push_back(edge);

						world[i].edge_id[WEST] = edge_id;
						//world[i].edge_exist[WEST] = true;
						SetBit(&world[i].edge_exist_cell_exist, WEST);
					}
				}

				//If this cell has no western neighbour, it needs a eastern edge
				//if (!world[e].exist) {
				if (!TestBit(&world[e].edge_exist_cell_exist, CELL_EXIST)) {

					//It can either extend its northern neighbours western edge or 
					//Or it can start a new one if neither of these had one
					//if (world[n].edge_exist[EAST]) {
					if (TestBit(&world[n].edge_exist_cell_exist, EAST)) {
						//Northern neighbour has a western edge, so we can extend it downward
						edges[world[n].edge_id[EAST]].end.y += blockWidth;
						world[i].edge_id[EAST] = world[n].edge_id[EAST];
						//world[i].edge_exist[EAST] = true;
						SetBit(&world[i].edge_exist_cell_exist, EAST);
					}
					else {
						Edge edge;
						edge.start.x = (sx + x + 1) * blockWidth;
						edge.start.y = (sy + y) * blockWidth;
						edge.end.x = edge.start.x;
						edge.end.y = edge.start.y + blockWidth;

						int edge_id = (int)edges.size();
						edges.push_back(edge);

						world[i].edge_id[EAST] = edge_id;
						//world[i].edge_exist[EAST] = true;
						SetBit(&world[i].edge_exist_cell_exist, EAST);
					}
				}

				//If this cell has no northern neighbour, it needs a northern edge
				//if (!world[n].exist) {
				if (!TestBit(&world[n].edge_exist_cell_exist, CELL_EXIST)) {

					//It can either extend its western neighbour e or 
					//Or it can start a new one if neither of these had one
					//if (world[w].edge_exist[NORTH]) {
					if (TestBit(&world[w].edge_exist_cell_exist, NORTH)) {
						//Northern neighbour has a western edge, so we can extend it downward
						edges[world[w].edge_id[NORTH]].end.x += blockWidth;
						world[i].edge_id[NORTH] = world[w].edge_id[NORTH];
						//world[i].edge_exist[NORTH] = true;
						SetBit(&world[i].edge_exist_cell_exist, NORTH);
					}
					else {
						Edge edge;
						edge.start.x = (sx + x) * blockWidth;
						edge.start.y = (sy + y) * blockWidth;
						edge.end.x = edge.start.x + blockWidth;
						edge.end.y = edge.start.y;

						int edge_id = edges.size();
						edges.push_back(edge);

						world[i].edge_id[NORTH] = edge_id;
						//world[i].edge_exist[NORTH] = true;
						SetBit(&world[i].edge_exist_cell_exist, NORTH);
					}
				}


				//If this cell has no southern neighbour, it needs a northern edge
				//if (!world[s].exist) {
				if (!TestBit(&world[s].edge_exist_cell_exist, CELL_EXIST)) {

					//It can either extend its western neighbour e or 
					//Or it can start a new one if neither of these had one
					//if (world[w].edge_exist[SOUTH]) {
					if (TestBit(&world[w].edge_exist_cell_exist, SOUTH)) {
						//Northern neighbour has a western edge, so we can extend it downward
						edges[world[w].edge_id[SOUTH]].end.x += blockWidth;
						world[i].edge_id[SOUTH] = world[w].edge_id[SOUTH];
						//world[i].edge_exist[SOUTH] = true;
						SetBit(&world[i].edge_exist_cell_exist, SOUTH);
					}
					else {
						Edge edge;
						edge.start.x = (sx + x) * blockWidth;
						edge.start.y = (sy + y + 1) * blockWidth;
						edge.end.x = edge.start.x + blockWidth;
						edge.end.y = edge.start.y;

						int edge_id = edges.size();
						edges.push_back(edge);

						world[i].edge_id[SOUTH] = edge_id;
						//world[i].edge_exist[SOUTH] = true;
						SetBit(&world[i].edge_exist_cell_exist, SOUTH);
					}
				}

			}

		}
	
		/*Edge edge;
		edge.sx = 150.0f;
		edge.sy = 450.0f;
		edge.ex = 300.0f;
		edge.ey = 320.0f;
		edges.push_back(edge);*/

		//Add the world edges

		camera.y = -camera.y;
		for(unsigned int i = 0; i < frustum.worldEdges.size(); i++ )
		{
			Edge newEdge;
			newEdge.start = frustum.worldEdges[i].start + camera;
			newEdge.end   = frustum.worldEdges[i].end + camera;
			edges.push_back(newEdge);
		}
}

static float
PseudoAngle(float dx, float dy)
{
	float p = dx / (fabs(dx) + fabs(dy));
	if (dy < 0)
		return 3 + p;
	else
		return 1 - p;
}


static int
NormalizeAngle(int angle)
{
	/*int newAngle = angle;
	while (newAngle <= -180) newAngle += 360;
	while (newAngle > 180) newAngle -= 360;
	return newAngle;*/
	// reduce the angle  
	angle = angle % 360;

	// force it to be the positive remainder, so that 0 <= angle < 360  
	angle = (angle + 360) % 360;

	// force into the minimum absolute value residue class, so that -180 < angle <= 180  
	if (angle > 180)
		angle -= 360;
	return angle;
}
static void
CalculateConeShadow(Shadow* shadow, float radius)
{
	shadow->visibilityPolygonPoints.clear();

	//Note that y is negative
	//float ox = shadow->position.x;
	//float oy = -shadow->position.y;

	vec2<float> start = V2(shadow->position.x, -shadow->position.y);

	//This is because everything else is calculated from a mid point position
	//start = start + V2(16.0f, -16.0f);

/*	vec2<float> direction = V2(1.0f, 0.0f);

	vec2<float> left = RotateVec(direction, 45);
	vec2<float> right = RotateVec(direction, -45);
	*/


	vec2<float> left = RotateVec(shadow->direction, -30.0);
	vec2<float> right = RotateVec(shadow->direction, 30.0);

	/*if ((left.x < 0.0f && left.y > 0.0f) && (right.x < 0.0f && right.y < 0.0f))
	{
		vec2<float> temp = right;
		right = left;
		left = temp;
	}*/
	//left = Normalize(left);
	//right = Normalize(right);


	//if (right.x < 0.0f && right.y < 0.0f)
	//	right.y *= -1.0f;


	//std::vector<vec3<float>> tempResults;
	for (int h = 0; h < 2; h++ )
	{
		vec2<float> ray;
		if (h == 0)
			ray = left;
		if (h == 1)
			ray = right;


		float min_t1 = INFINITY;
		float min_px = 0, min_py = 0, min_ang = 0;
		bool valid = false;

		for (auto &e2 : edges)
		{
			float sdx = e2.end.x - e2.start.x;
			float sdy = e2.end.y - e2.start.y;

			//Check that ray isnt collinear with the edge, if either one of the abs are then theyre on the same line
			//if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
			if (fabs(sdx - ray.x) > 0.0f && fabs(sdy - ray.y) > 0.0f)
			{
				// t2 is normalized distance from line segment start to line segment end of intersection point
				//float t2 = (rdx * (e2.start.y - oy) + (rdy * (ox - e2.start.x))) / (sdx * rdy - sdy * rdx);
				float t2 = (ray.x * (e2.start.y - start.y) + (ray.y * (start.x - e2.start.x))) / (sdx * ray.y - sdy * ray.x);
				// t1 is normalized distance from source along ray to ray length of intesrect point
				//float t1 = (e2.start.x + sdx * t2 - ox) / rdx;
				float t1 = (e2.start.x + sdx * t2 - start.x) / ray.x;

				// if intersect point exists along ray, and along line
				// segment then intersect point is valid
				if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
				{
					// Check if this intersetct point is closet to source, if
					//it is then store this point and reject others
					if (t1 < min_t1)
					{
						/*min_t1 = t1;
						min_px = ox + rdx * t1;
						min_py = oy + rdy * t1;
						min_ang = atan2f(min_py - oy, min_px - ox);
						valid = true;*/
						min_t1 = t1;
						min_px = start.x + ray.x * t1;
						min_py = start.y + ray.y * t1;
						min_ang = atan2f(min_py - start.y, min_px - start.x);


						valid = true;
					}
				}
			}
		}

		if (valid)
		{
			/*vec2<float> currentRay = V2(min_px, min_py) - start;
			if (Len(currentRay) > radius)
				currentRay = Normalize(currentRay) * radius;
			shadow->visibilityPolygonPoints.push_back(V3(min_ang, currentRay.x, currentRay.y));*/
			float convertRadiansToDegrees = 180.0 / 3.14159265359;
			float resultInDegrees = min_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees
			min_ang = resultInDegrees;// NormalizeAngle(resultInDegrees);
			shadow->visibilityPolygonPoints.push_back(V3(min_ang, min_px, min_py));
			//tempResults.push_back(V3(min_ang, min_px, min_py));
		}

	}
	
	/*if (shadow->visibilityPolygonPoints.size() == 0)
		std::cout << "its 0 " <<std::endl;
	if (shadow->visibilityPolygonPoints.size() == 1)
		std::cout << "its 1 " << std::endl;*/
	//1 is right
	shadow->reversingNeeded = false;
	if (shadow->visibilityPolygonPoints.size() == 2) 
	{
		if (abs(shadow->visibilityPolygonPoints[0].x - shadow->visibilityPolygonPoints[1].x) > 65.0f)
		{
			if (shadow->visibilityPolygonPoints[0].x > shadow->visibilityPolygonPoints[1].x)
			{
				shadow->reversingNeeded = true;
				/*vec2<float> temp = V2(shadow->visibilityPolygonPoints[0].y, shadow->visibilityPolygonPoints[0].z);
				shadow->visibilityPolygonPoints[0].y = shadow->visibilityPolygonPoints[1].y;
				shadow->visibilityPolygonPoints[0].z = shadow->visibilityPolygonPoints[1].z;
				shadow->visibilityPolygonPoints[1].y = temp.x;
				shadow->visibilityPolygonPoints[1].z = temp.y;*/

				
				//shadow->visibilityPolygonPoints[0].x = -shadow->visibilityPolygonPoints[0].x;
				/*vec3<float> temp = shadow->visibilityPolygonPoints[0];
				shadow->visibilityPolygonPoints[0] = shadow->visibilityPolygonPoints[1];
				shadow->visibilityPolygonPoints[1] = temp;*/
			}

			/*vec3<float> temp = shadow->visibilityPolygonPoints[0];
			shadow->visibilityPolygonPoints[0] = shadow->visibilityPolygonPoints[1];
			shadow->visibilityPolygonPoints[1] = temp;*/

			/*float temp = shadow->visibilityPolygonPoints[0].x;
			shadow->visibilityPolygonPoints[0].x = shadow->visibilityPolygonPoints[1].x;
			shadow->visibilityPolygonPoints[1].x = temp;*/
				//shadow->visibilityPolygonPoints[0].x = -shadow->visibilityPolygonPoints[0].x;
		/*	if (shadow->visibilityPolygonPoints[0].x > shadow->visibilityPolygonPoints[1].x)
				shadow->visibilityPolygonPoints[0].x = -shadow->visibilityPolygonPoints[0].x;
			else
				shadow->visibilityPolygonPoints[1].x = -shadow->visibilityPolygonPoints[1].x;*/

		}
	}
	




	//	if (ray.x < 0.0f && ray.y < 0.0f)
//		min_ang *= -min_ang;
	/*float convertRadiansToDegrees = 180.0 / 3.14159265359;
	//float resultInDegrees = min_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees

	//std::cout << "min degree: " << std::to_string(resultInDegrees) << std::endl;

	float direction_ang = atan2f(shadow->direction.y, shadow->direction.x);
	float direction_degrees = direction_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees
	//std::cout << "left degree: " << std::to_string(direction_ang) << std::endl;

	float left_ang = atan2f(left.y, left.x);
	shadow->left_ang = left_ang;
	float left_degrees = left_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees
	//std::cout << "left degree: " << std::to_string(direction_ang) << std::endl;

	float right_ang = atan2f(right.y, right.x);
	shadow->right_ang = right_ang;
	float right_degrees = right_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees
	//std::cout << "left degree: " << std::to_string(direction_ang) << std::endl;
	//min_ang = resultInDegrees;

	std::cout << "D : " << std::to_string(direction_degrees)
		<< " L : " << std::to_string(left_degrees)
		<< " R : " << std::to_string(right_degrees);*/
	
	
	// << std::endl;

	/*std::cout << " D : " << std::to_string(shadow->direction.y)
		      << " L : " << std::to_string(left.y)
		      << " R : " << std::to_string(right.y);
	*/

















	// For each edge
	//unsigned int index = -1;
	for (auto &e1 : edges)
	{
		// Take the start point, then the end point 

		for (int i = 0; i < 2; i++)
		{
			//float rdx, rdy;
			vec2<float> ray;

			//vectors from start point to both edges of vertex
			ray = (i == 0 ? V2(e1.start.x, e1.start.y) : V2(e1.end.x, e1.start.y)) - start;

			vec2<float> normalizedRay = Normalize(ray);
			float crossLeft = Cross(left, normalizedRay);
			float crossRight = Cross(normalizedRay, right);

			if (!(crossLeft > 0 && crossRight > 0))
				continue;

			/*float one = Cross(right, left);
			float two = Cross(right, normalizedRay);
			float three = Cross(normalizedRay, left);
			float four = Cross(normalizedRay, right);

			float dotOne = Dot(left, V2(0.0f, 1.0f));
			float dotTwo = Dot(normalizedRay, V2(0.0f, 1.0f));
			float dotThree = Dot(right, V2(0.0f, 1.0f));*/
			//if (!(dotTwo < dotThree && dotTwo > dotOne))
			//	continue;
			//if (crossLeft > 0 && crossRight > 0)
			//if (!(one * two >= 0 && three * four >= 0))
			//	continue;
			//ray = Normalize(ray);

		//	float length = Len(ray - start);
			//float radius = 400.0f;
			//if (length > radius)
			///	continue;

			//rdx = (i == 0 ? e1.start.x : e1.end.x) - ox;
			//rdy = (i == 0 ? e1.start.y : e1.end.y) - oy;


			//float base_ang = atan2f(rdy, rdx);




			float base_ang = atan2f(ray.y, ray.x);
			//float base_ang = copysign(1. - ray.x / (fabs(ray.x) + fabs(ray.y)), ray.y);



			/*float test_ang = atan2f(left.y, left.x);
			float test_ang2 = atan2f(right.y, right.x);
			if (!(base_ang > test_ang && base_ang < test_ang2))
				continue;*/

			float ang = 0;

			// For each point, cast 3 rays, 1 directly at point and 1 a little bit to either side
			for (int j = 0; j < 3; j++)
			{
				if (j == 0)    ang = base_ang - 0.0001f;
				if (j == 1)    ang = base_ang;
				if (j == 2)    ang = base_ang + 0.0001f;

				//vec2<float> ray;
				ray.x = radius * cosf(ang);
				ray.y = radius * sinf(ang);


				//rdx = radius * cosf(ang);
				//rdy = radius * sinf(ang);

				float min_t1 = INFINITY;
				float min_px = 0, min_py = 0, min_ang = 0;
				bool valid = false;

				for (auto &e2 : edges)
				{
					float sdx = e2.end.x - e2.start.x;
					float sdy = e2.end.y - e2.start.y;

					//Check that ray isnt collinear with the edge, if either one of the abs are then theyre on the same line
					//if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
					if (fabs(sdx - ray.x) > 0.0f && fabs(sdy - ray.y) > 0.0f)
					{
						// t2 is normalized distance from line segment start to line segment end of intersection point
						//float t2 = (rdx * (e2.start.y - oy) + (rdy * (ox - e2.start.x))) / (sdx * rdy - sdy * rdx);
						float t2 = (ray.x * (e2.start.y - start.y) + (ray.y * (start.x - e2.start.x))) / (sdx * ray.y - sdy * ray.x);
						// t1 is normalized distance from source along ray to ray length of intesrect point
						//float t1 = (e2.start.x + sdx * t2 - ox) / rdx;
						float t1 = (e2.start.x + sdx * t2 - start.x) / ray.x;

						// if intersect point exists along ray, and along line
						// segment then intersect point is valid
						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
						{
							// Check if this intersetct point is closet to source, if
							//it is then store this point and reject others
							if (t1 < min_t1)
							{
								/*min_t1 = t1;
								min_px = ox + rdx * t1;
								min_py = oy + rdy * t1;
								min_ang = atan2f(min_py - oy, min_px - ox);
								valid = true;*/




								min_t1 = t1;
								min_px = start.x + ray.x * t1;
								min_py = start.y + ray.y * t1;
								min_ang = atan2f(min_py - start.y, min_px - start.x);
								//float dx = min_px - start.x;
								//float dy = min_py - start.y; 
								//min_ang = copysign(1. - dx / (fabs(dx) + fabs(dy)), dy);
								//min_ang = PseudoAngle(dx, dy);




							/*	float convertRadiansToDegrees = 180.0 / 3.14159265359;
								float resultInDegrees = min_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees
								min_ang = resultInDegrees;*/



								valid = true;
							}
						}
					}
				}

				if (valid)
				{
					float convertRadiansToDegrees = 180.0 / 3.14159265359;
					float resultInDegrees = min_ang * convertRadiansToDegrees + 180.0; // 0 to 360 degrees
					min_ang = resultInDegrees;// NormalizeAngle(resultInDegrees);
					shadow->visibilityPolygonPoints.push_back(V3(min_ang, min_px, min_py));
				}
					
			}
		}
	}

	
	/*if (reversingNeeded) 
	{
		std::sort
		(
			shadow->visibilityPolygonPoints.begin(),
			shadow->visibilityPolygonPoints.end(),
			[&](const vec3<float>& t1, const vec3<float>& t2)
			{
				return t1.x > t2.x;
			}
		);
	}
	else 
	{
		std::sort
		(
			shadow->visibilityPolygonPoints.begin(),
			shadow->visibilityPolygonPoints.end(),
			[&](const vec3<float>& t1, const vec3<float>& t2)
			{
				return t1.x < t2.x;
			}
		);
	}
	*/


	/*float threshold = 360.0f - 60.0f; //entire cone width
	if (shadow->reversingNeeded)
	{
		for (unsigned int i = 0; i < shadow->visibilityPolygonPoints.size(); i++)
		{
			//if (shadow->visibilityPolygonPoints[i].x > threshold && shadow->visibilityPolygonPoints[i].x < 360.0f)
			//if (shadow->visibilityPolygonPoints[i].x < threshold)
			if (shadow->visibilityPolygonPoints[i].x > threshold && shadow->visibilityPolygonPoints[i].x < 360.0f)
			{
				shadow->visibilityPolygonPoints[i].x = -shadow->visibilityPolygonPoints[i].x;
			}
		}
	}*/

	std::sort
	(
		shadow->visibilityPolygonPoints.begin(),
		shadow->visibilityPolygonPoints.end(),
		[&](const vec3<float>& t1, const vec3<float>& t2)
		{
			return t1.x < t2.x;
		}
	);




	/*float threshold = 360.0f - 30.0f;
	std::sort
	(
		shadow->visibilityPolygonPoints.begin(),
		shadow->visibilityPolygonPoints.end(),
		[&](const vec3<float>& t1, const vec3<float>& t2)
		{
			if ((t1.x >= threshold && t1.x <= 360.0f) && (t2.x >= threshold && t2.x <= 360.0f))
				return t1.x > t2.x;
			else
				return t1.x < t2.x;
		}
	);*/

	/*if (reversingNeeded) 
	{

	}*/

	/*for(unsigned int i = 0; i < tempResults.size(); i++) 
	{
	//	shadow->visibilityPolygonPoints.push_back(tempResults[i]);
	}
	if (tempResults.size() == 1)
	{
		shadow->visibilityPolygonPoints.push_back(tempResults[0]);
	}
	if (tempResults.size() == 2) 
	{
		shadow->visibilityPolygonPoints.push_back(tempResults[0]);
		shadow->visibilityPolygonPoints.push_back(tempResults[1]);
	}*/
	


	//probably not needed, light algorithm is way heavier than drawing a few triangles
	int raysCast = shadow->visibilityPolygonPoints.size();
	//std::cout << "rays cast: " << raysCast << std::endl;

	auto it = unique(
		shadow->visibilityPolygonPoints.begin(),
		shadow->visibilityPolygonPoints.end(),
		[&](vec3<float>& t1, vec3<float>& t2)
		{
			return fabs(t1.y - t2.y) < 0.1f && fabs(t1.z - t2.z) < 0.1f;
		});
	shadow->visibilityPolygonPoints.resize(distance(shadow->visibilityPolygonPoints.begin(), it));
	




	shadow->index = -1;
	if (shadow->reversingNeeded) 
	{
		shadow->index = -1;
		float threshold = 360.0f - 60.0f; // enitre cone width
		for (unsigned int i = 0; i < shadow->visibilityPolygonPoints.size(); i++)
		{
			if (shadow->visibilityPolygonPoints[i].x > threshold)
			{
				shadow->index = i;
				break;
			}
		}

		/*if (shadow->index != -1)
		{
			std::vector<vec3<float>> aboveThreshold;

			for (unsigned int i = shadow->index; i < shadow->visibilityPolygonPoints.size(); i++)
			{
				aboveThreshold.push_back(shadow->visibilityPolygonPoints[i]);
			}

			for (unsigned int i = 0; i < shadow->index; i++)
			{
				aboveThreshold.push_back(shadow->visibilityPolygonPoints[i]);
			}
			int bp = 10;

			shadow->visibilityPolygonPoints.clear();
			for (unsigned int i = 0; i < aboveThreshold.size(); i++)
			{
				shadow->visibilityPolygonPoints.push_back(aboveThreshold[i]);
			}
		}*/
	}
	//shadow->reversingNeeded = false;

	//shadow->reversingNeeded = false;





	/*for (unsigned int i = 0; i < shadow->visibilityPolygonPoints.size(); i++)
	{
		std::cout << "V : " << std::to_string(shadow->visibilityPolygonPoints[i].x);

	}*/
	/*for (unsigned int i = 0; i < shadow->visibilityPolygonPoints.size(); i++)
	{
		std::cout << "V : " << std::to_string(shadow->visibilityPolygonPoints[i].x);
		
	}*/
	//std::cout << std::endl;




	


	int raysDrawn = shadow->visibilityPolygonPoints.size();

}

static void 
CalculatePointShadow(Shadow* shadow, float radius)
{
	shadow->visibilityPolygonPoints.clear();

	//Note that y is negative
	float ox = shadow->position.x;
	float oy = -shadow->position.y;


	//needed because of how we render stuff, this shadow thing is just a "render" step anyway so its in line with other stuff
	//start = start + V2(-16.0f, 16.0f);
	// For each edge
	for (auto &e1 : edges)
	{
		// Take the start point, then the end point 

		for (int i = 0; i < 2; i++)
		{
			float rdx, rdy;

			//vectors from start point to both edges of vertex
			rdx = (i == 0 ? e1.start.x : e1.end.x) - ox;
			rdy = (i == 0 ? e1.start.y : e1.end.y) - oy;

			float base_ang = atan2f(rdy, rdx);

			float ang = 0;

			// For each point, cast 3 rays, 1 directly at point and 1 a little bit to either side
			for (int j = 0; j < 3; j++)
			{
				if (j == 0)    ang = base_ang - 0.0001f;
				if (j == 1)    ang = base_ang;
				if (j == 2)    ang = base_ang + 0.0001f;

				rdx = radius * cosf(ang);
				rdy = radius * sinf(ang);

				float min_t1 = INFINITY;
				float min_px = 0, min_py = 0, min_ang = 0;
				bool valid = false;

				for (auto &e2 : edges)
				{
					float sdx = e2.end.x - e2.start.x;
					float sdy = e2.end.y - e2.start.y;

					//Check that ray isnt collinear with the edge
					if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
					{
						// t2 is normalized distance from line segment start to line segment end of intersection point
						float t2 = (rdx * (e2.start.y - oy) + (rdy * (ox - e2.start.x))) / (sdx * rdy - sdy * rdx);
						// t1 is normalized distance from source along ray to ray length of intesrect point
						float t1 = (e2.start.x + sdx * t2 - ox) / rdx;

						// if intersect point exists along ray, and along line
						// segment then intersect point is valid
						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
						{
							// Check if this intersetct point is closet to source, if
							//it is then store this point and reject others
							if (t1 < min_t1)
							{
								min_t1 = t1;
								min_px = ox + rdx * t1;
								min_py = oy + rdy * t1;
								min_ang = atan2f(min_py - oy, min_px - ox);
								valid = true;
							}
						}
					}
				}

				if (valid)
					shadow->visibilityPolygonPoints.push_back(V3(min_ang, min_px, min_py));
			}
		}
	}

	std::sort
	(
		shadow->visibilityPolygonPoints.begin(),
		shadow->visibilityPolygonPoints.end(),
		[&](const vec3<float>& t1, const vec3<float>& t2)
		{
			return t1.x < t2.x;
		}
	);


	//probably not needed, light algorithm is way heavier than drawing a few triangles
	int raysCast = shadow->visibilityPolygonPoints.size();
	//std::cout << "rays cast: " << raysCast << std::endl;

	auto it = unique(
		shadow->visibilityPolygonPoints.begin(),
		shadow->visibilityPolygonPoints.end(),
		[&](vec3<float>& t1, vec3<float>& t2)
		{
			return fabs(t1.y - t2.y) < 0.1f && fabs(t1.z - t2.z) < 0.1f;
		});
	shadow->visibilityPolygonPoints.resize(distance(shadow->visibilityPolygonPoints.begin(), it));


	int raysDrawn = shadow->visibilityPolygonPoints.size();

}

static void
CalculateVisibilityPolgyons(Shadow* shadow, float radius)
{

	if (shadow->type == Shadow::SHADOW_TYPE::POINT)
		CalculatePointShadow(shadow, radius);
	else 
		CalculateConeShadow(shadow, radius);
}

static void
CalculateVisibilityPolgyons(float radius)
{
	for (unsigned int k = 0; k < shadows.numberOfActiveShadows; k++) 
	{
		if (shadows.shadows[k].type == Shadow::SHADOW_TYPE::POINT)
			CalculatePointShadow(&shadows.shadows[k], radius);
		else
			CalculateConeShadow(&shadows.shadows[k], radius);

	}

}
