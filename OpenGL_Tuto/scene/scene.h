#pragma once

namespace scene
{
	extern handle cameraId;
	void init(int screenWidth, int screenHeight);
	void update(float deltaTime);
	void render();
	void showDebug();
}