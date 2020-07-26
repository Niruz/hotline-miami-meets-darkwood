


typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
	const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

// See https://www.opengl.org/registry/specs/ARB/wgl_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
	const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

// See https://www.opengl.org/registry/specs/ARB/wgl_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

static void
fatal_error(char *msg)
{
	MessageBoxA(NULL, msg, "Error", MB_OK | MB_ICONEXCLAMATION);
	exit(EXIT_FAILURE);
}

static void
init_opengl_extensions(void)
{
	// Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
	// We use a dummy window because you can only set the pixel format for a window once. For the
	// real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
	// that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
	// have a context.
	WNDCLASSA window_class = {
		/*  .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
  .lpfnWndProc = DefWindowProcA,
  .hInstance = GetModuleHandle(0),
  .lpszClassName = "Dummy_WGL_djuasiodwa",*/
	};
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = DefWindowProcA;
	window_class.hInstance = GetModuleHandle(0);
	window_class.lpszClassName = "Dummy_WGL_djuasiodwa";

	if (!RegisterClassA(&window_class)) {
		fatal_error((char*)"Failed to register dummy OpenGL window.");
	}

	HWND dummy_window = CreateWindowExA(
		0,
		window_class.lpszClassName,
		"Dummy OpenGL Window",
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		window_class.hInstance,
		0);

	if (!dummy_window) {
		fatal_error((char*)"Failed to create dummy OpenGL window.");
	}

	HDC dummy_dc = GetDC(dummy_window);

	PIXELFORMATDESCRIPTOR pfd = {
		/*    .nSize = sizeof(pfd),
  .nVersion = 1,
  .iPixelType = PFD_TYPE_RGBA,
  .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
  .cColorBits = 32,
  .cAlphaBits = 8,
  .iLayerType = PFD_MAIN_PLANE,
  .cDepthBits = 24,
  .cStencilBits = 8,*/
	};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = 32;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
	if (!pixel_format) {
		fatal_error((char*)"Failed to find a suitable pixel format.");
	}
	if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
		fatal_error((char*)"Failed to set the pixel format.");
	}

	HGLRC dummy_context = wglCreateContext(dummy_dc);
	if (!dummy_context) {
		fatal_error((char*)"Failed to create a dummy OpenGL rendering context.");
	}

	if (!wglMakeCurrent(dummy_dc, dummy_context)) {
		fatal_error((char*)"Failed to activate dummy OpenGL rendering context.");
	}

	wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
		"wglCreateContextAttribsARB");
	wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
		"wglChoosePixelFormatARB");

	wglMakeCurrent(dummy_dc, 0);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummy_window, dummy_dc);
	DestroyWindow(dummy_window);
}

static HGLRC
init_opengl(HDC real_dc)
{
	init_opengl_extensions();

	// Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
	int pixel_format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
		WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,         32,
		WGL_DEPTH_BITS_ARB,         24,
		WGL_STENCIL_BITS_ARB,       8,
		0
	};

	int pixel_format;
	UINT num_formats;
	wglChoosePixelFormatARB(real_dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
	if (!num_formats) {
		fatal_error((char*)"Failed to set the OpenGL 3.3 pixel format.");
	}

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(real_dc, pixel_format, sizeof(pfd), &pfd);
	if (!SetPixelFormat(real_dc, pixel_format, &pfd)) {
		fatal_error((char*)"Failed to set the OpenGL 3.3 pixel format.");
	}

	// Specify that we want to create an OpenGL 4.4 core profile context
	int gl33_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 4,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0,
	};

	HGLRC gl33_context = wglCreateContextAttribsARB(real_dc, 0, gl33_attribs);
	if (!gl33_context) {
		fatal_error((char*)"Failed to create OpenGL 3.3 context.");
	}

	if (!wglMakeCurrent(real_dc, gl33_context)) {
		fatal_error((char*)"Failed to activate OpenGL 3.3 rendering context.");
	}

	return gl33_context;
}

