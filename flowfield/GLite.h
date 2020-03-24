#pragma once

#pragma comment(lib, "opengl32.lib")

#include <windows.h>
#include <gl\GL.h>
#include "wglext.h"

//APIENTRY = WINAPI = _stdcall
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ARRAY_BUFFER                   0x8892
#define GL_BGRA                           0x80E1
#define GL_STATIC_DRAW                    0x88E4
#define GL_WRITE_ONLY                     0x88B9
#define GL_ACTIVE_UNIFORMS                0x8B86
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_FRAMEBUFFER                    0x8D40
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_DEPTH_TEXTURE_MODE             0x884B
#define GL_COMPARE_R_TO_TEXTURE           0x884E
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_TEXTURE_COMPARE_FUNC           0x884D
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_DEPTH_STENCIL_TEXTURE_MODE     0x90EA
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49
#define GL_UNSIGNED_INT_24_8              0x84FA
#define GL_DEPTH_STENCIL                  0x84F9
#define GL_R32UI                          0x8236

#define GL_RENDERBUFFER                   0x8D41
#define GL_RENDERBUFFER_WIDTH             0x8D42
#define GL_RENDERBUFFER_HEIGHT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT   0x8D44

#define GL_RGBA32UI                       0x8D70
#define GL_RGB32UI                        0x8D71
#define GL_RGBA16UI                       0x8D76
#define GL_RGB16UI                        0x8D77
#define GL_RGBA8UI                        0x8D7C
#define GL_RGB8UI                         0x8D7D
#define GL_RGBA32I                        0x8D82
#define GL_RGB32I                         0x8D83
#define GL_RGBA16I                        0x8D88
#define GL_RGB16I                         0x8D89
#define GL_RGBA8I                         0x8D8E
#define GL_RGB8I                          0x8D8F
#define GL_RED_INTEGER                    0x8D94
#define GL_GREEN_INTEGER                  0x8D95
#define GL_BLUE_INTEGER                   0x8D96
#define GL_RGB_INTEGER                    0x8D98
#define GL_RGBA_INTEGER                   0x8D99
#define GL_BGR_INTEGER                    0x8D9A
#define GL_BGRA_INTEGER                   0x8D9B

#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_CLAMP 0x2900
#define GL_REPEAT 0x2901
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#define GL_DEPTH24_STENCIL8               0x88F0
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A
#define GL_CLAMP_TO_EDGE                  0x812F


#define GL_FLOAT 0x1406
#define GL_FALSE 0

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_DEPTH_TEST 0x0B71
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

#define GL_TRIANGLES 0x0004
#define GL_UNSIGNED_INT 0x1405

//https://www.khronos.org/registry/OpenGL/api/GL/glext.h
//extern PFNGLCREATEPROGRAMPROC glCreateProgram;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

typedef GLuint(APIENTRYP createprogram) (void);
typedef void (APIENTRYP deleteprogram) (GLuint program);
typedef void (APIENTRYP useprogram) (GLuint program);
typedef void (APIENTRYP attachshader) (GLuint program, GLuint shader);
typedef void (APIENTRYP detachshader) (GLuint program, GLuint shader);
typedef void (APIENTRYP linkprogram) (GLuint program);
typedef void (APIENTRYP getprogramiv) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRYP getshaderinfolog) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLint(APIENTRYP getuniformlocation) (GLuint program, const GLchar *name);
typedef void (APIENTRYP uniform1i) (GLint location, GLint v0);
typedef void (APIENTRYP uniform1f) (GLint location, GLfloat v0);
typedef void (APIENTRYP uniform4f) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP uniform3f) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP uniform2f) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP uniform1iv) (GLint location, GLsizei count, const GLint *value);
typedef void (APIENTRYP uniform1fv) (GLint location, GLsizei count, const GLfloat *value);


typedef GLint(APIENTRYP getattriblocation) (GLuint program, const GLchar *name);
typedef void (APIENTRYP enablevertexattribarray) (GLuint index);
typedef void (APIENTRYP bindattriblocation) (GLuint program, GLuint index, const GLchar *name);

typedef GLuint(APIENTRYP createshader) (GLenum type);
typedef void (APIENTRYP deleteshader) (GLuint shader);
typedef void (APIENTRYP shadersource) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRYP compileshader) (GLuint shader);

typedef void (APIENTRYP getshaderiv) (GLuint shader, GLenum pname, GLint *params);

