#pragma once

namespace scene
{
	void init(int screenWidth, int screenHeight);
	void update(float deltaTime, Camera& camera);
	void render(Camera& camera);
}