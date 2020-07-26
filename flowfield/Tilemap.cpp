struct Tilemap
{
	int* tilemap;
	unsigned int width;
	unsigned int height;
	vec2<float> worldXRange;
	vec2<float> worldYRange;
	vec2<float> tileXRange;
	vec2<float> tileYRange;
};


static void
InitializeTilemap(Tilemap* tilemap, float width, float height)
{
	unsigned int realSize = ((unsigned int)(width*height) / 32) + 1;
	//tilemap->tilemap = _Alloc_Memory(int, u64(realSize));
	tilemap->tilemap = new int[realSize];

	tilemap->width = (unsigned int)width;
	tilemap->height = (unsigned int)height;

	tilemap->tileXRange.x = 0;
	tilemap->tileXRange.y = width;

	tilemap->tileYRange.x = 0;
	tilemap->tileYRange.y = height;

	tilemap->worldXRange.x = 0;
	tilemap->worldXRange.y = width * 32.0f;

	tilemap->worldYRange.x = 0;
	tilemap->worldYRange.y = -height * 32.0f;
}


static Tilemap
GenerateTestTilemap()
{
	//height, width
	/*int gridTemp[20][20] = //Easier to visualize
	{
		{ 1 ,0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};*/

	/*int gridTemp[20][20] = //Easier to visualize
	{
		{ 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};*/

/*	int gridTemp[20][30] = //Easier to visualize
	{
		{ 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};*/

	int gridTemp[30][20] = //Easier to visualize
	{
		{ 1 ,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};


	Tilemap tilemap;
	InitializeTilemap(&tilemap, 20, 30);

	for (unsigned int i = 0; i < tilemap.width * tilemap.height; i++)
	{
		//grid[i] = gridTemp[i/width][i%width];
		//int test = i / tilemap.width;
		//int test2 = i % tilemap.height;
		//if (gridTemp[i / tilemap.width][i % tilemap.height] == 1)
		if (gridTemp[i % tilemap.height][i / tilemap.height] == 1)
		{
			SetBit(tilemap.tilemap, i);
		}
		else
		{
			ClearBit(tilemap.tilemap, i);
		}
	}

	return tilemap;
}

static void
EvaluateNode(const vec2<int>& currentNode)
{

}

static vec2<int>
WorldToTileCoordinates(Tilemap* tilemap, vec2<float> position)
{
	float remappedX = map(position.x, tilemap->worldXRange.x, tilemap->worldXRange.y, tilemap->tileXRange.x, tilemap->tileXRange.y);
	float remappedY = map(position.y, tilemap->worldYRange.x, tilemap->worldYRange.y, tilemap->tileYRange.x, tilemap->tileYRange.y);
	remappedX = Round(remappedX);
	remappedY = Round(remappedY);

	return V2((int)remappedX, (int)remappedY);
}

static void
SetTileBit(Tilemap* tilemap, vec2<float> position)
{
	float remappedX = map(position.x, tilemap->worldXRange.x, tilemap->worldXRange.y, tilemap->tileXRange.x, tilemap->tileXRange.y);
	float remappedY = map(position.y, tilemap->worldYRange.x, tilemap->worldYRange.y, tilemap->tileYRange.x, tilemap->tileYRange.y);
	remappedX = Round(remappedX);
	remappedY = Round(remappedY);

	if (ValidIndex((int)remappedX, (int)remappedY, tilemap->width, tilemap->height))
	{
		if (TestBit(tilemap->tilemap, GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width)))
			ClearBit(tilemap->tilemap, GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width));
		else 
			SetBit(tilemap->tilemap, GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width));
	}

}

