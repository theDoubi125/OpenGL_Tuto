#include <iostream>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "util/bit_array.h"

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

	std::vector<handle> toRemove;

	Column<handle> ids;
	Column<vec3> positions;
	Column<quat> rotations;
	Column<vec3> scales;
	BitArray bitArray;

	void init()
	{
		table >> ids >> positions >> rotations >> scales;
		table.allocate(500);
		bitArray.init(500);
	}

	handle add(const vec3& pos, const quat& rotation, const vec3& scale)
	{
		int index = bitArray.allocate();
		handle result = { index };
		TableElement elt = table.element(index);
		elt << result << pos << rotation << scale;
		return result;
	}

	int indexOf(handle id)
	{
		return id.id;
	}

	int count()
	{
		return table.count;
	}

	void remove(handle id)
	{
		toRemove.push_back(id);
	}

	void update()
	{
		for (handle toRemoveHandle : toRemove)
		{
			bitArray.free(toRemoveHandle.id);
		}
		toRemove.clear();
	}
}