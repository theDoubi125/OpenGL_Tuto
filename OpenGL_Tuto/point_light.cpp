#include "point_light.h"

void PointLightManager::add(handle transformId, float intensity, vec3 diffuse, vec3 specular)
{
	handle result = nextHandle;
	nextHandle.id++;
	transformIds[count] = transformId;
	intensities[count] = intensity;
	lightIds[count] = result;
	diffuses[count] = diffuse;
	speculars[count] = specular;
	count++;
}

void PointLightManager::assignShaderData(unsigned int shader)
{
	glUseProgram(shader);

	vec3 positions[MAX_POINT_LIGHT_COUNT];
	for (int i = 0; i < count; i++)
	{
		positions[i] = (*transforms)[transformIds[i]].position;
	}

	glUniform1i(glGetUniformLocation(shader, "light.count"), count);
	glUniform3f(glGetUniformLocation(shader, "light.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3fv(glGetUniformLocation(shader, "light.diffuse"), count, (float*)&diffuses);
	glUniform3fv(glGetUniformLocation(shader, "light.specular"), count, (float*)&speculars);
	glUniform1fv(glGetUniformLocation(shader, "light.intensities"), count, (float*)&intensities);
	glUniform3fv(glGetUniformLocation(shader, "light.positions"), count, (float*)&positions);
}