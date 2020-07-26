
struct Weapon
{
	bool closeRange = false;
	float range = 200.0f;
	float cooldown = 500.0f; //ms 
	bool isCooldown = false;
	float damage = 100.0f;
	int textureID;
};


struct Camera
{
	vec2<float> position;
};

struct Player
{
	vec2<float> position;
	Camera camera;
	Light* light;
	Animation animation;
	Weapon weapon;
};

static void
InitializePlayer(Player* player, const vec2<float>& position)
{
	player->position = position;
	player->camera.position = -position;
}

static void
UpdateCamera(Player* player)
{
	player->camera.position = -player->position;
}

struct Projectile
{
	vec2<float> AABB = V2(4.0f, 4.0f);
	vec2<float> direction;
	vec2<float> position;
	int textureID;
	float damage = 100.0f;
	bool shotByPlayer = false;
};

struct Projectiles
{
	static const unsigned int MAX_PROJECTILES = 100;
	Projectile projectiles[MAX_PROJECTILES];
	unsigned int currentActiveProjectiles = 0;
};

Projectile PROJECTILE(const vec2<float>& direction, const vec2<float>& position, int textureID, float damage, bool shotByPlayer = false)
{
	Projectile projectile;
	projectile.direction = direction;
	projectile.position = position;
	projectile.textureID = textureID;
	projectile.damage = damage;
	projectile.shotByPlayer = shotByPlayer;
	return projectile;
}


struct Ai
{
	enum State 
	{
		IDLE_ROAM,
		IDLE_PATROL,
		IDLE_STATIC,
		INSPECT,
		INSPECT_STAND_STILL,
		RETURN_TO_IDLE,
		ATTACK,
		DEAD
	};

	//For A Star
	PathStack pathstack;
	//For patrolling
	PathStack patrolStack;

	vec2<float> position;
	vec2<float> halfSize = V2(16.0f, 16.0f);

	//Default paths
	//vec2<int> patrolFrom;
	//vec2<int> patrolTo;
	vec2<int> currentGoal;
	vec2<float> direction;

	vec2<int> inspectionGoal;

	/*unsigned int frameCounter = 0;
	unsigned int aiRoamWalkTimer = 0;
	unsigned int aiRoamStandStillTimer = 0;*/
	float frameCounter = 0;
	float aiRoamWalkTimer = 0;
	float aiRoamStandStillTimer = 0;
	float aiStandStillAfterInspectionTimer = 0;
	unsigned int nextPatrolIndex = 0;
	unsigned int maxPatrolPaths;
	unsigned int lastKnownPatrolStackIndex = 0;

	State state;
	State originalIdleState; //The one we used to go back to what we were doing after inspecting a position

	Weapon weapon;

	Animation animation;
};

struct AIs
{
	static const unsigned int MAX_AIS = 10;
	unsigned int currentNumberOfAis = 0;
	Ai ais[MAX_AIS];
};

struct Level
{
	Player player;
	//Ai ais[10];
	AIs ais;
	Lights lights;
	Door doors[2];
	Tilemap tilemap;
	Projectiles projectiles;

	vec2<float> moveDelta = V2(0.0f, 0.0f);

	vec2<float> cursorPos = V2(0.0f, 0.0f);
	vec2<float> shadowDirectionToCursor = V2(0.0f, 0.0f);
	vec2<float> realDirectionToCursor = V2(0.0f, 0.0f);

	//This is the LOS path.. //also just used for testing, will remove later
	PathStack losPathStack;
	bool pathClear = false;
};

static void
SpawnProjectile(Level* level, vec2<float> position, vec2<float> direction, int textureID, float damage, bool shotByPlayer = false)
{
	if (level->projectiles.currentActiveProjectiles == level->projectiles.MAX_PROJECTILES - 1)
		return;
	level->projectiles.projectiles[level->projectiles.currentActiveProjectiles] = PROJECTILE(direction, position, textureID, damage, shotByPlayer);
	level->projectiles.currentActiveProjectiles++;
}

