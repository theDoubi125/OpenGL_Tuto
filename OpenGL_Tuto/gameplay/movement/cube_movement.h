#pragma once
#include "transform.h"

namespace voxel { struct Chunk; }

namespace movement
{
	namespace cube
	{
		extern Column<handle> transformIds;
		extern Column<float> cubeSizes;
		extern Column<vec3> cubeInput;

		void init();
		handle add(handle transformId, float cubeSize);
		void update(const voxel::Chunk& collisionData, float deltaTime);
	}
}