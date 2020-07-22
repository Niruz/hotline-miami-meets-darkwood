#pragma once

struct RigidBody
{
	// Angular components
	float orientation; // radians
	float angularVelocity;
	float torque;
};

struct Door 
{
	vec2<float> vertices[4];
	vec2<float> originalPosition; // mid point of the door, used when rotating and placing the door 
	vec2<float> relativePosition; //this is the new mid point after rotation has happened; //basically this is the AABB midpoint....
	vec2<float> hingePosition;
	float length = 0.0f;
	float doorSize = 0.0f;
	float rotationRadians = 0.0f;
	float angularVelocity = 0.0f;
	float angularAcceleration = 0.0f;
	float rotationDegrees = 0.0f;
	bool active = false;
	std::vector<vec2<float>> collisionPoints;
	bool collidedThisFrame = false;
	Edge edge; //used for the shadowing
};

//returns the index where this door lies in the map
static void
AddDoorEdges(Door* door)
{
	doorEdges.push_back(door->edge);
}

//called from physics
static void
UpdateDoorEdges(Door* door, int index)
{
	doorEdges[index] = door->edge;
}

//called before rendering
static void
RefreshDoorEdges()
{
	//3 + 2
	int startIndex = edges.size() - doorEdges.size();

	int otherIndex = 0;
	for (int i = startIndex; i < edges.size(); i++)
	{
		edges[i] = doorEdges[otherIndex];
		otherIndex++;
	}
}

static void
UpdateEdge(Door* door, vec2<float> camera)
{
	vec2<float> start = (door->vertices[0] + door->vertices[1]) / 2.0f;
	vec2<float> end = (door->vertices[2] + door->vertices[3]) / 2.0f;
	//	start = start + fakeCamera;
	//	end = end + fakeCamera;
	start.y = -start.y;
	end.y = -end.y;
	start = start + V2(16.0f, 16.0f);
	end = end + V2(16.0f, 16.0f);
	Edge edge = EDGE(start, end);
	door->edge = edge;
}

static void
InitializeDoor(Door* door, vec2<float> position) 
{
	door->originalPosition = position;

	door->length = 128.0f;
	door->doorSize = 16.0f;
	vec2<float> size = V2(door->length / 2.0f, door->doorSize / 2.0f);

	door->vertices[0] = V2(position.x - size.x, position.y - size.y);
	door->vertices[1] = V2(position.x - size.x, position.y + size.y);
	door->vertices[2] = V2(position.x + size.x, position.y + size.y);
	door->vertices[3] = V2(position.x + size.x, position.y - size.y);

	door->hingePosition = (door->vertices[1] + door->vertices[0]) / 2.0f;
	door->relativePosition = position;
}

