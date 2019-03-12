#include "rotation.h"
#include <iostream>

AnchoredRotationTable::AnchoredRotationTable()
{
	handles = dataTable.addColumn<handle>();
	transformHandles = dataTable.addColumn<handle>();
	offsets = dataTable.addColumn<vec3>();
	anchorPoints = dataTable.addColumn<vec3>();
	dataTable.allocate(10);
}

void AnchoredRotationTable::update()
{
	for (int i = 0; i < dataTable.count; i++)
	{
		int transformIndex = transforms->indexOf(transformHandles[i]);
		quat rotation = transforms->rotations[transformIndex];
		transforms->positions[transformIndex] = anchorPoints[i] - rotation * offsets[i];
	}
}


handle AnchoredRotationTable::add(handle transformHandle, vec3 offset, vec3 anchorPoint)
{
	handle result = nextHandle;
	dataTable.push<data>({ result, transformHandle, offset, anchorPoint });
	
	nextHandle.id++;
	return result;
}


handle AnimatedRotationTable::add(handle transformHandle, float duration, quat startRotation, quat targetRotation)
{
	handle result = nextHandle;
	transformHandles.push_back(transformHandle);
	durations.push_back(duration);
	startRotations.push_back(startRotation);
	targetRotations.push_back(targetRotation);
	times.push_back(0);
	nextHandle.id++;
	return result;
}

void AnimatedRotationTable::update(float deltaTime)
{
	for (int i = 0; i < transformHandles.size(); i++)
	{
		int transformIndex = transforms->indexOf(transformHandles[i]);
		times[i] += deltaTime;
		float animRatio = times[i] / durations[i];
		transforms->rotations[transformIndex] = glm::mix(glm::normalize(startRotations[i]), glm::normalize(targetRotations[i]), animRatio < 1 ? animRatio : 1);
	}
}