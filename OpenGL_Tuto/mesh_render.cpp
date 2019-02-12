#include <glm/vec3.hpp>
#include <glad/glad.h>
using vec3 = glm::vec3;

#include "mesh_render.h"
#include <cstring>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

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
		outPos[i] = ((*transforms)[transformIds[i]]).position;
	}
}

void MeshRenderer::initRender()
{

}

void MeshRenderer::render()
{
	for (int i = 0; i < count; i++)
	{
		TransformManager::entity entity = (*transforms)[transformIds[i]];
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, entity.position);
		model = glm::scale(model, entity.scale);
		shader->setMat4("model", model);
		shader->use();
		glBindVertexArray(VAOs[i]);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount[i]);
	}
	glBindVertexArray(0);
}