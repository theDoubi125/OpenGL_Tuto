#pragma once
#include "transform.h"
namespace render
{
	namespace debug
	{
		void init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT);
		void drawLine(vec3 A, vec3 B, vec4 color);
		void drawCube(vec3 center, vec3 size, vec4 color);
		void render();
	}
}