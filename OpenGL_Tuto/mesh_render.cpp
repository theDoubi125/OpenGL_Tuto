#include <glm/vec3.hpp>
#include <glad/glad.h>
using vec3 = glm::vec3;

#include "mesh_render.h"
#include <cstring>
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


MeshData MeshLibrary::loadMesh(const std::string& name, float* data, size_t size)
{
	if (loadedMeshes.count(name) == 0)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		loadedMeshes[name] = { VBO, size / 8 / sizeof(float) };
	}

	return loadedMeshes[name];
}

MeshData MeshLibrary::getMesh(const std::string& name)
{
	return loadedMeshes[name];
}

handle MeshRenderer::add(handle transformId, const MeshData& mesh)
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

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
	vertexCount[count] = mesh.vertexCount;
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