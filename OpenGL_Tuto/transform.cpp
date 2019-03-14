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

bool handle::operator==(const handle& handle) const
{
	return id == handle.id;
}

TransformManager::TransformManager()
{
	ids = dataTable.addColumn<handle>();
	positions = dataTable.addColumn<vec3>();
	rotations = dataTable.addColumn<quat>();
	scales = dataTable.addColumn<vec3>();
	dataTable.allocate(500);
}

handle TransformManager::add(const vec3& pos, const quat& rotation, const vec3& scale)
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
		scales[count] = scale;
		count++;
		nextId++;
		return id;
	}
	return { -1 };
}

TransformManager::entity TransformManager::operator[](handle id)
{
	for (int i = 0; i < count; i++)
	{
		if (ids[i] == id)
		{
			return { positions[i], rotations[i], scales[i] };

		}
	}
}

int TransformManager::indexOf(handle id) const
{
	for (int i = 0; i < count; i++)
	{
		if (ids[i] == id)
		{
			return i;
		}
	}
	return -1;
}