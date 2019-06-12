#include "util/bit_array.h"
#include "rotation.h"
#include <iostream>

#include "util/debug/table_display.h"

namespace rotation
{
	namespace anchor
	{
		Table dataTable;
		Column<handle> transformHandles;
		Column<vec3> offsets;
		Column<vec3> anchorPoints;
		std::vector<handle> toRemove;

		BitArray bitArray;

		void init()
		{
			dataTable >> transformHandles >> offsets >> anchorPoints;
			dataTable.allocate(100);
			bitArray.init(100);
		}

		void update()
		{
			for(auto it=bitArray.begin(); it.isValid(); it++)
			{
				int transformIndex = transform::indexOf(transformHandles[*it]);
				quat rotation = transform::rotations[transformIndex];
				transform::positions[transformIndex] = anchorPoints[*it] - rotation * offsets[*it];
				vec3 position = transform::positions[transformIndex];
			}
			for (handle element : toRemove)
			{
				bitArray.free(element.id);
			}
			toRemove.clear();
		}

		handle add(handle transformHandle, vec3 offset, vec3 anchorPoint)
		{
			handle result = { bitArray.allocate() };
			TableElement elt = dataTable.element(result.id);
			elt << transformHandle << offset << anchorPoint;
			return result;
		}

		void remove(handle element)
		{
			toRemove.push_back(element);
		}

		void showDebug()
		{
			static int bounds[2] = { 0, 10 };
			ImGui::DragInt2("Min Max", bounds);
			if (bounds[1] <= bounds[0])
				bounds[1] = bounds[0] + 1;
			ImGui::Columns(4);
			ImGui::Text("Active");
			ImGui::NextColumn();
			ImGui::Text("transformHandles");
			ImGui::NextColumn();
			ImGui::Text("offsets");
			ImGui::NextColumn();
			ImGui::Text("anchorPoints");
			ImGui::NextColumn();

			for (int i = bounds[0]; i < bounds[1]; i++)
			{
				bool isActive = bitArray.value(i);
				if (ImGui::Checkbox("active", &isActive))
				{

				}
				ImGui::NextColumn();
				debug::displayElement("transformHandles", transformHandles[i]);
				ImGui::NextColumn();
				debug::displayElement("offsets", offsets[i]);
				ImGui::NextColumn();
				debug::displayElement("anchorPoints", anchorPoints[i]);
				ImGui::NextColumn();
			}
			ImGui::Columns();
		}
	}

	namespace animation
	{
		Table dataTable;
		Column<handle> anchorHandles;
		Column<handle> transformHandles;
		Column<float> durations;
		Column<float> times;
		Column<quat> startRotations;
		Column<quat> targetRotations;
		BitArray bitArray;
		std::vector<handle> removed;

		handle nextHandle = { 0 };

		void init()
		{
			dataTable.init(50, anchorHandles + transformHandles + durations + times + startRotations + targetRotations);
			bitArray.init(50);
		}

		handle add(handle transformHandle, vec3 offset, vec3 anchorPoint, float duration, quat startRotation, quat targetRotation)
		{
			handle result = { bitArray.allocate() };
			TableElement elt = dataTable.element(result.id);
			handle anchorHandle = anchor::add(transformHandle, offset, anchorPoint);
			elt << anchorHandle << transformHandle << duration << 0 << startRotation << targetRotation;
			return result;
		}
		
		void update(float deltaTime)
		{
			removed.clear();
			std::vector<int> toRemove;
			for (int i : bitArray)
			{
				handle transformHandle = transformHandles[i];
				int transformIndex = transform::indexOf(transformHandle);
				times[i] += deltaTime;
				float animRatio = times[i] / durations[i];
				quat startRotation = startRotations[i];
				quat targetRotation = targetRotations[i];
				transform::rotations[transformIndex] = glm::mix(glm::normalize(startRotations[i]), glm::normalize(targetRotations[i]), animRatio < 1 ? animRatio : 1);
				
				if (times[i] > durations[i])
				{
					toRemove.push_back(i);
				}
			}
			for (int i = 0; i < toRemove.size(); i++)
			{
				int index = toRemove[i];
				anchor::remove(anchorHandles[index]);
				bitArray.free(index);
				removed.push_back({ index });
			}
		}

		void showDebug()
		{

		}
	}
}