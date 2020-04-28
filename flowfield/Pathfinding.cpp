struct Node
{
	int parent_i;
	int parent_j;
	float f;
	float g;
	float h;
};

static bool
IsValid(int row, int col, int width, int height)
{
	return (row >= 0) && (row < width) &&
		(col >= 0) && (col < height);
}

static bool
TileBlocked(int* grid, int row, int col, int width) //IsUnblocked is used for pathfinding, TileWalkable is used for the player
{
	// Returns true if the cell is not blocked else false 
	//if (grid[row][col] == 1)
	/*if (grid[TwoToOne(row,col,width)])
		return (true);
	else
		return (false);
	*/
	return TestBit(grid, GetIndex(row, col, width));
}

static bool
IsDestination(int row, int col, vec2<int>* dest)
{
	if (row == dest->x && col == dest->y)
		return true;
	else
		return false;
}

static float
CalculateHValue(int row, int col, vec2<int>* dest)
{
	// Return using the distance formula 
	return ((float)SQRT((row - dest->x)*(row - dest->x)
		+ (col - dest->y)*(col - dest->y)));
}

static void
TracePath(Node* cellDetails, vec2<int>* dest, int width, PathStack* stack)
{
	int row = dest->x;
	int col = dest->y;

	//InitializeStack(stack, 1);
	ResetStack(stack);
	vec2<int> vec;

	while (!(cellDetails[GetIndex(row, col, width)].parent_i == row
		&& cellDetails[GetIndex(row, col, width)].parent_j == col))
	{
		vec.x = row;
		vec.y = col;
		PushStack(stack, vec);
		int temp_row = cellDetails[GetIndex(row, col, width)].parent_i;
		int temp_col = cellDetails[GetIndex(row, col, width)].parent_j;
		row = temp_row;
		col = temp_col;
	}

	vec.x = row;
	vec.y = col;
	PushStack(stack, vec);

	return;
}

static bool
CheckNeighbourNode(Node* nodeDetails, int* closedList, int* grid, MinHeap* openList, int i, int j, int width, int height, vec2<int>* dest, float* gNew, float* hNew, float* fNew, int iToCheck, int jToCheck, PathStack* stack)
{
	bool foundDestination = false;
	if (IsValid(iToCheck, jToCheck, width, height) == true)
	{
		// If the destination cell is the same as the 
		// current successor 
		if (IsDestination(iToCheck, jToCheck, dest) == true)
		{
			// Set the Parent of the destination cell 
			nodeDetails[GetIndex(iToCheck, jToCheck, width)].parent_i = i;
			nodeDetails[GetIndex(iToCheck, jToCheck, width)].parent_j = j;

			//printf("The destination cell is found\n");
			TracePath(nodeDetails, dest, width, stack);
			foundDestination = true;
		}
		// If the successor is already on the closed 
		// list or if it is blocked, then ignore it. 
		// Else do the following 
		//else if (closedList[TwoToOne(iToCheck, jToCheck, width)] == false &&
		else if (!TestBit(closedList, GetIndex(iToCheck, jToCheck, width)) &&
			TileBlocked(grid, iToCheck, jToCheck, width) == true)
		{
			//gNew = nodeDetails[i][j].g + 1.0f;
			*gNew = nodeDetails[GetIndex(i, j, width)].g + 1.0f;
			*hNew = CalculateHValue(iToCheck, jToCheck, dest);
			*fNew = *gNew + *hNew;

			// If it isn’t on the open list, add it to 
			// the open list. Make the current square 
			// the parent of this square. Record the 
			// f, g, and h costs of the square cell 
			//                OR 
			// If it is on the open list already, check 
			// to see if this path to that square is better, 
			// using 'f' cost as the measure. 
			if (nodeDetails[GetIndex(iToCheck, jToCheck, width)].f == FLT_MAX ||
				nodeDetails[GetIndex(iToCheck, jToCheck, width)].f > *fNew)
			{
				InsertKey(openList, CELL(*fNew, iToCheck, jToCheck));

				// Update the details of this cell 
				nodeDetails[GetIndex(iToCheck, jToCheck, width)].f = *fNew;
				nodeDetails[GetIndex(iToCheck, jToCheck, width)].g = *gNew;
				nodeDetails[GetIndex(iToCheck, jToCheck, width)].h = *hNew;
				nodeDetails[GetIndex(iToCheck, jToCheck, width)].parent_i = i;
				nodeDetails[GetIndex(iToCheck, jToCheck, width)].parent_j = j;
			}
		}
	}
	return foundDestination;
}

