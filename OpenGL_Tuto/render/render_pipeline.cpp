#include "../shader_m.h"
#include "../camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


#include "../imgui.h"

#include "scene/scene.h"

namespace render
{
	unsigned int gBufferShader = 0, deferredLightingShader = 0;
	unsigned int renderQuadShader = 0;

	unsigned int gBuffer, gLightFrameBuffer, renderFrameBuffer;
	unsigned int gPosition, gNormal, gColorSpec, gLight, gDepth;

	unsigned int screenRenderVao, screenRenderVbo;

	unsigned int render_width, render_height;

	unsigned int finalTexture;

	unsigned int currentShader = 0;

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
		gBufferShader = shader::compileShader("./shaders/gbuffer.vert", "./shaders/gbuffer.frag");
		renderQuadShader = shader::compileShader("./shaders/render_quad.vert", "./shaders/render_quad.frag");
		deferredLightingShader = shader::compileShader("./shaders/deferred_lighting.vert", "./shaders/deferred_lighting.frag");

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

	}

	void clear_frame()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void start_render(const Camera& camera)
	{
		glUseProgram(gBufferShader);
		currentShader = gBufferShader;

		glm::mat4 projection = glm::perspective(glm::radians(camera::getZoom(scene::cameraId)), (float)render_width / (float)render_height, 0.1f, 100.0f);
		glm::mat4 view = camera::getViewMatrix(scene::cameraId);

		int projectionMatrixAttr = glGetUniformLocation(gBufferShader, "projection");
		int viewMatrixAttr = glGetUniformLocation(gBufferShader, "view");
		int viewPosAttr = glGetUniformLocation(gBufferShader, "viewPos");
		glUniformMatrix4fv(projectionMatrixAttr, 1, false, (float*)&projection);
		glUniformMatrix4fv(viewMatrixAttr, 1, false, (float*)&view);
		glm::vec3 cameraPos = camera::getCameraPos(scene::cameraId);
		glUniform3f(viewPosAttr, cameraPos.x, cameraPos.y, cameraPos.z);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void start_lighting()
	{
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
	}

	void render_deferred()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);

		glUseProgram(deferredLightingShader);
		currentShader = deferredLightingShader;

		int gPositionAttr = glGetUniformLocation(deferredLightingShader, "gPosition");
		int gNormalAttr = glGetUniformLocation(deferredLightingShader, "gNormal");
		int gAlbedoSpecAttr = glGetUniformLocation(deferredLightingShader, "gAlbedoSpec");
		int gLightAttr = glGetUniformLocation(deferredLightingShader, "gLight");
		
		glUniform1i(gPositionAttr, 0);
		glUniform1i(gNormalAttr, 1);
		glUniform1i(gAlbedoSpecAttr, 2);
		glUniform1i(gLightAttr, 3);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(true);
	}

	void render_screen(int texture)
	{
		glEnable(GL_FRAMEBUFFER_SRGB);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(renderQuadShader);
		currentShader = renderQuadShader;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(screenRenderVao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glDisable(GL_FRAMEBUFFER_SRGB);
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