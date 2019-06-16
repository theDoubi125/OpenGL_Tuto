#include <glm/vec3.hpp>
#include <glad/glad.h>
using vec3 = glm::vec3;

#include "mesh_render.h"
#include <cstring>
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "util/bit_array.h"

namespace mesh
{
	Table meshTable;
	Column<unsigned int> vbos;
	Column<size_t> sizes;
	BitArray allocation;

	void init()
	{
		meshTable.init(100, vbos + sizes);
		allocation.init(100);
	}

	handle loadMesh(char* data, size_t size)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		handle result = { allocation.allocate() };
		TableElement element = meshTable.element(result.id);
		element << VBO << size / 8 / sizeof(float);
		return result;
	}

	void replaceMesh(char* data, size_t size, handle meshId)
	{
		unsigned int VBO = vbos[meshId];

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		sizes[meshId] = size / 8 / sizeof(float);
	}

	MeshData getMesh(handle meshId)
	{
		return { vbos[meshId], sizes[meshId] };
	}
}

handle MeshRenderer::add(handle transformId, handle meshId)
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	MeshData meshData = mesh::getMesh(meshId);

	glBindBuffer(GL_ARRAY_BUFFER, meshData.vbo);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	handle result = nextHandle;
	handles[count] = result;
	VAOs[count] = VAO;
	vertexCount[count] = meshData.vertexCount;
	transformIds[count] = transformId;
	count++;
	nextHandle.id++;
	return result;
}

MeshRenderer::entity MeshRenderer::operator[](int index)
{
	return { handles[index], transformIds[index], VAOs[index] };
}

void MeshRenderer::getPositions(vec3* outPos, size_t maxCount) const
{
	for (int i = 0; i < count; i++)
	{
		int transformIndex = transform::indexOf(transformIds[i]);
		outPos[i] = transform::positions[transformIndex];
	}
}

void MeshRenderer::render(unsigned int shaderId)
{
	for (int i = 0; i < count; i++)
	{
		int transformIndex = transform::indexOf(transformIds[i]);
		// world transformation
		glm::mat4 translation = glm::translate(transform::positions[transformIndex]);
		glm::mat4 scale = glm::scale(transform::scales[transformIndex]);
		glm::mat4 rotation = glm::toMat4(transform::rotations[transformIndex]);
		glm::mat4 model = translation * rotation * scale;

		glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, false, (float*)&model);
		glBindVertexArray(VAOs[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount[i]);
	}
	glBindVertexArray(0);
}

namespace screen_effect
{
	unsigned int vbo;
	unsigned int vao;

	void init()
	{
		float quadVertices[] = {
			-1, -1, 0, 0,		1, -1, 1, 0,	1, 1, 1, 1,
			-1, -1, 0, 0,		1, 1, 1, 1,		-1, 1, 0, 1
		};

		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}