

namespace Game
{
	bool running = true;
	mat4x4 ortho = setOrthoFrustum(-640.0f, 640.0f, -360.0f, 360.0f, -100.0f, 100.0f);
	vec2<float> windowSize;
}


static void
GameResize(float newWidth, float newHeight)
{
	Game::windowSize = V2(newWidth, newHeight);
	//DirectX::XMMATRIX orthoMatrix = DirectX::XMMatrixOrthographicLH((float)newWidth, (float)newHeight, /*screenNear*/ 1.0f, /*screenDepth*/ 10.0f);
	ResizeGL(&renderers[COLOR_RENDERER], Game::ortho, newWidth, newHeight);
	//SetUniformProjectionMatrix(&shader, orthoMatrix);
}


#define SHADOWS false
#define DJIKSTRA false
#if SHADOWS
static int
GameMain(Window window)
{
	glViewport(0, 0, window.width, window.height);
	Game::windowSize = V2((float)window.width, (float)window.height);

	//TODO: DONT SWAP CALL HERE
	wglSwapIntervalEXT(0);

	InitializeRendererData(Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[COLOR_RENDERER], &vertexBuffers[COLOR_VERTEX_BUFFER], 60000, Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[SHADOW_RENDERER], &vertexBuffers[SHADOW_VERTEX_BUFFER], 1000, Game::ortho, (float)window.width, (float)window.height, true);

	Tilemap tilemap = GenerateTestTilemap();

	InitializeShadowEdges((float)window.width, (float)window.height);


	Shadow* shadow = CreateShadow(V2(0.0f, 0.0f), V3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
		                                             static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
		                                             static_cast <float> (rand()) / static_cast <float> (RAND_MAX)),
		Shadow::SHADOW_TYPE::CONE);


	world = new Cell[tilemap.width * tilemap.height];
	//populate the world with edges
		//width 
	for (int x = 1; x < (tilemap.width - 1); x++)
	{
		SetBit(&world[1 * tilemap.width + x].edge_exist_cell_exist, CELL_EXIST);
		SetBit(&world[(tilemap.height - 2) * tilemap.width + x].edge_exist_cell_exist, CELL_EXIST);
	}

	//height
	for (int x = 1; x < (tilemap.height - 1 ); x++)
	{
		SetBit(&world[x * tilemap.width + 1].edge_exist_cell_exist, CELL_EXIST);
		SetBit(&world[x * tilemap.width + (tilemap.width - 2)].edge_exist_cell_exist, CELL_EXIST);
	}

	//world[37].exist = true;



	vec2<float> player = V2(64.0f, -64.0f);


	//ConvertTileMapToPolyMap(0, 0, 20, 20, blockWidth, 20);
	ConvertTileMapToPolyMap(0, 0, tilemap.width, tilemap.height, blockWidth, tilemap.width, player);
	//CalculateVisibilityPolgyon(0.0f, 0.0f, 1000.0f);
	CalculateVisibilityPolgyons(1000.0f);








	

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
			std::cout << "mouse released" << std::endl;
			//vec2<float> cursorPos = V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f)); //this is the actual cursor pos;
			SetTileBit(&tilemap, cursorPos);
			SetCell(&tilemap, world, cursorPos);

			ConvertTileMapToPolyMap(0, 0, tilemap.width, tilemap.height, blockWidth, tilemap.width, player);

			CalculateVisibilityPolgyons(1000.0f);
		} 
		if (MouseReleased(RIGHT))
		{
			shadow = CreateShadow(cursorPos - V2(-16.0f, 16.0f), V3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
				                                                    static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
				                                                    static_cast <float> (rand()) / static_cast <float> (RAND_MAX)),
				Shadow::SHADOW_TYPE::POINT);
		}
		if (ScroolWheelUp())
		{
			shadow->shadowZPos += 1.0f;
		}
		if (ScroolWheelDown())
		{
			shadow->shadowZPos -= 1.0f;		
		}

		
		UpdateShadowFrustum(player);
		CalculateVisibilityPolgyons(shadow, 200.0f);
		






		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[OFF_SCREEN_CIRCLE_BUFFER].fbo);
		glViewport(0, 0, window.width, window.height);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		Bind(&shaders[CIRCLE_SHADER]);
		Begin(&renderers[SHADOW_RENDERER]);
		PushTransform(camera, -3.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawCircle(cursorPos, 200.0f, GetColor(1.0f, 0.0f, 0.0f, 1.0f), 24);
		PopTransform();
		End();
		Flush();
		Unbind(&shaders[CIRCLE_SHADER]);




		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[OFF_SCREEN_COLOR_BUFFER].fbo);
		glViewport(0, 0, window.width, window.height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Do OpenGL rendering here




		Bind(&shaders[FORWARD_SHADER]);
		Begin(&renderers[COLOR_RENDERER]);



		//Cellmap
		PushTransform(camera, -5.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawCells(&tilemap, world, V2(32.0f, 32.0f), -1);
		PopTransform();

		//Draw edges
		PushTransform(camera, -4.5f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		for (auto &e : edges) {
			//Notice the 16.0f paddings
			DrawLine(V2(e.start.x - 16.0f, -e.start.y + 16.0f), V2(e.end.x - 16.0f, -e.end.y + 16.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), 2.5f);
			PushTransform(V2(0.0f, 0.0f), 0.1f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
			DrawQuad(V2(e.start.x - 16.0f, -e.start.y + 16.0f), V2(5.0f, 5.0f), GetColor(V4(1.0f, 0.0f, 0.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
			DrawQuad(V2(e.end.x - 16.0f, -e.end.y + 16.0f), V2(5.0f, 5.0f), GetColor(V4(1.0f, 0.0f, 0.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
			PopTransform();
		}
		PopTransform();


		vec2<float> cursor = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + player;
		vec2<float> direction = Normalize(cursor - player);
		direction.y = -direction.y;
		vec2<float> left = RotateVec(direction, 90.0);
		vec2<float> right = RotateVec(direction, -90.0);
		left = Normalize(left);
		right = Normalize(right);

		shadow->position = player -V2(-16.0f, 16.0f);
		shadow->direction = Normalize(cursor - shadow->position);
		shadow->direction = direction;
		


		//player
		PushTransform(camera, -4.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(player, V2(32.0f, 32.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		PopTransform();


		End();
		Flush();
		Unbind(&shaders[FORWARD_SHADER]);





		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[OFF_SCREEN_SHADOW_BUFFER].fbo);
		glViewport(0, 0, window.width, window.height);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		GLenum err2;
		while ((err2 = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "before:" << err2 << std::endl;
		}

		vec3<float> lightColors[2] = { V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 1.0f) };


		while ((err2 = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "after:" << err2 << std::endl;
		}
		//visibility polygonsd
		PushTransform(camera, -2.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		if (shadow->visibilityPolygonPoints.size() > 0)
		{
			for (int i = 0; i < shadow->visibilityPolygonPoints.size() - 1; i++)
			{

				DrawLine(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[i].y - 16.0f, -shadow->visibilityPolygonPoints[i].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
				DrawLine(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[i + 1].y - 16.0f, -shadow->visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
			}
		}
		//DrawTriangle(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[shadow->visibilityPolygonPoints.size() - 1].y - 16.0f, -shadow->visibilityPolygonPoints[shadow->visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadow->visibilityPolygonPoints[0].y - 16.0f, -shadow->visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
		PopTransform();


		GLenum er3r;
		while ((er3r = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "GameMain before shadow:" << er3r << std::endl;
		}


		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_CIRCLE_BUFFER].stencilTextureID);



		float depthOffset = -2.0f;
		for (unsigned int k = 0; k < shadows.numberOfActiveShadows; k++)
		{

			Bind(&shaders[SHADOW_SHADER]);
			Begin(&renderers[SHADOW_RENDERER]);


			setUniform2f(&shaders[SHADOW_SHADER], "pos", shadows.shadows[k].position + V2(-16.0f, 16.0f) - player);
			setUniform3f(&shaders[SHADOW_SHADER], "color1", shadows.shadows[k].color);
			setUniform1i(&shaders[SHADOW_SHADER], "currentActiveOutput", 0);
			setUniform1f(&shaders[SHADOW_SHADER], "lightZPosition", shadows.shadows[k].shadowZPos);
			

			if (shadows.shadows[k].type == Shadow::POINT) 
			{
				setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestricted", 0);
				PushTransform(camera, depthOffset, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
				depthOffset += 0.1f;
				if (shadows.shadows[k].visibilityPolygonPoints.size() > 0)
				{
					for (int i = 0; i < shadows.shadows[k].visibilityPolygonPoints.size() - 1; i++)
					{

						DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						
					}
				}


				DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[0].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
			
				PopTransform();
			} 
			else if (shadows.shadows[k].type == Shadow::CONE) 
			{



				setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestrictedrino", 0);


				if (shadows.shadows[k].reversingNeeded && shadows.shadows[k].index != -1)
				{


					PushTransform(camera, depthOffset, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
					depthOffset += 0.1f;
					if (shadows.shadows[k].visibilityPolygonPoints.size() > 0)
					{

						for (int i = 0; i < shadows.shadows[k].index - 1; i++)
						{
							DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						}
						DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[0].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[0].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						for (int i = shadows.shadows[k].index; i < shadows.shadows[k].visibilityPolygonPoints.size() - 1; i++)
						{
							DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						}

					}
					PopTransform();
				}
				else 
				{
					PushTransform(camera, depthOffset, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
					depthOffset += 0.1f;
					if (shadows.shadows[k].visibilityPolygonPoints.size() > 0)
					{
						for (int i = 0; i < shadows.shadows[k].visibilityPolygonPoints.size() - 1; i++)
						{
							
							DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f , shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
	
						}
					}

					PopTransform();
					//}
				}


				//draw 
			}
			

			End();
			Flush();


			if (shadows.shadows[k].type == Shadow::CONE) 
			{
				

				setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestricted", 0);
			}
			Unbind(&shaders[SHADOW_SHADER]);
		}
		//glBindTexture(GL_TEXTURE_2D, 0);
		


		








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
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, framebuffers[OFF_SCREEN_CIRCLE_BUFFER].stencilTextureID);

		setUniform1i(&shaders[QUAD_SHADER], "shouldLightBeRestricted", 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		setUniform1i(&shaders[QUAD_SHADER], "shouldLightBeRestricted", 0);

		glEnable(GL_DEPTH_TEST);
		Unbind(&shaders[QUAD_SHADER]);

		//glDisable(GL_CULL_FACE);
//glCullFace(GL_BACK);
//glEnable(GL_CULL_FACE); //remove these, keep them for finding errors
//glCullFace(GL_BACK);

		/*	UpdateEntities();

			UpdatePlayer();*/



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
				ai.state == Ai::State::ROAM;
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


		//Player
		PushTransform(camera, -4.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(player, V2(32.0f, 32.0f), GetColor(V4(0.0f, 1.0f, 0.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
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
			DrawQuad(V2(pathStack.arr[i].x * 32.0f, pathStack.arr[i].y * -32.0f), V2(30.0f, 30.0f), GetColor(V4(1.0f, 0.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		}
		PopTransform();

		//Ai
		PushTransform(camera, -1.5f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(ai.position, V2(30.0f, 30.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
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