//Again this is possible somehow with bit shift magic
static bool
TileWalkable(Tilemap* tilemap, vec2<float> position)
{
	float remappedX = map(position.x, tilemap->worldXRange.x, tilemap->worldXRange.y, tilemap->tileXRange.x, tilemap->tileXRange.y);
	float remappedY = map(position.y, tilemap->worldYRange.x, tilemap->worldYRange.y, tilemap->tileYRange.x, tilemap->tileYRange.y);
	remappedX = Round(remappedX);
	remappedY = Round(remappedY);

	if (ValidIndex((int)remappedX, (int)remappedY, tilemap->width, tilemap->height))
	{
		return TestBit(tilemap->tilemap, GetIndex((int)remappedX, (int)remappedY, (int)tilemap->width));
		//return GetTile(tilemap->tilemap, (unsigned int)remappedX, (unsigned int)remappedY, tilemap->width)->walkable;
	}
	else
	{
		return false;
	}
}


//NOTE: This will change the position that is entered
//NOTE: This will fill in the 
static void
CheckTileCollision(vec2<float>* position, vec2<float> moveDelta, Tilemap* tilemap, vec2<float> AABB)
{

	if (moveDelta.x > 0)
	{
		vec2<float> oldPos = *position;
		position->x += moveDelta.x;

		if (!TileWalkable(tilemap, V2(position->x + AABB.x, position->y + AABB.y))
			|| !TileWalkable(tilemap, V2(position->x + AABB.x, position->y - AABB.y)))
		{
			*position = oldPos;
		}
	}
	if (moveDelta.x < 0)
	{
		vec2<float> oldPos = *position;
		position->x += moveDelta.x;

		if (!TileWalkable(tilemap, V2(position->x - AABB.x, position->y + AABB.y))
			|| !TileWalkable(tilemap, V2(position->x - AABB.x, position->y - AABB.y)))
		{
			*position = oldPos;
		}
	}
	if (moveDelta.y > 0)
	{
		vec2<float> oldPos = *position;
		position->y += moveDelta.y;

		if (!TileWalkable(tilemap, V2(position->x + AABB.x, position->y + AABB.y))
			|| !TileWalkable(tilemap, V2(position->x - AABB.x, position->y + AABB.y)))
		{
			*position = oldPos;
		}
	}
	if (moveDelta.y < 0)
	{
		vec2<float> oldPos = *position;
		position->y += moveDelta.y;

		if (!TileWalkable(tilemap, V2(position->x + AABB.x, position->y - AABB.y))
			|| !TileWalkable(tilemap, V2(position->x - AABB.x, position->y - AABB.y)))
		{
			*position = oldPos;
		}
	}
}

//NOTE: This will change the position that is entered
//NOTE: This will return if we collided and the normal will be filled in
static bool
CheckTileCollision(vec2<float>* position, vec2<float> moveDelta, Tilemap* tilemap, vec2<float> AABB, vec2<float>* normal)
{

	bool collided = false;
	if (moveDelta.x > 0)
	{
		vec2<float> oldPos = *position;
		position->x += moveDelta.x;

		if (!TileWalkable(tilemap, V2(position->x + AABB.x, position->y + AABB.y))
			|| !TileWalkable(tilemap, V2(position->x + AABB.x, position->y - AABB.y)))
		{
			*position = oldPos;
			collided = true;
			*normal += V2(-1.0f, 0.0f);
		}
	}
	if (moveDelta.x < 0)
	{
		vec2<float> oldPos = *position;
		position->x += moveDelta.x;

		if (!TileWalkable(tilemap, V2(position->x - AABB.x, position->y + AABB.y))
			|| !TileWalkable(tilemap, V2(position->x - AABB.x, position->y - AABB.y)))
		{
			*position = oldPos;
			collided = true;
			*normal += V2(1.0f, 0.0f);
		}
	}
	if (moveDelta.y > 0)
	{
		vec2<float> oldPos = *position;
		position->y += moveDelta.y;

		if (!TileWalkable(tilemap, V2(position->x + AABB.x, position->y + AABB.y))
			|| !TileWalkable(tilemap, V2(position->x - AABB.x, position->y + AABB.y)))
		{
			*position = oldPos;
			collided = true;
			*normal += V2(0.0f, -1.0f);
		}
	}
	if (moveDelta.y < 0)
	{
		vec2<float> oldPos = *position;
		position->y += moveDelta.y;

		if (!TileWalkable(tilemap, V2(position->x + AABB.x, position->y - AABB.y))
			|| !TileWalkable(tilemap, V2(position->x - AABB.x, position->y - AABB.y)))
		{
			*position = oldPos;
			collided = true;
			*normal += V2(0.0f, 1.0f);
		}
	}
	Normalize(*normal);
	return collided;
}

