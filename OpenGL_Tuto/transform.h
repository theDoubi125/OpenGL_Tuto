#pragma once

#define MAX_TRANSFORM_COUNT 500

struct handle
{
	int id;

	void operator=(const handle& handle);
	bool operator<(const handle& handle) const;
};

class TransformManager
{
	handle ids		[MAX_TRANSFORM_COUNT];
	vec3 positions	[MAX_TRANSFORM_COUNT];
	quat rotations	[MAX_TRANSFORM_COUNT];
	int count = 0;
	int nextId = 0;

	struct entity
	{
		vec3& position;
		quat& rotation;
	};

	handle add(vec3 pos, quat rotation);

	entity operator[](int index);
};