#include "anchored_rotation.h"

namespace gameplay
{
	namespace movement
	{
		std::vector<handle> handles;
		std::vector<handle> transformHandles;
		std::vector<vec3> anchorOffsets;
		std::vector<vec3> anchorPoints;
		handle nextHandle = { 0 };
		int count = 0;

		handle add(handle transformHandle, vec3 anchorOffset, vec3 anchorPoint)
		{
			handle result = nextHandle;
			nextHandle.id++;
			handles[count] = result;
			transformHandles[count] = transformHandle;
			anchorOffsets[count] = anchorOffset;
			anchorPoints[count] = anchorPoint;
			return result;
		}

		void update()
		{

		}
	}
}