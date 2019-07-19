#pragma once

#include "transform.h"

namespace entities
{
	enum Direction
	{
		North, South, East, West
	};

	namespace walker
	{
		void init();
		handle add(handle cellTransformId, Direction dir);
		void remove(handle element);
		void update(); // to call every step
	}
}