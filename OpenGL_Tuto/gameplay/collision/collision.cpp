#include "collision.h"
#include "util/table.h"
#include "util/bit_array.h"

namespace collision
{
	namespace sphere
	{
		Table table;
		Column<handle> transformIds;
		Column<float> radiuses;
		Column<unsigned int> collisionFlags;
		BitArray allocation;

		void init()
		{
			table.init(500, transformIds + radiuses + collisionFlags);
			allocation.init(500);
		}

		handle add(handle transformId, float r, unsigned int collisionFlag)
		{
			handle result = { allocation.allocate() };
			TableElement element = table.element(result.id);
			element << transformId << r << collisionFlag;
			return result;
		}

		void remove(handle id)
		{
			allocation.free(id.id);
		}
	}
}