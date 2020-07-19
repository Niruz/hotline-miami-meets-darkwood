

enum TEXTURES
{
	TEST1,
	SHADOWMAP,
	MAX_TEXTURES
};

struct Texture
{
	std::string path;
	GLuint ID;
	GLuint width, height;
	GLuint internalFormat, imageFormat;
	GLuint wrap_S, wrap_T;
	GLuint filterMin, filterMax;
};

Texture textures[MAX_TEXTURES];


static void
SetInternalFormat(Texture* texture, GLuint internalformat) 
{
	texture->internalFormat = internalformat;
};
static void
SetImageFormat(Texture* texture, GLuint imageformat) 
{
	texture->imageFormat = imageformat;
};
static void
SetWrapperFormat(Texture* texture, GLuint s, GLuint t)
{
	texture->wrap_S = s;
	texture->wrap_T = t;
}

static void 
SetFilterFormat(Texture* texture, GLuint min, GLuint max)
{
	texture->filterMin = min;
	texture->filterMax = max;
}

static void
Initialize(Texture* texture, GLuint internalFormat, GLuint imageFormat, GLuint wrapS, GLuint wrapT, GLuint filtermin, GLuint filtermax)
{
	glGenTextures(1, &texture->ID);

	texture->width = 0;
	texture->height = 0;
	texture->internalFormat = internalFormat;
	texture->imageFormat = imageFormat;
	texture->wrap_S = wrapS;
	texture->wrap_T = wrapT;
	texture->filterMin = filtermin;
	texture->filterMax = filtermax;

}

static void 
DeInitialize(Texture* texture) 
{
	glDeleteTextures(1, &texture->ID);
}

//TODO: should the glActiveTexture(GL_TEXTURE0) call be done from here?
//Or whoever is calling should probably know which one to bind
static void
Bind(Texture* texture)
{
	glBindTexture(GL_TEXTURE_2D, texture->ID);
}

static void
Unbind(Texture* texture)
{
	glBindTexture(GL_TEXTURE_2D, 0);
}


static void
Generate(Texture* texture, GLuint width, GLuint height, unsigned char* data)
{
	Bind(texture);
	texture->width = width;
	texture->height = height;

	glTexImage2D(GL_TEXTURE_2D, 0, texture->internalFormat, texture->width, texture->height, 0, texture->imageFormat, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap_T);
	//note to self, this is just the filtering method when scaling up or down a texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->filterMax);

	Unbind(texture);
}

static bool
LoadTexture(std::string filename, TEXTURES identifier)
{

/*	int width, height;
	unsigned char *data = LoadBMP(filename.c_str(), width, height);
	//unsigned char* data;

	//Initialize(&textures[identifier], GL_RGBA, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	Initialize(&textures[identifier], GL_RGBA, GL_BGRA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);

	Generate(&textures[identifier], width, height, data);

	delete[] data;

	//return success
	return true;*/
	//Either assume every png has alpha or check for the number of channels
	//stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	Initialize(&textures[identifier], GL_RGBA, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	//Initialize(&textures[identifier], GL_RGBA, GL_BGRA, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	Generate(&textures[identifier], width, height, data);
	//textures[identifier].Generate(width, height, data);

	stbi_image_free(data);

	//return success
	return true;
}

static Texture* 
GetTexture(TEXTURES texture)
{
	return &textures[texture];
}