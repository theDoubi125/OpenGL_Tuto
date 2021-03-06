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
	extern vec2 mousePos;
	extern vec2 mousePosOffset;

	void init();
	void update();
	void reset();
	handle registerKey(int keyCode);
	KeyState getState(handle key);
	KeyState getButtonState(int button);

	void mouseMovement(GLFWwindow* window, double xpos, double ypos);

	void setCursorVisibility(bool visible);
}