#include "input.h"
#include <iostream>
#include "util/table.h"

namespace input
{
	vec3 movementInput;
	vec3 cameraInput;
	GLFWwindow* window = nullptr;

	Table inputTable;
	Column<int> keys;
	Column<KeyState> states;

	handle spaceStateHandle;

	vec2 mousePos;
	vec2 mousePosOffset;
	bool mousePosInit = false;

	void init()
	{
		inputTable.init(50, keys + states);
		mousePosInit = false;
	}

	void update()
	{
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			movementInput.z = -1;
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			movementInput.z = 1;
		else
			movementInput.z = 0;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			movementInput.x = 1;
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			movementInput.x = -1;
		else
			movementInput.x = 0;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraInput.z = -1;
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraInput.z = 1;
		else cameraInput.z = 0;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraInput.x = -1;
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraInput.x = 1;
		else cameraInput.x = 0;

		for (int i = 0; i < inputTable.count; i++)
		{
			int isDown = (glfwGetKey(window, keys[i]) == GLFW_PRESS) ? 1 : 0;
			int wasDown = ((int)states[i] & 1);
			
			states[i] = (KeyState)(isDown + (wasDown != isDown ? 2 : 0));
		}
	}

	void reset()
	{
		mousePosOffset = { 0, 0 };
	}

	handle registerKey(int keyCode)
	{
		TableElement element = inputTable.push();
		element << keyCode << KeyState::UP;
		return { element.elementIndex };

	}

	KeyState getState(handle key)
	{
		return states[key.id];
	}

	void mouseMovement(GLFWwindow* window, double xpos, double ypos)
	{
		if (!mousePosInit)
		{
			mousePos.x = (float)xpos;
			mousePos.y = (float)ypos;
			mousePosInit = true;
		}

		mousePosOffset.x = (float)xpos - mousePos.x;
		mousePosOffset.y = mousePos.y - (float)ypos; // reversed since y-coordinates go from bottom to top

		mousePos.x = (float)xpos;
		mousePos.y = (float)ypos;
	}
}