static void
InitializeAi(Ai* ai, vec2<int> patrolFrom, vec2<int> patrolTo)
{
	/*ai->pathstack = {};
	ai->patrolFrom = patrolFrom;
	ai->patrolTo = patrolTo;
	ai->currentGoal = patrolFrom; //For some reason we start on our own?
	ai->position = V2(patrolFrom.x * 32.0f, patrolFrom.y * -32.0f);
	ai->direction = Normalize(V2(ai->patrolTo.x * 32.0f, ai->patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::PATROL;

	InitializeStack(&ai->pathstack, 10);*/

	InitializeStack(&ai->pathstack, 10);
	InitializeStack(&ai->patrolStack, 10);
	ai->pathstack = {};

	PushStack(&ai->patrolStack, patrolFrom);
	PushStack(&ai->patrolStack, patrolTo);
	ai->currentGoal = patrolFrom; //For some reason we start on our own?

	ai->position = V2(patrolFrom.x * 32.0f, patrolFrom.y * -32.0f);
	ai->direction = Normalize(V2(patrolTo.x * 32.0f, patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::IDLE_PATROL;

	ai->maxPatrolPaths = ai->patrolStack.capacity;

}

static void
InitializeAi(Ai* ai, int count, ...)
{
	ai->pathstack = {};
	ai->patrolStack = {};
	InitializeStack(&ai->pathstack, 10);
	InitializeStack(&ai->patrolStack, 10);


	va_list paths;
	va_start(paths, count);

	for (int i = 0; i < count; ++i)
	{
		PushStack(&ai->patrolStack, va_arg(paths, vec2<int>));
	}

	va_end(paths);


	ai->currentGoal = ai->patrolStack.arr[0]; //For some reason we start on our own?
	vec2<int> patrolTo = ai->patrolStack.arr[1];

	ai->position = V2(ai->currentGoal.x * 32.0f, ai->currentGoal.y * -32.0f);
	ai->direction = Normalize(V2(patrolTo.x * 32.0f, patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::IDLE_PATROL;

	ai->maxPatrolPaths = ai->patrolStack.capacity;
}

static Ai*
InitializeAi(AIs* ais, Ai::State state, int count, ...)
{
	if (ais->currentNumberOfAis == AIs::MAX_AIS)
		return nullptr;

	ais->ais[ais->currentNumberOfAis].pathstack = {};
	ais->ais[ais->currentNumberOfAis].patrolStack = {};
	InitializeStack(&ais->ais[ais->currentNumberOfAis].pathstack, 10);
	InitializeStack(&ais->ais[ais->currentNumberOfAis].patrolStack, 10);


	va_list paths;
	va_start(paths, count);

	for (int i = 0; i < count; ++i)
	{
		PushStack(&ais->ais[ais->currentNumberOfAis].patrolStack, va_arg(paths, vec2<int>));
	}

	va_end(paths);


	ais->ais[ais->currentNumberOfAis].lastKnownPatrolStackIndex = 0;
	ais->ais[ais->currentNumberOfAis].currentGoal = ais->ais[ais->currentNumberOfAis].patrolStack.arr[0]; //For some reason we start on our own?
	vec2<int> patrolTo = ais->ais[ais->currentNumberOfAis].patrolStack.arr[1];

	ais->ais[ais->currentNumberOfAis].position = V2(ais->ais[ais->currentNumberOfAis].currentGoal.x * 32.0f, ais->ais[ais->currentNumberOfAis].currentGoal.y * -32.0f);
	ais->ais[ais->currentNumberOfAis].direction = Normalize(V2(patrolTo.x * 32.0f, patrolTo.y * -32.0f) - ais->ais[ais->currentNumberOfAis].position);
	ais->ais[ais->currentNumberOfAis].state = state;
	ais->ais[ais->currentNumberOfAis].originalIdleState = state;

	ais->ais[ais->currentNumberOfAis].maxPatrolPaths = ais->ais[ais->currentNumberOfAis].patrolStack.capacity;

	ais->currentNumberOfAis++;
	return &ais->ais[ais->currentNumberOfAis - 1];
}

static void
SetStateRoaming(Ai* ai)
{
	float LO = -1.0f;
	float HI = 1.0f;
	float rX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	float rY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	ai->direction = Normalize(V2(rX, rY));
	ai->frameCounter = 0;
	ai->state = Ai::State::IDLE_ROAM;
}

static vec2<float>
GetRandomDirection()
{
	float LO = -1.0f;
	float HI = 1.0f;
	float rX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	float rY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

	return Normalize(V2(rX, rY));
}

static unsigned int
GetRandomTimeMillis(unsigned int low, unsigned int high)
{
	return (low + (rand() % ((high - low) + 1))) * 1000;
}


static void
InitializeTestLevel(Level* level)
{
	srand(static_cast <unsigned> (time(0)));

	level->tilemap = GenerateTestTilemap();

	InitializePlayer(&level->player, V2(64.0f, -64.0f));

	level->player.light = CreateLight(&level->lights, V2(0.0f, 0.0f), V3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
													                     static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
		                                                                 static_cast <float> (rand()) / static_cast <float> (RAND_MAX)),
														                 Light::LIGHT_TYPE::POINT);

	tileNeighbourMap = new TileNeighbourMap[level->tilemap.width * level->tilemap.height];
	//populate the world with edges
	for (int x = 1; x < (level->tilemap.width - 1); x++) //width 
	{
		SetBit(&tileNeighbourMap[1 * level->tilemap.width + x].edge_exist_cell_exist, CELL_EXIST);
		SetBit(&tileNeighbourMap[(level->tilemap.height - 2) * level->tilemap.width + x].edge_exist_cell_exist, CELL_EXIST);
	}
	for (int x = 1; x < (level->tilemap.height - 1); x++) //height
	{
		SetBit(&tileNeighbourMap[x * level->tilemap.width + 1].edge_exist_cell_exist, CELL_EXIST);
		SetBit(&tileNeighbourMap[x * level->tilemap.width + (level->tilemap.width - 2)].edge_exist_cell_exist, CELL_EXIST);
	}

	InitializeDoor(&level->doors[0], V2(320.0f, -320.0f));
	InitializeDoor(&level->doors[1], V2(180.0f, -180.0f));

	//call this before the above two methods so theyre taken into account for all lights
//wont this index always be the same....?
	vec2<float> fakeCamera = level->player.camera.position;
	fakeCamera.y = -fakeCamera.y;
	for (unsigned int i = 0; i < 2; i++)
	{
		UpdateEdge(&level->doors[i], level->player.camera.position);
		AddDoorEdges(&level->doors[i]);
	}

	//ConvertTileMapToPolyMap(0, 0, 20, 20, blockWidth, 20);
	ConvertTileMapToPolyMap(0, 0, level->tilemap.width, level->tilemap.height, blockWidth, level->tilemap.width/*, player*/);
	//CalculateVisibilityPolgyon(0.0f, 0.0f, 1000.0f);
	CalculateVisibilityPolgyons(1000.0f, &level->lights);

	//ai
	InitializeAi(&level->ais, Ai::State::IDLE_PATROL, 4, V2(4, 4), V2(4, 8), V2(10, 8), V2(10, 4));
	InitializeAi(&level->ais, Ai::State::IDLE_ROAM, 1, V2(4, 8));
	InitializeAi(&level->ais, Ai::State::IDLE_STATIC, 1, V2(8, 12));

	InitializeStack(&level->losPathStack, 1);
	level->pathClear = IsPathToTargetClear(&level->tilemap, V2(1, 4), V2(18, 11), &level->losPathStack);

	SetTextureCoordinates(480, 87, 80, 87, 0, &level->player.animation);
	InitializeAnimation(&level->player.animation, 480, 87, 80, 87, 0, 6, 50, textures[PLAYER_RUN].ID);

	//SetTextureCoordinates(32, 41, 32, 41, 0, &level->player.animation);
	//InitializeAnimation(&level->player.animation, 32, 41, 32, 41, 0, 1, 1000, textures[TEST1].ID);
}

static bool
AABBIntersects(const vec2<float>& position1, const vec2<float>& halfSize1, const vec2<float>& position2, const vec2<float>& halfSize2)
{

	vec2<float> aMin = V2(position1.x - halfSize1.x, position1.y - halfSize1.y);
	vec2<float> aMax = V2(position1.x + halfSize1.x, position1.y + halfSize1.y);

	vec2<float> bMin = V2(position2.x - halfSize2.x, position2.y - halfSize2.y);
	vec2<float> bMax = V2(position2.x + halfSize2.x, position2.y + halfSize2.y);
	// Exit with no intersection if found separated along an axis
	if (aMax.x < bMin.x || aMin.x > bMax.x) return false;
	if (aMax.y < bMin.y || aMin.y > bMax.y) return false;

	// No separating axis found, therefor there is at least one overlapping axis
	return true;

}

static void
UpdateProjectiles(Level* level, float dt)
{
	for (unsigned int i = 0; i < level->projectiles.currentActiveProjectiles; i++)
	{
		Projectile* currentProjectile = &level->projectiles.projectiles[i];
		currentProjectile->position += currentProjectile->direction * 95.0f * dt;

		//I guess we can check if we hit any of the wall objects or doors rather than tiles?

		bool hitSomething = false;
		if (currentProjectile->shotByPlayer)
		{
			for (unsigned int k = 0; k < level->ais.currentNumberOfAis; k++)
			{
				Ai* currentAi = &level->ais.ais[k];
				if (currentAi->state == Ai::State::DEAD)
					continue;
				if (AABBIntersects(currentProjectile->position, currentProjectile->AABB, currentAi->position, currentAi->halfSize))
				{
					currentAi->state = Ai::State::DEAD;
					hitSomething = true;
					break;
				}
			}
		}
		else 
		{

		}

		if (hitSomething)
		{
			level->projectiles.projectiles[i] = level->projectiles.projectiles[level->projectiles.currentActiveProjectiles - 1];
			level->projectiles.currentActiveProjectiles--;
			if (level->projectiles.currentActiveProjectiles < 0)
				level->projectiles.currentActiveProjectiles = 0;
			i--;
			if (i < 0)
				break;
		}
	}
}

static void
UpdateAi(Level* level, float dt)
{
	//this is terrible since it relies on the 1/fps in the main loop, but its to get the time in milli seconds
	float dtMultiplier = 100.0f;// = 1.0f / 100.0f;
	for (unsigned int i = 0; i < level->ais.currentNumberOfAis; i++)
	{
		Ai* currentAi = &level->ais.ais[i];

		if (currentAi->state == Ai::State::IDLE_PATROL)
		{
			vec2<float> goalPosition = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f);
			float lengthToGoal = Len(goalPosition - currentAi->position);
			currentAi->position += currentAi->direction * 60.0f * dt;
			if (lengthToGoal < 1.0f)
			{
				currentAi->position = goalPosition;
				currentAi->nextPatrolIndex++;
				currentAi->nextPatrolIndex = currentAi->nextPatrolIndex % currentAi->patrolStack.capacity;

				vec2<int> newGoal = currentAi->patrolStack.arr[currentAi->nextPatrolIndex];

				currentAi->currentGoal = currentAi->patrolStack.arr[currentAi->nextPatrolIndex];
				currentAi->direction = Normalize(V2(newGoal.x * 32.0f, newGoal.y * -32.0f) - currentAi->position);
			}

		}
		else if (currentAi->state == Ai::State::IDLE_ROAM)
		{
			if (currentAi->aiRoamStandStillTimer > 0)
			{
				//currentAi->aiRoamStandStillTimer--;
				currentAi->aiRoamStandStillTimer -= 10;// *dtMultiplier;
			}
			else
			{
				if (currentAi->frameCounter >= currentAi->aiRoamWalkTimer)
				{
					currentAi->direction = GetRandomDirection();
					currentAi->aiRoamWalkTimer = (float)GetRandomTimeMillis(3, 6);
					currentAi->aiRoamStandStillTimer = (float)GetRandomTimeMillis(0, 7);
					//std::cout << "roam time: " << currentAi->aiRoamWalkTimer << std::endl;
					//std::cout << "stand still time: " << currentAi->aiRoamStandStillTimer << std::endl;
					currentAi->frameCounter = 0;
				}

				currentAi->position += currentAi->direction * 35.0f * dt;
				vec2<float> reflectionNormal = V2(0.0f, 0.0f);
				vec2<float> moveDeltaAgent = currentAi->direction * 0.05f;
				if (CheckTileCollision(&currentAi->position, moveDeltaAgent, &level->tilemap, V2(16.0f, 16.0f), &reflectionNormal))
				{
					currentAi->direction = Normalize(Reflect(currentAi->direction, reflectionNormal));
				}

				//currentAi->frameCounter++;
				currentAi->frameCounter += 10;// *dtMultiplier;
			}

		}
		else if (currentAi->state == Ai::State::IDLE_STATIC)
		{
			//I guess we just do nothing, is if we are on guard duty?

		}
		else if (currentAi->state == Ai::State::INSPECT)
		{
			vec2<float> goalPosition = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f);
			float lengthToGoal = Len(goalPosition - currentAi->position);
			currentAi->position += currentAi->direction * 60.0f * dt;
			if (lengthToGoal < 1.0f)
			{
				if (currentAi->pathstack.capacity > 0)
				{
					currentAi->currentGoal = PopStack(&currentAi->pathstack);
					vec2<float> goalFloat = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f); //y is reversed in directx
					currentAi->direction = Normalize(goalFloat - currentAi->position);
				}
				else 
				{
					currentAi->aiStandStillAfterInspectionTimer = (float)GetRandomTimeMillis(1, 3);
					currentAi->state = Ai::State::INSPECT_STAND_STILL;
				}
			}

			float length = Len(level->player.position - currentAi->position);
			float radius = 128.0f;
			//If we are closer than a certain length, do the heavier calculations and check if
			//1. the player is in front of us
			//2. check if we have a path to them, if so start chasing
			if (length < radius)
			{
				vec2<float> heading = Normalize(level->player.position - currentAi->position);
				float dotResult = Dot(currentAi->direction, heading);
				if (dotResult > 0)
				{
					vec2<int> playerTile = WorldToTileCoordinates(&level->tilemap, level->player.position);
					vec2<int> aiTile = WorldToTileCoordinates(&level->tilemap, currentAi->position);
					bool pathClear = IsPathToTargetClear(&level->tilemap, aiTile, playerTile);
					//There is a case where this path will be the same as a star but if its not then eh..
					if (pathClear)
					{
						currentAi->state = Ai::State::ATTACK;
					}
				}
			}

		}
		else if (currentAi->state == Ai::State::INSPECT_STAND_STILL)
		{
			//goal reached
			if (currentAi->aiStandStillAfterInspectionTimer > 0)
			{
				currentAi->aiStandStillAfterInspectionTimer -= 10;
			}
			else
			{
				//go back to previous idle state
				if (currentAi->originalIdleState == Ai::State::IDLE_ROAM || currentAi->originalIdleState == Ai::State::IDLE_STATIC)
				{
					currentAi->state = Ai::State::IDLE_ROAM;
				}
				else 
				{
					currentAi->state = Ai::State::RETURN_TO_IDLE;
					vec2<int> aiTile = WorldToTileCoordinates(&level->tilemap, currentAi->position);
					AStarSearch(level->tilemap.tilemap, &aiTile, &currentAi->patrolStack.arr[currentAi->nextPatrolIndex], (int)level->tilemap.width, (int)level->tilemap.height, &currentAi->pathstack);
					currentAi->currentGoal = PopStack(&currentAi->pathstack); // i think we cna double pop it here
					//currentAi->currentGoal = PopStack(&currentAi->pathstack);

					vec2<float> goalFloat = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f); //y is reversed in directx
					currentAi->direction = Normalize(goalFloat - currentAi->position);
				}
			}
		}
		//Inspect and return to idle are awfully similar
		else if (currentAi->state == Ai::State::RETURN_TO_IDLE)
		{
			vec2<float> goalPosition = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f);
			float lengthToGoal = Len(goalPosition - currentAi->position);
			currentAi->position += currentAi->direction * 60.0f * dt;
			if (lengthToGoal < 1.0f)
			{
				if (currentAi->pathstack.capacity > 0)
				{
					currentAi->currentGoal = PopStack(&currentAi->pathstack);
					vec2<float> goalFloat = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f); //y is reversed in directx
					currentAi->direction = Normalize(goalFloat - currentAi->position);
				}
				else
				{
					//currentAi->aiStandStillAfterInspectionTimer = (float)GetRandomTimeMillis(1, 3);
					currentAi->state = Ai::State::IDLE_PATROL;
				}
			}
		}
		else if (currentAi->state == Ai::State::ATTACK)
		{
			currentAi->direction = Normalize(level->player.position - currentAi->position);
			float lengthToGoal = Len(level->player.position - currentAi->position);
			currentAi->position += currentAi->direction * 60.0f * dt;
			if (lengthToGoal < currentAi->weapon.range)
			{
				//swing blade or whatever
				if (currentAi->weapon.closeRange)
				{

				}
				else 
				{

				}
			}
			else if (lengthToGoal > 350.0f) 
			{
				currentAi->state = Ai::State::IDLE_ROAM;
			}
		}
		else if (currentAi->state == Ai::State::DEAD)
		{

		}
	}
	
}

