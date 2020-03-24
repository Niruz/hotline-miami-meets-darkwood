

enum FONTS
{
	ADVENTURER_128,
	ADVENTURER_28,
	ADVENTURER_14,
	MAX_FONTS
};


struct Font
{
	stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
	unsigned int textureID;

	int size;
	float width;
	float height;
	std::string filename;
	FONTS name;
};

Font fonts[MAX_FONTS];
/*
static void 
Bind(Font* font)
{
	glBindTexture(GL_TEXTURE_2D, font->textureID);
}

static void
Unbind(Font* font)
{
	glBindTexture(GL_TEXTURE_2D, 0);
}*/

#pragma warning( push )
#pragma warning( disable : 4996)
static Font
FONT(FONTS name, const std::string& filename, float width, float height, float size)
{
	Font font;
	font.name = name;
	font.filename = filename;
	font.width = width;
	font.height = height;
	font.size = size;

	unsigned char* ttf_buffer = new unsigned char[1 << 20];
	unsigned char* temp_bitmap = new unsigned char[1024 * 1024];

	fread(ttf_buffer, 1, 1 << 20, fopen(filename.c_str(), "rb"));
	//	fread(ttf_buffer, 1, 1 << 20, fopen("Fonts/SourceCodePro-Light.ttf", "rb"));

	stbtt_BakeFontBitmap(ttf_buffer, 0, size, temp_bitmap, 1024, 1024, 32, 96, font.cdata); // no guarantee this fits!

																					   // can free ttf_buffer at this point
	glGenTextures(1, &font.textureID);
	glBindTexture(GL_TEXTURE_2D, font.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_bitmap);
	// can free temp_bitmap at this point
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] ttf_buffer;
	delete[] temp_bitmap;

	return font;
}
#pragma warning( pop ) 

static int 
LoadFont(std::string filename, FONTS identifier, int size)
{
	//TODO: actually check where it failed
	fonts[identifier] = FONT(identifier, filename, 1024, 1024, size);
	return 1;
}


static Font* GetFont(FONTS identifier)
{
	return &fonts[identifier];
}
