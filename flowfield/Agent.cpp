struct Agent
{
	vec2<float> position;
	vec2<float> velocity;

	float maxForce = 0.25f;
	float maxSpeed = 4.0f;
	//float rotation = 0.0f;
	float cos;
	float sin;
	float radius = 0.0f;
	float minSeparation;
	float maxCohesion;

	vec2<float> forceToApply;

	int id = -1;

};

struct Agents
{
	static const unsigned int maxAgents = 10;
	Agent agents[maxAgents];

	unsigned int currentActiveAgents = 0;
};

Agents agents;

static Agent*
InitializeAgent(const vec2<float>& position, const vec2<float>& velocity, float radius)
{
	if (agents.currentActiveAgents == agents.maxAgents)
		return nullptr;
	agents.agents[agents.currentActiveAgents].position = position;
	agents.agents[agents.currentActiveAgents].velocity = velocity;
	agents.agents[agents.currentActiveAgents].radius = radius;
	agents.agents[agents.currentActiveAgents].minSeparation = radius * 5.0f; //TODO: set these manually
	agents.agents[agents.currentActiveAgents].maxCohesion = radius * 10.0f; //TODO: set these manually
	agents.agents[agents.currentActiveAgents].id = agents.currentActiveAgents;

	Agent* toReturn = &agents.agents[agents.currentActiveAgents];
	agents.currentActiveAgents++;
	return toReturn;
}

static vec2<float>
SteeringBehaviourFlee(Agent* agent, const vec2<float>& destination)
{
	vec2<float> desired = agent->position - destination;

	float length = Len(desired);
	//Normalize the direction and move there at max speed
	vec2<float> test = Normalize(desired) * agent->maxSpeed;
	desired = desired * (agent->maxSpeed / Len(desired));

	vec2<float> force = desired - agent->velocity;

	return force * agent->maxForce / agent->maxSpeed;
}

static vec2<float>
SteeringBehaviourSeek(Agent* agent, const vec2<float>& destination)
{
	vec2<float> desired = destination - agent->position;

	float length = Len(desired);
	//Normalize the direction and move there at max speed
	vec2<float> test = Normalize(desired) * agent->maxSpeed;
	desired = desired * (agent->maxSpeed / Len(desired));

	vec2<float> force = desired - agent->velocity;

	return force * (agent->maxForce / agent->maxSpeed);
}

static vec2<float>
SteeringBehaviourSeparation(Agent* agent) 
{
	vec2<float> totalForce = V2(0.0f, 0.0f);
	unsigned int neighboursCount = 0;

	for (unsigned int i = 0; i < agents.currentActiveAgents; i++)
	{
		Agent* otherAgent = &agents.agents[i];

		if (agent->id == otherAgent->id)
			continue;

		float distance = Len(agent->position - otherAgent->position);

		if (distance < agent->minSeparation && distance > 0) 
		{
			vec2<float> pushForce = agent->position - otherAgent->position;

			totalForce = totalForce + (pushForce / agent->radius); //shouldnt this be multiplied if we want it stronger if they are closer?
			neighboursCount++;
		}
		
	}

	if (neighboursCount == 0)
		return V2(0.0f, 0.0f);

	totalForce = totalForce / (float)neighboursCount;
	return totalForce * agent->maxForce;
}

static vec2<float>
SteeringBehaviourCohesion(Agent* agent)
{
	vec2<float> centerOfMass = agent->position;
	unsigned int neighboursCount = 1;

	for (unsigned int i = 0; i < agents.currentActiveAgents; i++)
	{
		Agent* otherAgent = &agents.agents[i];

		if (agent->id == otherAgent->id)
			continue;

		float distance = Len(agent->position - otherAgent->position);
		if (distance < agent->maxCohesion) 
		{
			centerOfMass = centerOfMass + otherAgent->position;
			neighboursCount++;
		}
	}

	if (neighboursCount == 1)
		return V2(0.0f, 0.0f);

	centerOfMass = centerOfMass / (float)neighboursCount;

	return SteeringBehaviourSeek(agent, centerOfMass);
}

static vec2<float>
SteeringBehaviourAlignment(Agent* agent)
{

	vec2<float> averageHeading = V2(0.0f, 0.0f);
	unsigned int neighboursCount = 0;

	for (unsigned int i = 0; i < agents.currentActiveAgents; i++)
	{
		Agent* otherAgent = &agents.agents[i];

		float distance = Len(agent->position - otherAgent->position);
		if (distance < agent->maxCohesion && Len(otherAgent->velocity) > 0) 
		{
			averageHeading = averageHeading + Normalize(otherAgent->velocity);
			neighboursCount++;
		}
	}

	if (neighboursCount == 0)
		return V2(0.0f, 0.0f);

	averageHeading = averageHeading / (float)neighboursCount;

	vec2<float> desired = averageHeading * agent->maxSpeed;
	vec2<float> force = desired - agent->velocity;
	return force * (agent->maxForce / agent->maxSpeed);
}

static void
UpdateAgent(Agent* agent, vec2<float> updatedVelocity, float dt) 
{
	agent->velocity = agent->velocity + (updatedVelocity * dt);

	float speed = Len(agent->velocity);
	if (speed > agent->maxSpeed) {
		agent->velocity = agent->velocity * (agent->maxSpeed / speed);
		//should be this?
		//agent->velocity = agent->velocity * (agent->maxSpeed / speed); //ie if its 4/5 we scale it down a bit again
	}

	vec2<float> up = V2(0.0f, 1.0f);

	vec2<float> normalized = Normalize(agent->velocity);
	agent->cos = Dot(normalized, up);
	agent->sin = -((normalized.x * up.y) - (normalized.y * up.x));
	

	agent->position = agent->position + (agent->velocity * dt);
}


/*
var agent = agents[i];

		//Work out the force for our behaviour
		var seek = steeringBehaviourSeek(agent);

		//Apply the force
		agent.velocity = agent.velocity.plus(seek.mul(dt));

		//Cap speed as required
		var speed = agent.velocity.length();
		if (speed > agent.maxSpeed) {
			agent.velocity = agent.velocity.mul(4 / speed);
		}

		//Calculate our new movement angle
		agent.rotation = agent.velocity.angle();

		//Move a bit
		agent.position = agent.position.plus(agent.velocity.mul(dt));


*/








/*
	//Desired change of location
	var desired = destination.minus(agent.position);
	//Desired velocity (move there at maximum speed)
	desired = desired.mul(agent.maxSpeed / desired.length());
	//The velocity change we want
	var force = desired.minus(agent.velocity);
	//Convert to a force
	return force.mul(agent.maxForce / agent.maxSpeed);

*/