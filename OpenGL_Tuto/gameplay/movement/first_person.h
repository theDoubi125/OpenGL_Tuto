#include "transform.h"

namespace movement
{
	namespace first_person
	{
		void init();
		handle add(handle transform, float sensitivity, float movementSpeed);
		void remove(handle id);
		void update(float deltaTime);
	}
}