#pragma once

#include "transform.h"

#define MAX_MESH_COUNT 100
#define MAX_MESH_RENDER_COUNT 500
#define MESH_DATA_BUFFER_SIZE 10000
#define MAX_MESH_VERTEX 1000

#include <map>

class MeshLibrary
{
	std::map<const std::string, unsigned int> loadedMeshes;
	

	unsigned int loadMesh(const std::string& name, float* data, size_t size);

	unsigned int getMesh(const std::string& name);
};

class MeshRenderer
{
	TransformManager* transforms;
	handle handles[MAX_MESH_RENDER_COUNT];
	handle transformIds[MAX_MESH_RENDER_COUNT];
	unsigned int VAOs[MAX_MESH_RENDER_COUNT];
	int count = 0;
	handle nextHandle = { 0 };

	unsigned int shader;

	struct entity
	{
		handle& id;
		handle& transformId;
		unsigned int& VAO;
	};

	handle add(handle transformId, unsigned int VBO);
	void getPositions(vec3* outPos, size_t maxCount) const;

	void initRender();
	void render();
	
	entity operator[](int index);
};