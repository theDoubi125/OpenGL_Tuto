#include "render_debug_line.h"
#include <glad/glad.h>
#include "shader_m.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "camera.h"

namespace render
{
	namespace debug
	{
		Table table;
		Column<vec3> points;
		Column<vec4> colors;

		GLuint vao, vbo;
		GLuint shaderId;

		unsigned int render_width, render_height;
		GLuint projectionMatrixAttr, viewMatrixAttr;

		void init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
		{
			render_width = SCR_WIDTH;
			render_height = SCR_HEIGHT;
			table.init(1400, points + colors);
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); // positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))); // colors
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			shaderId = shader::compileShader("./shaders/debug.vert", "./shaders/debug.frag");
			projectionMatrixAttr = glGetUniformLocation(shaderId, "projection");
			viewMatrixAttr = glGetUniformLocation(shaderId, "view");
		}

		void drawLine(vec3 A, vec3 B, vec4 color)
		{
			TableElement element = table.push();
			element << A << color;
			element = table.push();
			element << B << color;
		}

		void render()
		{
			float data[500 * 2 * 7];

			for (int i = 0; i < table.count; i++)
			{
				memcpy((char*)&(data[i * 7]), (char*)&points[i], sizeof(vec3));
				memcpy((char*)&(data[i * 7 + 3]), (char*)&colors[i], sizeof(vec4));
			}
			glUseProgram(shaderId);
			glm::mat4 projection = glm::perspective(glm::radians(camera::getZoom(camera::mainCamera)), (float)render_width / (float)render_height, 0.1f, 1000.0f);
			glm::mat4 view = camera::getViewMatrix(camera::mainCamera);
			glUniformMatrix4fv(projectionMatrixAttr, 1, false, (float*)&projection);
			glUniformMatrix4fv(viewMatrixAttr, 1, false, (float*)&view);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 7 * table.count, data, GL_DYNAMIC_DRAW);
			glBindVertexArray(vao);
			glDrawArrays(GL_LINES, 0, table.count);
			glBindVertexArray(0);
			glUseProgram(0);
			table.clear();
		}

		void drawCube(vec3 center, vec3 size, vec4 color)
		{
			vec3 As[] = {
				vec3(0, 0, 0), vec3(1, 0, 0),
				vec3(1, 0, 0), vec3(1, 1, 0),
				vec3(1, 1, 0), vec3(0, 1, 0),
				vec3(0, 1, 0), vec3(0, 0, 0),
				vec3(0, 0, 1), vec3(1, 0, 1),
				vec3(1, 0, 1), vec3(1, 1, 1),
				vec3(1, 1, 1), vec3(0, 1, 1),
				vec3(0, 1, 1), vec3(0, 0, 1),
				vec3(0, 0, 0), vec3(0, 0, 1),
				vec3(1, 0, 0), vec3(1, 0, 1),
				vec3(1, 1, 0), vec3(1, 1, 1),
				vec3(0, 1, 0), vec3(0, 1, 1)
			};

			for (int i = 0; i < 12; i++)
			{
				drawLine(center - size * 0.5f + As[i * 2], center - size * 0.5f + As[i * 2 + 1], color);
			}
		}
	}
}