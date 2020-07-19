
/*
#define RENDERER_MAX_SPRITES	60000
#define RENDERER_VERTEX_SIZE	sizeof(VertexData)
#define RENDERER_SPRITE_SIZE	(RENDERER_VERTEX_SIZE * 4)
#define RENDERER_BUFFER_SIZE	(RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES)
#define RENDERER_INDICES_SIZE   (RENDERER_MAX_SPRITES * 6)*/
#define RENDERER_MAX_TEXTURES    16

#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX     1
#define SHADER_TID_INDEX    2
#define SHADER_COLOR_INDEX  3

enum FRAME_BUFFERS
{
	OFF_SCREEN_COLOR_BUFFER,
	OFF_SCREEN_SHADOW_BUFFER,
	OFF_SCREEN_CIRCLE_BUFFER,
	MAX_FRAME_BUFFERS,
};

enum NDC_QUADS
{
	REGULAR_PASS,
	SHADOW_PASS,
	MAX_NDC_QUADS
};

enum RENDERERS
{
	COLOR_RENDERER,
	SHADOW_RENDERER,
	MAX_RENDERERS
};

enum VERTEX_BUFFERS
{
	COLOR_VERTEX_BUFFER,
	SHADOW_VERTEX_BUFFER,
	MAX_VERTEX_BUFFERS
};

struct Framebuffer
{
	GLuint fbo;
	GLuint textureIDs[16];
	GLuint rbo;
	GLuint depthID;
	GLuint stencilID;
	GLuint stencilTextureID; //main shadow stenicl
	GLuint stencilTextureID2; // //used to restrict point lights
	float width;
	float height;
};

struct NDCQuad
{
	unsigned int vao, vbo;
	float ndcVertices[24] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
};

struct Transform
{
	vec2<float> position;
	vec2<float> rotation;
	float depth;
};

struct TransformationStack
{
	unsigned int currentIndex = 0;
	Transform* transformationStack;
	const Transform* transformationStackBack;
};

struct VertexData
{
	vec3<float> position;
	vec2<float> texture;
	float textureID;
	unsigned int color;
};

struct VertexBuffer
{
	GLuint VAO;
	IndexBuffer IBO;
	GLuint VBO;
	GLsizei indexCount;

	bool triangleBuffer = false;

	VertexData* buffer;
};

struct GLRenderer
{
	TransformationStack transformationStack;

	VertexBuffer* vertexBuffer;

	//Shader* shader;

	std::vector<GLuint> textureSlots;

	mat4x4 projectionMatrix;
};

//Set active renderer using bind
GLRenderer* activeRenderer;
GLRenderer renderers[MAX_RENDERERS];
Framebuffer framebuffers[MAX_FRAME_BUFFERS];
VertexBuffer vertexBuffers[MAX_VERTEX_BUFFERS];
NDCQuad ndcQuads[2];

#pragma warning( push )
#pragma warning( disable : 4244)
static unsigned int
GetColor(const vec4<float>& color)
{
	unsigned int col = 0;
	int r = color.x * 255.0f;
	int g = color.y * 255.0f;
	int b = color.z * 255.0f;
	int a = color.w * 255.0f;

	col = a << 24 | b << 16 | g << 8 | r;
	return col;
}
#pragma warning( pop ) 

#pragma warning( push )
#pragma warning( disable : 4244)
static unsigned int
GetColor(float x, float y, float z, float w)
{
	unsigned int col = 0;
	int r = x * 255.0f;
	int g = y * 255.0f;
	int b = z * 255.0f;
	int a = w * 255.0f;

	col = a << 24 | b << 16 | g << 8 | r;
	return col;
}
#pragma warning( pop ) 

static Transform 
SetTransform(vec2<float> position, vec2<float> rotation, float depth)
{
	Transform transform;
	transform.position = position;
	transform.rotation = rotation;
	transform.depth = depth;
	return transform;
}

static void
InitializeTransformationStack(GLRenderer* renderer)
{
	renderer->transformationStack.transformationStack = new Transform[10]; //TODO: resize
	renderer->transformationStack.transformationStack[renderer->transformationStack.currentIndex] = SetTransform(V2(0.0f, 0.0f), V2(0.0f, 0.0f), 0.0f);
	renderer->transformationStack.transformationStackBack = &renderer->transformationStack.transformationStack[renderer->transformationStack.currentIndex];
	renderer->transformationStack.currentIndex++;
}

static void
PushTransform(const vec2<float>& position, float depth, const vec2<float>& rotation, bool doOverride = false)
{
	if (doOverride)
	{
		activeRenderer->transformationStack.transformationStack[activeRenderer->transformationStack.currentIndex] = SetTransform(position, rotation, depth);
	}
	else
	{
		// x + y stack. Z value is used as depth. Rotations do not stack
		activeRenderer->transformationStack.transformationStack[activeRenderer->transformationStack.currentIndex] = SetTransform(activeRenderer->transformationStack.transformationStackBack->position + position, rotation, depth);
	}
	activeRenderer->transformationStack.transformationStackBack = &activeRenderer->transformationStack.transformationStack[activeRenderer->transformationStack.currentIndex];
	activeRenderer->transformationStack.currentIndex++;
}

