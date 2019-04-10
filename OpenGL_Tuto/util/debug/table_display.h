#pragma once

#include "../table.h"
#include "../bit_array.h"
#include <imgui.h>
#include "transform.h"
#include <cmath>

template<typename T>
T min(T a, T b)
{
	return a > b ? b : a;
}

namespace debug
{
	template<typename T>
	bool displayElement(const char* title, T& element)
	{
		ImGui::Text(title);
		return false;
	}

	template<> bool displayElement<float>(const char* title, float& element);
	template<> bool displayElement<vec3>(const char* title, vec3& element);
	template<> bool displayElement<handle>(const char* title, handle& element);

	template<typename T>
	void displayColumn(const char* columnTitle, Column<T> column, int first, int last)
	{
		char buffer[500];
		for (int i = first; i < min(last, column.table->allocatedSize); i++)
		{
			sprintf_s(buffer, "%s_%d", columnTitle, i);
			displayElement<T>(buffer, column[i]);
		}
	}
}