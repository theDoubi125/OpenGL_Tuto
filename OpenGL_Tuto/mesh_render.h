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

class MeshLibrary
{
	std::map<const std::string, MeshData> loadedMeshes;
	
public:
	MeshData loadMesh(const std::string& name, float* data, size_t size);

	MeshData getMesh(const std::string& name);
};

class MeshRenderer
{
	handle handles[MAX_MESH_RENDER_COUNT];
	handle transformIds[MAX_MESH_RENDER_COUNT];
	unsigned int VAOs[MAX_MESH_RENDER_COUNT];
	unsigned int vertexCount[MAX_MESH_RENDER_COUNT];
	int count = 0;
	handle nextHandle = { 0 };

	struct entity
	{
		handle& id;
		handle& transformId;
		unsigned int& VAO;
	};

public:
	TransformManager* transforms;
	Shader* shader = nullptr;

	handle add(handle transformId, const MeshData& mesh);
	void getPositions(vec3* outPos, size_t maxCount) const;

	void initRender();
	void render();
	
	entity operator[](int index);
};