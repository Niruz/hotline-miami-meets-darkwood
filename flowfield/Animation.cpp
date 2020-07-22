
enum ANIMATIONS
{
	ANIM_PLAYER_RUN,
	MAX_ANIMS
};

struct Animation
{

	//TODO: if we use textureArrays the spriteSheetWidth will be a constant
	unsigned int spriteXWidth;
	unsigned int spriteYWidth;
	unsigned int spriteSheetXWidth;
	unsigned int spriteSheetYWidth;
	unsigned int myFrameCounter;
	unsigned int myNumberOfFrames;
	unsigned int myStartFrame;
	unsigned int myFrameSpeed;
	unsigned int currentIndex;
	bool isDone = false; //depends on how we will determine if a state change should occur

	vec2<float> uvSize;
	vec2<float> uv1;
	vec2<float> uv2;
	vec2<float> uv3;
	vec2<float> uv4;

	int textureId = -1;
};

static void
SetAnimation(ANIMATIONS anim)
{
	//TODO: use some sort of lookup table to just change the variables below when calling InitializeAnimation
}

static void
InitializeAnimation(Animation* animation, unsigned int spriteSheetWidth, unsigned int spriteSheetHeight, unsigned int spriteWidth, unsigned int spriteHeight, unsigned int startFrame, unsigned int numberOfFrames, unsigned int frameSpeed, int textureId)
{
	animation->spriteSheetXWidth = spriteSheetWidth;
	animation->spriteSheetYWidth = spriteSheetHeight;
	animation->spriteXWidth = spriteWidth;
	animation->spriteYWidth = spriteHeight;
	animation->isDone = false;
	animation->myFrameCounter = 0;
	animation->myNumberOfFrames = numberOfFrames;
	animation->myStartFrame = startFrame;
	animation->currentIndex = startFrame;
	animation->myFrameSpeed = frameSpeed;
	animation->textureId = textureId;
}

static void
SetTextureCoordinates(int width, int height, int spriteWidth, int spriteHeight, int index, Animation* animation)
{
	//float cols = width / 32.0;
	//int rows = height / 32.0f;
	float cols = width / spriteWidth;
	int rows = height / spriteHeight;

	vec2<float> pointIn2D;
	pointIn2D.x = index % (int)cols;
	pointIn2D.y = index / (int)cols;

	pointIn2D.y = (rows - 1) - pointIn2D.y;

	float x1 = (pointIn2D.x * spriteWidth + 1) / width;
	float x2 = ((pointIn2D.x + 1) * spriteWidth - 1) / width;
	float y1 = (pointIn2D.y * spriteHeight + 1) / height;
	float y2 = ((pointIn2D.y + 1) * spriteHeight - 1) / height;

	vec2<float> uv1 = V2(x1, y1);
	vec2<float> uv2 = V2(x1, y2);
	vec2<float> uv3 = V2(x2, y2);
	vec2<float> uv4 = V2(x2, y1);

	/*vec2<float> uv1 = V2(uvBotLeft.x, uvBotLeft.y + uvSize.y);
	vec2<float> uv2 = uvBotLeft;
	vec2<float> uv3 = V2(uvBotLeft.x + uvSize.x, uvBotLeft.y);
	vec2<float> uv4 = V2(uvBotLeft.x + uvSize.x, uvBotLeft.y + uvSize.y);*/
	/*
		winding order
		uv1 uv4
		uv2 uv3
	*/
	animation->uv2 = uv2; //this is bottom left, UV 0,0 

	animation->uvSize.x = uv3.x - uv2.x;
	animation->uvSize.y = uv1.y - uv2.y;

	//then in submitrenderable we can add this later
	/*vec2_ uv1 = V2(renderable->uv2.x, renderable->uv2.y + renderable->uvSize.y);
	vec2_ uv2 = renderable->uv2;
	vec2_ uv3 = V2(renderable->uv2.x + renderable->uvSize.x, renderable->uv2.y);
	vec2_ uv4 = V2(renderable->uv2.x + renderable->uvSize.x, renderable->uv2.y + renderable->uvSize.y);*/
}

static void
UpdateAnimation(Animation* animation)
{
	//0%0 == 0 first time but then it sets the correct position on the first go which is why we increment afterwards
	if ((animation->myFrameCounter % animation->myFrameSpeed) == 0)
	{
		SetTextureCoordinates(animation->spriteSheetXWidth, animation->spriteSheetYWidth, animation->spriteXWidth, animation->spriteYWidth, animation->currentIndex, animation);
		animation->currentIndex++;
	}

	animation->myFrameCounter++;

	if (animation->myFrameCounter >= (animation->myFrameSpeed * (animation->myNumberOfFrames - 1)) + (animation->myFrameSpeed - 1))
	{
		animation->myFrameCounter = 0;
		animation->currentIndex = animation->myStartFrame;
		animation->isDone = true;
	}
}