typedef void (APIENTRYP genbuffers) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP bindbuffer) (GLenum target, GLuint buffer);
typedef void (APIENTRYP bufferdata) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRYP vertexattribpointer) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

typedef void (APIENTRYP getprograminfologproc)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

typedef void (APIENTRYP genvertexarrays) (GLsizei n, GLuint *arrays);
typedef void (APIENTRYP bindvertexarray) (GLuint array);

typedef void (APIENTRYP uniformmatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRYP uniformmatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

typedef void (APIENTRYP gldeletebuffers) (GLsizei n, const GLuint *buffers);
typedef void *(APIENTRYP glmapbuffer) (GLenum target, GLenum access);
typedef GLboolean(APIENTRYP glunmapbuffer) (GLenum target);
typedef void (APIENTRYP glactivetexture) (GLenum texture);

typedef void (APIENTRYP glgetactiveuniform) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);

typedef void (APIENTRYP glgenframebuffers) (GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP glbindframebuffer) (GLenum target, GLuint framebuffer);
typedef void (APIENTRYP gldeleteframebuffers) (GLsizei n, const GLuint *framebuffers);
typedef GLenum(APIENTRYP glcheckframebufferstatus) (GLenum target);
typedef void (APIENTRYP glframebuffertexture2d) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void (APIENTRYP glgenrenderbuffers) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP glbindrenderbuffer) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP glrenderbufferstorage) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP glframebufferrenderbuffer) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP gldrawbuffers) (GLsizei n, const GLenum *bufs);
typedef void (APIENTRYP glblendfuncseperate) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (APIENTRYP gluniform2fv) (GLint location, GLsizei count, const GLfloat *value);


createprogram glCreateProgram;
deleteprogram glDeleteProgram;
useprogram glUseProgram;
attachshader glAttachShader;
detachshader glDetachShader;
linkprogram glLinkProgram;
getprogramiv glGetProgramiv;
getshaderinfolog glGetShaderInfoLog;
getuniformlocation glGetUniformLocation;
uniform1i glUniform1i;
getattriblocation glGetAttribLocation;
enablevertexattribarray glEnableVertexAttribArray;
bindattriblocation glBindAttribLocation;
// Shader
createshader glCreateShader;
deleteshader glDeleteShader;
shadersource glShaderSource;
compileshader glCompileShader;
getshaderiv glGetShaderiv;
genbuffers glGenBuffers;
bindbuffer glBindBuffer;
bufferdata glBufferData;
vertexattribpointer glVertexAttribPointer;

//easier to read function pointer
getprograminfologproc glGetProgramInfoLog;
genvertexarrays glGenVertexArrays;
bindvertexarray glBindVertexArray;
uniformmatrix4fv glUniformMatrix4fv;
uniformmatrix3fv glUniformMatrix3fv;
uniform4f glUniform4f;
uniform3f glUniform3f;
uniform2f glUniform2f;
uniform1f glUniform1f;
uniform1iv glUniform1iv;
uniform1fv glUniform1fv;
gluniform2fv glUniform2fv;
glgetactiveuniform glGetActiveUniform;

//buffers
gldeletebuffers glDeleteBuffers;
glmapbuffer glMapBuffer;
glunmapbuffer glUnmapBuffer;
glgenframebuffers glGenFramebuffers;
glbindframebuffer glBindFramebuffer;
gldeleteframebuffers glDeleteFramebuffers;
glcheckframebufferstatus glCheckFramebufferStatus;
glframebuffertexture2d glFramebufferTexture2D;
glgenrenderbuffers glGenRenderbuffers;
glbindrenderbuffer glBindRenderbuffer;
glrenderbufferstorage glRenderbufferStorage;
glframebufferrenderbuffer glFramebufferRenderbuffer;
gldrawbuffers glDrawBuffers;
glblendfuncseperate glBlendFuncSeparate;
//texture
glactivetexture glActiveTexture;



///EXTENSIONS BELOW

bool WGLExtensionSupported(const char *extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}

//https://www.khronos.org/registry/OpenGL/api/GL/wglext.h
//TODO: just type these out manually adn remove wglext.h, only need one function from it
PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

