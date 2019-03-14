#pragma once

#define MAX_TRANSFORM_COUNT 500

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "util/table.h"

using quat = glm::quat;
using vec3 = glm::vec3;

struct TransformManager
{
	Column<handle> ids;
	Column<vec3> positions;
	Column<quat> rotations;
	Column<vec3> scales;
	int count = 0;
	int nextId = 0;

	Table dataTable;

	struct entity
	{
		vec3& position;
		quat& rotation;
		vec3& scale;
	};

	TransformManager();

	handle add(const vec3& pos, const quat& rotation, const vec3& scale);

	entity operator[](handle id);

	int indexOf(handle id) const;
};

namespace transform
{
	struct entity
	{
		vec3& position;
		quat& rotation;
		vec3& scale;
	};

	extern Column<handle> ids;
	extern Column<vec3> positions;
	extern Column<quat> rotations;
	extern Column<vec3> scales;

	handle add(const vec3& pos, const quat& rotation, const vec3& scale);

	int indexOf(handle id);
}