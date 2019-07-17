#include "transform.h"

namespace world
{
	namespace voxel_transform
	{
		extern Column<handle> transformIds;
		extern Column<ivec3> cells;

		handle add(handle transformId, ivec3 cell);
		void remove(handle elementId);
		void update(float deltaTime);
	}
}