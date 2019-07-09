#pragma once
#include "transform.h"

namespace collision
{
	namespace sphere
	{
		void init();
		handle add(handle transformId, float r);
		void remove(handle id);
	}
}