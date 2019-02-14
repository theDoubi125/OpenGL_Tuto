#include "point_light.h"

handle PointLightManager::add(handle transformId, float intensity, vec3 diffuse, vec3 specular)
{
	handle result = nextHandle;
	nextHandle.id++;
	transformIds[count] = transformId;
	intensities[count] = intensity;
	lightIds[count] = result;
	diffuses[count] = diffuse;
	speculars[count] = specular;
	count++;
	return result;
}

void PointLightManager::assignShaderData(unsigned int shader)
{
	glUseProgram(shader);

	vec3 positions[MAX_POINT_LIGHT_COUNT];
	for (int i = 0; i < count; i++)
	{
		positions[i] = (*transforms)[transformIds[i]].position;
	}

	glUniform1i(glGetUniformLocation(shader, "light.point.count"), count);
	glUniform3f(glGetUniformLocation(shader, "light.point.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3fv(glGetUniformLocation(shader, "light.point.diffuse"), count, (float*)&diffuses);
	glUniform3fv(glGetUniformLocation(shader, "light.point.specular"), count, (float*)&speculars);
	glUniform1fv(glGetUniformLocation(shader, "light.point.intensities"), count, (float*)&intensities);
	glUniform3fv(glGetUniformLocation(shader, "light.point.positions"), count, (float*)&positions);
}