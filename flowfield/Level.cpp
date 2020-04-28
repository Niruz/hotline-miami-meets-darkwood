


struct Ai
{
	enum State 
	{
		ROAM,
		PATROL,
		INSPECT,
		ATTACK
	};

	//For A Star
	PathStack pathstack;
	//For patrolling
	PathStack patrolStack;

	vec2<float> position;

	//Default paths
	//vec2<int> patrolFrom;
	//vec2<int> patrolTo;
	vec2<int> currentGoal;
	vec2<float> direction;

	vec2<int> inspectionGoal;

	unsigned int frameCounter = 0;
	unsigned int aiRoamWalkTimer = 0;
	unsigned int aiRoamStandStillTimer = 0;
	unsigned int nextPatrolIndex = 0;
	unsigned int maxPatrolPaths;

	State state;
};

static void
InitializeAi(Ai* ai, vec2<int> patrolFrom, vec2<int> patrolTo)
{
	/*ai->pathstack = {};
	ai->patrolFrom = patrolFrom;
	ai->patrolTo = patrolTo;
	ai->currentGoal = patrolFrom; //For some reason we start on our own?
	ai->position = V2(patrolFrom.x * 32.0f, patrolFrom.y * -32.0f);
	ai->direction = Normalize(V2(ai->patrolTo.x * 32.0f, ai->patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::PATROL;

	InitializeStack(&ai->pathstack, 10);*/

	InitializeStack(&ai->pathstack, 10);
	InitializeStack(&ai->patrolStack, 10);
	ai->pathstack = {};

	PushStack(&ai->patrolStack, patrolFrom);
	PushStack(&ai->patrolStack, patrolTo);
	ai->currentGoal = patrolFrom; //For some reason we start on our own?

	ai->position = V2(patrolFrom.x * 32.0f, patrolFrom.y * -32.0f);
	ai->direction = Normalize(V2(patrolTo.x * 32.0f, patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::PATROL;

	ai->maxPatrolPaths = ai->patrolStack.capacity;

}

static void
InitializeAi(Ai* ai, int count, ...)
{
	ai->pathstack = {};
	ai->patrolStack = {};
	InitializeStack(&ai->pathstack, 10);
	InitializeStack(&ai->patrolStack, 10);


	va_list paths;
	va_start(paths, count);

	for (int i = 0; i < count; ++i) 
	{
		PushStack(&ai->patrolStack, va_arg(paths, vec2<int>));
	}

	va_end(paths);


	ai->currentGoal = ai->patrolStack.arr[0]; //For some reason we start on our own?
	vec2<int> patrolTo = ai->patrolStack.arr[1];

	ai->position = V2(ai->currentGoal.x * 32.0f, ai->currentGoal.y * -32.0f);
	ai->direction = Normalize(V2(patrolTo.x * 32.0f, patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::PATROL;

	ai->maxPatrolPaths = ai->patrolStack.capacity;
}

static void
SetStateRoaming(Ai* ai)
{
	float LO = -1.0f;
	float HI = 1.0f;
	float rX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	float rY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	ai->direction = Normalize(V2(rX, rY));
	ai->frameCounter = 0;
	ai->state = Ai::State::ROAM;
}

static vec2<float>
GetRandomDirection() 
{
	float LO = -1.0f;
	float HI = 1.0f;
	float rX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	float rY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
	
	return Normalize(V2(rX, rY));
}

static unsigned int
GetRandomTimeMillis(unsigned int low, unsigned int high)
{
	return (low + (rand() % ((high - low) + 1))) * 1000;
}
