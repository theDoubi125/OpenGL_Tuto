#pragma once

namespace scene
{
	void init(int screenWidth, int screenHeight);
	void update(float deltaTime, vec3 input, Camera& camera);
	void render(Camera& camera);
}