static std::vector<vec2<float>>
GetPointsInsideDoor(Door* door, vec2<float> position)
{
	//NOTE: For now we return we return the first point we find, we might need to get the one furthest out for the rotation to work

	std::vector<vec2<float>> toReturn;
	//Draw the normals
	vec2<float> up = Normalize(door->vertices[2] - door->vertices[1]);
	up = V2(-up.y, up.x);
	vec2<float> right = Normalize(door->vertices[3] - door->vertices[2]);
	right = V2(-right.y, right.x);
	vec2<float> bottom = Normalize(door->vertices[0] - door->vertices[3]);
	bottom = V2(-bottom.y, bottom.x);
	vec2<float> left = Normalize(door->vertices[1] - door->vertices[0]);
	left = V2(-left.y, left.x);

	//TODO: refactor the AABB stuff

	vec2<float> botLeft  = V2(position.x - 16.0f, position.y - 16.0f);
	vec2<float> topLeft  = V2(position.x - 16.0f, position.y + 16.0f);
	vec2<float> topRight = V2(position.x + 16.0f, position.y + 16.0f);
	vec2<float> botRight = V2(position.x + 16.0f, position.y - 16.0f);

	std::vector<vec2<float>> positions;
	positions.push_back(V2(position.x - 16.0f, position.y - 16.0f)); //botLeft
	positions.push_back(V2(position.x - 16.0f, position.y + 16.0f)); //topLeft
	positions.push_back(V2(position.x + 16.0f, position.y + 16.0f)); //topRight
	positions.push_back(V2(position.x + 16.0f, position.y - 16.0f)); //botRight

	vec2<float> temp = V2(0.0f, 0.0f);
	//for now return all of them
	bool found = false;
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		float upDot = Dot((door->vertices[2] - positions[i]), up);
		if (upDot < 0.0f)
			continue;
		float rightDot = Dot((door->vertices[3] - positions[i]), right);
		if (rightDot < 0.0f)
			continue;
		float botDot = Dot((door->vertices[0] - positions[i]), bottom);
		if (botDot < 0.0f)
			continue;
		float leftDot = Dot((door->vertices[1] - positions[i]), left);
		if (leftDot < 0.0f)
			continue;
		temp = positions[i];
		toReturn.push_back(positions[i]);
		//found = true;
		//break;
	}

	//Check if the door is inside the player
	if (!found) 
	{
		//Maybe just push to stack here 
		std::vector<vec2<float>> doorPositions;
		doorPositions.push_back(door->vertices[0]); //botleft
		doorPositions.push_back(door->vertices[1]); //topLeft
		doorPositions.push_back(door->vertices[2]); //topRight
		doorPositions.push_back(door->vertices[3]); //botRight

		vec2<float> up    = V2( 0.0f,  1.0f);
		vec2<float> right = V2( 1.0f,  0.0f);
		vec2<float> down  = V2( 0.0f, -1.0f);
		vec2<float> left  = V2(-1.0f,  0.0f);

		for (unsigned int i = 0; i < doorPositions.size(); i++)
		{
			float upDot = Dot((positions[1] - doorPositions[i]), up);
			if (upDot < 0.0f)
				continue;
			float rightDot = Dot((positions[2] - doorPositions[i]), right);
			if (rightDot < 0.0f)
				continue;
			float botDot = Dot((positions[3] - doorPositions[i]), down);
			if (botDot < 0.0f)
				continue;
			float leftDot = Dot((positions[1] - doorPositions[i]), left);
			if (leftDot < 0.0f)
				continue;
			temp = doorPositions[i];
			toReturn.push_back(doorPositions[i]);
			//break;
		}
	}

	return toReturn;
	//return temp;
}

static void
RotateDoorRadians(Door* door, float radians)
{
	float cos = cosf(radians);
	float sin = sinf(radians);

	vec2<float> size = V2(door->length / 2.0f, door->doorSize / 2.0f);

/*	vec2<float> vertices[4] = {
		V2(-size.x, -size.y),
		V2(-size.x,  size.y),
		V2(size.x,  size.y),
		V2(size.x, -size.y)
	};*/ //around origin

	vec2<float> vertices[4] = {
		V2(-size.x, -size.y),
		V2(-size.x,  size.y),
		V2(size.x,  size.y),
		V2(size.x, -size.y)
	};

	vec2<float> hingeOrigin = (vertices[1] + vertices[0]) / 2.0f;

	/*vec2<float> hingeVertices[4] = {

	};*/

	vertices[0] -= hingeOrigin;
	vertices[1] -= hingeOrigin;
	vertices[2] -= hingeOrigin;
	vertices[3] -= hingeOrigin;


	vertices[0] = V2(vertices[0].x * cos - vertices[0].y * sin, vertices[0].x * sin + vertices[0].y * cos);

	vertices[1] = V2(vertices[1].x * cos - vertices[1].y * sin, vertices[1].x * sin + vertices[1].y * cos);

	vertices[2] = V2(vertices[2].x * cos - vertices[2].y * sin, vertices[2].x * sin + vertices[2].y * cos);

	vertices[3] = V2(vertices[3].x * cos - vertices[3].y * sin, vertices[3].x * sin + vertices[3].y * cos);

	vertices[0] += hingeOrigin;
	vertices[1] += hingeOrigin;
	vertices[2] += hingeOrigin;
	vertices[3] += hingeOrigin;

	/*vec2<float> newOrigin = V2(0.0f, 0.0f);
	for (unsigned int i = 0; i < 4; i++)
	{
		newOrigin += vertices[i];
	}
	newOrigin = newOrigin / 4.0f;

	door->position += newOrigin;*/


	door->vertices[0] = V2(door->originalPosition.x + vertices[0].x, door->originalPosition.y + vertices[0].y);
	door->vertices[1] = V2(door->originalPosition.x + vertices[1].x, door->originalPosition.y + vertices[1].y);
	door->vertices[2] = V2(door->originalPosition.x + vertices[2].x, door->originalPosition.y + vertices[2].y);
	door->vertices[3] = V2(door->originalPosition.x + vertices[3].x, door->originalPosition.y + vertices[3].y);

	vec2<float> newOrigin = V2(0.0f, 0.0f);
	for (unsigned int i = 0; i < 4; i++)
	{
		newOrigin += door->vertices[i];
	}
	newOrigin = newOrigin / 4.0f;

	door->relativePosition = newOrigin;
}


