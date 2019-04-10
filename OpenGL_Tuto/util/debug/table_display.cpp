#include "table_display.h"
#include "transform.h"

namespace debug
{
	template<>
	bool displayElement<float>(const char* title, float& element)
	{
		return ImGui::DragFloat(title, &element);
	}

	template<>
	bool displayElement<vec3>(const char* title, vec3& element)
	{
		return ImGui::DragFloat3(title, (float*)&element, 0.01f);
	}

	template<>
	bool displayElement<handle>(const char* title, handle& element)
	{
		return ImGui::DragInt(title, &element.id);
	}
}