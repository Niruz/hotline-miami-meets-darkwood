

struct Shader
{
	int mVertexShader;
	int mFragmentShader;
	int mGeometryShader;
	int mProgram;

	std::map<std::string, int> mUniforms;
};

enum SHADERS
{
	SIMPLE_SHADER,
	FORWARD_SHADER,
	QUAD_SHADER,
	SHADOW_SHADER,
	CIRCLE_SHADER,
	MAX_SHADERS
};

Shader shaders[MAX_SHADERS];

//Map the name of an uniform to its location.
static void 
SetUniformLocation(Shader* shader, const char* inName, const int inLocation)
{
	shader->mUniforms[inName] = inLocation;
}

static int 
GetUniformLocation(Shader* shader, const char* inName)
{
	if (shader->mUniforms.find(std::string(inName)) != shader->mUniforms.end())
		return shader->mUniforms[inName];
	else
		return -1;
		//assert(false && "ERROR: Failed to load shader uniform location");
		//return -1;
}

static void 
Bind(Shader* shader)
{
	glUseProgram(shader->mProgram);
}

static void 
Unbind(Shader* shader) //I don't know why, consistency I guess?
{
	glUseProgram(0);
}

static inline void 
setUniformMatrix4fv(Shader* shader, const char* inName, int count, bool transpose, const mat4x4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(shader, inName), count, transpose, &matrix.arr[0]);
}
static inline void setUniformMatrix3fv(Shader* shader, const char* inName, int count, bool transpose, const mat4x4& matrix)
{
	glUniformMatrix3fv(GetUniformLocation(shader, inName), count, transpose, &matrix.arr[0]);
}
static inline void setUniform4f(Shader* shader, const char* inName, const vec4<float>& vec)
{
	glUniform4f(GetUniformLocation(shader, inName), vec.x, vec.y, vec.z, vec.w);
}
static inline void setUniform3f(Shader* shader, const char* inName, const vec3<float>& vec)
{
	glUniform3f(GetUniformLocation(shader, inName), vec.x, vec.y, vec.z);
}
static inline void setUniform2fv(Shader* shader, const char* inName, const vec2<float>& vec)
{
	glUniform2fv(GetUniformLocation(shader, inName), 1, &vec.x);
}
static inline void setUniform2f(Shader* shader, const char* inName, const vec2<float>& vec)
{
	int shit = GetUniformLocation(shader, inName);
	glUniform2f(GetUniformLocation(shader, inName), vec.x, vec.y);
}
static inline void setUniform1i(Shader* shader, const char* inName, const int value)
{
	glUniform1i(GetUniformLocation(shader, inName), value);
}
static inline void setUniform1fv(Shader* shader, const char* inName, float value, int count)
{
	glUniform1fv(GetUniformLocation(shader, inName), count, &value);
}
static inline void setUniform1f(Shader* shader, const char* inName, const float& val)
{
	glUniform1f(GetUniformLocation(shader, inName), val);
}
static inline void setUniform1iv(Shader* shader, const char* inName, int* value, int count)
{
	//NOTE, ON SOME DRIVERS YOU NEED TO USE THE NAME AND THE ARRAY SUBSCRIPT [0]
	glUniform1iv(GetUniformLocation(shader, inName), count, value);
}


static bool 
LoadFile(const char* inFileName, std::string& outFileStr)
{
	///open up an in stream
	std::ifstream in(inFileName);
	///If it fails to open, return
	if (!in.is_open())
		return false;

	///Temporary char to hold a line
	char tmp[300];
	while (!in.eof())
	{
		///Get the line
		in.getline(tmp, 300);
		///Read in the the line to the string
		outFileStr += tmp;
		outFileStr += '\n';
	}

	return true;
}

static unsigned int 
LoadShader(const std::string& source, const unsigned int mode)
{
	///id used by opengl to reference the shader object
	unsigned int id;
	///Gl create shader creates a shader object with a certain specified shadertype
	id = glCreateShader(mode);
	const char* csource = source.c_str();
	///glShaderSource sets the source code in shader to the source code in the array of strings specified by string, number of strings is the second parameter (count)
	glShaderSource(id, 1, &csource, NULL);
	///Compiles the shader and used the reference to find it
	glCompileShader(id);



	///Check if there were any errors
	char error[1000];
	glGetShaderInfoLog(id, 1000, NULL, error);
	std::cout << "Compile Status: \n" << error << std::endl;

	return id;
}

