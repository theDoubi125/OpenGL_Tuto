#pragma once

#include "transform.h"

namespace world
{
    namespace raytracing
    {
		ivec3 raytraceScreen(vec2 pos);
		void raytrace(vec3 pos, vec3 dir, ivec3* result, int resultSize);
    }
}