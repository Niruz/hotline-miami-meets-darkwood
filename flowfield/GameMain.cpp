

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

	InitializeRendererData(Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[COLOR_RENDERER], &vertexBuffers[COLOR_VERTEX_BUFFER], 60000, Game::ortho, (float)window.width, (float)window.height);
	Initialize(&renderers[SHADOW_RENDERER], &vertexBuffers[SHADOW_VERTEX_BUFFER], 1000, Game::ortho, (float)window.width, (float)window.height, true);

	Tilemap tilemap = GenerateTestTilemap();

	InitializeShadowEdges((float)window.width, (float)window.height);




	//vec2<float> lightPosition = V2(224.0f, -224.0f);
	//Shadow* shadow1 = CreateShadow(V2(224.0f, -224.0f), V3(1.0f, 0.0f, 0.0f));
	//Shadow* shadow2 = CreateShadow(V2(448.0f, -448.0f), V3(1.0f, 0.0f, 0.0f));
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
		//SetBit(&world[1 * 20 + x].edge_exist_cell_exist, CELL_EXIST);
		//SetBit(&world[(20 - 2) * 20 + x].edge_exist_cell_exist, CELL_EXIST);
		//world[1 * 20 + x].exist = true; //width
		//world[(20 - 2) * 20 + x].exist = true; //height
	}

	//height
	for (int x = 1; x < (tilemap.height - 1 ); x++)
	{
		SetBit(&world[x * tilemap.width + 1].edge_exist_cell_exist, CELL_EXIST);
		SetBit(&world[x * tilemap.width + (tilemap.width - 2)].edge_exist_cell_exist, CELL_EXIST);
		//SetBit(&world[x * 20 + 1].edge_exist_cell_exist, CELL_EXIST);
		//SetBit(&world[x * 20 + (20 - 2)].edge_exist_cell_exist, CELL_EXIST);
		//world[x * 20 + 1].exist = true;
		//world[x * 20 + (20 - 2)].exist = true;
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
			//pitch == world width
			//ConvertTileMapToPolyMap(0, 0, 20, 20, blockWidth, 20);
			ConvertTileMapToPolyMap(0, 0, tilemap.width, tilemap.height, blockWidth, tilemap.width, player);
			//CalculateVisibilityPolgyon(camera.x, camera.y, 1000.0f);
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
		
		//CalculateVisibilityPolgyon(lightPosition.x, -lightPosition.y, 1000.0f);
		
		//CalculateVisibilityPolgyon(camera.x, camera.y, 1000.0f);






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


		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
		//glEnable(GL_CULL_FACE); //remove these, keep them for finding errors
		//glCullFace(GL_BACK);


		Bind(&shaders[FORWARD_SHADER]);
		Begin(&renderers[COLOR_RENDERER]);

		//Tilemap
		/*PushTransform(&Game::renderer, camera, -5.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawTilemap(&Game::renderer, &tilemap, V2(32.0f, 32.0f), -1);
		PopTransform(&Game::renderer);*/

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
		//left.y = left.y * -1.0f;
		//left = left * 50.0f;
		//shadow->position = cursorPos - V2(-16.0f, 16.0f);// , cursorPos + V2(16.0f, -16.0f);
		//This is due to where "shadow" space is calculated, we either do this or render triangles at an offset (everything else is rendered by midpoints)
		shadow->position = player -V2(-16.0f, 16.0f);
		shadow->direction = Normalize(cursor - shadow->position);
		shadow->direction = direction;
		
	/*	PushTransform(camera, 0.1f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawLine(player, cursor, GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), 2.5f);
		DrawLine(player, player + (left * 100.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), 2.5f);
		DrawQuad(player + (left * 200.0f), V2(32.0f, 32.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		DrawLine(player, player + (right * 100.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), 2.5f);
		DrawQuad(player + (right * 200.0f), V2(32.0f, 32.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
	//	DrawLine(player, player + (right * 100000.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), 2.5f);
		PopTransform();*/

		//player
		PushTransform(camera, -4.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawQuad(player, V2(32.0f, 32.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		PopTransform();

	//	vec2<float> cursorPos = (V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f))) + player;


		/*//visibility polygon
		PushTransform(&Game::renderer, camera, -2.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		if (visibilityPolygonPoints.size() > 0) 
		{
			for (int i = 0; i < visibilityPolygonPoints.size() - 1; i++)
			{
				//note padding here again...
				DrawTriangle(&Game::renderer, V2(cursorPos.x - 16.0f, cursorPos.y + 16.0f), V2(visibilityPolygonPoints[i].y - 16.0f, -visibilityPolygonPoints[i].z + 16.0f), V2(visibilityPolygonPoints[i + 1].y - 16.0f, -visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
				//DrawLine(&Game::renderer, cursorPos, V2(visibilityPolygonPoints[i].y - 16.0f, -visibilityPolygonPoints[i].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
				//DrawLine(&Game::renderer, cursorPos, V2(visibilityPolygonPoints[i + 1].y - 16.0f, -visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
			}
		}
		DrawTriangle(&Game::renderer, V2(cursorPos.x - 16.0f, cursorPos.y + 16.0f), V2(visibilityPolygonPoints[visibilityPolygonPoints.size() - 1].y - 16.0f, -visibilityPolygonPoints[visibilityPolygonPoints.size() - 1].z + 16.0f), V2(visibilityPolygonPoints[0].y - 16.0f, -visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
		//DrawTriangle(&Game::renderer, cursorPos, V2(32.0f, 32.0f), GetColor(1.0f, 0.0f, 1.0f, 1.0f));
		PopTransform(&Game::renderer);*/

		//Line
		/*PushTransform(camera, 1.01f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawLine(V2(32.0f * 5, 32.0f * -10), player, GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), 5.0f);
		PopTransform();*/


		/*PushTransform(&Game::renderer, V2(0.0f, 0.0f), -60.0f, V2(CosRadians(DegreesToRadians(10)), SinRadians(DegreesToRadians(10))));
		Submit(&Game::renderer, V2(0.0f, 0.0f), V2(320.0f, 410.0f), GetColor(V4(1.0f, 1.0f, 1.0f, 1.0f)), textures[TEST1].ID, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		PopTransform(&Game::renderer);

		glDisable(GL_CULL_FACE);
		PushTransform(&Game::renderer, V2(0.0f, 0.0f), -1.0f, V2(CosRadians(DegreesToRadians(180)), SinRadians(DegreesToRadians(180))));
		DrawString(&Game::renderer, "Adventurer\ntest", V4(0.0f, 0.0f, 0.0f, 1.0f), *GetFont(ADVENTURER_128), GetColor(0.0f, 1.0f, 0.0f, 1.0f));
		PopTransform(&Game::renderer);*/

		End();
		Flush();
		Unbind(&shaders[FORWARD_SHADER]);

		//glFlush();



		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[OFF_SCREEN_SHADOW_BUFFER].fbo);
		glViewport(0, 0, window.width, window.height);
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		

		//vec2<float> cursorPos2 = V2(mouseState.x, mouseState.y) - (Game::windowSize * V2(0.5f, 0.5f)); //this is the actual cursor pos;
	//	vec2<float> lightPos = V2((float)(mouseState.x * 16.0f / 1280.0f), (float)(9.0f - mouseState.y * 9.0f /720.0f));
		GLenum err2;
		while ((err2 = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "before:" << err2 << std::endl;
		}

		//setUniform2f(&shaders[SHADOW_SHADER], "shit", V2(mouseState.x, mouseState.y));

		
		//NOTE: since im a retard, cameraPos is used here since everything else is in world space, but the pushtransform puts it into camera space ...
		//vec2<float> lightCameraPosition = lightPosition - player;
		//vec2<float> lightCameraPosition1 = shadow1->position - player;
		//vec2<float> lightCameraPosition2 = shadow2->position - player;
		//vec2<float> lightCameraPos[2] = { lightCameraPosition1 , lightCameraPosition2 };
		vec3<float> lightColors[2] = { V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 1.0f) };
		/*setUniform2f(&shaders[SHADOW_SHADER], "shit", lightCameraPosition1);
		setUniform2f(&shaders[SHADOW_SHADER], "shit2", lightCameraPosition2);
		setUniform3f(&shaders[SHADOW_SHADER], "color1", V3(1.0f, 0.0f, 0.0f));
		setUniform3f(&shaders[SHADOW_SHADER], "color2", V3(0.0f, 0.0f, 1.0f));*/

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
				//note padding here again...
				//DrawTriangle(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[i].y - 16.0f, -shadow->visibilityPolygonPoints[i].z + 16.0f), V2(shadow->visibilityPolygonPoints[i + 1].y - 16.0f, -shadow->visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
				//DrawTriangle(V2(cursorPos.x - 16.0f, cursorPos.y + 16.0f), V2(visibilityPolygonPoints[i].y - 16.0f, -visibilityPolygonPoints[i].z + 16.0f), V2(visibilityPolygonPoints[i + 1].y - 16.0f, -visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));

				//DrawLine(shadow->position, V2(shadow->visibilityPolygonPoints[i].y - 16.0f, -shadow->visibilityPolygonPoints[i].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
				//DrawLine(shadow->position, V2(shadow->visibilityPolygonPoints[i + 1].y - 16.0f, -shadow->visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
				DrawLine(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[i].y - 16.0f, -shadow->visibilityPolygonPoints[i].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
				DrawLine(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[i + 1].y - 16.0f, -shadow->visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f), 2.0f);
			}
		}
		//DrawTriangle(V2(shadow->position.x - 16.0f, shadow->position.y + 16.0f), V2(shadow->visibilityPolygonPoints[shadow->visibilityPolygonPoints.size() - 1].y - 16.0f, -shadow->visibilityPolygonPoints[shadow->visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadow->visibilityPolygonPoints[0].y - 16.0f, -shadow->visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
		PopTransform();


		/*setUniform2f(&shaders[SHADOW_SHADER], "shit", lightCameraPosition1);
		setUniform2f(&shaders[SHADOW_SHADER], "shit2", lightCameraPosition2);
		setUniform3f(&shaders[SHADOW_SHADER], "color1", V3(1.0f, 0.0f, 0.0f));
		setUniform3f(&shaders[SHADOW_SHADER], "color2", V3(0.0f, 0.0f, 1.0f));*/

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

			//mouse
			/*PushTransform(camera, -3.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
			//DrawTriangle(cursorPos - V2(16.0f, 16.0f), cursorPos + V2(16.0f, -16.0f), cursorPos + V2(0.0f, 16.0f), GetColor(1.0f, 0.0f, 0.0f, 1.0f));
			DrawCircle(cursorPos, 200.0f, GetColor(1.0f, 0.0f, 0.0f, 1.0f), 24);
			PopTransform();*/

			setUniform2f(&shaders[SHADOW_SHADER], "shit", shadows.shadows[k].position + V2(-16.0f, 16.0f) - player);
			setUniform3f(&shaders[SHADOW_SHADER], "color1", shadows.shadows[k].color);
			setUniform1i(&shaders[SHADOW_SHADER], "currentActiveOutput", 0);
			setUniform1f(&shaders[SHADOW_SHADER], "lightZPosition", shadows.shadows[k].shadowZPos);
			

			if (shadows.shadows[k].type == Shadow::POINT) 
			{
				setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestrictedrino", 1);
				PushTransform(camera, depthOffset, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
				depthOffset += 0.1f;
				if (shadows.shadows[k].visibilityPolygonPoints.size() > 0)
				{
					for (int i = 0; i < shadows.shadows[k].visibilityPolygonPoints.size() - 1; i++)
					{
						//note padding here again...
						//this one was correct before DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						//DrawTriangle(V2(shadows.shadows[k].position.x, shadows.shadows[k].position.y), V2(shadows.shadows[k].visibilityPolygonPoints[i].y, -shadows.shadows[k].visibilityPolygonPoints[i].z), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						//DrawTriangle(V2(cursorPos.x - 16.0f, cursorPos.y + 16.0f), V2(visibilityPolygonPoints[i].y - 16.0f, -visibilityPolygonPoints[i].z + 16.0f), V2(visibilityPolygonPoints[i + 1].y - 16.0f, -visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
					}
				}
				//if (shadows.shadows[k].visibilityPolygonPoints.size() > 0 && shadows.shadows[k].visibilityPolygonPoints.size() % 2 == 0)
				//this is jsut to complete the "light circle"

				DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[0].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
				//DrawTriangle(V2(shadows.shadows[k].position.x, shadows.shadows[k].position.y ), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y , -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z ), V2(shadows.shadows[k].visibilityPolygonPoints[0].y , -shadows.shadows[k].visibilityPolygonPoints[0].z), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
				//this one was correct.. DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[0].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
				PopTransform();
			} 
			else if (shadows.shadows[k].type == Shadow::CONE) 
			{



				setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestrictedrino", 0);

				/*if(!shadows.shadows[k].reversingNeeded) 
				{
					PushTransform(camera, depthOffset, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
					depthOffset += 0.1f;
					if (shadows.shadows[k].visibilityPolygonPoints.size() > 0)
					{
						for (int i = shadows.shadows[k].visibilityPolygonPoints.size() - 1; i > 0; i--)
						{
							//note padding here again...
							DrawTriangle(V2(shadows.shadows[k].position.x, shadows.shadows[k].position.y), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i - 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						}
					}
					PopTransform();
				}*/
				//else 
				//{
				if (shadows.shadows[k].reversingNeeded && shadows.shadows[k].index != -1)
				{
					//This case is needed if we have some angles above and below 360 degrees
					/*std::vector<vec3<float>> aboveThreshold;

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
					}*/

					PushTransform(camera, depthOffset, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
					depthOffset += 0.1f;
					if (shadows.shadows[k].visibilityPolygonPoints.size() > 0)
					{
						/*for (int i = 0; i < shadows.shadows[k].visibilityPolygonPoints.size() - 1; i++)
						{
							DrawTriangle(V2(shadows.shadows[k].position.x, shadows.shadows[k].position.y), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));	
						}*/
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
							//note padding here again...
							//this one was correct before DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
							DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f , shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
							//DrawTriangle(V2(shadows.shadows[k].position.x, shadows.shadows[k].position.y), V2(shadows.shadows[k].visibilityPolygonPoints[i].y, -shadows.shadows[k].visibilityPolygonPoints[i].z), V2(shadows.shadows[k].visibilityPolygonPoints[i + 1].y, -shadows.shadows[k].visibilityPolygonPoints[i + 1].z), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
							//DrawTriangle(V2(cursorPos.x - 16.0f, cursorPos.y + 16.0f), V2(visibilityPolygonPoints[i].y - 16.0f, -visibilityPolygonPoints[i].z + 16.0f), V2(visibilityPolygonPoints[i + 1].y - 16.0f, -visibilityPolygonPoints[i + 1].z + 16.0f), GetColor(0.5f, 0.5f, 0.5f, 1.0f));
						}
					}
					//if (shadows.shadows[k].visibilityPolygonPoints.size() > 0 && shadows.shadows[k].visibilityPolygonPoints.size() % 2 == 0)
					//	DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[0].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
					//DrawTriangle(V2(shadows.shadows[k].position.x, shadows.shadows[k].position.y ), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y , -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z ), V2(shadows.shadows[k].visibilityPolygonPoints[0].y , -shadows.shadows[k].visibilityPolygonPoints[0].z), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
					//this one was correct.. DrawTriangle(V2(shadows.shadows[k].position.x - 16.0f, shadows.shadows[k].position.y + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[shadows.shadows[k].visibilityPolygonPoints.size() - 1].z + 16.0f), V2(shadows.shadows[k].visibilityPolygonPoints[0].y - 16.0f, -shadows.shadows[k].visibilityPolygonPoints[0].z + 16.0f), GetColor(0.5f, 1.0f, 0.5f, 1.0f));
					PopTransform();
					//}
				}


				//draw 
			}
			

			End();
			Flush();


			if (shadows.shadows[k].type == Shadow::CONE) 
			{
				

				setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestrictedrino", 0);
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

		setUniform1i(&shaders[QUAD_SHADER], "shouldLightBeRestricted", 1);

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
	InitializeAi(&ai, V2(4, 4), V2(10, 10));
	SetStateRoaming(&ai);

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
				if (ai.currentGoal == ai.patrolFrom)
				{
					ai.currentGoal = ai.patrolTo;
					ai.direction = Normalize(V2(ai.patrolTo.x * 32.0f, ai.patrolTo.y * -32.0f) - ai.position);
				}
				else
				{
					ai.currentGoal = ai.patrolFrom;
					ai.direction = Normalize(V2(ai.patrolFrom.x * 32.0f, ai.patrolFrom.y * -32.0f) - ai.position);
				}

			}

		} 
		else if (ai.state == Ai::State::ROAM) 
		{
			//Give some new direction
			if (ai.frameCounter >= 4000)
			{
				float LO = -1.0f;
				float HI = 1.0f;
				float rX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
				float rY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
				ai.direction = Normalize(V2(rX, rY));
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

		std::cout << "FPS: "<< FPS << std::endl;
		/*
		1 / counts / frame  = frames / count
			then multiply that by counts / secocond

			counts / second is the frequency
			frames per count = 1
			frames / count * counts / second == frames / second
			frequency / counter
			*/

		LastCounter = EndCounter;
	}


	return 0;
}

#endif

/*
int main() {

	Tilemap tilemap = GenerateTestTilemap();

	Agent agent;
	InitializeAgent(&agent, VEC(0.0f, 0.0f), VEC(0.0f, 0.0f));

	int counter = 0;
	bool firstTime = true;
	for (int i = 0; i < tilemap.width * tilemap.height; i++) {

		counter++;
		if (firstTime)
		{
			counter = 0;
			firstTime = false;
		}
		if (counter == tilemap.width) {
			std::cout << '\n';
			counter = 0;
		}

		std::cout << TestBit(tilemap.tilemap, i);
	}

	std::cout << std::endl;
	std::cout << std::endl;

	//GenerateDjikstra(VEC(4,4), &tilemap);
	GenerateDjikstra(VEC(18, 1), &tilemap);

	return 0;
}
*/