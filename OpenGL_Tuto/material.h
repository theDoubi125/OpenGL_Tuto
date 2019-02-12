#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include <glm/vec3.hpp>

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

#endif MATERIAL_H_INCLUDED