static void
RotateDoor(Door* door, float degrees)
{
	door->rotationDegrees += degrees;
	if (door->rotationDegrees > 359.0f)
		door->rotationDegrees = 0.0f;

	//float cos = CosRadians(DegreesToRadians(door->rotationDegrees));
	//float sin = SinRadians(DegreesToRadians(door->rotationDegrees));
	float cos = cosf(DegreesToRadians(door->rotationDegrees));
	float sin = sinf(DegreesToRadians(door->rotationDegrees));

	vec2<float> size = V2(door->length / 2.0f, door->doorSize / 2.0f);

	vec2<float> vertices[4] = {
		V2(-size.x, -size.y),
		V2(-size.x,  size.y),
		V2( size.x,  size.y),
		V2( size.x, -size.y)
	};


	vertices[0] = V2(vertices[0].x * cos - vertices[0].y * sin, vertices[0].x * sin + vertices[0].y * cos);

	vertices[1] = V2(vertices[1].x * cos - vertices[1].y * sin, vertices[1].x * sin + vertices[1].y * cos);

	vertices[2] = V2(vertices[2].x * cos - vertices[2].y * sin, vertices[2].x * sin + vertices[2].y * cos);

	vertices[3] = V2(vertices[3].x * cos - vertices[3].y * sin, vertices[3].x * sin + vertices[3].y * cos);

	door->vertices[0] = V2(door->originalPosition.x + vertices[0].x, door->originalPosition.y + vertices[0].y);
	door->vertices[1] = V2(door->originalPosition.x + vertices[1].x, door->originalPosition.y + vertices[1].y);
	door->vertices[2] = V2(door->originalPosition.x + vertices[2].x, door->originalPosition.y + vertices[2].y);
	door->vertices[3] = V2(door->originalPosition.x + vertices[3].x, door->originalPosition.y + vertices[3].y);


	/*door->vertices[0] = V2(door->vertices[0].x * cos - door->vertices[0].y * sin
		                  ,door->vertices[0].x * sin + door->vertices[0].y * cos);

	door->vertices[1] = V2(door->vertices[1].x * cos - door->vertices[1].y * sin
		                 , door->vertices[1].x * sin + door->vertices[1].y * cos);

	door->vertices[2] = V2(door->vertices[2].x * cos - door->vertices[2].y * sin
		                 , door->vertices[2].x * sin + door->vertices[2].y * cos);

	door->vertices[3] = V2(door->vertices[3].x * cos - door->vertices[3].y * sin
		                 , door->vertices[3].x * sin + door->vertices[3].y * cos); */

	/*door->vertices[1] = V2(position.x - size.x, position.y + size.y);
	door->vertices[2] = V2(position.x + size.x, position.y + size.y);
	door->vertices[3] = V2(position.x + size.x, position.y - size.y);

	rotated.x = convertedPosition.x * rotation.cos - convertedPosition.y * rotation.sin;
	rotated.y = convertedPosition.x * rotation.sin + convertedPosition.y * rotation.cos;*/
}

