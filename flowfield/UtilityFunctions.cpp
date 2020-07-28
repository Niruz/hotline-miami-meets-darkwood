

namespace EngineUtilities
{
	float RandomFloat(float a, float b)
	{
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = b - a;
		float r = random * diff;
		return a + r;
	}

	/*
	std::string split implementation by using delimiter as a character.
	*/
	std::vector<std::string> Split(std::string strToSplit, char delimeter)
	{
		std::stringstream ss(strToSplit);
		std::string item;
		std::vector<std::string> splittedStrings;
		while (std::getline(ss, item, delimeter))
		{
			splittedStrings.push_back(item);
		}
		return splittedStrings;
	}
	/*
	std::string split implementation by using delimeter as an another string
	*/
	std::vector<std::string> Split(std::string stringToBeSplitted, std::string delimeter)
	{
		std::vector<std::string> splittedString;
		int startIndex = 0;
		int  endIndex = 0;
		while ((endIndex = stringToBeSplitted.find(delimeter, startIndex)) < stringToBeSplitted.size())
		{
			std::string val = stringToBeSplitted.substr(startIndex, endIndex - startIndex);
			splittedString.push_back(val);
			startIndex = endIndex + delimeter.size();
		}
		if (startIndex < stringToBeSplitted.size())
		{
			std::string val = stringToBeSplitted.substr(startIndex);
			splittedString.push_back(val);
		}
		return splittedString;
	}
}


static void
SetBit(int *A, int k)
{
	int i = k / 32;
	int pos = k % 32;

	A[k / 32] |= 1 << (k % 32);  // Set the bit at the k-th position in A[i]
}

static void
ClearBit(int* A, int k)
{
	int i = k / 32;
	int pos = k % 32;

	A[k / 32] &= ~(1 << (k % 32));
}

static bool
TestBit(int* A, int k)
{
	if ((A[k / 32] & (1 << (k % 32))) != 0)
		return true;
	else
		return false;
}

static int
GetIndex(const vec2<int>& vec, int width)
{
	return (vec.y * width) + vec.x;
}

static int
GetIndex(int x, int y, int width)
{
	return (y * width) + x;
}

static vec2<int>
GetIndex(int index, int width)
{
	vec2<int> vec;
	//vec.x = index / width;
	//vec.y = index % width;
	vec.x = index % width;
	vec.y = index / width;
	return vec;
}

static int
MAX(int a, int b)
{
	return b < a ? a : b;
}

static bool
ValidIndex(int x, int y, int width, int height)
{
	return (x >= 0 && x < (int)width && y >= 0 && y < (int)height);
}

static bool
ValidIndex(const vec2<int>& vec, int width, int height)
{
	return (vec.x >= 0 && vec.x < (int)width && vec.y >= 0 && vec.y < (int)height);
}

static unsigned int
ABS(int value)
{
	return value < 0 ? 0 - value : value;
}

static float
map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + (s - a1)*(b2 - b1) / (a2 - a1);
}


static float Floor(float val)
{
	if (val >= 0.0f)
	{
		return (float)((int)val);
	}
	else
	{
		return (float)((int)val - 1);
	}

}

static float Round(float val)
{
	return Floor(val + 0.5f);
}

unsigned char* str_reverse_in_place(unsigned char *str, int len)
{
	unsigned char *p1 = str;
	unsigned char *p2 = str + len - 1;

	while (p1 < p2) {
		unsigned char tmp = *p1;
		*p1++ = *p2;
		*p2-- = tmp;
	}

	return str;
}

static void
flipImage(unsigned char* data, int size, int width)
{
/*	for (int i = 0; i < size; i++) 
	{

	}*/
}

#pragma pack(push, 1)
struct bitmap_header
{
	unsigned short FileType;
	unsigned int FileSize;
	unsigned short Reserved1;
	unsigned short Reserved2;
	unsigned int BitmapOffset;
	unsigned int Size;
	int Width;
	int Height;
	unsigned short Planes;
	unsigned short BitsPerPixel;
};
#pragma pack(pop)

#pragma warning( push )
#pragma warning( disable : 4996)
static unsigned char*
LoadBMP(const char* filename, int& _width, int& _height)
{
	//int i;
	FILE* f = fopen(filename, "rb");

	if (f == NULL)
		std::cout << "File: " << filename << " not found." << std::endl;

	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	//extract the image height and width from header
	bitmap_header *Header = (bitmap_header*)info; //get the header

	std::cout << "Name: " << filename << std::endl;
	std::cout << "Width: " << Header->Width << std::endl;
	std::cout << "Height: " << Header->Height << std::endl;
	std::cout << "Planes: " << Header->Planes << std::endl;
	std::cout << "Bits: " << Header->BitsPerPixel << std::endl;

	int width = Header->Width;
	int height = Header->Height;

	_width = width;
	_height = height;

	unsigned int size = 4 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 4 bytes per pixel
	fseek(f, Header->BitmapOffset, SEEK_SET);
	fread(data, sizeof(unsigned char), size, f);


	fclose(f);

	//return str_reverse_in_place(data, size);
	flipImage(data, size, width);
	return data;
}
#pragma warning( pop ) 