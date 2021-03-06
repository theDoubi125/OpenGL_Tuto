#include <glad/glad.h>
#include "../shader_m.h"
#include "../camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


#include "../imgui.h"

#include "scene/scene.h"
#include "util/profiler/profiler.h"
#include "render/debug/render_debug_line.h"

#include <map>

namespace render
{
	namespace shaders
	{
		namespace gBuffer
		{
			GLuint shader;
			GLuint diffuseAttr;
			GLuint specularAttr;
			GLuint modelAttr;
			GLuint projectionAttr;
			GLuint viewAttr;
			GLuint viewPosAttr;
		}

		namespace deferredLighting
		{
			GLuint shader;
			GLuint gPositionAttr, gNormalAttr, gAlbedoSpecAttr, gLightAttr;
		}

		namespace renderQuad
		{

		}
	}
	unsigned int renderQuadShader = 0;

	unsigned int gBuffer, gLightFrameBuffer, renderFrameBuffer;
	unsigned int gPosition, gNormal, gColorSpec, gLight, gDepth;

	unsigned int screenRenderVao, screenRenderVbo;

	unsigned int render_width, render_height;

	unsigned int finalTexture;

	int projectionMatrixAttr;
	int viewMatrixAttr;
	int viewPosAttr;

	void init_screen_render()
	{
		float quadVertices[] = {
			-1, -1, 0, 0,		1, -1, 1, 0,	1, 1, 1, 1,
			-1, -1, 0, 0,		1, 1, 1, 1,		-1, 1, 0, 1
		};

		glGenBuffers(1, &screenRenderVbo);
		glGenVertexArrays(1, &screenRenderVao);
		glBindVertexArray(screenRenderVao);

		glBindBuffer(GL_ARRAY_BUFFER, screenRenderVbo);

		glBindBuffer(GL_ARRAY_BUFFER, screenRenderVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, screenRenderVbo);

		// position attribute
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT)
	{
		shaders::gBuffer::shader = shader::compileShader("./shaders/gbuffer.vert", "./shaders/gbuffer.frag");
		renderQuadShader = shader::compileShader("./shaders/render_quad.vert", "./shaders/render_quad.frag");
		shaders::deferredLighting::shader = shader::compileShader("./shaders/deferred_lighting.vert", "./shaders/deferred_lighting.frag");

		shaders::gBuffer::projectionAttr = glGetUniformLocation(shaders::gBuffer::shader, "projection");
		shaders::gBuffer::viewAttr = glGetUniformLocation(shaders::gBuffer::shader, "view");
		shaders::gBuffer::viewPosAttr = glGetUniformLocation(shaders::gBuffer::shader, "viewPos");
		shaders::gBuffer::diffuseAttr = glGetUniformLocation(shaders::gBuffer::shader, "diffuse");
		shaders::gBuffer::modelAttr = glGetUniformLocation(shaders::gBuffer::shader, "model");
		shaders::gBuffer::specularAttr = glGetUniformLocation(shaders::gBuffer::shader, "specular");
		shaders::deferredLighting::gPositionAttr = glGetUniformLocation(shaders::deferredLighting::shader, "gPosition");
		shaders::deferredLighting::gNormalAttr = glGetUniformLocation(shaders::deferredLighting::shader, "gNormal");
		shaders::deferredLighting::gAlbedoSpecAttr = glGetUniformLocation(shaders::deferredLighting::shader, "gAlbedoSpec");
		shaders::deferredLighting::gLightAttr = glGetUniformLocation(shaders::deferredLighting::shader, "gLight");

		init_screen_render();

		glGenFramebuffers(1, &gBuffer);
		glGenFramebuffers(1, &gLightFrameBuffer);
		glGenFramebuffers(1, &renderFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// - position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// - normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);


		// - color + specular color buffer
		glGenTextures(1, &gColorSpec);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

		unsigned int rboDepth;
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);


		// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, gLightFrameBuffer);
		// light buffer
		glGenTextures(1, &gLight);
		glBindTexture(GL_TEXTURE_2D, gLight);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gLight, 0);

		glDrawBuffer(GL_COLOR_ATTACHMENT3);

		glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		glGenTextures(1, &finalTexture);
		glBindTexture(GL_TEXTURE_2D, finalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTexture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		render_width = SCR_WIDTH;
		render_height = SCR_HEIGHT;

		projectionMatrixAttr =	shaders::gBuffer::projectionAttr;
		viewMatrixAttr = shaders::gBuffer::viewAttr;
		viewPosAttr =	shaders::gBuffer::viewPosAttr;
	}

	void clear_frame()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void start_render()
	{
		P_START("start render");
		glUseProgram(shaders::gBuffer::shader);

		glm::mat4 projection = glm::perspective(glm::radians(camera::getZoom(camera::mainCamera)), (float)render_width / (float)render_height, 0.1f, 1000.0f);
		glm::mat4 view = camera::getViewMatrix(camera::mainCamera);
		glUniformMatrix4fv(projectionMatrixAttr, 1, false, (float*)&projection);
		glUniformMatrix4fv(viewMatrixAttr, 1, false, (float*)&view);
		glm::vec3 cameraPos = camera::getCameraPos(camera::mainCamera);
		glUniform3f(viewPosAttr, cameraPos.x, cameraPos.y, cameraPos.z);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		P_END;
	}

	void start_lighting()
	{
		P_START("start lighting");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gLight);
		glBindVertexArray(screenRenderVao);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);
		glBindFramebuffer(GL_FRAMEBUFFER, gLightFrameBuffer);

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		P_END;
	}

	void render_deferred()
	{
		P_START("render deferred");
		glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);

		glUseProgram(shaders::deferredLighting::shader);

		
		glUniform1i(shaders::deferredLighting::gPositionAttr, 0);
		glUniform1i(shaders::deferredLighting::gNormalAttr, 1);
		glUniform1i(shaders::deferredLighting::gAlbedoSpecAttr, 2);
		glUniform1i(shaders::deferredLighting::gLightAttr, 3);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);
		P_END;
	}

	void render_screen(int texture)
	{
		glDisable(GL_DEPTH_TEST);
		P_START("render screen");
		glEnable(GL_FRAMEBUFFER_SRGB);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(renderQuadShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(screenRenderVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glDisable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		glBlitFramebuffer(
			0, 0, render_width, render_height, 0, 0, render_width, render_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLineWidth(20);
		render::debug::render();
		glDisable(GL_BLEND);
		P_END;
	}

	void debug_texture_selector(unsigned int* selectedTexture)
	{

		if (ImGui::Selectable("Position"))
		{
			*selectedTexture = gPosition;
		}
		if (ImGui::Selectable("Colors"))
		{
			*selectedTexture = gColorSpec;
		}
		if (ImGui::Selectable("Normal"))
		{
			*selectedTexture = gNormal;
		}
		if (ImGui::Selectable("Light"))
		{
			*selectedTexture = gLight;
		}
		if (ImGui::Selectable("Final"))
		{
			*selectedTexture = finalTexture;
		}
	}
}