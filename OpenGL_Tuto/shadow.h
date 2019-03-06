#pragma once

class MeshRenderer;

glm::mat4 getLightMatrix(const vec3& lightDirection);

class ShadowRenderManager
{
public:
	void init();
	void render(unsigned int depthMapShader, const glm::mat4& lightSpaceMatrix);

	MeshRenderer* shadowCasters;

	unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;
};