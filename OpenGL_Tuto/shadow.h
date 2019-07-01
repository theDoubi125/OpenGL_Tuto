#pragma once

class MeshRenderer;

glm::mat4 getLightMatrix(const vec3& lightDirection, float povDistance);

class ShadowRenderManager
{
public:
	void init();
	void render(unsigned int depthMapShader, const glm::mat4& lightSpaceMatrix, GLuint lightMatrixAttr);

	MeshRenderer* shadowCasters;

	unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	unsigned int depthMap;
};