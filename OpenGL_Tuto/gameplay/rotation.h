#pragma once
#include <vector>
#include "../transform.h"

struct AnchoredRotationTable
{
	std::vector<handle> handles;
	std::vector<handle> transformHandles;
	std::vector<vec3> offsets;
	std::vector<vec3> anchorPoints;
	handle nextHandle = { 0 };

	TransformManager* transforms;
	handle add(handle transformHandle, vec3 offset, vec3 anchorPoint);
	void update();
};

struct AnimatedRotationTable
{
	TransformManager* transforms;
	std::vector<handle> transformHandles;
	std::vector<float> durations;
	std::vector<float> times;
	std::vector<quat> startRotations;
	std::vector<quat> targetRotations;
	handle nextHandle = { 0 };

	handle add(handle transformHandle, float duration, quat startRotation, quat targetRotation);
	void update(float deltaTime);
};