static void
PopTransform()
{
	if (activeRenderer->transformationStack.currentIndex > 1)
		activeRenderer->transformationStack.currentIndex--;
	activeRenderer->transformationStack.transformationStackBack = &activeRenderer->transformationStack.transformationStack[activeRenderer->transformationStack.currentIndex - 1];
}

static void
ResizeGL(GLRenderer* renderer, mat4x4& projectionMatrix, float width, float height)
{
	projectionMatrix = setOrthoFrustum(-(width*0.5f), (width*0.5f), -(height*0.5f), (height*0.5f), -100.0f, 100.0f);
	glViewport(0, 0, width, height);
	//Bind(renderer->shader);
	Bind(&shaders[FORWARD_SHADER]);

	//setUniformMatrix4fv(renderer->shader, "projectionMatrix", 1, GL_FALSE, projectionMatrix);

	//Unbind(renderer->shader);
	Unbind(&shaders[FORWARD_SHADER]);
}

static void
InitializeOffScreenBuffer(float width, float height)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout <<"InitializeOffScreenError Begin: "<< err << std::endl;
	}

	//TODO: delete & resize 
	for (int i = 0; i < MAX_FRAME_BUFFERS; i++ )
	{
		glGenFramebuffers(1, &framebuffers[i].fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i].fbo);


		if (i == OFF_SCREEN_SHADOW_BUFFER) 
		{

			for (unsigned int k = 0; k < 1; k++) 
			{
				glGenTextures(1, &framebuffers[i].textureIDs[k]);
				glBindTexture(GL_TEXTURE_2D, framebuffers[i].textureIDs[k]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + k, GL_TEXTURE_2D, framebuffers[i].textureIDs[k], 0);
			}

			/*
			glGenRenderbuffers(1, &framebuffers[i].rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, framebuffers[i].rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffers[i].rbo);
			*/

			glGenTextures(1, &framebuffers[i].stencilID);
			glBindTexture(GL_TEXTURE_2D, framebuffers[i].stencilID);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_UNSIGNED_INT_24_8, GL_FLOAT, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffers[i].stencilID, 0);


			glGenTextures(1, &framebuffers[i].stencilTextureID);
			glBindTexture(GL_TEXTURE_2D, framebuffers[i].stencilTextureID);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, framebuffers[i].stencilTextureID, 0);



			GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, DrawBuffers);

		}
		else if (i == OFF_SCREEN_COLOR_BUFFER)
		{
			glGenTextures(1, &framebuffers[i].textureIDs[0]);
			glBindTexture(GL_TEXTURE_2D, framebuffers[i].textureIDs[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffers[i].textureIDs[0], 0);

			/*
			glGenRenderbuffers(1, &framebuffers[i].rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, framebuffers[i].rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffers[i].rbo);
			*/

			glGenTextures(1, &framebuffers[i].stencilID);
			glBindTexture(GL_TEXTURE_2D, framebuffers[i].stencilID);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_UNSIGNED_INT_24_8, GL_FLOAT, 0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffers[i].stencilID, 0);

			GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, DrawBuffers);
		}
		else if (i == OFF_SCREEN_CIRCLE_BUFFER)
		{

			glGenTextures(1, &framebuffers[i].stencilID);
			glBindTexture(GL_TEXTURE_2D, framebuffers[i].stencilID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffers[i].stencilID, 0);


			glGenTextures(1, &framebuffers[i].stencilTextureID);
			glBindTexture(GL_TEXTURE_2D, framebuffers[i].stencilTextureID);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffers[i].stencilTextureID, 0);

			GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, DrawBuffers);
		}





		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR: Unable to initialize off screen buffer" << std::endl;


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "InitializeOffScreenError End: " << err << std::endl;
	}
	//glBindTexture(GL_TEXTURE_2D, 0);
}

static void
InitializeNDCQuad()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "InitializeNDCQuad Begin: " << err << std::endl;
	}

	for (int i = 0; i < MAX_NDC_QUADS; i++) 
	{
		glGenVertexArrays(1, &ndcQuads[i].vao);
		glGenBuffers(1, &ndcQuads[i].vbo);
		glBindVertexArray(ndcQuads[i].vao);
		glBindBuffer(GL_ARRAY_BUFFER, ndcQuads[i].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, &ndcQuads[i].ndcVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "InitializeNDCQuad End: " << err << std::endl;
	}
}

static void
InitializeStencilTexture()
{

}

