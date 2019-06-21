#pragma once
#define PROFILING


#ifdef PROFILING

// Profiling : call start_function and end_function with a unique ID and a display name
namespace profiler
{
	void start_function(const char* displayName);
	void hash(const char* file, long line, char* buffer);
}

#else // PROFILING

#define P_START(displayName)

#endif // PROFILING