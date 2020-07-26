

enum MOUSEBUTTON
{
	LEFT,
	RIGHT
};

struct Key
{
	bool down;
	bool released;
	bool pressed;
};

//https://gamedev.stackexchange.com/questions/62830/how-to-detect-same-keyboard-key-press-only-once
unsigned int NUM_KEYS = 256;
Key keys[256];

static bool 
IsKeyDown(unsigned int keyCode)
{
	return keys[keyCode].down;
}

static bool
WasKeyPressed(unsigned int keyCode)
{
	return keys[keyCode].pressed;
}
static bool 
WasKeyReleased(unsigned int keyCode)
{
	return keys[keyCode].released;
}

static void 
SetKey(unsigned int keyCode, bool isDown, bool isReleased, bool isPressed)
{
	keys[keyCode].down = isDown;
	keys[keyCode].released = isReleased;
	keys[keyCode].pressed = isPressed;
}

static void 
ResetKeys()
{
	for (int i = 0; i < NUM_KEYS; i++)
	{
		keys[i].released = false;
		keys[i].pressed = false;
	}
}


/*
struct KeyState
{
	bool currentKeyState[256];
	bool previousKeyState[256];
};*/

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

//KeyState keyState = {};
MouseState mouseState = {};

static bool
MousePressed(MOUSEBUTTON button)
{
	if (button == LEFT)
	{
		return mouseState.leftMouseDown;
	}
	if (button == RIGHT)
	{
		return mouseState.rightMouseDown;
	}
	return false;
}

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
/*
static bool
KeyPressed(int keycode)
{
	if (keyState.currentKeyState[keycode] && !keyState.previousKeyState[keycode])
		return true;
	return false;
}*/

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