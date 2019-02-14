#pragma once

#include "transform.h"
#include "shader_m.h"

#define MAX_POINT_LIGHT_COUNT 100

class PointLightManager
{
public:
	TransformManager* transforms;

	handle add(handle transformId, float intensity, vec3 diffuse, vec3 specular);
	void assignShaderData(unsigned int shader);

	handle lightIds[MAX_POINT_LIGHT_COUNT];
	handle transformIds[MAX_POINT_LIGHT_COUNT];
	float intensities[MAX_POINT_LIGHT_COUNT];
	vec3 diffuses[MAX_POINT_LIGHT_COUNT];
	vec3 speculars[MAX_POINT_LIGHT_COUNT];
	int count = 0;
	handle nextHandle = { 0 };
};