static LRESULT CALLBACK
window_callback(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT result = 0;

	switch (msg)
	{
	case WM_EXITSIZEMOVE:
	{
		RECT rect = {};
		GetClientRect(window, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		GameResize((float)width, (float)height);
	}
	break;
	case WM_SIZE:
	{
		
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYUP:
	{
		bool wasDown = ((lparam & (1 << 30)) != 0);
		SetKey(wparam, false, wasDown, false);
	}
	break;
	case WM_KEYDOWN:
	{
		bool wasDown = ((lparam & (1 << 30)) != 0);
		bool isDown = ((lparam & (1 << 31)) == 0);
		SetKey(wparam, isDown, false, !wasDown && isDown);
	}
	break;
	/*case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		unsigned int VKCode = wparam;
		bool wasDown = ((lparam & (1 << 30)) != 0);
		bool isDown = ((lparam & (1 << 31)) == 0);
		printf("%s wasDown: %i\n", (char*)&VKCode, wasDown);
		printf("%s isDown: %i\n", (char*)&VKCode, wasDown);

		keyState.currentKeyState[VKCode] = isDown;
		keyState.previousKeyState[VKCode] = wasDown;

		if (keyState.currentKeyState[VK_SPACE] && !keyState.previousKeyState[VK_SPACE])
			PlayWaveFile(&sound, 0, true);
	}
	break;*/

	case WM_MOUSEMOVE:
	{
		POINT p;
		if (GetCursorPos(&p))
		{
			//cursor position now in p.x and p.y
		}
		if (ScreenToClient(window, &p))
		{
			//p.x and p.y are now relative to hwnd's client area
		}
		RECT rect = {};
		GetClientRect(window, &rect);
		//int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		//mouseState.currentLeftMouseState = true;
		mouseState.x = (float)p.x;
		mouseState.y = (float)height - p.y;

	}
	break;
	
	case WM_LBUTTONDOWN:
	{
		mouseState.leftMouseDown = true;
		mouseState.leftMouseReleased = false;
	}
	break;

	case WM_RBUTTONDOWN:
	{
		mouseState.rightMouseDown = true;
		mouseState.rightMouseReleased = false;
	}
	break;

	case WM_LBUTTONUP:
	{

		mouseState.leftMouseDown = false;
		mouseState.leftMouseReleased = true;
	}
	break;

	case WM_RBUTTONUP:
	{

		mouseState.rightMouseDown = false;
		mouseState.rightMouseReleased = true;
	}
	break;

	case WM_MOUSEHWHEEL:
	{

	}
	break;

	case WM_MOUSEWHEEL:
	{

		unsigned short fwKeys = GET_KEYSTATE_WPARAM(wparam);
		short zDelta = GET_WHEEL_DELTA_WPARAM(wparam);
		if (zDelta > 0)
			mouseState.scrolledUp = true;
		else 
			mouseState.scrolledDown = true;

		//int xPos = GET_X_LPARAM(lparam);
		//int yPos = GET_Y_LPARAM(lparam);

	}
	break;

	default:
		result = DefWindowProcA(window, msg, wparam, lparam);
		break;
	}
	return result;
}

static void
create_window(HINSTANCE inst, Window* window, int width, int height)
{
	//Create a window and register it, basically having windows call " new window "
	WNDCLASSA window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = window_callback;
	window_class.hInstance = inst;
	window_class.hCursor = LoadCursor(0, IDC_ARROW);
	window_class.hbrBackground = 0;
	window_class.lpszClassName = "Game";

	if (!RegisterClassA(&window_class))
	{
		fatal_error((char*)"Failed to register window.");
	}
	//Use the rect to set the desired window size
	RECT rect = {};
	rect.right = width;
	rect.bottom = height;
	DWORD window_style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&rect, window_style, false);

	//now windows will use that new window to actually make it
	HWND hwnd = CreateWindowExA(
		0,
		window_class.lpszClassName,
		"Game",
		window_style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		inst,
		0);

	if (!hwnd)
	{
		fatal_error((char*)"Failed to create window.");
	}

	window->width = width;
	window->height = height;
	window->window = hwnd;

	//return hwnd;
}



int WINAPI
WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd_line, int show)
{
	Window window;
	create_window(inst, &window, 1280, 720);

	window.gldc = GetDC(window.window);
	window.glrc = init_opengl(window.gldc);
	ShowWindow(window.window, show);
	UpdateWindow(window.window);

	//Allocate a console for debug output
	AllocConsole();

#pragma warning( push )
#pragma warning( disable : 4996)
	freopen("CONOUT$", "w", stdout);
#pragma warning( pop ) 
	

	if (!InitializeOpenGL())
	{
		MessageBox(window.window, "Could not initialize OpenGL", "Error", MB_OK);
		return 0;
	}
	if (!InitializeDirectSound(window.window, &directSound))
	{
		MessageBox(window.window, "Could not initialize DirectSound", "Error", MB_OK);
		return 0;
	}
	if (!LoadAssets(&window))
	{
		MessageBox(window.window, "Could not initialize Assets", "Error", MB_OK);
		return 0;
	}
	if (!StartCounter())
	{
		MessageBox(window.window, "Could not initialize Timer", "Error", MB_OK);
		return 0;
	}


	return GameMain(window);
}