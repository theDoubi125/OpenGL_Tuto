#include <glad/glad.h>

#include "transform.h"
#include "shadow.h"
#include "mesh_render.h"
#include "imgui.h"

void ShadowRenderManager::init()
{
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowRenderManager::render(unsigned int depthMapShader, const glm::mat4& lightSpaceMatrix)
{
	glUseProgram(depthMapShader);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader, "lightSpaceMatrix"), 1, GL_FALSE, (float*)(&lightSpaceMatrix));

	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowCasters->render(depthMapShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 getLightMatrix(const vec3& lightDirection, float povDistance)
{
	static float near_plane = 5.0f, far_plane = 50.f;
	ImGui::DragFloat("near plane", &near_plane, 1);
	ImGui::DragFloat("far plane", &far_plane, 0.01f);
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(glm::vec3(lightDirection) * 10.0f,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	lightView = glm::translate(lightView, -lightDirection * povDistance);
	return lightProjection * lightView;
}