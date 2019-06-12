#pragma once

#include "transform.h"
#include <GLFW/glfw3.h>

namespace input
{
	extern vec3 input;
	extern GLFWwindow* window;

	void update();
}