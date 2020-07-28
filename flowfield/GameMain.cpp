

static void
GameResize(float newWidth, float newHeight)
{
	Game::windowSize = V2(newWidth, newHeight);
	//DirectX::XMMATRIX orthoMatrix = DirectX::XMMatrixOrthographicLH((float)newWidth, (float)newHeight, /*screenNear*/ 1.0f, /*screenDepth*/ 10.0f);
	ResizeGL(&renderers[COLOR_RENDERER], Game::ortho, newWidth, newHeight);
	//SetUniformProjectionMatrix(&shader, orthoMatrix);
}


#define SHADOWS true
#define DJIKSTRA false
#if SHADOWS
static int
GameMain(Window window)
{
	glViewport(0, 0, window.width, window.height);
	Game::windowSize = V2((float)window.width, (float)window.height);

	//TODO: DONT SWAP CALL HERE
	wglSwapIntervalEXT(0);

	//Game specific "system" data
	InitializeRendererData(Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[COLOR_RENDERER], &vertexBuffers[COLOR_VERTEX_BUFFER], 60000, Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[SHADOW_RENDERER], &vertexBuffers[SHADOW_VERTEX_BUFFER], 1000, Game::ortho, (float)window.width, (float)window.height, true);

	OnInitialize(&tiledMapManager);


	//Level and gameplay stuff
	Level activeLevel;
	InitializeTestLevel(&activeLevel);


	//fps counter
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	__int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;

	LARGE_INTEGER LastCounter;
	QueryPerformanceCounter(&LastCounter);
	 
	const float fps = 100.0f;
	const float dt = 1.0f / fps;
	float accumulator = 0.0f;

	float frameStart = GetCounter() / 1000.0f;

	Game::running = true;
	while (Game::running) {

		//reset keys
		ResetKeys();
		//Windows messages
		PeekMessages();

		const float currentTime = GetCounter() / 1000.0f;
		accumulator += currentTime - frameStart;

		frameStart = currentTime;

		if (accumulator > 0.2f)
			accumulator = 0.2f;

		HandleInput(&activeLevel);

		while (accumulator > dt)
		{
			IntegratePhysics(&activeLevel, dt);
			accumulator -= dt;
		}
	

		RenderLevel(&activeLevel, window);

		


		ClearInput(); //TODO: remove this

		SwapBuffers(window.gldc);


		LARGE_INTEGER EndCounter;
		QueryPerformanceCounter(&EndCounter);

		__int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;

		__int32 MSPerFrame = (__int32)(((1000 * CounterElapsed) / PerfCountFrequency));
		__int32 FPS = PerfCountFrequency / CounterElapsed;

		//std::cout << "FPS: "<< FPS << "ms/frame: " << MSPerFrame << std::endl;


		LastCounter = EndCounter;
	}


	return 0;
}
#elif DJIKSTRA

