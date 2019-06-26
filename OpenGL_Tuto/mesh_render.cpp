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
#include <map>

#include "util/profiler/profiler.h"

namespace mesh
{
	namespace library
	{
		Table meshTable;
		Column<unsigned int> vbos;
		Column<size_t> sizes;
		BitArray allocation;

		std::map<std::string, handle> registeredMeshes;

		void init()
		{
			meshTable.init(100, vbos + sizes);
			allocation.init(100);
		}

		handle loadMesh(std::string name, char* data, size_t size)
		{
			handle result = loadMesh(data, size);
			registeredMeshes[name] = result;
			return result;
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

		handle findMesh(std::string name)
		{
			if (registeredMeshes.count(name) > 0)
			{
				return registeredMeshes[name];
			}
			return { -1 };
		}
	}

	namespace render
	{
		namespace main
		{
			Table table;
			BitArray allocation;
			Column<handle> transformIds;
			Column<handle> meshIds;
			Column<GLuint> shaderIds;
			Column<GLuint> vaos;
		}

		namespace count
		{
			// Counts the number of meshes to render for each shader. Used to build the renderDataTable
			Table table;
			BitArray allocation;
			Column<GLuint> shaderIds;
			Column<unsigned int> counts;
			// cursors is a column only used during the renderDataTable building process (in the render function)
			Column<unsigned int> cursors;
		}
		
		namespace render_data
		{
			// A table rebuilt each frame that contains tightly packed render data for each shader in the same order as in countTable
			Table table;
			Column<handle> transformIds;
			Column<handle> meshIds;
			Column<GLuint> vaos;
		}

		void init()
		{
			main::table.init(500, main::transformIds + main::meshIds + main::shaderIds + main::vaos);
			main::allocation.init(500);

			count::table.init(200, count::shaderIds + count::counts + count::cursors);
			count::allocation.init(200);

			render_data::table.init(500, render_data::transformIds + render_data::meshIds + render_data::vaos);
		}

		void addToCount(GLuint shaderId)
		{
			for (auto it = count::allocation.begin(); it.isValid(); it++)
			{
				if (count::shaderIds[*it] == shaderId)
				{
					count::counts[*it]++;
					return;
				}
			}
			TableElement element = count::table.element({ count::allocation.allocate() });
			element << shaderId << (unsigned int)1;
		}

		handle add(handle transformId, handle meshId, GLuint shaderId)
		{
			handle result = { main::allocation.allocate() };
			TableElement element = main::table.element(result.id);

			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			MeshData meshData = library::getMesh(meshId);
			

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

			element << transformId << meshId << shaderId << VAO;

			addToCount(shaderId);

			return result;
		}

		void render()
		{
			P_START("mesh render");
			// reset the cursors column in the count table
			unsigned int cursor = 0;
			for(auto it = count::allocation.begin(); it.isValid(); it++)
			{
				count::cursors[*it] = cursor;
				cursor += count::counts[*it];
			}

			// iterate through the main table to fill the render data
			for (auto it = main::allocation.begin(); it.isValid(); it++)
			{
				GLuint shaderId = main::shaderIds[*it];
				handle transformId = main::transformIds[*it];
				
				unsigned int cursor;
				for (auto it2 = count::allocation.begin(); it2.isValid(); it2++)
				{
					if (count::shaderIds[*it2] == shaderId)
					{
						cursor = count::cursors[*it2];
						count::cursors[*it2]++;
						break;
					}
				}

				render_data::transformIds[cursor] = transformId;
				render_data::meshIds[cursor] = main::meshIds[*it];
				render_data::vaos[cursor] = main::vaos[*it];
			}

			// do the actual render shader by shader
			int renderCursor = 0;
			for (auto it = count::allocation.begin(); it.isValid(); it++)
			{
				GLuint shaderId = count::shaderIds[*it];
				for (int i = 0; i < count::counts[*it]; i++)
				{
					handle transformId = render_data::transformIds[renderCursor];
					glm::mat4 translation = glm::translate(transform::positions[transformId]);
					glm::mat4 scale = glm::scale(transform::scales[transformId]);
					glm::mat4 rotation = glm::toMat4(transform::rotations[transformId]);
					glm::mat4 model = translation * rotation * scale;
					glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, false, (float*)&model);
					glBindVertexArray(render_data::vaos[i]);
					unsigned int vertexCount = mesh::library::getMesh(render_data::meshIds[i]).vertexCount;
					glDrawArrays(GL_TRIANGLES, 0, vertexCount);
					renderCursor++;
				}
				glBindVertexArray(0);
			}
			P_END;
		}
	}
}

handle MeshRenderer::add(handle transformId, handle meshId)
{
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	MeshData meshData = mesh::library::getMesh(meshId);

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