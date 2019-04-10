#pragma once
#include <vector>
#include "../transform.h"
#include "../util/table.h"

namespace rotation
{
	namespace anchor
	{
		extern Column<handle> transformHandles;
		extern Column<vec3> offsets;
		extern Column<vec3> anchorPoints;

		void init();
		void update();
		handle add(handle transformHandle, vec3 offset, vec3 anchorPoint);

		void showDebug();
	}

	namespace animation
	{
		extern Column<handle> transformHandles;
		extern Column<float> durations;
		extern Column<float> times;
		extern Column<quat> startRotations;
		extern Column<quat> targetRotations;

		extern std::vector<handle> removed; // all animations finished or manually removed this frame

		void init();
		handle add(handle transformHandle, vec3 offset, vec3 anchorPoint, float duration, quat startRotation, quat targetRotation);
		void update(float deltaTime);

		void showDebug();
	}
}