static int
InitShader(int inEnumName, const char* inVSname, const char* inFSname, const char* inGEOname = NULL)
{
	std::cout << "Initializing Shader(" << std::to_string(inEnumName) << "):" << std::endl;

	std::string source;
	bool success;
	bool useGEOshader = false;

	//Checks if the current shader has a geometry shader
	if (inGEOname)
		useGEOshader = true;

	///Load the vertex shader
	source = "";
	success = LoadFile(inVSname, source);
	assert(success && "ERROR: Failed to load the vertex shader");
	shaders[inEnumName].mVertexShader = LoadShader(source, GL_VERTEX_SHADER);

	///Load the fragment shader
	source = "";
	success = LoadFile(inFSname, source);
	assert(success && "ERROR: Failed to load the fragment shader");
	shaders[inEnumName].mFragmentShader = LoadShader(source, GL_FRAGMENT_SHADER);

	///Load the geometry shader
	if (useGEOshader)
	{
		source = "";
		success = LoadFile(inGEOname, source);
		assert(success && "ERROR: Failed to load the geometry shader");
		shaders[inEnumName].mGeometryShader = LoadShader(source, GL_GEOMETRY_SHADER);
	}

	///Creates, links and compiles the shader
	shaders[inEnumName].mProgram = glCreateProgram();
	glAttachShader(shaders[inEnumName].mProgram, shaders[inEnumName].mVertexShader);
	glAttachShader(shaders[inEnumName].mProgram, shaders[inEnumName].mFragmentShader);
	if (useGEOshader)
		glAttachShader(shaders[inEnumName].mProgram, shaders[inEnumName].mGeometryShader);
	glLinkProgram(shaders[inEnumName].mProgram);

	///Check if the compilation went without any problems
	GLint compiled;
	glGetProgramiv(shaders[inEnumName].mProgram, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		std::cout << "Shader Compilation Error for enum: " << inEnumName << std::endl;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "shader compiler error lines: " << err << std::endl;
		}

	}
		

	///Check if the linking went without any problems
	GLint linked;
	glGetProgramiv(shaders[inEnumName].mProgram, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		std::cout << "Shader Linkage Error" << std::endl;
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "shader linker error lines: " << err << std::endl;
		}
	}
		

	if (!compiled || !linked)
	{
		char error[256];
		for (int i = 0; i < 256; i++)
			error[i] = NULL;

		//Compilation errors
		glGetProgramInfoLog(shaders[inEnumName].mProgram, 256, NULL, error);
		if (error[0] != 0)
			std::cout << error << std::endl;

		for (int i = 0; i < 256; i++)
			error[i] = NULL;

		//Linkage errors
		glGetProgramInfoLog(shaders[inEnumName].mProgram, 256, NULL, error);
		if (error[0] != 0)
			std::cout << error << std::endl;

		return 0;
	}

	return 1;
}

static void
InitUniforms()
{
	int numberOfUniforms = -1;
	int nameLength = -1, uniformSize = -1;
	GLenum type = 0;
	char name[32];
	int location = -1;

	for (unsigned int shaderIt = 0; shaderIt < MAX_SHADERS; shaderIt++)
	{
		glGetProgramiv(shaders[shaderIt].mProgram, GL_ACTIVE_UNIFORMS, &numberOfUniforms);
		for (int uniformIt = 0; uniformIt < numberOfUniforms; uniformIt++)
		{
			glGetActiveUniform(shaders[shaderIt].mProgram, uniformIt, sizeof(name) - 1, &nameLength, &uniformSize, &type, name);
			location = -1;
			location = glGetUniformLocation(shaders[shaderIt].mProgram, name);

			/*	std::cout << "Uniform Info Shader(" << shaderIt << "): Name: '" << name << "' Location: " << location << " Type: '";
			if (type == GL_FLOAT_MAT4)	std::cout << "mat4";
			else if (type == GL_FLOAT_VEC3) std::cout << "vec3";
			else if (type == GL_FLOAT_VEC4) std::cout << "vec4";
			else if (type == GL_FLOAT)		std::cout << "float";
			else if (type == GL_INT)		std::cout << "int";
			else if (type == GL_BOOL)		std::cout << "bool";
			else if (type == GL_SAMPLER_2D) std::cout << "sampler2d";
			else							std::cout << type;
			std::cout << "'" << std::endl; */


			SetUniformLocation(&shaders[shaderIt], name, location);

		}
		std::cout << std::endl;
	}
}

static void 
DeinitializeShaders()
{
	for (int i = 0; i < MAX_SHADERS; ++i)
	{
		glDetachShader(shaders[i].mProgram, shaders[i].mVertexShader);
		glDetachShader(shaders[i].mProgram, shaders[i].mFragmentShader);
		glDeleteShader(shaders[i].mVertexShader);
		glDeleteShader(shaders[i].mFragmentShader);
		if (shaders[i].mGeometryShader != -1)
			glDeleteShader(shaders[i].mGeometryShader);
		glDeleteProgram(shaders[i].mProgram);
	}
}


