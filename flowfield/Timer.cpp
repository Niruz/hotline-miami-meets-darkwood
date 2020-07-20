
float PCFreq = 0.0;
__int64 CounterStart = 0;

bool StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		return false;

	PCFreq = float(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
	return true;
}

float GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return float(li.QuadPart - CounterStart) / PCFreq;
}
