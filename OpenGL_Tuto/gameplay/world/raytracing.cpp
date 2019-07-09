#include "raytracing.h"
#include <algorithm>

namespace world
{
	namespace raytracing
	{
		ivec3 raytraceScreen(vec2 pos)
		{
			return ivec3(0, 0, 0);
		}

		void raytrace(vec3 pos, vec3 dir, ivec3* result, int resultSize)
		{
			dir = glm::normalize(dir);
			vec3 currentPos = pos;
			ivec3 currentCell = (ivec3)currentPos;
			float x1 = (float)currentCell.x / dir.x - currentPos.x;
			float x2 = (float)(currentCell.x + 1) / dir.x - currentPos.x;
			float x = x1 > x2 ? x1 : x2;
			float y1 = (float)currentCell.y / dir.y - currentPos.y;
			float y2 = (float)(currentCell.y + 1) / dir.y - currentPos.y;
			float y = y1 > y2 ? y1 : y2;
			float z1 = (float)currentCell.z / dir.z - currentPos.z;
			float z2 = (float)(currentCell.z + 1) / dir.z - currentPos.z;
			float z = z1 > z2 ? z1 : z2;
			float f = std::min(std::min(x, y), z);
			vec3 nextPos = pos + dir * f;
			*result = (ivec3)nextPos;
			if (resultSize > 0)
				raytrace(nextPos, dir, result + 1, resultSize - 1);
		}
	}
}