struct PathStack
{
	int size;
	int capacity;
	vec2<int>* arr;
};

static void
InitializeStack(PathStack* stack, int size)
{
	stack->arr = new vec2<int>[size];
	stack->size = size;
	stack->capacity = 0;
}

//Use this one so we dont need to free memory all the time
static void
ResetStack(PathStack* stack)
{
	stack->capacity = 0;
}

static void
PushStack(PathStack* stack, vec2<int> vec)
{
	if (stack->capacity >= stack->size)
	{
		//TODO: determine a good size to resize with, there shouldn't be too many
		//nodes anyway so 10 each time?
		stack->size = stack->size * 2;
		//vec2_int* newArr = _Alloc_Memory(vec2_int, (u64)stack->size);
		vec2<int>* newArr = new vec2<int>[stack->size];
		for (unsigned int i = 0; i < (unsigned int)stack->capacity; i++)
		{
			newArr[i] = stack->arr[i];
		}
		//FreeMemory(stack->arr);
		delete[] stack->arr;

		stack->arr = newArr;

	}
	stack->arr[stack->capacity] = vec;
	stack->capacity++;
}

static vec2<int>
GetCurrentStack(PathStack* stack)
{
	return stack->arr[stack->capacity]; 
	/*if (stack->capacity == 0)
		return stack->arr[0];
	else
		return stack->arr[stack->capacity - 1];*/
}

static vec2<int>
PopStack(PathStack* stack)
{
	stack->capacity--;
	if (stack->capacity < 0)
		stack->capacity = 0;
	return stack->arr[stack->capacity];
}

static vec2<int>
LastEntry(PathStack* stack)
{
	return stack->arr[0];
}