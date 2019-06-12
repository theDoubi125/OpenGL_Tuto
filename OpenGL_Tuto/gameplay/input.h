#pragma once

#include "transform.h"
#include <GLFW/glfw3.h>

namespace input
{
	enum class KeyState {
		// 2 flags : up/down and changed last frame
		UP = 0, DOWN = 1, RELEASED = 2, PRESSED = 3
	};

	extern vec3 movementInput;
	extern vec3 cameraInput;
	extern GLFWwindow* window;

	void init();
	void update();
	handle registerKey(int keyCode);
	KeyState getState(handle key);
}