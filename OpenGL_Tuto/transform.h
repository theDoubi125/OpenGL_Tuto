#pragma once

#define MAX_TRANSFORM_COUNT 500

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "util/table.h"

using quat = glm::quat;
using vec3 = glm::vec3;
using vec2 = glm::vec2;
using ivec3 = glm::ivec3;
using vec4 = glm::vec4;

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

	void init();

	handle add(const vec3& pos, const quat& rotation, const vec3& scale);
	void remove(handle id);

	void update();

	int indexOf(handle id);

	int count();

	void showDebug();
}