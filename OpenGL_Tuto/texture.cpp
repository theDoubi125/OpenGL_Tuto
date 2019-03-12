#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"
#include <iostream>

unsigned int loadTexture(const char* texturePath)
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

	if (data == nullptr) {
		std::cerr << "Error while loading image " << texturePath << std::endl;
	}
	else
	{
		std::cout << "loading image " << texturePath << " : size(" << width << ", " << height << ")" << std::endl;
	}
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return texture;
}