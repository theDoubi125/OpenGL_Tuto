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
			handles = dataTable.addColumn<handle>();
			transformHandles = dataTable.addColumn<handle>();
			offsets = dataTable.addColumn<vec3>();
			anchorPoints = dataTable.addColumn<vec3>();
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
			dataTable.push() << result << transformHandle << offset << anchorPoint;

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
			handles = dataTable.addColumn<handle>();
			transformHandles = dataTable.addColumn<handle>();
			durations = dataTable.addColumn<float>();
			times = dataTable.addColumn<float>();
			startRotations = dataTable.addColumn<quat>();
			targetRotations = dataTable.addColumn<quat>();
			dataTable.allocate(50);
		}

		handle add(handle transformHandle, float duration, quat startRotation, quat targetRotation)
		{
			handle result = nextHandle;
			dataTable.push() << result << transformHandle << duration << startRotation << targetRotation;
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
				transform::rotations[transformIndex] = glm::mix(glm::normalize(startRotations[i]), glm::normalize(targetRotations[i]), animRatio < 1 ? animRatio : 1);
			}
		}
	}
}