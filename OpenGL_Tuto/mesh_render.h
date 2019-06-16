#pragma once

#include "transform.h"

#define MAX_MESH_COUNT 100
#define MAX_MESH_RENDER_COUNT 500
#define MESH_DATA_BUFFER_SIZE 10000
#define MAX_MESH_VERTEX 1000

#include <map>
#include "shader_m.h"

struct MeshData
{
	unsigned int vbo;
	size_t vertexCount;
};

namespace mesh
{
	void init();
	handle loadMesh(char* data, size_t size);
	void replaceMesh(char* data, size_t size, handle meshId);
	MeshData getMesh(handle meshId);
}

class MeshRenderer
{
	handle handles[MAX_MESH_RENDER_COUNT];
	handle transformIds[MAX_MESH_RENDER_COUNT];
	unsigned int VAOs[MAX_MESH_RENDER_COUNT];
	unsigned int vertexCount[MAX_MESH_RENDER_COUNT];
	handle nextHandle = { 0 };

	struct entity
	{
		handle& id;
		handle& transformId;
		unsigned int& VAO;
	};

public:
	int count = 0;

	handle add(handle transformId, handle meshId);
	void getPositions(vec3* outPos, size_t maxCount) const;

	void render(unsigned int shaderId);
	
	entity operator[](int index);
};

namespace screen_effect
{
	extern unsigned int vbo, vao;

	void init();
}