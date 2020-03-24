

enum MOUSEBUTTON
{
	LEFT,
	RIGHT
};

struct KeyState
{
	bool currentKeyState[256];
	bool previousKeyState[256];
};

struct MouseState
{
	bool leftMouseDown = false;
	bool leftMouseReleased = false;
	bool rightMouseDown = false;
	bool rightMouseReleased = false;
	bool scrolledUp = false;
	bool scrolledDown = false;

	float x;
	float y;
};

KeyState keyState = {};
MouseState mouseState = {};


static bool
MouseReleased(MOUSEBUTTON button)
{
	if (button == LEFT)
	{
		return mouseState.leftMouseReleased;
	}
	if (button == RIGHT)
	{
		return mouseState.rightMouseReleased;
	}
	return false;
}

static bool
ScroolWheelUp()
{
	return mouseState.scrolledUp;
}

static bool
ScroolWheelDown()
{
	return mouseState.scrolledDown;
}

static void
ClearInput()
{
	if (mouseState.leftMouseReleased) 
	{
		mouseState.leftMouseReleased = false;
	}

	if (mouseState.rightMouseReleased)
	{
		mouseState.rightMouseReleased = false;
	}
	mouseState.scrolledDown = false;
	mouseState.scrolledUp = false;
}