static void
InitializeRendererData(const mat4x4& projectionMatrix, float width, float height) 
{
	//Set shaders
	Bind(&shaders[FORWARD_SHADER]);
	setUniformMatrix4fv(&shaders[FORWARD_SHADER], "projectionMatrix", 1, GL_FALSE, projectionMatrix);
	GLint texIDS[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	setUniform1iv(&shaders[FORWARD_SHADER], "textureArray[0]", texIDS, 16);
	Unbind(&shaders[FORWARD_SHADER]);


	Bind(&shaders[SHADOW_SHADER]);
	setUniformMatrix4fv(&shaders[SHADOW_SHADER], "projectionMatrix", 1, GL_FALSE, projectionMatrix);
	setUniform1f(&shaders[SHADOW_SHADER], "lightZPosition", 30.0f);
	setUniform1i(&shaders[SHADOW_SHADER], "stencilTexturino", 4);
	setUniform1i(&shaders[SHADOW_SHADER], "shouldLightBeRestricted", 0);
	Unbind(&shaders[SHADOW_SHADER]);

	Bind(&shaders[CIRCLE_SHADER]);
	setUniformMatrix4fv(&shaders[CIRCLE_SHADER], "projectionMatrix", 1, GL_FALSE, projectionMatrix);
	setUniform1i(&shaders[QUAD_SHADER], "shouldLightBeRestricted", 0);
	Unbind(&shaders[CIRCLE_SHADER]);

	Bind(&shaders[QUAD_SHADER]);
	setUniform1i(&shaders[QUAD_SHADER], "colorTexture", 0);
	setUniform1i(&shaders[QUAD_SHADER], "shadowTexture", 1);
	setUniform1i(&shaders[QUAD_SHADER], "stencilTexture1", 2);
	setUniform1i(&shaders[QUAD_SHADER], "stencilTexture2", 3);
	Unbind(&shaders[QUAD_SHADER]);

	InitializeOffScreenBuffer(width, height);
	InitializeNDCQuad();
	InitializeStencilTexture();

}

static void 
Initialize(GLRenderer* renderer, VertexBuffer* vertexBuffer, unsigned int maxVertices, const mat4x4& projectionMatrix, float width, float height, bool isTriangleBuffer = false)
{

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "Initialize Begin:" << err << std::endl;
	}


	renderer->projectionMatrix = projectionMatrix;
	InitializeTransformationStack(renderer);

	renderer->vertexBuffer = vertexBuffer;


	unsigned int renderer_max_sprites = maxVertices;
	unsigned int renderer_vertex_size = sizeof(VertexData);

	unsigned int vertex_multiplier = 4;
	if (isTriangleBuffer)
		vertex_multiplier = 3;

	unsigned int renderer_sprite_size = (renderer_vertex_size * vertex_multiplier);
	unsigned int renderer_buffer_size = (renderer_sprite_size * renderer_max_sprites);

	unsigned int indices_multiplier = 6;
	if (isTriangleBuffer)
		indices_multiplier = 3;

	unsigned int renderer_indicies_size = (renderer_max_sprites * indices_multiplier);


	glGenVertexArrays(1, &vertexBuffer->VAO);
	glGenBuffers(1, &vertexBuffer->VBO);

	glBindVertexArray(vertexBuffer->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->VBO);
	glBufferData(GL_ARRAY_BUFFER, renderer_buffer_size, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(SHADER_VERTEX_INDEX);
	glEnableVertexAttribArray(SHADER_UV_INDEX);
	glEnableVertexAttribArray(SHADER_TID_INDEX);
	glEnableVertexAttribArray(SHADER_COLOR_INDEX);
	glVertexAttribPointer(SHADER_VERTEX_INDEX, 4, GL_FLOAT, GL_FALSE, renderer_vertex_size, (const GLvoid*)(offsetof(VertexData, VertexData::position)));
	//glVertexAttribPointer(SHADER_COLOR_INDEX, 4, GL_FLOAT, GL_FALSE, RENDERER_VERTEX_SIZE, (const GLvoid*)(4*sizeof(GLfloat)));
	glVertexAttribPointer(SHADER_UV_INDEX, 2, GL_FLOAT, GL_FALSE, renderer_vertex_size, (const GLvoid*)(offsetof(VertexData, VertexData::texture)));
	glVertexAttribPointer(SHADER_TID_INDEX, 1, GL_FLOAT, GL_FALSE, renderer_vertex_size, (const GLvoid*)(offsetof(VertexData, VertexData::textureID)));
	glVertexAttribPointer(SHADER_COLOR_INDEX, 4, GL_UNSIGNED_BYTE, GL_TRUE, renderer_vertex_size, (const GLvoid*)(offsetof(VertexData, VertexData::color)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//GLushort indices[RENDERER_INDICES_SIZE];
	GLuint* indices = new GLuint[renderer_indicies_size];
	int offset = 0;
	for (int i = 0; i < renderer_indicies_size; i += indices_multiplier)
	{
		/*indices[i] = offset + 0;  //old way, wrong culling 
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0; */

		if (!isTriangleBuffer)
		{
			indices[i] = offset + 2;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 0;


			indices[i + 3] = offset + 0;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 2;
		}
		else 
		{
			/*indices[i] = i;
			indices[i + 1] = i + 1;
			indices[i + 2] = i + 2;*/
			indices[i] = offset + 2;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 0;
		}


		if (isTriangleBuffer)
			offset += 3;
		else
			offset += 4;
	}

	Initialize(&vertexBuffer->IBO, indices, renderer_indicies_size);

	glBindVertexArray(0);

	delete[] indices;

	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "Initialize End:" << err << std::endl;
	}

}

static void
Begin(GLRenderer* renderer)
{
	activeRenderer = renderer;
	glBindBuffer(GL_ARRAY_BUFFER, activeRenderer->vertexBuffer->VBO);
	activeRenderer->vertexBuffer->buffer = (VertexData*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

static void
End()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
Flush()
{
	for (unsigned int i = 0; i < activeRenderer->textureSlots.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, activeRenderer->textureSlots[i]);
	}
	glBindVertexArray(activeRenderer->vertexBuffer->VAO);
	Bind(&activeRenderer->vertexBuffer->IBO);

	glDrawElements(GL_TRIANGLES, activeRenderer->vertexBuffer->indexCount, GL_UNSIGNED_INT, nullptr);
	Unbind(&activeRenderer->vertexBuffer->IBO);
	glBindVertexArray(0);

	activeRenderer->vertexBuffer->indexCount = 0;
	activeRenderer->textureSlots.clear();
	//glBindTexture(GL_TEXTURE_2D, 0);

}

static void
DrawLine(vec2<float> start, vec2<float> end, float cos, float sin, unsigned int color, float lineSize)
{

	//Ray origin is its midpoint
	vec2<float> midpoint = (end + start) * 0.5f;
	float length = Len(end - start);
	vec2<float> size = V2(lineSize / 2.0f, length / 2.0f);

	vec2<float> normalized = Normalize(end - start);
	vec2<float> up = V2(0.0f, 1.0f);


	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	vec2<float> translation = transform.position;
	const float& depth = transform.depth;
	vec2<float> rotation = V2(cos, sin);

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	vec2<float> rotated;

	vec3<float> convertedPosition = V3((-size.x), (-size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((-size.x), (size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((size.x), (size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((size.x), (-size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	activeRenderer->vertexBuffer->indexCount += 6;
}

static void
DrawLine(vec2<float> start, vec2<float> end, unsigned int color, float lineSize)
{

	//Ray origin is its midpoint
	vec2<float> midpoint = (end + start) * 0.5f;
	float length = Len(end - start);
	vec2<float> size = V2(lineSize / 2.0f, length / 2.0f);

	vec2<float> normalized = Normalize(end - start);
	vec2<float> up = V2(0.0f, 1.0f);

	float cos = Dot(normalized, up);
	//float sin = SqRoot(Square(Cross(normalized, up)));
	float sin = -((normalized.x * up.y) - (normalized.y * up.x));
	//float sin = (normalized.y * up.x) - (normalized.x * up.y);
	//det = x1*y2 - y1*x2
	//float sin = SqRoot(1 - Square(cos));
	//float sin = Cross(normalized, up);

	//normalized.x < 0 ? cos = -cos : cos;
	//normalized.y < 0 ? sin = -sin : sin;

	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	vec2<float> translation = transform.position;
	const float& depth = transform.depth;
	vec2<float> rotation = V2(cos, sin);

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	vec2<float> rotated;

	vec3<float> convertedPosition = V3((-size.x), (-size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((-size.x), (size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((size.x), (size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((size.x), (-size.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + midpoint.x;
	convertedPosition.y = rotated.y + translation.y + midpoint.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	activeRenderer->vertexBuffer->indexCount += 6;
}

static void
DrawDoor(Door* door, bool collided)
{
	
	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	vec2<float> translation = transform.position;
	const float& depth = transform.depth;

	unsigned int color = collided ? GetColor(1.0f, 0.0f, 0.0f, 1.0f) : GetColor(0.0f, 0.0f, 1.0f, 1.0f);

	vec3<float> convertedPosition = V3(door->vertices[0].x, door->vertices[0].y, depth);
	convertedPosition.x = convertedPosition.x + translation.x;
	convertedPosition.y = convertedPosition.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(door->vertices[1].x, door->vertices[1].y, depth);
	convertedPosition.x = convertedPosition.x + translation.x;
	convertedPosition.y = convertedPosition.y + translation.y; activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(door->vertices[2].x, door->vertices[2].y, depth);
	convertedPosition.x = convertedPosition.x + translation.x;
	convertedPosition.y = convertedPosition.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(door->vertices[3].x, door->vertices[3].y, depth);
	convertedPosition.x = convertedPosition.x + translation.x;
	convertedPosition.y = convertedPosition.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	activeRenderer->vertexBuffer->indexCount += 6;
}


static void
DrawCircle(vec2<float> position, float radius, unsigned int color, unsigned int numberOfSegments)
{
	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	const vec2<float>& translation = transform.position;
	const vec2<float>& rotation = transform.rotation;
	const float& depth = transform.depth;


	vec3<float> midPosition = V3(position.x + translation.x, position.y + translation.y, depth);

	for (unsigned int i = 0; i < numberOfSegments; i += 1 )
	{
		float theta1 = 2.0f * 3.1415926f * float(i) / float(numberOfSegments);//get the current angle 
		float theta2 = 2.0f * 3.1415926f * float(i + 1) / float(numberOfSegments);//get the current angle 

		activeRenderer->vertexBuffer->buffer->position = midPosition;// Bottom left.
		activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
		activeRenderer->vertexBuffer->buffer->color = color;
		activeRenderer->vertexBuffer->buffer->textureID = -1;
		activeRenderer->vertexBuffer->buffer++;

		activeRenderer->vertexBuffer->buffer->position = V3(radius * cosf(theta2) + midPosition.x, radius * sinf(theta2) + midPosition.y, depth);
		activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
		activeRenderer->vertexBuffer->buffer->color = color;
		activeRenderer->vertexBuffer->buffer->textureID = -1;
		activeRenderer->vertexBuffer->buffer++;

		activeRenderer->vertexBuffer->buffer->position = V3(radius * cosf(theta1) + midPosition.x, radius * sinf(theta1) + midPosition.y, depth);  // Top right.
		activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
		activeRenderer->vertexBuffer->buffer->color = color;
		activeRenderer->vertexBuffer->buffer->textureID = -1;
		activeRenderer->vertexBuffer->buffer++;

		activeRenderer->vertexBuffer->indexCount += 3;
	}

}

static void
DrawTriangle(vec2<float> left, vec2<float> right, vec2<float> top, unsigned int color)
{


	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	const vec2<float>& translation = transform.position;
	const vec2<float>& rotation = transform.rotation;
	const float& depth = transform.depth;

	vec2<float> position = (left + right + top) / 3.0f;

	float halfX = fabs(left.x - right.x);
	float halfY = fabs(top.y - right.y);

	/*vec2<float> left = V2(-halfX, -halfY);
	vec2<float> right = V2(halfX, -halfY);
	vec2<float> top = V2(0.0f, +halfY);*/

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	vec2<float> rotated;

	vec3<float> convertedPosition = V3(left.x, left.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x;
	convertedPosition.y = rotated.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(top.x, top.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x;
	convertedPosition.y = rotated.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(right.x, right.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x;
	convertedPosition.y = rotated.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	//Fakeout to not disturb the index buffer
	/*convertedPosition = V3(left.x, left.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x;
	convertedPosition.y = rotated.y + translation.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;*/

	activeRenderer->vertexBuffer->indexCount += 3;
}
/*
static void
DrawTriangle(vec2<float> position, vec2<float> size, unsigned int color)
{


	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	const vec2<float>& translation = transform.position;
	const vec2<float>& rotation = transform.rotation;
	const float& depth = transform.depth;

	
	float halfX = size.x * 0.5f;
	float halfY = size.y * 0.5f;

	vec2<float> left = V2(-halfX, -halfY);
	vec2<float> right = V2(halfX, -halfY);
	vec2<float> top = V2(0.0f, +halfY);

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	vec2<float> rotated;

	vec3<float> convertedPosition = V3(left.x, left.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(top.x, top.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3(right.x, right.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	//Fakeout to not disturb the index buffer
	convertedPosition = V3(left.x, left.y, depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
	activeRenderer->vertexBuffer->buffer->texture = V2(0.0f, 0.0f);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = -1;
	activeRenderer->vertexBuffer->buffer++;

	activeRenderer->vertexBuffer->indexCount += 6;
}
*/

static void 
DrawQuad(vec2<float> position, vec2<float> size, unsigned int color, int textureId, vec2<float> uvBotLeft, vec2<float> uvSize)
{
	const int tid = textureId;

	float ts = 0.0f;
	if (tid > 0)
	{
		bool found = false;
		for (int i = 0; i < activeRenderer->textureSlots.size(); i++)
		{
			if (activeRenderer->textureSlots[i] == tid)
			{
				ts = (float)(i + 1);
				found = true;
				break;
			}
		}

		if (!found)
		{
			//This needs to be done if we run out of texture slots
			if (activeRenderer->textureSlots.size() >= RENDERER_MAX_TEXTURES)
			{
				End();
				Flush();
				Begin(activeRenderer);
				//myTextureSlots.clear();
			}
			activeRenderer->textureSlots.push_back(tid);
			ts = (float)(activeRenderer->textureSlots.size());
		}
	}

	vec2<float> halfSize = V2(size.x * 0.5f, size.y * 0.5f);
	vec2<float> uv1 = V2(uvBotLeft.x, uvBotLeft.y + uvSize.y);
	vec2<float> uv2 = uvBotLeft;
	vec2<float> uv3 = V2(uvBotLeft.x + uvSize.x, uvBotLeft.y);
	vec2<float> uv4 = V2(uvBotLeft.x + uvSize.x, uvBotLeft.y + uvSize.y);

	Transform transform = *activeRenderer->transformationStack.transformationStackBack;
	const vec2<float>& translation = transform.position;
	const vec2<float>& rotation = transform.rotation;
	const float& depth = transform.depth;

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	//x = x * rotation.cos + y * (-1.0f*rotation.sin);
	//y = y * rotation.sin + y * rotation.cos;

	vec2<float> rotated;

	vec3<float> convertedPosition = V3((-halfSize.x), (-halfSize.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
	activeRenderer->vertexBuffer->buffer->texture = V2(uv1.x, uv1.y);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = ts;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((-halfSize.x), (halfSize.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
	activeRenderer->vertexBuffer->buffer->texture = V2(uv2.x, uv2.y);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = ts;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((halfSize.x), (halfSize.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
	activeRenderer->vertexBuffer->buffer->texture = V2(uv3.x, uv3.y);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = ts;
	activeRenderer->vertexBuffer->buffer++;

	convertedPosition = V3((halfSize.x), (-halfSize.y), depth);
	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
	convertedPosition.x = rotated.x + translation.x + position.x;
	convertedPosition.y = rotated.y + translation.y + position.y;
	activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
	activeRenderer->vertexBuffer->buffer->texture = V2(uv4.x, uv4.y);
	activeRenderer->vertexBuffer->buffer->color = color;
	activeRenderer->vertexBuffer->buffer->textureID = ts;
	activeRenderer->vertexBuffer->buffer++;

	activeRenderer->vertexBuffer->indexCount += 6;
}

static void GetBakedQuad(const stbtt_bakedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, int opengl_fillrule)
{
#define STBTT_ifloor(x)   ((int) floor(x))
#define STBTT_iceil(x)    ((int) ceil(x))
	float d3d_bias = opengl_fillrule ? 0 : -0.5f;
	float ipw = 1.0f / pw, iph = 1.0f / ph;
	const stbtt_bakedchar *b = chardata + char_index;
	int round_x = STBTT_ifloor((*xpos + b->xoff) + 0.5f);
	int round_y = STBTT_ifloor((*ypos + b->yoff) + 0.5f);

	q->x0 = round_x + d3d_bias;
	q->y0 = round_y + d3d_bias;
	q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
	q->y1 = round_y + b->y1 - b->y0 + d3d_bias;

	q->s0 = b->x0 * ipw;
	q->t0 = b->y0 * iph;
	q->s1 = b->x1 * ipw;
	q->t1 = b->y1 * iph;

	*xpos += b->xadvance;
	/*float d3d_bias = opengl_fillrule ? 0 : -0.5f;
	float ipw = 1.0f / pw, iph = 1.0f / ph;
	const stbtt_bakedchar *b = chardata + char_index;
	int round_x = STBTT_ifloor((*xpos + b->xoff) + 0.5f);
	int round_y = STBTT_ifloor((*ypos + b->yoff) + 0.5f);

	q->x0 = round_x + d3d_bias;
	q->y0 = round_y + d3d_bias;
	q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
	q->y1 = round_y + b->y1 - b->y0 + d3d_bias;

	q->s0 = b->x0 * ipw;
	q->t0 = b->y0 * iph;
	q->s1 = b->x1 * ipw;
	q->t1 = b->y1 * iph;

	*xpos += b->xadvance;*/
}

static void 
DrawString(const std::string& text, const vec4<float>& position, const Font& font, unsigned int color)
{

	/*int r = color.x * 255.0f;
	int g = color.y * 255.0f;
	int b = color.z * 255.0f;
	int a = color.w * 255.0f;

	unsigned int col = a << 24 | b << 16 | g << 8 | r;*/

	float ts = 0.0f;

	bool found = false;
	for (int i = 0; i < activeRenderer->textureSlots.size(); i++)
	{
		if (activeRenderer->textureSlots[i] == font.textureID)
		{
			ts = (float)(i + 1);
			found = true;
			break;
		}
	}

	if (!found)
	{
		//This needs to be done if we run out of texture slots
		if (activeRenderer->textureSlots.size() >= RENDERER_MAX_TEXTURES)
		{
			End();
			Flush();
			Begin(activeRenderer);
			//myTextureSlots.clear();
		}
		activeRenderer->textureSlots.push_back(font.textureID);
		ts = (float)(activeRenderer->textureSlots.size());
	}

	float originalX = position.x;
	float x = position.x;
	float y = position.y;

	char* charText = (char*)text.c_str();

	while (*charText) {
		if (*charText >= 32 && *charText < 128) {
			stbtt_aligned_quad q;
			GetBakedQuad(font.cdata, 1024, 1024, *charText - 32, &x, &y, &q, 1);//1=opengl & d3d10+,0=d3d9
			/*glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y0);
			glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y0);
			glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y1);
			glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y1);*/

			Transform transform = *activeRenderer->transformationStack.transformationStackBack;
			const vec2<float>& translation = transform.position;
			const vec2<float>& rotation = transform.rotation;
			const float& depth = transform.depth;

			//x = x * rotation.cos + y * (-1.0f*rotation.sin);
			//y = y * rotation.sin + y * rotation.cos;

			//x = x * rotation.cos + y * (-1.0f*rotation.sin);
			//y = y * rotation.sin + y * rotation.cos;

			vec3<float> rotated;
			 
			vec2<float> midPoint = V2((q.x0 + q.x1) / 2.0f, (q.y0 + q.y1) / 2.0f);

			vec2<float> halfSize = V2(abs((q.x0 - q.x1) * 0.5f), abs((q.y0 - q.y1) * 0.5f));

			vec3<float> convertedPosition = V3(q.x0, q.y0, position.z);
			//vec3<float> convertedPosition = V3(q.x0, q.y1, position.z);
			//vec3<float> convertedPosition = V3((-halfSize.x), (-halfSize.y), position.z);

			//myBuffer->vertex = *myTransformationStackBack  *glm::vec4(q.x0, q.y0, position.z, 1);
			rotated.x = convertedPosition.x;// *rotation.cos - convertedPosition.y * rotation.sin;
			rotated.y = convertedPosition.y * rotation.cos - convertedPosition.z * rotation.sin;//convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
			rotated.z = convertedPosition.y * rotation.sin + convertedPosition.z * rotation.cos;
			convertedPosition.x = rotated.x + translation.x + position.x;
			convertedPosition.y = rotated.y + translation.y + position.y;
			//convertedPosition.z = rotated.z + position.z;
			activeRenderer->vertexBuffer->buffer->position = convertedPosition;// Bottom left.
			activeRenderer->vertexBuffer->buffer->texture = V2(q.s0, q.t0);
			//renderer->buffer->texture = V2(q.s0, q.t1); //flipped, for all
			activeRenderer->vertexBuffer->buffer->textureID = ts;
			activeRenderer->vertexBuffer->buffer->color = color;
			activeRenderer->vertexBuffer->buffer++;

			//renderer->buffer->vertex = *myTransformationStackBack  *glm::vec4(q.x0, q.y1, position.z, 1);
			convertedPosition = V3(q.x0, q.y1, position.z);
			//convertedPosition = V3(q.x0, q.y0, position.z);
			//convertedPosition = V3((-halfSize.x), (halfSize.y), position.z);
			rotated.x = convertedPosition.x;// *rotation.cos - convertedPosition.y * rotation.sin;
			rotated.y = convertedPosition.y * rotation.cos - convertedPosition.z * rotation.sin;//convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
			rotated.z = convertedPosition.y * rotation.sin + convertedPosition.z * rotation.cos;
			convertedPosition.x = rotated.x + translation.x + position.x;
			convertedPosition.y = rotated.y + translation.y + position.y;
			//convertedPosition.z = rotated.z + position.z;
			activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top Left.
			activeRenderer->vertexBuffer->buffer->texture = V2(q.s0, q.t1);
			//renderer->buffer->texture = V2(q.s0, q.t0);
			activeRenderer->vertexBuffer->buffer->textureID = ts;
			activeRenderer->vertexBuffer->buffer->color = color;
			activeRenderer->vertexBuffer->buffer++;

			//renderer->buffer->vertex = *myTransformationStackBack  *glm::vec4(q.x1, q.y1, position.z, 1);
			convertedPosition = V3(q.x1, q.y1, position.z);
			//convertedPosition = V3(q.x1, q.y0, position.z);
			//convertedPosition = V3((halfSize.x), (halfSize.y), position.z);
			rotated.x = convertedPosition.x;// *rotation.cos - convertedPosition.y * rotation.sin;
			rotated.y = convertedPosition.y * rotation.cos - convertedPosition.z * rotation.sin;//convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
			rotated.z = convertedPosition.y * rotation.sin + convertedPosition.z * rotation.cos;
			convertedPosition.x = rotated.x + translation.x + position.x;
			convertedPosition.y = rotated.y + translation.y + position.y;
			//convertedPosition.z = rotated.z + position.z;
			activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Top right.
			activeRenderer->vertexBuffer->buffer->texture = V2(q.s1, q.t1);
			//renderer->buffer->texture = V2(q.s1, q.t0);
			activeRenderer->vertexBuffer->buffer->textureID = ts;
			activeRenderer->vertexBuffer->buffer->color = color;
			activeRenderer->vertexBuffer->buffer++;

			//renderer->buffer->vertex = *myTransformationStackBack  *glm::vec4(q.x1, q.y0, position.z, 1);
			convertedPosition = V3(q.x1, q.y0, position.z);
			//convertedPosition = V3(q.x1, q.y1, position.z);
			//convertedPosition = V3((halfSize.x), (-halfSize.y), position.z);
			rotated.x = convertedPosition.x;// *rotation.cos - convertedPosition.y * rotation.sin;
			rotated.y = convertedPosition.y * rotation.cos - convertedPosition.z * rotation.sin;//convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;
			rotated.z = convertedPosition.y * rotation.sin + convertedPosition.z * rotation.cos;
			convertedPosition.x = rotated.x + translation.x + position.x;
			convertedPosition.y = rotated.y + translation.y + position.y;
			//convertedPosition.z = rotated.z + position.z;
			activeRenderer->vertexBuffer->buffer->position = convertedPosition;  // Bottom right.
			activeRenderer->vertexBuffer->buffer->texture = V2(q.s1, q.t0);
			//renderer->buffer->texture = V2(q.s1, q.t1);
			activeRenderer->vertexBuffer->buffer->textureID = ts;
			activeRenderer->vertexBuffer->buffer->color = color;
			activeRenderer->vertexBuffer->buffer++;

			activeRenderer->vertexBuffer->indexCount += 6;


			//	y += 10.0f;
		}
		else
		{
			//y += 128.0f;
			y += font.size;
			x = originalX;
		}
		++charText;
	}


}

static void
DrawTilemap(Tilemap* tilemap, vec2<float> tileSize, int textureId)
{
	float startX = 0.0f;
	float startY = 0.0f;
	vec2<float> size = V2(31.0f, 31.0f);
	for (unsigned int i = 0; i < tilemap->width*tilemap->height; i++)
	{
		if (i > 0 && i % tilemap->width == 0)
		{
			startX = 0.0f;
			startY -= 32.0f;
		}
		vec2<float> pos; pos.x = startX; pos.y = startY;
		vec2<float> size; size.x = 30.0f; size.y = 30.0f;
		vec4<float> col;

		if (TestBit(tilemap->tilemap, i))
		{
			col.x = 0.0f; col.y = 1.0f; col.z = 0.0f, col.w = 1.0f;
		}
		else
		{
			col.x = 1.0f; col.y = 0.0f; col.z = 0.0f, col.w = 1.0f;
		}
		(TestBit(tilemap->tilemap, i)) ? col = V4(0.0f, 1.0f, 0.0f, 1.0f) : col = V4(0.0f, 0.0f, 1.0f, 1.0f);
		unsigned int color = GetColor(col);

		DrawQuad(V2(startX, startY), size, color, textureId, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		startX += 32.0f;
	}
}

static void
DrawCells(Tilemap* tilemap, TileCell* cells, vec2<float> tileSize, int textureId)
{
	float startX = 0.0f;
	float startY = 0.0f;
	vec2<float> size = V2(31.0f, 31.0f);
	for (unsigned int i = 0; i < tilemap->width*tilemap->height; i++)
	{
		if (i > 0 && i % tilemap->width == 0)
		{
			startX = 0.0f;
			startY -= 32.0f;
		}
		vec2<float> pos; pos.x = startX; pos.y = startY;
		vec2<float> size; size.x = 30.0f; size.y = 30.0f;
		vec4<float> col;


		//if (cells[i].exist) {
		if (TestBit(&cells[i].edge_exist_cell_exist, CELL_EXIST)) {
			col.x = 1.0f; col.y = 1.0f; col.z = 1.0f, col.w = 1.0f;
		}
		else {
			col.x = 0.1f; col.y = 0.1f; col.z = 0.1f, col.w = 1.0f;
		}
		DrawQuad(V2(startX, startY), size, GetColor(col), textureId, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		startX += 32.0f;

		/*if (TestBit(tilemap->tilemap, i))
		{
			col.x = 0.0f; col.y = 1.0f; col.z = 0.0f, col.w = 1.0f;
		}
		else
		{
			col.x = 1.0f; col.y = 0.0f; col.z = 0.0f, col.w = 1.0f;
		}
		(TestBit(tilemap->tilemap, i)) ? col = V4(0.0f, 1.0f, 0.0f, 1.0f) : col = V4(0.0f, 0.0f, 1.0f, 1.0f);
		unsigned int color = GetColor(col);*/

		//DrawQuad(renderer, V2(startX, startY), size, color, textureId, V2(0.0f, 0.0f), V2(1.0f, 1.0f));
		//startX += 32.0f;
	}
}

static void
DrawDjikstraGrid(DjikstraGrid* djikstraGrid)
{
	float startX = 0.0f;
	float startY = 0.0f;
	vec2<float> size = V2(31.0f, 31.0f);
	for (unsigned int i = 0; i < djikstraGrid->width*djikstraGrid->height; i++)
	{
		if (i > 0 && i % djikstraGrid->width == 0)
		{
			startX = 0.0f;
			startY -= 32.0f;
		}
		vec2<float> pos; pos.x = startX; pos.y = startY;
		vec2<float> size; size.x = 30.0f; size.y = 30.0f;
		vec4<float> col = V4(1.0f, 1.0f, 1.0f, 1.0f);


		
		DrawQuad(V2(startX, startY), size, GetColor(col), -1, V2(0.0f, 0.0f), V2(1.0f, 1.0f));

		if (djikstraGrid->djikstraGrid[i] == INT_MAX) 
		{

		}
		else 
		{
			PushTransform(V2(startX - 5.0f, startY - 3.0f), -1.0f, V2(CosRadians(DegreesToRadians(180)), SinRadians(DegreesToRadians(180))));
			std::string val = std::to_string(djikstraGrid->djikstraGrid[i]);
			DrawString(val, V4(0.0f, 0.0f, 0.0f, 1.0f), *GetFont(ADVENTURER_14), GetColor(0.0f, 0.0f, 0.0f, 1.0f));
			PopTransform();
		}
		
	
		startX += 32.0f;
	}
}


static void
DrawFlowField(DjikstraGrid* djikstraGrid)
{
	float startX = 0.0f;
	float startY = 0.0f;
	vec2<float> size = V2(31.0f, 31.0f);
	for (unsigned int i = 0; i < djikstraGrid->width*djikstraGrid->height; i++)
	{
		if (i > 0 && i % djikstraGrid->width == 0)
		{
			startX = 0.0f;
			startY -= 32.0f;
		}
		vec2<float> pos; pos.x = startX; pos.y = startY;
		vec2<float> size; size.x = 30.0f; size.y = 30.0f;
		vec4<float> col = V4(1.0f, 1.0f, 0.0f, 1.0f);


		vec2<float> end = pos + Normalize(djikstraGrid->flowField[i]) * 28.0f;
		PushTransform(V2(0.0f, 0.0f), 0.0f, V2(CosRadians(DegreesToRadians(0)), SinRadians(DegreesToRadians(0))));
		DrawLine(pos, end, GetColor(0.0f, 0.0f, 1.0f, 1.0f), 3.0f);
		PopTransform();

		startX += 32.0f;
	}
}

static void 
ResizeGL(int width, int height) 
{
	//TODO: IMPLEMENT - keep aspect ratio you noob :)
}


