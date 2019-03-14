#pragma once

#include "transform.h"
#include "shader_m.h"

#define MAX_DIRECTIONAL_LIGHT_COUNT 100

class DirectionalLightManager
{
public:
	handle add(handle transformId, float intensity, vec3 diffuse, vec3 specular);
	void assignShaderData(unsigned int shader);

	handle lightIds[MAX_DIRECTIONAL_LIGHT_COUNT];
	handle transformIds[MAX_DIRECTIONAL_LIGHT_COUNT];
	float intensities[MAX_DIRECTIONAL_LIGHT_COUNT];
	vec3 diffuses[MAX_DIRECTIONAL_LIGHT_COUNT];
	vec3 speculars[MAX_DIRECTIONAL_LIGHT_COUNT];
	glm::mat4 lightMatrixes[MAX_DIRECTIONAL_LIGHT_COUNT];

	int count = 0;
	handle nextHandle = { 0 };
};