#include "cell_transform.h"
#include "util/bit_array.h"

namespace world
{
	namespace voxel_transform
	{
		Table table;
		BitArray allocation;
		BitArray moving;
		Column<handle> transformIds;
		Column<ivec3> cells;

		float speed = 1;
		float threshold = 0.01f;

		void init()
		{
			table.init(500, transformIds + cells);
			allocation.init(500);
			moving.init(500);
		}

		handle add(handle transformId, ivec3 cell)
		{
			handle result = { allocation.allocate() };
			moving.allocate(result.id);
			TableElement element = table.element(result.id);
			element << transformId << cell;
			return result;
		}

		void move(handle element, ivec3 offset)
		{
			moving.allocate(element.id);
			cells[element] += offset;
		}

		void remove(handle elementId)
		{
			allocation.free(elementId.id);
		}

		void update(float deltaTime)
		{
			for (auto it = moving.begin(); it.isValid(); it++)
			{
				vec3& position = transform::positions[transformIds[*it]];
				vec3 targetPosition = (vec3)cells[*it];
				vec3 direction = targetPosition - position;
				vec3 nDirection = glm::normalize(direction);
				float distance = glm::length(direction);
				if (distance < threshold)
				{
					moving.free(*it);
					position = targetPosition;

				}
				else
				{
					position = position + nDirection * deltaTime * speed;
				}
			}
		}
	}
}