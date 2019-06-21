#include "profiler.h"
#include <iostream>
#include "util/table.h"
#include "util/bit_array.h"
#include <time.h>
#include <map>
#include <GLFW/glfw3.h>
#include "imgui.h"

namespace profiler
{
	namespace stack
	{
		Table table;
		Column<char[30]> displayNames;
		Column<double> startTimes;
		int count = 0;

	}
	namespace result
	{
		Table table;
		Column<char[30]> displayNames;
		Column<double> durations;
		Column<handle> parents;
		BitArray allocation;
		std::map<std::string, handle> registeredMap;
	}

	void init()
	{
		stack::table.init(50, stack::displayNames + stack::startTimes);
		result::allocation.init(500);
		result::table.init(500, result::displayNames + result::durations + result::parents);
	}

	void start_function(const char* displayName)
	{
		strcpy_s(stack::displayNames[stack::count], displayName);
		stack::startTimes[stack::count] = glfwGetTime();
		stack::count++;
	}

	void end_function()
	{
		stack::count--;
		handle targetId;
		std::string displayName = stack::displayNames[stack::count];
		if (result::registeredMap.count(stack::displayNames[stack::count]) > 0)
		{
			targetId = result::registeredMap[displayName];
		}
		else
		{
			targetId = { result::allocation.allocate() };
			result::registeredMap[stack::displayNames[stack::count]] = targetId;
			TableElement element = result::table.element(targetId.id);
			handle parentId = { -1 };
			element << stack::displayNames[stack::count] << 0. << parentId;
		}
		double endTime = glfwGetTime();
		result::durations[targetId] += endTime - stack::startTimes[stack::count];
		std::cout << "end time : " << endTime << ", start time : " << stack::startTimes[stack::count] << std::endl;
		std::cout << "duration of " << displayName.c_str() << " : " << result::durations[targetId] << std::endl;
	}

	void end_frame()
	{
		result::allocation.clear();
		result::registeredMap.clear();
	}

	void hash(const char* file, long line, char* buffer)
	{
		std::cout << file << line << std::endl;
	}

	void display()
	{
		char buffer[500];
		if (ImGui::Begin("Profiler"))
		{
			for (auto it = result::allocation.begin(); it.isValid(); it++)
			{
				ImGui::Text(result::displayNames[*it]);
				sprintf_s(buffer, "%f", result::durations[*it]);
				ImGui::Text(buffer);
			}
			ImGui::End();
		}
	}
}