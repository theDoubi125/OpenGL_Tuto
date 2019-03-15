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

namespace transform
{
	Table table;

	Column<handle> ids;
	Column<vec3> positions;
	Column<quat> rotations;
	Column<vec3> scales;

	handle nextHandle = {0};

	void init()
	{
		table >> ids >> positions >> rotations >> scales;
		table.allocate(500);
	}

	handle add(const vec3& pos, const quat& rotation, const vec3& scale)
	{
		handle result = nextHandle;
		TableElement elt = table.push();
		elt << result << pos << rotation << scale;
		nextHandle.id++;
		return result;
	}

	int indexOf(handle id)
	{
		for (int i = 0; i < table.count; i++)
		{
			if (ids[i] == id)
				return i;
		}
		return -1;
	}

	int count()
	{
		return table.count;
	}
}