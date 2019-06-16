#include "transform.h"

namespace movement
{
	namespace third_person
	{
		void init();
		handle add(handle transform, handle targetTransform, float sensitivity, float distance);
		void remove(handle id);
		void update(float deltaTime);
	}
}