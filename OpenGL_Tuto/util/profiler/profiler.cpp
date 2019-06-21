#include "profiler.h"
#include <iostream>
#include "util/table.h"
#include "util/bit_array.h"
#include <time.h>

namespace profiler
{
	namespace stack
	{
		Table table;
		Column<char[30]> displayNames;
		Column<time_t> startTimes;
		int count = 0;

	}
	namespace result
	{
		Table table;
		Column<char[30]> displayNames;
		Column<time_t> durations;
		Column<handle> parents;
	}

	void init()
	{
		stack::table.init(50, stack::displayNames + stack::startTimes);
		result::table.init(500, result::displayNames + result::durations + result::parents);
	}

	void start_function(const char* displayName)
	{
		strcpy_s(stack::displayNames[stack::count], displayName);
		stack::startTimes[stack::count] = time(NULL);
		stack::count++;
	}

	void end_function()
	{
		stack::count--;
		//strcpy(result::displayNames[];
	}

	void hash(const char* file, long line, char* buffer)
	{
		std::cout << file << line << std::endl;
	}
}