static bool
IsPathToTargetClear(Tilemap* tilemap, vec2<int> p0, vec2<int> p1)
{
	//InitializeStack(stack, 1);
	//ResetStack(stack);
	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;
	int N = MAX(ABS(dx), ABS(dy));
	float divN = (N == 0) ? 0.0 : 1.0 / N;
	float xstep = dx * divN;
	float ystep = dy * divN;
	float x = p0.x, y = p0.y;
	for (int step = 0; step <= N; step++, x += xstep, y += ystep) {
		vec2<int> point = V2((int)Round(x), (int)Round(y));
		int index = GetIndex(point.x, point.y, tilemap->width);
		if (ValidIndex(point.x, point.y, tilemap->width, tilemap->height) && TestBit(tilemap->tilemap, index))
		{
			
		}
		else
		{
			return false;
		}
	}
	return true;
}


static bool
IsPathToTargetClear(Tilemap* tilemap, vec2<int> p0, vec2<int> p1, PathStack* stack)
{
	//InitializeStack(stack, 1);
	ResetStack(stack);
	int dx = p1.x - p0.x;
	int dy = p1.y - p0.y;
	int N = MAX(ABS(dx), ABS(dy));
	float divN = (N == 0) ? 0.0 : 1.0 / N;
	float xstep = dx * divN;
	float ystep = dy * divN;
	float x = p0.x, y = p0.y;
	for (int step = 0; step <= N; step++, x += xstep, y += ystep) {
		vec2<int> point = V2((int)Round(x), (int)Round(y));
		int index = GetIndex(point.x, point.y, tilemap->width);
		if (ValidIndex(point.x, point.y, tilemap->width, tilemap->height) && TestBit(tilemap->tilemap, index))
		{
			PushStack(stack, point);
		}
		else
		{
			return false;
		}
	}
	return true;
}





















struct DjikstraGrid
{
	int* djikstraGrid;
	vec2<float>* flowField;
	int width;
	int height;
	std::vector<int> toVisit;
	std::vector<int> toVisitFlowField;
};

static DjikstraGrid
InitializeDjikstraGrid(int width, int height) 
{
	DjikstraGrid djikstraGrid;
	djikstraGrid.djikstraGrid = new int[width * height];
	djikstraGrid.flowField = new vec2<float>[width * height];
	djikstraGrid.width = width;
	djikstraGrid.height = height;
	return djikstraGrid;
}


