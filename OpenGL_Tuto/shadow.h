#pragma once

class MeshRenderer;

class ShadowRenderManager
{
public:
	void init();
	void render(unsigned int depthMapShader);

	MeshRenderer* shadowCasters;

	unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;
};