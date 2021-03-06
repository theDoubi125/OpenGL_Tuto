#pragma once

#include "transform.h"

namespace world
{
    namespace raytracing
    {
		struct ray
		{
			vec3 pos;
			vec3 dir;

			ivec3 getCell()
			{
				ivec3 cell = (ivec3)(pos + 0.5f);
				return cell;
			}
		};

		ivec3 raytraceScreen(vec2 pos);
		void raytrace(vec3 pos, vec3 dir, ivec3* result, int resultSize);
		ivec3 raytraceNext(ray& r);
    }
}