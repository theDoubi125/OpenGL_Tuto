#pragma once
#include <vector>
#include "../transform.h"
#include "../util/table.h"

namespace rotation
{
	namespace anchor
	{
		extern Column<handle> handles;
		extern Column<handle> transformHandles;
		extern Column<vec3> offsets;
		extern Column<vec3> anchorPoints;

		void init();
		void update();
		handle add(handle transformHandle, vec3 offset, vec3 anchorPoint);
	}

	namespace animation
	{
		extern Column<handle> transformHandles;
		extern Column<float> durations;
		extern Column<float> times;
		extern Column<quat> startRotations;
		extern Column<quat> targetRotations;

		void init();
		handle add(handle transformHandle, float duration, quat startRotation, quat targetRotation);
		void update(float deltaTime);
	}
}