static void
HandleInput(Level* level)
{
	level->moveDelta = V2(0.0f, 0.0f);
	/*if (keyState.currentKeyState['D']) //these all used to be 0.5f before we multiplied with dt
		level->moveDelta.x = 200.0f;
	if (keyState.currentKeyState['A'])
		level->moveDelta.x = -200.0f;
	if (keyState.currentKeyState['W'])
		level->moveDelta.y = 200.0f;
	if (keyState.currentKeyState['S'])
		level->moveDelta.y = -200.0f;*/

	if (IsKeyDown('D')) //these all used to be 0.5f before we multiplied with dt
		level->moveDelta.x = 200.0f;
	if (IsKeyDown('A'))
		level->moveDelta.x = -200.0f;
	if (IsKeyDown('W'))
		level->moveDelta.y = 200.0f;
	if (IsKeyDown('S'))
		level->moveDelta.y = -200.0f;

	level->cursorPos = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + level->player.position;
	level->shadowDirectionToCursor = Normalize(level->cursorPos - level->player.position);
	level->realDirectionToCursor = level->shadowDirectionToCursor;
	level->shadowDirectionToCursor.y = -level->shadowDirectionToCursor.y;

	if (MousePressed(LEFT))
	{
		Player* player = &level->player;
		if (!player->weapon.isCooldown) 
		{
			player->weapon.isCooldown = true;
			SpawnProjectile(level, player->position, level->realDirectionToCursor, textures[TEST1].ID, 100.0f, true);
		}
	}
	if (MouseReleased(RIGHT))
	{
		level->player.light = CreateLight(&level->lights, level->cursorPos - V2(-16.0f, 16.0f), V3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
																								   static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
																								   static_cast <float> (rand()) / static_cast <float> (RAND_MAX)),
																								   Light::LIGHT_TYPE::POINT);
	}
	if (ScroolWheelUp())
	{
		level->player.light->lightZPos += 1.0f;
	}
	if (ScroolWheelDown())
	{
		level->player.light->lightZPos -= 1.0f;
	}
	//if (GetKeyState(0x54) & 0x8000)
	if (WasKeyReleased('Y'))
	{
		std::cout << "Y UP" << std::endl;
		//vec2<float> cursorPos = V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f)); //this is the actual cursor pos;
		SetTileBit(&level->tilemap, level->cursorPos);
		SetCell(&level->tilemap, tileNeighbourMap, level->cursorPos);

		ConvertTileMapToPolyMap(0, 0, level->tilemap.width, level->tilemap.height, blockWidth, level->tilemap.width/*, player*/);

		CalculateVisibilityPolgyons(1000.0f, &level->lights);
	}
	if (WasKeyReleased(0x54))
	{
		std::cout << "T UP" << std::endl;
		int shit = 5;
		for (unsigned int i = 0; i < level->ais.currentNumberOfAis; i++)
		{
			Ai* currentAi = &level->ais.ais[i];
			currentAi->inspectionGoal = WorldToTileCoordinates(&level->tilemap, level->cursorPos);
			currentAi->state = Ai::State::INSPECT;
			vec2<int> aiTile = WorldToTileCoordinates(&level->tilemap, currentAi->position);
			//vec2<int> playerTile = WorldToTileCoordinates(&level->tilemap, level->player.position);
			AStarSearch(level->tilemap.tilemap, &aiTile, &currentAi->inspectionGoal, (int)level->tilemap.width, (int)level->tilemap.height, &currentAi->pathstack);
			currentAi->currentGoal = PopStack(&currentAi->pathstack); // i think we cna double pop it here
			currentAi->currentGoal = PopStack(&currentAi->pathstack);

			vec2<float> goalFloat = V2(currentAi->currentGoal.x * 32.0f, currentAi->currentGoal.y * -32.0f); //y is reversed in directx
			currentAi->direction = Normalize(goalFloat - currentAi->position);
		}
	}

}

