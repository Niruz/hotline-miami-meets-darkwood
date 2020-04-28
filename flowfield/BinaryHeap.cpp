//#define FLT_MAX          3.402823466e+38F        // max value

struct Cell
{
	float f;
	int i;
	int j;
};

struct MinHeap
{
	Cell* arr; // pointer to array of elements in heap 
	unsigned int capacity; // maximum possible size of min heap 
	unsigned int heap_size; // Current number of elements in min heap 
};

Cell
CELL(float f, int i, int j)
{
	Cell cell;
	cell.f = f;
	cell.i = i;
	cell.j = j;

	return cell;
}

static void
MakeHeap(MinHeap* heap, unsigned int count)
{
	//heap->arr = _Alloc_Memory(Cell, count);
	heap->arr = new Cell[count];
	heap->capacity = count;
	heap->heap_size = 0;
}

static unsigned int
GetParent(unsigned int index)
{
	return (index - 1) / 2;
}

static unsigned int
GetLeftChild(unsigned int index)
{
	return (2 * index) + 1;
}

static unsigned int
GetRightChild(unsigned int index)
{
	return (2 * index) + 2;
}

static void
Swap(Cell* x, Cell* y)
{
	Cell temp = *x;
	*x = *y;
	*y = temp;
}

static void
InsertKey(MinHeap* minHeap, Cell key)
{
	if (minHeap->heap_size == minHeap->capacity)
	{
		//Handle resizing
	}

	minHeap->heap_size++;
	unsigned int i = minHeap->heap_size - 1;
	minHeap->arr[i] = key;

	while (i != 0 && minHeap->arr[GetParent(i)].f > minHeap->arr[i].f)
	{
		Swap(&minHeap->arr[i], &minHeap->arr[GetParent(i)]);
		i = GetParent(i);
	}
}

static void
DecreaseKey(MinHeap* minHeap, unsigned int i, Cell newVal)
{
	minHeap->arr[i] = newVal;

	while (i != 0 && minHeap->arr[GetParent(i)].f > minHeap->arr[i].f)
	{
		Swap(&minHeap->arr[i], &minHeap->arr[GetParent(i)]);
		i = GetParent(i);
	}
}

static void
MinHeapify(MinHeap* minHeap, unsigned int i)
{
	unsigned int left = GetLeftChild(i);
	unsigned int right = GetRightChild(i);
	unsigned int smallest = i;

	if (left < minHeap->heap_size && minHeap->arr[left].f < minHeap->arr[i].f)
	{
		smallest = left;
	}
	if (right < minHeap->heap_size && minHeap->arr[right].f < minHeap->arr[smallest].f)
	{
		smallest = right;
	}

	if (smallest != i)
	{
		Swap(&minHeap->arr[i], &minHeap->arr[smallest]);
		MinHeapify(minHeap, smallest);
	}
}

static Cell //Remove the root, ie the smallest element from the heap
ExtractMin(MinHeap* minHeap)
{
	if (minHeap->heap_size <= 0)
	{
		Cell cell;
		cell.f = FLT_MAX;
		return cell; //Return this because there is never any point where INT_MAX will be the smallest value
	}
	if (minHeap->heap_size == 1)
	{
		minHeap->heap_size--;
		return minHeap->arr[0];
	}

	Cell root = minHeap->arr[0];
	minHeap->arr[0] = minHeap->arr[minHeap->heap_size - 1];
	minHeap->heap_size--;
	MinHeapify(minHeap, 0);

	return root;
}