static int
GameMain(Window window)
{
	glViewport(0, 0, window.width, window.height);
	Game::windowSize = V2((float)window.width, (float)window.height);

	//TODO: DONT SWAP CALL HERE
	wglSwapIntervalEXT(0);

	InitializeRendererData(Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[COLOR_RENDERER], &vertexBuffers[COLOR_VERTEX_BUFFER], 60000, Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[SHADOW_RENDERER], &vertexBuffers[SHADOW_VERTEX_BUFFER], 1000, Game::ortho, (float)window.width, (float)window.height);

	
	
	
	Tilemap tilemap = GenerateTestTilemap();


	
	InitializeAgent(V2(0.0f, 0.0f), V2(0.0f, 0.0f), 8.0f);
	InitializeAgent(V2(0.0f, -640.0f), V2(0.0f, 0.0f), 8.0f);
	InitializeAgent(V2(0.0f, -800.0f), V2(0.0f, 0.0f), 8.0f);
	InitializeAgent(V2(640.0f, 0.0f), V2(0.0f, 0.0f), 8.0f);
	InitializeAgent(V2(672.0f, -320.0f), V2(0.0f, 0.0f), 8.0f);
	InitializeAgent(V2(320.0f, -520.0f), V2(0.0f, 0.0f), 8.0f);


	DjikstraGrid djikstraGrid = InitializeDjikstraGrid(tilemap.width, tilemap.height);
	GenerateDjikstra(&djikstraGrid, &tilemap, V2(18, 1));
	GenerateFlowField(&djikstraGrid);
	//GenerateDjikstraRegion(&djikstraGrid, &tilemap, V2(18, 1), 3);
	





	vec2<float> player = V2(64.0f, -64.0f);

	vec2<int> tilePositionMouse;

	Game::running = true;
	while (Game::running) {
		//TODO: extract this to a function
		MSG msg;
		while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				Game::running = false;
			}
			else {

				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
		}


		vec2<float> moveDelta = V2(0.0f, 0.0f);
		if (keyState.currentKeyState['D'])
			moveDelta.x = 0.5f;
		if (keyState.currentKeyState['A'])
			moveDelta.x = -0.5f;
		if (keyState.currentKeyState['W'])
			moveDelta.y = 0.5f;
		if (keyState.currentKeyState['S'])
			moveDelta.y = -0.5f;

		CheckTileCollision(&player, moveDelta, &tilemap, V2(14.0f, 14.0f));

		vec2<float> camera = -player;

		vec2<float> cursorPos = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + player;
		//	vec2<float> cursorPos = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + player;
		
		if (MouseReleased(LEFT))
		{
			std::cout << "mouse left released" << std::endl;
			//vec2<float> cursorPos = V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f)); //this is the actual cursor pos;
			//SetTileBit(&tilemap, cursorPos);
			

			//GenerateDjikstraRegion(&djikstraGrid, &tilemap, tilePosition, 5.0f, 5.0f, 5.0f, 5.0f);
			//GenerateDjikstraRegion(&djikstraGrid, &tilemap, tilePosition, 3);
			tilePositionMouse = WorldToTileCoordinates(&tilemap, cursorPos);
			GenerateDjikstra(&djikstraGrid, &tilemap, tilePositionMouse);
			GenerateFlowField(&djikstraGrid);
		}
		if (MouseReleased(RIGHT))
		{
			std::cout << "mouse right released" << std::endl;
			SetTileBit(&tilemap, cursorPos);
		}
		if (ScroolWheelUp())
		{

		}
		if (ScroolWheelDown())
		{

		}


		// Do OpenGL rendering here
		vec2<int> tilePosition = WorldToTileCoordinates(&tilemap, player);
		//GenerateDjikstra(&djikstraGrid, &tilemap, tilePosition);
		//vec2<float> seek = SteeringBehaviourFlee(&agent, cursorPos);//SteeringBehaviourSeek(&agent, cursorPos);
		for(unsigned int i = 0 ; i < agents.currentActiveAgents; i++ )
		{
			vec2<float> seek = SteeringBehaviourSeek(&agents.agents[i], cursorPos);
			vec2<float> separation = SteeringBehaviourSeparation(&agents.agents[i]);
			vec2<float> cohesion = SteeringBehaviourCohesion(&agents.agents[i]);
			vec2<float> alignment = SteeringBehaviourAlignment(&agents.agents[i]);

			//Note scaling cohesion
			agents.agents[i].forceToApply = seek + separation + (cohesion  * 0.05f) + (alignment);
			
		}
		
		for (unsigned int i = 0; i < agents.currentActiveAgents; i++)
		{
			UpdateAgent(&agents.agents[i], agents.agents[i].forceToApply, 0.10f);
		}
		


		Bind(&shaders[FORWARD_SHADER]);
		Begin(&renderers[COLOR_RENDERER]);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[OFF_SCREEN_COLOR_BUFFER].fbo);
		glViewport(0, 0, window.width, window.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Djikstra grid
		PushTransform(camera, -5.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawDjikstraGrid(&djikstraGrid);
		PopTransform();

		//Flowfield grid
		PushTransform(camera, -4.8f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawFlowField(&djikstraGrid);
		PopTransform();

		//Player
		PushTransform(camera, -4.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(player, V2(32.0f, 32.0f), GetColor(V4(0.0f, 1.0f, 0.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		PopTransform();

		//Agent
		PushTransform(camera, -3.5f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		for (unsigned int i = 0; i < agents.currentActiveAgents; i ++ )
		{
			PushTransform(V2(0.0f, 0.0f), 0.0f, V2(agents.agents[i].cos, agents.agents[i].sin));
			DrawQuad(agents.agents[i].position, V2(16.0f, 16.0f), GetColor(V4(1.0f, 0.0f, 0.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
			PopTransform();
			vec2<float> end = agents.agents[i].position + Normalize(agents.agents[i].velocity) * 28.0f;
			PushTransform(V2(0.0f,0.0f), -0.5f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
			DrawLine(agents.agents[i].position, end, GetColor(0.0f, 0.0f, 1.0f, 1.0f), 3.0f);
			//DrawLine(agents.agents[i].position, end, agents.agents[i].cos, agents.agents[i].sin, GetColor(0.0f, 0.0f, 1.0f, 1.0f), 3.0f);
			PopTransform();
		}
		PopTransform();
		
		

		End();
		Flush();
		Unbind(&shaders[FORWARD_SHADER]);



		Bind(&shaders[QUAD_SHADER]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window.width, window.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glBindVertexArray(ndcQuads[REGULAR_PASS].vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_COLOR_BUFFER].textureIDs[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_SHADOW_BUFFER].textureIDs[0]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_SHADOW_BUFFER].stencilTextureID);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		Unbind(&shaders[QUAD_SHADER]);


		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "GameMain End:" << err << std::endl;
		}

		ClearInput(); //TODO: remove this

		SwapBuffers(window.gldc);
	}


	return 0;
}
#else
static int
GameMain(Window window)
{
	glViewport(0, 0, window.width, window.height);
	Game::windowSize = V2((float)window.width, (float)window.height);

	//TODO: DONT SWAP CALL IN GAME MAIN
	wglSwapIntervalEXT(0);

	InitializeRendererData(Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[COLOR_RENDERER], &vertexBuffers[COLOR_VERTEX_BUFFER], 60000, Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[SHADOW_RENDERER], &vertexBuffers[SHADOW_VERTEX_BUFFER], 1000, Game::ortho, (float)window.width, (float)window.height);




	Tilemap tilemap = GenerateTestTilemap();

	Ai ai;
	//InitializeAi(&ai, V2(4, 4), V2(10, 10));
	InitializeAi(&ai, 4, V2(4, 4), V2(4, 8), V2(10, 8), V2(10, 4));
	//SetStateRoaming(&ai);

	float doorRot = 0.0f;
	Door door;
	InitializeDoor(&door, V2(320.0f, -320.0f));

	//This is the LOS path..
	PathStack pathStack;
	InitializeStack(&pathStack, 1);
	bool pathClear = IsPathToTargetClear(&tilemap, V2(1, 4), V2(18, 11), &pathStack);

	srand(static_cast <unsigned> (time(0)));



	vec2<float> player = V2(64.0f, -64.0f);

	vec2<int> tilePositionMouse;

	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	__int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;

	LARGE_INTEGER LastCounter;
	QueryPerformanceCounter(&LastCounter);

	Game::running = true;
	while (Game::running) {

		//TODO: extract windows code to function
		MSG msg;
		while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				Game::running = false;
			}
			else {

				TranslateMessage(&msg);
				DispatchMessageA(&msg);
			}
		}


		vec2<float> moveDelta = V2(0.0f, 0.0f);
		if (keyState.currentKeyState['D'])
			moveDelta.x = 0.25f;
		if (keyState.currentKeyState['A'])
			moveDelta.x = -0.25f;
		if (keyState.currentKeyState['W'])
			moveDelta.y = 0.25f;
		if (keyState.currentKeyState['S'])
			moveDelta.y = -0.25f;

		CheckTileCollision(&player, moveDelta, &tilemap, V2(14.0f, 14.0f));

		vec2<float> camera = -player;

		vec2<float> cursorPos = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + player;
		//	vec2<float> cursorPos = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + player;

		if (MouseReleased(LEFT))
		{
			std::cout << "mouse left released" << std::endl;
			tilePositionMouse = WorldToTileCoordinates(&tilemap, cursorPos);
			SetTileBit(&tilemap, cursorPos);

		}
		if (MouseReleased(RIGHT))
		{
			ai.inspectionGoal = WorldToTileCoordinates(&tilemap, cursorPos);
			ai.state = Ai::State::INSPECT;
			vec2<int> aiTile = WorldToTileCoordinates(&tilemap, ai.position);
			vec2<int> playerTile = WorldToTileCoordinates(&tilemap, player);
			AStarSearch(tilemap.tilemap, &aiTile, &playerTile, (int)tilemap.width, (int)tilemap.height, &ai.pathstack);
			ai.currentGoal = PopStack(&ai.pathstack); // i think we cna double pop it here
			ai.currentGoal = PopStack(&ai.pathstack);

			vec2<float> goalFloat = V2(ai.currentGoal.x * 32.0f, ai.currentGoal.y * -32.0f); //y is reversed in directx
			ai.direction = Normalize(goalFloat - ai.position);
			std::cout << "mouse right released" << std::endl;
		}
		if (ScroolWheelUp())
		{

		}
		if (ScroolWheelDown())
		{

		}







		if (ai.state == Ai::State::PATROL)
		{
			vec2<float> goalPosition = V2(ai.currentGoal.x * 32.0f, ai.currentGoal.y * -32.0f); 
			float lengthToGoal = Len(goalPosition - ai.position);
			ai.position += ai.direction * 0.05f;
			if (lengthToGoal < 1.0f)
			{
				ai.position = goalPosition;
				ai.nextPatrolIndex++;
				ai.nextPatrolIndex = ai.nextPatrolIndex % ai.patrolStack.capacity;

				vec2<int> newGoal = ai.patrolStack.arr[ai.nextPatrolIndex];

				ai.currentGoal = ai.patrolStack.arr[ai.nextPatrolIndex];
				ai.direction = Normalize(V2(newGoal.x * 32.0f, newGoal.y * -32.0f) - ai.position);


				/*if (ai.currentGoal == ai.patrolFrom)
				{
					ai.currentGoal = ai.patrolTo;
					ai.direction = Normalize(V2(ai.patrolTo.x * 32.0f, ai.patrolTo.y * -32.0f) - ai.position);
				}
				else
				{
					ai.currentGoal = ai.patrolFrom;
					ai.direction = Normalize(V2(ai.patrolFrom.x * 32.0f, ai.patrolFrom.y * -32.0f) - ai.position);
				}*/

			}

		} 
		else if (ai.state == Ai::State::ROAM) 
		{
			//Give some new direction
			if (ai.aiRoamStandStillTimer > 0) 
			{
				ai.aiRoamStandStillTimer--;
			}
			else 
			{
				if (ai.frameCounter >= ai.aiRoamWalkTimer)
				{
					ai.direction = GetRandomDirection();
					ai.aiRoamWalkTimer = GetRandomTimeMillis(3, 7);
					ai.aiRoamStandStillTimer = GetRandomTimeMillis(0, 3);
					std::cout << "roam time: " << ai.aiRoamWalkTimer << std::endl;
					std::cout << "stand still time: " << ai.aiRoamStandStillTimer << std::endl;
					ai.frameCounter = 0;
				}

				ai.position += ai.direction * 0.025f;
				vec2<float> reflectionNormal = V2(0.0f, 0.0f);
				vec2<float> moveDeltaAgent = ai.direction * 0.05f;
				if (CheckTileCollision(&ai.position, moveDeltaAgent, &tilemap, V2(16.0f, 16.0f), &reflectionNormal))
				{
					ai.direction = Normalize(Reflect(ai.direction, reflectionNormal));
				}

				ai.frameCounter++;
			}
			
		}
		else if (ai.state == Ai::State::INSPECT) 
		{
			vec2<float> goalPosition = V2(ai.currentGoal.x * 32.0f, ai.currentGoal.y * -32.0f);
			float lengthToGoal = Len(goalPosition - ai.position);
			ai.position += ai.direction * 0.05f;
			if (lengthToGoal < 1.0f)
			{
				if (ai.pathstack.capacity > 0)
				{
					ai.currentGoal = PopStack(&ai.pathstack);
					vec2<float> goalFloat = V2(ai.currentGoal.x * 32.0f, ai.currentGoal.y * -32.0f); //y is reversed in directx
					ai.direction = Normalize(goalFloat - ai.position);
				}
			} 
			else 
			{
				ai.state = Ai::State::ROAM;
			}
		}




		// Do OpenGL rendering here
		vec2<int> tilePosition = WorldToTileCoordinates(&tilemap, player);
	

		Bind(&shaders[FORWARD_SHADER]);
		Begin(&renderers[COLOR_RENDERER]);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[OFF_SCREEN_COLOR_BUFFER].fbo);
		glViewport(0, 0, window.width, window.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		CollisionData collided = CheckIfDoorAgentCollided(&door, player);

		std::vector<vec2<float>> collisionPoints;
		if (collided.collided)
		{
			collisionPoints = GetPointsInsideDoor(&door, player);
			player += V2(collided.mtv.x, collided.mtv.y);
		}

		//Player
		PushTransform(camera, -2.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(player, V2(32.0f, 32.0f), GetColor(V4(1.0f, 0.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		PopTransform();

		//Tilemap
		PushTransform(camera, -3.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawTilemap(&tilemap, V2(32.0f, 32.0f), -1);
		PopTransform();


		//Pathstack
		vec2<int> playerTilePos = WorldToTileCoordinates(&tilemap, player);
		pathClear = IsPathToTargetClear(&tilemap, V2(1, 4), playerTilePos, &pathStack);
		PushTransform(camera, -2.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		for (int i = 0; i < pathStack.capacity; i++)
		{
			//DrawQuad(V2(pathStack.arr[i].x * 32.0f, pathStack.arr[i].y * -32.0f), V2(30.0f, 30.0f), GetColor(V4(1.0f, 0.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		}
		PopTransform();

		//Ai
		PushTransform(camera, -1.5f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(ai.position, V2(30.0f, 30.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		PopTransform();


		//Door
		PushTransform(camera, -1.5f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		//DrawQuad(door.position, V2(30.0f, 30.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		
		doorRot += 0.0001f;
		if (doorRot > 359.0f)
			doorRot = 0.0f;

		//RotateDoor(&door, 0.05f);


		

		//vec2<float> collisionPoint = GetPointsInsideDoor(&door, player);
		

		//Draw the normals
		vec2<float> up = Normalize(door.vertices[2] - door.vertices[1]);
		up = V2(-up.y, up.x);
		vec2<float> right = Normalize(door.vertices[3] - door.vertices[2]);
		right = V2(-right.y, right.x);
		vec2<float> bottom = Normalize(door.vertices[0] - door.vertices[3]);
		bottom = V2(-bottom.y, bottom.x);
		vec2<float> left = Normalize(door.vertices[1] - door.vertices[0]);
		left = V2(-left.y, left.x);
		
		DrawLine(door.relativePosition, door.relativePosition + (up * 64.0f),     GetColor(1.0f, 0.0f, 0.0f, 1.0f), 3.0f);
		DrawLine(door.relativePosition, door.relativePosition + (right * 64.0f),  GetColor(0.0f, 1.0f, 0.0f, 1.0f), 3.0f);
		DrawLine(door.relativePosition, door.relativePosition + (bottom * 64.0f), GetColor(0.0f, 0.0f, 1.0f, 1.0f), 3.0f);
		DrawLine(door.relativePosition, door.relativePosition + (left * 64.0f),   GetColor(1.0f, 1.0f, 1.0f, 1.0f), 3.0f);

		for (unsigned int i = 0; i < collisionPoints.size(); i++)
		{
			DrawQuad(collisionPoints[i], V2(8.0f, 8.0f), GetColor(V4(1.0f, 1.0f, 0.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		}

	//	need to rotate around the origin for htis to work
		DrawDoor(&door, collided.collided);
		PopTransform();


		float dt = 0.0005f;
		if (collided.collided)
		{
			if (collisionPoints.size() > 0)
			{
				//Apply the force in the opposite way of the minimum translation vector
				vec2<float> force = -collided.mtv * 5000.0f;
				vec2<float> centerOfMass = door.hingePosition;
				vec2<float> momentArm = collisionPoints[0] - centerOfMass;

				float inertia = 5.0f;
				float torque = Cross(momentArm, force);
			//	std::cout << "torque: " << torque << std::endl;

			//	if ((!door.angularVelocity > 80.0f || !door.angularVelocity < 80.0f) && (!door.angularAcceleration > 80.0f || !door.angularAcceleration < 80.0f))
			//	{
				door.angularAcceleration += (torque / inertia) * dt;
				door.angularVelocity += door.angularAcceleration * dt;
				//door.angularVelocity = torque < 0 ? -7 : 7;
			//	}

				float cappedValue = 35.0f;

				if (door.angularVelocity > cappedValue)
				{
					door.angularVelocity = cappedValue;
				}
				if (door.angularVelocity < -cappedValue)
				{
					door.angularVelocity = -cappedValue;
				}

				if (door.angularAcceleration > cappedValue)
				{
					door.angularAcceleration = cappedValue;
				}
				if (door.angularAcceleration < -cappedValue)
				{
					door.angularAcceleration = -cappedValue;
				}
				std::cout << "angular vel: " << door.angularVelocity << std::endl;
				std::cout << "angular acc: " << door.angularAcceleration << std::endl;
				door.active = true;
			}
		}

		if (door.active)
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
				door.angularVelocity += -door.angularVelocity * 0.003f;

				//If we're close to 0 we stop
				if (fabsf(door.angularVelocity) < 0.0000001f)
				{
					door.angularAcceleration = 0.0f;
					door.angularVelocity = 0.0f;
					door.active = false;
				}
			}

		}

		door.rotationRadians += door.angularVelocity * dt;

		RotateDoorRadians(&door, door.rotationRadians);
		//door.rotationRadians += door.angularVelocity * dt;

		End();
		Flush();
		Unbind(&shaders[FORWARD_SHADER]);



		Bind(&shaders[QUAD_SHADER]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window.width, window.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glBindVertexArray(ndcQuads[REGULAR_PASS].vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_COLOR_BUFFER].textureIDs[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_SHADOW_BUFFER].textureIDs[0]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_SHADOW_BUFFER].stencilTextureID);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		Unbind(&shaders[QUAD_SHADER]);


		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "GameMain End:" << err << std::endl;
		}

		ClearInput(); //TODO: remove this

		SwapBuffers(window.gldc);


		LARGE_INTEGER EndCounter;
		QueryPerformanceCounter(&EndCounter);

		__int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;

		__int32 MSPerFrame = (__int32)(((1000 * CounterElapsed) / PerfCountFrequency));
		__int32 FPS = PerfCountFrequency / CounterElapsed;

		//std::cout << "FPS: "<< FPS << std::endl;


		LastCounter = EndCounter;
	}


	return 0;
}

#endif
