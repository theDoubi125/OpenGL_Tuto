#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

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

handle TransformManager::add(vec3 pos, quat rotation)
{
	if (count >= MAX_TRANSFORM_COUNT)
	{
		std::cerr << "Transform manager full" << std::endl;
	}
	else
	{
		handle id = { nextId };
		ids[count] = id;
		positions[count] = pos;
		rotations[count] = rotation;
		count++;
		nextId++;
		return id;
	}
	return { -1 };
}

TransformManager::entity TransformManager::operator[](int index)
{
	return { positions[index], rotations[index] };
}