static void
GenerateDjikstraRegion(DjikstraGrid* djikstraGrid, Tilemap* tilemap, const vec2<int>& startNode, int radius)
{

	for (int i = 0; i < tilemap->width * tilemap->height; i++) {
		djikstraGrid->djikstraGrid[i] = INT_MIN;
		if (!TestBit(tilemap->tilemap, i)) {
			djikstraGrid->djikstraGrid[i] = INT_MAX;
		}
	}

	djikstraGrid->toVisit.clear();
	int width = tilemap->width;
	int height = tilemap->height;
	int startIndex = GetIndex(startNode, width);

	djikstraGrid->djikstraGrid[startIndex] = 0;

	djikstraGrid->toVisit.push_back(GetIndex(startNode.x, startNode.y, width));


	int size = djikstraGrid->toVisit.size();
	for (int i = 0; i < size; i++) {

		vec2<int> currentNode = GetIndex(djikstraGrid->toVisit[i], width);

		float length = Len(V2((float)currentNode.x, (float)currentNode.y) - V2((float)startNode.x, (float)startNode.y));
		//floating point offset
		float offset = 0.1f;

		if ((int)length >= radius)
			continue;

		int currentIndex = GetIndex(currentNode, width);


		vec2<int> north = V2(currentNode.x, currentNode.y - 1);
		vec2<int> south = V2(currentNode.x, currentNode.y + 1); //when printed the positive y is downwards, like [0,1], [0,2]
		vec2<int> east = V2(currentNode.x + 1, currentNode.y);
		vec2<int> west = V2(currentNode.x - 1, currentNode.y);

		//check if these are valid before adding
		std::vector<vec2<int>> neighbours;

		if (ValidIndex(north, width, height) && djikstraGrid->djikstraGrid[GetIndex(north, width)] != INT_MAX)
			neighbours.push_back(north);

		if (ValidIndex(south, width, height) && djikstraGrid->djikstraGrid[GetIndex(south, width)] != INT_MAX)
			neighbours.push_back(south);

		if (ValidIndex(east, width, height) && djikstraGrid->djikstraGrid[GetIndex(east, width)] != INT_MAX)
			neighbours.push_back(east);

		if (ValidIndex(west, width, height) && djikstraGrid->djikstraGrid[GetIndex(west, width)] != INT_MAX)
			neighbours.push_back(west);

		for (int k = 0; k < neighbours.size(); k++) {
			vec2<int> currentNeighbour = neighbours[k];

			int neighbourIndex = GetIndex(currentNeighbour, width);
			if (djikstraGrid->djikstraGrid[neighbourIndex] == INT_MIN) {
				djikstraGrid->djikstraGrid[neighbourIndex] = djikstraGrid->djikstraGrid[djikstraGrid->toVisit[i]] + 1;
				djikstraGrid->toVisit.push_back(neighbourIndex);
				size++;
			}


		}
	}
}

