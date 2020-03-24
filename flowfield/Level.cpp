


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

	vec2<float> position;

	//Default paths
	vec2<int> patrolFrom;
	vec2<int> patrolTo;
	vec2<int> currentGoal;
	vec2<float> direction;

	unsigned int frameCounter = 0;

	State state;
};

static void
InitializeAi(Ai* ai, vec2<int> patrolFrom, vec2<int> patrolTo)
{
	ai->pathstack = {};
	ai->patrolFrom = patrolFrom;
	ai->patrolTo = patrolTo;
	ai->currentGoal = patrolFrom; //For some reason we start on our own?
	ai->position = V2(patrolFrom.x * 32.0f, patrolFrom.y * -32.0f);
	ai->direction = Normalize(V2(ai->patrolTo.x * 32.0f, ai->patrolTo.y * -32.0f) - ai->position);
	ai->state = Ai::State::PATROL;

	InitializeStack(&ai->pathstack, 10);
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