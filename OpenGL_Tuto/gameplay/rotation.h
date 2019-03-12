#pragma once
#include <vector>
#include "../transform.h"
#include "../util/table.h"

struct AnchoredRotationTable
{
	AnchoredRotationTable();

	Column<handle> handles;
	Column<handle> transformHandles;
	Column<vec3> offsets;
	Column<vec3> anchorPoints;

	struct data {
		handle h;
		handle transformHandle;
		vec3 offset;
		vec3 anchorPoint;
	};

	Table dataTable;

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