//entire region
static void
GenerateDjikstra(DjikstraGrid* djikstraGrid, Tilemap* tilemap, const vec2<int>& startNode)
{

	for (int i = 0; i < tilemap->width * tilemap->height; i++) {
		djikstraGrid->djikstraGrid[i] = INT_MIN;
		if (!TestBit(tilemap->tilemap, i)) {
			djikstraGrid->djikstraGrid[i] = INT_MAX;
		}
	}

	djikstraGrid->toVisit.clear();
	int width = tilemap->width;
	int height = tilemap->height;
	int startIndex = GetIndex(startNode, width);

	djikstraGrid->djikstraGrid[startIndex] = 0;

	djikstraGrid->toVisit.push_back(GetIndex(startNode.x, startNode.y, width));


	int size = djikstraGrid->toVisit.size();
	for (int i = 0; i < size; i++) {

		vec2<int> currentNode = GetIndex(djikstraGrid->toVisit[i], width);
		int currentIndex = GetIndex(currentNode, width);


		vec2<int> north = V2(currentNode.x, currentNode.y - 1);
		vec2<int> south = V2(currentNode.x, currentNode.y + 1); //when printed the positive y is downwards, like [0,1], [0,2]
		vec2<int> east = V2(currentNode.x + 1, currentNode.y);
		vec2<int> west = V2(currentNode.x - 1, currentNode.y);

		//check if these are valid before adding
		std::vector<vec2<int>> neighbours;

		if (ValidIndex(north, width, height) && djikstraGrid->djikstraGrid[GetIndex(north, width)] != INT_MAX)
			neighbours.push_back(north);

		if (ValidIndex(south, width, height) && djikstraGrid->djikstraGrid[GetIndex(south, width)] != INT_MAX)
			neighbours.push_back(south);

		if (ValidIndex(east, width, height) && djikstraGrid->djikstraGrid[GetIndex(east, width)] != INT_MAX)
			neighbours.push_back(east);

		if (ValidIndex(west, width, height) && djikstraGrid->djikstraGrid[GetIndex(west, width)] != INT_MAX)
			neighbours.push_back(west);

		for (int k = 0; k < neighbours.size(); k++) {
			vec2<int> currentNeighbour = neighbours[k];

			int neighbourIndex = GetIndex(currentNeighbour, width);
			if (djikstraGrid->djikstraGrid[neighbourIndex] == INT_MIN) {
				djikstraGrid->djikstraGrid[neighbourIndex] = djikstraGrid->djikstraGrid[djikstraGrid->toVisit[i]] + 1;
				djikstraGrid->toVisit.push_back(neighbourIndex);
				size++;
			}


		}
	}


	/*for (int i = 0; i < tilemap->width * tilemap->height; i++) {
		djikstraGrid->djikstraGrid[i] = INT_MIN;
		if (!TestBit(tilemap->tilemap, i)) {
			djikstraGrid->djikstraGrid[i] = INT_MAX;
		}
	}

	int counter = 0;
	bool firstTime = true;


	djikstraGrid->toVisit.clear();
	int width = tilemap->width;
	int height = tilemap->height;
	int startIndex = GetIndex(startNode, width);

	djikstraGrid->djikstraGrid[startIndex] = 0;

	djikstraGrid->toVisit.push_back(GetIndex(startNode.x, startNode.y, width));


	int size = djikstraGrid->toVisit.size();
	for (int i = 0; i < size; i++) {

		vec2<int> currentNode = GetIndex(djikstraGrid->toVisit[i], width);
		int currentIndex = GetIndex(currentNode, width);


		vec2<int> north = V2(currentNode.x, currentNode.y - 1);
		vec2<int> south = V2(currentNode.x, currentNode.y + 1); //when printed the positive y is downwards, like [0,1], [0,2]
		vec2<int> east = V2(currentNode.x + 1, currentNode.y);
		vec2<int> west = V2(currentNode.x - 1, currentNode.y);

		//check if these are valid before adding
		std::vector<vec2<int>> neighbours;

		if (ValidIndex(north, width, height) && djikstraGrid->djikstraGrid[GetIndex(north, width)] != INT_MAX)
			neighbours.push_back(north);

		if (ValidIndex(south, width, height) && djikstraGrid->djikstraGrid[GetIndex(south, width)] != INT_MAX)
			neighbours.push_back(south);

		if (ValidIndex(east, width, height) && djikstraGrid->djikstraGrid[GetIndex(east, width)] != INT_MAX)
			neighbours.push_back(east);

		if (ValidIndex(west, width, height) && djikstraGrid->djikstraGrid[GetIndex(west, width)] != INT_MAX)
			neighbours.push_back(west);

		for (int k = 0; k < neighbours.size(); k++) {
			vec2<int> currentNeighbour = neighbours[k];

			int neighbourIndex = GetIndex(currentNeighbour, width);
			if (djikstraGrid->djikstraGrid[neighbourIndex] == INT_MIN) {
				djikstraGrid->djikstraGrid[neighbourIndex] = djikstraGrid->djikstraGrid[djikstraGrid->toVisit[i]] + 1;
				djikstraGrid->toVisit.push_back(neighbourIndex);
				size++;
			}


		}
	}


	std::cout << std::endl;
	std::cout << std::endl;
	counter = 0;
	firstTime = true;
	for (int i = 0; i < tilemap->width * tilemap->height; i++) {

		counter++;
		if (firstTime)
		{
			counter = 0;
			firstTime = false;
		}
		if (counter == tilemap->width) {
			std::cout << '\n';
			counter = 0;
		}

		if (djikstraGrid->djikstraGrid[i] == INT_MAX)
			std::cout << " ,.., ";
		else
		{
			if (djikstraGrid->djikstraGrid[i] < 10)
				std::cout << " ,0" << djikstraGrid->djikstraGrid[i] << ", ";
			else
				std::cout << " ," << djikstraGrid->djikstraGrid[i] << ", ";
		}

	}

	std::cout << std::endl;
	std::cout << std::endl;*/

}