bool InitializeOpenGL()
{
	HINSTANCE dll = LoadLibraryA("opengl32.dll");
	//typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);

	// Program
	glCreateProgram = (createprogram)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (deleteprogram)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (useprogram)wglGetProcAddress("glUseProgram");
	glAttachShader = (attachshader)wglGetProcAddress("glAttachShader");
	glDetachShader = (detachshader)wglGetProcAddress("glDetachShader");
	glLinkProgram = (linkprogram)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (getprogramiv)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (getshaderinfolog)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (getuniformlocation)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (uniform1i)wglGetProcAddress("glUniform1i");
	glGetAttribLocation = (getattriblocation)wglGetProcAddress("glGetAttribLocation");
	glEnableVertexAttribArray = (enablevertexattribarray)wglGetProcAddress("glEnableVertexAttribArray");
	glBindAttribLocation = (bindattriblocation)wglGetProcAddress("glBindAttribLocation");

	// Shader
	glCreateShader = (createshader)wglGetProcAddress("glCreateShader");
	glDeleteShader = (deleteshader)wglGetProcAddress("glDeleteShader");
	glShaderSource = (shadersource)wglGetProcAddress("glShaderSource");
	glCompileShader = (compileshader)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (getshaderiv)wglGetProcAddress("glGetShaderiv");

	// VBO
	glGenBuffers = (genbuffers)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (bindbuffer)wglGetProcAddress("glBindBuffer");
	glBufferData = (bufferdata)wglGetProcAddress("glBufferData");
	glVertexAttribPointer = (vertexattribpointer)wglGetProcAddress("glVertexAttribPointer");
	glGetProgramInfoLog = (getprograminfologproc)wglGetProcAddress("glGetProgramInfoLog");
	glGenVertexArrays = (genvertexarrays)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (bindvertexarray)wglGetProcAddress("glBindVertexArray");
	

	//Uniforms
	glUniformMatrix4fv = (uniformmatrix4fv)wglGetProcAddress("glUniformMatrix4fv");
	glUniformMatrix3fv = (uniformmatrix3fv)wglGetProcAddress("glUniformMatrix3fv");
	glUniform4f = (uniform4f)wglGetProcAddress("glUniform4f");
	glUniform3f = (uniform3f)wglGetProcAddress("glUniform3f");
	glUniform2f = (uniform2f)wglGetProcAddress("glUniform2f");
	glUniform1f = (uniform1f)wglGetProcAddress("glUniform1f");
	glUniform1iv = (uniform1iv)wglGetProcAddress("glUniform1iv");
	glUniform1fv = (uniform1fv)wglGetProcAddress("glUniform1fv");
	glGetActiveUniform = (glgetactiveuniform)wglGetProcAddress("glGetActiveUniform");
	glUniform2fv = (gluniform2fv)wglGetProcAddress("glUniform2fv");

	//Buffers
	glDeleteBuffers = (gldeletebuffers)wglGetProcAddress("glDeleteBuffers");
	glMapBuffer = (glmapbuffer)wglGetProcAddress("glMapBuffer");
	glUnmapBuffer = (glunmapbuffer)wglGetProcAddress("glUnmapBuffer");
	glGenFramebuffers = (glgenframebuffers)wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer = (glbindframebuffer)wglGetProcAddress("glBindFramebuffer");
	glDeleteFramebuffers = (gldeleteframebuffers)wglGetProcAddress("glDeleteFramebuffers");
	glCheckFramebufferStatus = (glcheckframebufferstatus)wglGetProcAddress("glCheckFramebufferStatus");
	glFramebufferTexture2D = (glframebuffertexture2d)wglGetProcAddress("glFramebufferTexture2D");
	glGenRenderbuffers = (glgenrenderbuffers)wglGetProcAddress("glGenRenderbuffers");
	glBindRenderbuffer = (glbindrenderbuffer)wglGetProcAddress("glBindRenderbuffer");
	glRenderbufferStorage = (glrenderbufferstorage)wglGetProcAddress("glRenderbufferStorage");
	glFramebufferRenderbuffer = (glframebufferrenderbuffer)wglGetProcAddress("glFramebufferRenderbuffer");
	glDrawBuffers = (gldrawbuffers)wglGetProcAddress("glDrawBuffers");
	glBlendFuncSeparate = (glblendfuncseperate)wglGetProcAddress("glBlendFuncSeparate");


	//texture
	glActiveTexture = (glactivetexture)wglGetProcAddress("glActiveTexture");



	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
	}

	return true;	
}

void *GetAnyGLFuncAddress(const char* name)
{
	void *p = (void *)wglGetProcAddress(name);
	if (p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1))
	{
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}

	return p;
}


