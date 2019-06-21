#pragma once
#define PROFILING

#ifdef PROFILING

#define P_FRAME_END profiler::end_frame();
#define P_INIT profiler::init()
#define P_START(displayName) profiler::start_function(displayName)
#define P_END profiler::end_function()

// Profiling : call start_function and end_function with a unique ID and a display name
namespace profiler
{
	void init();
	void start_function(const char* displayName);
	void end_frame();
	void end_function();
	void hash(const char* file, long line, char* buffer);

	void display();
}

#else // PROFILING

#define P_FRAME_END
#define P_INIT
#define PROFILE(displayName)
#define P_END

#endif // PROFILING