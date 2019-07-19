#include "transform.h"

namespace world
{
	namespace voxel_transform
	{
		extern Column<handle> transformIds;
		extern Column<ivec3> cells;

		void init();
		handle add(handle transformId, ivec3 cell);
		void move(handle element, ivec3 offset);
		void remove(handle elementId);
		void update(float deltaTime);
	}
}