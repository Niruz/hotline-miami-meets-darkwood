



#include <iostream>
#include <fcntl.h>
#include <ctime>
#include <fstream>
#include <assert.h>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iterator>     // std::distance

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "DirectSound.cpp" //this actually includes some windows/c++ headers that we might want to move

#include "Timer.cpp"

#include "GLite.h"
#include "Input.cpp"
#include "Window.cpp"

//Functions not part of any specific OS
#include "Math.cpp"
#include "UtilityFunctions.cpp"

#include "Font.cpp"
#include "Texture.cpp"
#include "Animation.cpp"
#include "IndexBuffer.cpp"
#include "Shader.cpp"
#include "AssetLoader.cpp"
#include "PathStack.cpp"
#include "Tilemap.cpp"
#include "Shadow.cpp"
#include "Agent.cpp"
#include "Door.cpp"

#include "GameGlobals.cpp"
#include "Level.cpp"
#include "GLRenderer.cpp"

#include "BinaryHeap.cpp"
#include "Pathfinding.cpp"


#include "PeekMessage.cpp"
#include "Physics.cpp"
#include "GameMain.cpp"


#include "WinMain.cpp"



