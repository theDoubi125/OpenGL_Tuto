#include "input.h"
#include <iostream>

namespace input
{
	vec3 input;
	GLFWwindow* window = nullptr;

	void update()
	{
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			input.z = -1;
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			input.z = 1;
		else
			input.z = 0;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			input.x = -1;
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			input.x = 1;
		else
			input.x = 0;
	}
}