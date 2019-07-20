#include "walker.h"
#include "util/bit_array.h"
#include "gameplay/world/cell_transform.h"

namespace entities
{
	namespace walker
	{
		Table table;

		Column<handle> cellTransformIds;
		Column<Direction> directions;
		BitArray allocation;

		void init()
		{
			table.init(100, cellTransformIds + directions);
			allocation.init(100);
		}

		handle add(handle cellTransformId, Direction dir)
		{
			handle result = { allocation.allocate() };
			TableElement element = table.element(result.id);
			element << cellTransformId << dir;
			return result;
		}

		void remove(handle element)
		{
			allocation.free(element.id);
		}

		void update()
		{
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				switch (directions[*it])
				{
				case Direction::East:
					world::cellTransform::move(cellTransformIds[*it], ivec3(1, 0, 0));
					break;
				case Direction::West:
					world::cellTransform::move(cellTransformIds[*it], ivec3(-1, 0, 0));
					break;
				case Direction::North:
					world::cellTransform::move(cellTransformIds[*it], ivec3(0, 0, -1));
					break;
				case Direction::South:
					world::cellTransform::move(cellTransformIds[*it], ivec3(0, 0, 1));
					break;
				}
			}
		}
	}
}