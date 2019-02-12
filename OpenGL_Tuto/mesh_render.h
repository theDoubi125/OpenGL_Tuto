#pragma once

#include "transform.h"

#define MAX_MESH_COUNT 100
#define MAX_MESH_RENDER_COUNT 500
#define MESH_DATA_BUFFER_SIZE 10000

class MeshLibrary
{
	float meshData[MESH_DATA_BUFFER_SIZE];

	handle handles[MAX_MESH_COUNT];
	unsigned int meshPos[MAX_MESH_COUNT];
	

	handle loadMesh(float* data, int size);

	const float* operator[](handle index)
	{

	}
};

class MeshRenderer
{
	TransformManager* transforms;
	handle handles[MAX_MESH_RENDER_COUNT];
	handle transformIds[MAX_MESH_RENDER_COUNT];
	handle meshIds[MAX_MESH_RENDER_COUNT];
	int count = 0;

	struct entity
	{
		handle& id;
		handle& transformId;
		handle& meshId;
	};

	handle add(handle mesh, handle transformId, handle meshId);
	
	entity operator[](int index)
	{
		return { handles[index], transformIds[index], meshIds[index] };
	}
};