#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "util/bit_array.h"
#include "imgui.h"
#include "util/debug/table_display.h"

using vec3 = glm::vec3;
using quat = glm::quat;

#include "transform.h"

void handle::operator=(const handle& handle)
{
	id = handle.id;
}

bool handle::operator<(const handle& handle) const
{
	return id < handle.id;
}

bool handle::operator==(const handle& handle) const
{
	return id == handle.id;
}

namespace transform
{
	Table table;

	std::vector<handle> toRemove;

	Column<handle> ids;
	Column<vec3> positions;
	Column<quat> rotations;
	Column<vec3> scales;
	BitArray bitArray;

	void init()
	{
		table.init(500, ids + positions + rotations + scales);
		bitArray.init(500);
	}

	handle add(const vec3& pos, const quat& rotation, const vec3& scale)
	{
		int index = bitArray.allocate();
		handle result = { index };
		TableElement elt = table.element(index);
		elt << result << pos << rotation << scale;
		return result;
	}

	int indexOf(handle id)
	{
		return id.id;
	}

	int count()
	{
		return table.count;
	}

	void remove(handle id)
	{
		toRemove.push_back(id);
	}

	void update()
	{
		for (handle toRemoveHandle : toRemove)
		{
			bitArray.free(toRemoveHandle.id);
		}
		toRemove.clear();
	}

	void showDebug()
	{
		static int bounds[2] = { 0, 10 };
		ImGui::DragInt2("Min Max", bounds);
		if (bounds[1] <= bounds[0])
			bounds[1] = bounds[0] + 1;
		ImGui::Columns(3);
		ImGui::Text("positions");
		ImGui::NextColumn();
		ImGui::Text("rotations");
		ImGui::NextColumn();
		ImGui::Text("scales");
		ImGui::NextColumn();

		for (int i = bounds[0]; i < bounds[1]; i++)
		{
			char idBuffer[10];
			sprintf_s(idBuffer, "%d", i);
			ImGui::PushID(idBuffer);
				bool isActive = bitArray.value(i);
				debug::displayElement("positions", positions[i]);
				ImGui::NextColumn();
				debug::displayElement("rotations", rotations[i]);
				ImGui::NextColumn();
				debug::displayElement("scales", scales[i]);
				ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns();


	}
}