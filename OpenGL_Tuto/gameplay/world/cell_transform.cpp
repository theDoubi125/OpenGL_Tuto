#include "cell_transform.h"
#include "util/bit_array.h"

namespace world
{
	namespace voxel_transform
	{
		Table table;
		BitArray allocation;
		Column<handle> transformIds;
		Column<ivec3> cells;

		float speed = 1;
		float threshold = 0.01f;

		void init()
		{
			table.init(500, transformIds + cells);
			allocation.init(500);
		}

		handle add(handle transformId, ivec3 cell)
		{
			handle result = { allocation.allocate() };
			TableElement element = table.element(result.id);
			element << transformId << cell;
			return result;
		}

		void remove(handle elementId)
		{
			allocation.free(elementId.id);
		}

		void update(float deltaTime)
		{
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				vec3& position = transform::positions[transformIds[*it]];
				vec3 targetPosition = (vec3)cells[*it];
				vec3 direction = glm::normalize(targetPosition - position);
				position = direction.length() < threshold ? targetPosition : position + direction * deltaTime * speed;
			}
		}
	}
}