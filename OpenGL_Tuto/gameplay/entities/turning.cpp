#include "turning.h"
#include "util/alloc_table.h"
#include "gameplay/world/cell_transform.h"

namespace entities
{
	namespace turning
	{
		AllocTable table;
		Column<handle> cellTransformIds;
		Column<int> directionOffsets;

		void init()
		{
			table.init(500, cellTransformIds + directionOffsets);
		}

		handle add(handle cellTransformId, int directionOffset)
		{
			TableElement element = table.allocate();
			element << cellTransformId << directionOffset;
			return { element.elementIndex };
		}

		void update()
		{
			for (auto it = table.begin(); it.isValid(); it++)
			{
				world::cellTransform::turn(cellTransformIds[*it], directionOffsets[*it]);
			}
		}

		void remove(handle element)
		{
			table.free(element);
		}
	}
}