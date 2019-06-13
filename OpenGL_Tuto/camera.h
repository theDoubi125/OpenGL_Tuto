#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "util/table.h"

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

namespace camera
{
	void init();
	handle add(handle transformId, float zoom);
	void remove(handle cameraHandle);
	float getZoom(handle cameraHandle);
	glm::mat4 getViewMatrix(handle cameraHandle);
	glm::vec3 getCameraPos(handle cameraHandle);
	glm::quat getCameraRot(handle cameraHandle);
};

#endif