struct CollisionData
{
	//minimum translation vector
	vec2<float> mtv;
	bool collided;
};

static float 
IntervalDistance(float minA, float maxA, float minB, float maxB) {
	if (minA < minB) {
		return minB - maxA;
	}
	else {
		return minA - maxB;
	}
}

static CollisionData
CheckIfDoorAgentCollided(Door* door, vec2<float> position)
{
	CollisionData temp;

	//Draw the normals
	vec2<float> doorNormals[4] = 
	{
		Normalize(door->vertices[2] - door->vertices[1]), // up
		Normalize(door->vertices[3] - door->vertices[2]), // right
		Normalize(door->vertices[0] - door->vertices[3]), //bottom
		Normalize(door->vertices[1] - door->vertices[0]) //left
	};

	doorNormals[0] = V2(-doorNormals[0].y, doorNormals[0].x);
	doorNormals[1] = V2(-doorNormals[1].y, doorNormals[1].x);
	doorNormals[2] = V2(-doorNormals[2].y, doorNormals[2].x);
	doorNormals[3] = V2(-doorNormals[3].y, doorNormals[3].x);

	vec2<float> agentNormals[4] = 
	{
		V2(0.0f, 1.0f),
		V2(1.0f, 0.0f),
		V2(0.0f, -1.0f),
		V2(-1.0f, 0.0f)
	};

	//Only need to use two of the axis from each one since they are squares
	vec2<float> allNormals[4] = 
	{
		doorNormals[0],
		doorNormals[1],
		agentNormals[0],
		agentNormals[1],
	};

	//Midpoints + AABB size (32.0f)
	vec2<float> agentPositions[4] =
	{
		V2(position.x - 16.0f, position.y - 16.0f), //botLeft
		V2(position.x - 16.0f, position.y + 16.0f), //topLeft
		V2(position.x + 16.0f, position.y + 16.0f), //topRight
		V2(position.x + 16.0f, position.y - 16.0f) //botRight
	};


	std::vector<vec2<float>> mtvs;

	//for every axis
	for (unsigned int i = 0; i < 4; i++)
	{
		float scalars1[4];
		float scalars2[4];

		//Dot all of the vertices of the two objects against the normals
		for (unsigned int k = 0; k < 4; k++) 
		{
			scalars1[k] = Dot(door->vertices[k], allNormals[i]);
			scalars2[k] = Dot(agentPositions[k], allNormals[i]);
		}

		float s1Min = *std::min_element(std::begin(scalars1), std::end(scalars1));
		float s1Max = *std::max_element(std::begin(scalars1), std::end(scalars1));

		float s2Min = *std::min_element(std::begin(scalars2), std::end(scalars2));
		float s2Max = *std::max_element(std::begin(scalars2), std::end(scalars2));

		if (s2Min > s1Max || s2Max < s1Min)
		{
			temp.collided = false;
			return temp;
		}

		float localOverlap = Abs(IntervalDistance(s1Min, s1Max, s2Min, s2Max));
	
		//If the vector from the door to the agent center dotted with the normal is positive then everything is ok
		//Otherwise we need to flip normal so we push the player out of the door since it is " static "
		//if (Dot(allNormals[i], (position - door->originalPosition)) > 0.0f) 
		if (Dot(allNormals[i], (position - door->relativePosition)) > 0.0f)
			mtvs.push_back(V2(allNormals[i] * localOverlap ));
		else 
			mtvs.push_back(V2(allNormals[i] * localOverlap* -1.0f));
	}

	std::sort
	(
		mtvs.begin(),
		mtvs.end(),
		[&](const vec2<float>& t1, const vec2<float>& t2)
		{
			return Len(t1) < Len(t2);
		}
	);

	
	temp.mtv = mtvs.front();
	temp.collided = true;
	return temp;
}