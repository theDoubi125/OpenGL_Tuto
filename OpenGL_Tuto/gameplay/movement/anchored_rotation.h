#pragma once

#include <vector>
#include "../../transform.h"

namespace gameplay
{
	namespace movement
	{
		handle add(handle transformHandle, vec3 anchorOffset, handle anchorTransform, vec3 anchorTransformOffset);
		void update();
	}
}