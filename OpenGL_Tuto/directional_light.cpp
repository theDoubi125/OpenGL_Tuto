#include "directional_light.h"

handle DirectionalLightManager::add(handle transformId, float intensity, vec3 diffuse, vec3 specular)
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

void DirectionalLightManager::assignShaderData(unsigned int shader)
{
	glUseProgram(shader);

	vec3 directions[MAX_DIRECTIONAL_LIGHT_COUNT];
	for (int i = 0; i < count; i++)
	{
		directions[i] = (*transforms)[transformIds[i]].rotation * vec3(0, 0, 1);
	}

	glUniform1i(glGetUniformLocation(shader, "light.directional.count"), count);
	glUniform3f(glGetUniformLocation(shader, "light.directional.ambient"), 0.1f, 0.1f, 0.1f);
	glUniform3fv(glGetUniformLocation(shader, "light.directional.diffuse"), count, (float*)&diffuses);
	glUniform3fv(glGetUniformLocation(shader, "light.directional.specular"), count, (float*)&speculars);
	glUniform1fv(glGetUniformLocation(shader, "light.directional.intensities"), count, (float*)&intensities);
	glUniform3fv(glGetUniformLocation(shader, "light.directional.directions"), count, (float*)&directions);
	glUniformMatrix4fv(glGetUniformLocation(shader, "light.directional.lightSpaceMatrix"), count, GL_FALSE, (float*)(&lightMatrixes));
}