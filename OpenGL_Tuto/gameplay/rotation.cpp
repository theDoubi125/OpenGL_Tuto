#include "rotation.h"
#include <iostream>

namespace rotation
{
	namespace anchor
	{
		Table dataTable;
		handle nextHandle = { 0 };
		Column<handle> handles;
		Column<handle> transformHandles;
		Column<vec3> offsets;
		Column<vec3> anchorPoints;

		void init()
		{
			dataTable >> handles >> transformHandles >> offsets >> anchorPoints;
			dataTable.allocate(100);
		}

		void update()
		{
			for (int i = 0; i < dataTable.count; i++)
			{
				int transformIndex = transform::indexOf(transformHandles[i]);
				quat rotation = transform::rotations[transformIndex];
				transform::positions[transformIndex] = anchorPoints[i] - rotation * offsets[i];
			}
		}

		handle add(handle transformHandle, vec3 offset, vec3 anchorPoint)
		{
			handle result = nextHandle;
			TableElement elt = dataTable.push();
			elt << result << transformHandle << offset << anchorPoint;

			nextHandle.id++;
			return result;
		}
	}

	namespace animation
	{
		Table dataTable;
		Column<handle> handles;
		Column<handle> transformHandles;
		Column<float> durations;
		Column<float> times;
		Column<quat> startRotations;
		Column<quat> targetRotations;

		handle nextHandle = { 0 };

		void init()
		{
			dataTable >> handles >> transformHandles >> durations >> times >> startRotations >> targetRotations;
			dataTable.allocate(50);
		}

		handle add(handle transformHandle, float duration, quat startRotation, quat targetRotation)
		{
			handle result = nextHandle;
			TableElement elt = dataTable.push();
			elt << result << transformHandle << duration << 0 << startRotation << targetRotation;
			nextHandle.id++;
			return result;
		}
		
		void update(float deltaTime)
		{
			for (int i = 0; i < dataTable.count; i++)
			{
				int transformIndex = transform::indexOf(transformHandles[i]);
				times[i] += deltaTime;
				float animRatio = times[i] / durations[i];
				quat startRotation = startRotations[i];
				quat targetRotation = targetRotations[i];
				transform::rotations[transformIndex] = glm::mix(glm::normalize(startRotations[i]), glm::normalize(targetRotations[i]), animRatio < 1 ? animRatio : 1);
			}
		}
	}
}