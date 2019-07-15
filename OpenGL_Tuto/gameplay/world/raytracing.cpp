#include "raytracing.h"
#include <algorithm>

#include "render/debug/render_debug_line.h"

#define PRC 0.001f

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
			ivec3 currentCell = (ivec3)(currentPos + 0.5f);
			if (currentPos.x < 0)
				currentCell.x--;
			if (currentPos.y < 0)
				currentCell.y--;
			if (currentPos.z < 0)
				currentCell.z--;
			float x = ((float)(currentCell.x + 0.5f) - currentPos.x) / dir.x;
			if (dir.x < 0)
				x = (currentPos.x - (float)currentCell.x + 0.5f) / -dir.x;
			float y = ((float)(currentCell.y + 0.5f) - currentPos.y) / dir.y;
			if (dir.y < 0)
				y = (currentPos.y - (float)currentCell.y + 0.5f) / -dir.y;
			float z = ((float)(currentCell.z + 0.5f) - currentPos.z) / dir.z;
			if (dir.z < 0)
				z = (currentPos.z - (float)currentCell.z + 0.5f) / -dir.z;
			float f = std::min(std::min(x, y), z);
			vec3 nextPos = pos + dir * (f + PRC);
			*result = (ivec3)nextPos;
			
			render::debug::drawCube((vec3)currentCell, vec3(1, 1, 1), vec4(1, 0, 0, 0.4f));
			if (resultSize > 0)
				raytrace(nextPos, dir, result + 1, resultSize - 1);
		}

		ivec3 raytraceNext(ray& r)
		{
			vec3 dir = glm::normalize(r.dir);
			vec3 currentPos = r.pos;
			ivec3 currentCell = (ivec3)(currentPos + 0.5f);
			float x = ((float)(currentCell.x + 0.5f) - currentPos.x) / dir.x;
			if (dir.x < 0)
				x = (currentPos.x - (float)currentCell.x + 0.5f) / -dir.x;
			float y = ((float)(currentCell.y + 0.5f) - currentPos.y) / dir.y;
			if (dir.y < 0)
				y = (currentPos.y - (float)currentCell.y + 0.5f) / -dir.y;
			float z = ((float)(currentCell.z + 0.5f) - currentPos.z) / dir.z;
			if (dir.z < 0)
				z = (currentPos.z - (float)currentCell.z + 0.5f) / -dir.z;
			float f = std::min(std::min(x, y), z);
			
			// assign next pos
			r.pos = currentPos + dir * (f + PRC);
			return r.getCell();
		}
	}
}