static void
AStarSearch(int* grid, vec2<int>* src, vec2<int>* dest, int width, int height, PathStack* stack)
{
	if (!IsValid(src->x, src->y, width, height))
		return;
	if (!IsValid(dest->x, dest->y, width, height))
		return;
	if (!TileBlocked(grid, src->x, src->y, width) || !TileBlocked(grid, dest->x, dest->y, width))
		return;
	if (IsDestination(src->x, src->y, dest))
		return;

	//TODO: bit array, use one index
	unsigned int realSize = ((unsigned int)(width*height) / 32) + 1;
	int* closedList = new int[realSize];
	for (unsigned int i = 0; i < realSize; i++)
		closedList[i] = 0;

	Node* nodeDetails = new Node[width*height];
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			if (GetIndex(i, j, width) > 399)
				int shit = 5;
			nodeDetails[GetIndex(i, j, width)].f = FLT_MAX;
			nodeDetails[GetIndex(i, j, width)].g = FLT_MAX;
			nodeDetails[GetIndex(i, j, width)].h = FLT_MAX;
			nodeDetails[GetIndex(i, j, width)].parent_i = -1;
			nodeDetails[GetIndex(i, j, width)].parent_j = -1;
		}
	}

	//Starting node
	i = src->x;
	j = src->y;
	nodeDetails[GetIndex(i, j, width)].f = 0.0f;
	nodeDetails[GetIndex(i, j, width)].g = 0.0f;
	nodeDetails[GetIndex(i, j, width)].h = 0.0f;
	nodeDetails[GetIndex(i, j, width)].parent_i = i;
	nodeDetails[GetIndex(i, j, width)].parent_j = j;
	/*
		 Create an open list having information as-
		 <f, <i, j>>
		 where f = g + h,
		 and i, j are the row and column index of that cell
		 Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	*/
	MinHeap openList;
	//Not sure if this is enough or not? We'll see when we get a stack overflow :)
	MakeHeap(&openList, (unsigned int)width*height);
	//Put the first node on the openList and set its f to 0.0
	InsertKey(&openList, CELL(0.0f, i, j));

	bool foundDestination = false;

	while (openList.heap_size != 0)
	{
		Cell cell = ExtractMin(&openList);

		//Add this to the closed list
		i = cell.i;
		j = cell.j;
		SetBit(closedList, GetIndex(i, j, width));


		/*
		 Generating all the 8 neighbours of this cell

			 N.W   N   N.E
			   \   |   /
				\  |  /
			 W----Cell----E
				  / | \
				/   |  \
			 S.W    S   S.E

		 Cell-->Popped Cell (i, j)
		 N -->  North       (i-1, j)
		 S -->  South       (i+1, j)
		 E -->  East        (i, j+1)
		 W -->  West           (i, j-1)
		 N.E--> North-East  (i-1, j+1)
		 N.W--> North-West  (i-1, j-1)
		 S.E--> South-East  (i+1, j+1)
		 S.W--> South-West  (i+1, j-1)*/

		 // To store the 'g', 'h' and 'f' of the 8 neighbours
		float gNew, hNew, fNew;

		//----------- 1st Neighbour (North) ------------ 
		foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i - 1, j, stack);
		if (foundDestination)
		{
			/*FreeMemory(nodeDetails);
			FreeMemory(closedList);
			FreeMemory(openList.arr);*/
			delete[] nodeDetails;
			delete[] closedList;
			delete[] openList.arr;
			return;
		}


		//----------- 2nd Neighbour (South) ------------ 
		foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i + 1, j, stack);
		if (foundDestination)
		{
			/*FreeMemory(nodeDetails);
			FreeMemory(closedList);
			FreeMemory(openList.arr);*/
			delete[] nodeDetails;
			delete[] closedList;
			delete[] openList.arr;
			return;
		}


		//----------- 3rd Neighbour (East) ------------ 
		foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i, j + 1, stack);
		if (foundDestination)
		{
			/*FreeMemory(nodeDetails);
			FreeMemory(closedList);
			FreeMemory(openList.arr);*/
			delete[] nodeDetails;
			delete[] closedList;
			delete[] openList.arr;
			return;
		}


		//----------- 4th Neighbour (West) ------------ 
		foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i, j - 1, stack);
		if (foundDestination)
		{
			/*FreeMemory(nodeDetails);
			FreeMemory(closedList);
			FreeMemory(openList.arr);*/
			delete[] nodeDetails;
			delete[] closedList;
			delete[] openList.arr;
			return;
		}

		//To avoid cutting corners, check if correct corners are valid
		bool northValid = ValidIndex(i, j + 1, width, width) && TileBlocked(grid, i, j + 1, width);
		bool southValid = ValidIndex(i, j - 1, width, width) && TileBlocked(grid, i, j - 1, width);
		bool eastValid = ValidIndex(i + 1, j, width, width) && TileBlocked(grid, i + 1, j, width);
		bool westValid = ValidIndex(i - 1, j, width, width) && TileBlocked(grid, i - 1, j, width);

		//----------- 5th Neighbour (North-West) ------------ //NW
		if (northValid && westValid)
		{
			foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i - 1, j + 1, stack);
			if (foundDestination)
			{
				/*FreeMemory(nodeDetails);
				FreeMemory(closedList);
				FreeMemory(openList.arr);*/
				delete[] nodeDetails;
				delete[] closedList;
				delete[] openList.arr;
				return;
			}
		}

		//----------- 6th Neighbour (North-West) ------------ //SW
		if (southValid && westValid)
		{
			foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i - 1, j - 1, stack);
			if (foundDestination)
			{
				/*FreeMemory(nodeDetails);
				FreeMemory(closedList);
				FreeMemory(openList.arr);*/
				delete[] nodeDetails;
				delete[] closedList;
				delete[] openList.arr;
				return;
			}
		}

		//----------- 7th Neighbour (South-East) ------------ //NE
		if (southValid && eastValid)
		{
			foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i + 1, j - 1, stack);
			if (foundDestination)
			{
				/*FreeMemory(nodeDetails);
				FreeMemory(closedList);
				FreeMemory(openList.arr);*/
				delete[] nodeDetails;
				delete[] closedList;
				delete[] openList.arr;
				return;
			}
		}

		//----------- 8th Neighbour (South-West) ------------ //NW
		if (northValid && eastValid)
		{
			foundDestination = CheckNeighbourNode(nodeDetails, closedList, grid, &openList, i, j, width, height, dest, &gNew, &hNew, &fNew, i + 1, j + 1, stack);
			if (foundDestination)
			{
				/*FreeMemory(nodeDetails);
				FreeMemory(closedList);
				FreeMemory(openList.arr);*/
				delete[] nodeDetails;
				delete[] closedList;
				delete[] openList.arr;
				return;
			}
		}
	}

	// No destination was found
	return;
}
