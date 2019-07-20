#pragma once
#include "transform.h"

namespace entities
{
	namespace turning
	{
		void init();
		handle add(handle transformId, int directionOffset);
		void update();
		void remove(handle element);
	}
}