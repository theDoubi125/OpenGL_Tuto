#pragma once

#define MAX_TRANSFORM_COUNT 500

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "util/table.h"

using quat = glm::quat;
using vec3 = glm::vec3;

struct TransformManager
{
	handle ids		[MAX_TRANSFORM_COUNT];
	vec3 positions	[MAX_TRANSFORM_COUNT];
	quat rotations	[MAX_TRANSFORM_COUNT];
	vec3 scales		[MAX_TRANSFORM_COUNT];
	int count = 0;
	int nextId = 0;

	struct entity
	{
		vec3& position;
		quat& rotation;
		vec3& scale;
	};

	handle add(const vec3& pos, const quat& rotation, const vec3& scale);

	entity operator[](handle id);

	int indexOf(handle id) const;
};