static void 
UpdatePlayerDeltas(Level* level, float dt)
{
	Player* player = &level->player;
	if (player->weapon.isCooldown)
	{
		player->weapon.cooldown -= 10.0f;
		if (player->weapon.cooldown < 0.0f)
		{
			player->weapon.cooldown = 500.0f;
			player->weapon.isCooldown = false;
		}
	}
}

static void
IntegratePhysics(Level* level, float dt)
{

	UpdateAnimation(&level->player.animation);

	UpdatePlayerDeltas(level, dt);

	UpdateProjectiles(level, dt);

	UpdateAi(level, dt);

	CheckTileCollision(&level->player.position, level->moveDelta * dt, &level->tilemap, V2(14.0f, 14.0f));


	for (unsigned int i = 0; i < 2; i++)
	{
		CollisionData collided = CheckIfDoorAgentCollided(&level->doors[i], level->player.position);

		level->doors[i].collisionPoints.clear();
		level->doors[i].collidedThisFrame = false;
		if (collided.collided)
		{
			level->doors[i].collisionPoints = GetPointsInsideDoor(&level->doors[i], level->player.position);
			level->player.position += V2(collided.mtv.x, collided.mtv.y);
			level->doors[i].collidedThisFrame = true;
		}

		if (collided.collided)
		{
			if (level->doors[i].collisionPoints.size() > 0)
			{
				//Apply the force in the opposite way of the minimum translation vector
				vec2<float> force = -collided.mtv * 500.0f;
				vec2<float> centerOfMass = level->doors[i].hingePosition;
				vec2<float> momentArm = level->doors[i].collisionPoints[0] - centerOfMass;

				float inertia = 5.0f;
				float torque = Cross(momentArm, force);
				//	std::cout << "torque: " << torque << std::endl;

				//	if ((!door.angularVelocity > 80.0f || !door.angularVelocity < 80.0f) && (!door.angularAcceleration > 80.0f || !door.angularAcceleration < 80.0f))
				//	{
				level->doors[i].angularAcceleration += (torque / inertia) * dt;
				level->doors[i].angularVelocity += level->doors[i].angularAcceleration * dt;
				//door.angularVelocity = torque < 0 ? -7 : 7;
			//	}

				float cappedValue = 7.0f;

				if (level->doors[i].angularVelocity > cappedValue)
				{
					level->doors[i].angularVelocity = cappedValue;
				}
				if (level->doors[i].angularVelocity < -cappedValue)
				{
					level->doors[i].angularVelocity = -cappedValue;
				}

				if (level->doors[i].angularAcceleration > cappedValue)
				{
					level->doors[i].angularAcceleration = cappedValue;
				}
				if (level->doors[i].angularAcceleration < -cappedValue)
				{
					level->doors[i].angularAcceleration = -cappedValue;
				}
				std::cout << "angular vel: " << level->doors[i].angularVelocity << std::endl;
				std::cout << "angular acc: " << level->doors[i].angularAcceleration << std::endl;
				level->doors[i].active = true;
			}
		}

		if (level->doors[i].active)
		{
			//https://gamedev.stackexchange.com/questions/114983/how-to-apply-friction-vector-to-acceleration-in-top-down-2d-game
			//I think this is where something more sophisticated is needed
			//float fakeFriction = -door.angularAcceleration * 0.08f;
			if (!collided.collided)
			{
				//float fakeFriction = -door.angularAcceleration * 0.05f;
				//float frictionCoefficient = 0.9f;
				//float fakeFriction = door.angularAcceleration < 0 ? frictionCoefficient : -frictionCoefficient;
				//door.angularAcceleration += fakeFriction;
				//door.angularVelocity += door.angularAcceleration * dt;
				level->doors[i].angularVelocity += -level->doors[i].angularVelocity * 0.1f;

				//If we're close to 0 we stop
				if (fabsf(level->doors[i].angularVelocity) < 0.0000001f)
				{
					level->doors[i].angularAcceleration = 0.0f;
					level->doors[i].angularVelocity = 0.0f;
					level->doors[i].active = false;
				}
			}
			UpdateEdge(&level->doors[i], level->player.camera.position);
			UpdateDoorEdges(&level->doors[i], i);
		}

		level->doors[i].rotationRadians += level->doors[i].angularVelocity * dt;

		RotateDoorRadians(&level->doors[i], level->doors[i].rotationRadians);
	}
	


	UpdateCamera(&level->player);
}

static void
DestroyLevel(Level* level)
{
	//free up every pointer type in Level
	//free up tileNeghibourMap too
}