//entire region
static void
GenerateFlowField(DjikstraGrid* djikstraGrid)
{
	for (unsigned int i = 0; i < djikstraGrid->width * djikstraGrid->height; i++)
	{
		djikstraGrid->flowField[i] = V2(0.0f, 0.0f);
	}

	int width = djikstraGrid->width;
	int height = djikstraGrid->height;

	for (unsigned int i = 0; i < djikstraGrid->width * djikstraGrid->height; i++)
	{
		if (djikstraGrid->djikstraGrid[i] == INT_MAX)
			continue;


		vec2<int> currentNode = GetIndex(i, width);
		int currentIndex = GetIndex(currentNode, width);

		vec2<int> north     = V2(currentNode.x, currentNode.y - 1);
		vec2<int> northEast = V2(currentNode.x + 1, currentNode.y - 1);
		vec2<int> east      = V2(currentNode.x + 1, currentNode.y);
		vec2<int> southEast = V2(currentNode.x + 1, currentNode.y + 1);
		vec2<int> south     = V2(currentNode.x, currentNode.y + 1); //when printed the positive y is downwards, like [0,1], [0,2]
		vec2<int> southWest = V2(currentNode.x - 1, currentNode.y + 1);
		vec2<int> west      = V2(currentNode.x - 1, currentNode.y);
		vec2<int> northWest = V2(currentNode.x - 1, currentNode.y - 1);
		//check if these are valid before adding
		std::vector<vec2<int>> neighbours;

		if (ValidIndex(north, width, height) && djikstraGrid->djikstraGrid[GetIndex(north, width)] != INT_MAX)
			neighbours.push_back(north);

		if (ValidIndex(south, width, height) && djikstraGrid->djikstraGrid[GetIndex(south, width)] != INT_MAX)
			neighbours.push_back(south);

		if (ValidIndex(east, width, height) && djikstraGrid->djikstraGrid[GetIndex(east, width)] != INT_MAX)
			neighbours.push_back(east);

		if (ValidIndex(west, width, height) && djikstraGrid->djikstraGrid[GetIndex(west, width)] != INT_MAX)
			neighbours.push_back(west);

		if (ValidIndex(southWest, width, height) && djikstraGrid->djikstraGrid[GetIndex(southWest, width)] != INT_MAX)
			neighbours.push_back(southWest);

		if (ValidIndex(southEast, width, height) && djikstraGrid->djikstraGrid[GetIndex(southEast, width)] != INT_MAX)
			neighbours.push_back(southEast);

		if (ValidIndex(northEast, width, height) && djikstraGrid->djikstraGrid[GetIndex(northEast, width)] != INT_MAX)
			neighbours.push_back(northEast);

		if (ValidIndex(northWest, width, height) && djikstraGrid->djikstraGrid[GetIndex(northWest, width)] != INT_MAX)
			neighbours.push_back(northWest);

		vec2<int> min = V2(-1, -1);
		int minDist = 0;

		for (unsigned int k = 0; k < neighbours.size(); k++) 
		{
			vec2<int> currentNeighbour = neighbours[k];

			int distance = djikstraGrid->djikstraGrid[GetIndex(currentNeighbour, width)] - djikstraGrid->djikstraGrid[currentIndex];


			if (distance < minDist)
			{
				min = currentNeighbour;
				minDist = distance;
			}
		}

		if (min.x != -1) 
		{
			djikstraGrid->flowField[currentIndex] = Normalize(V2((float)min.x, -(float)min.y) - V2((float)currentNode.x, -(float)currentNode.y));
		}
	}
}
