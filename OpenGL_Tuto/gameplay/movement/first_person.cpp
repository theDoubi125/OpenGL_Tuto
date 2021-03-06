#include "first_person.h"
#include "util/bit_array.h"
#include "gameplay/input.h"

namespace movement
{
	namespace first_person
	{
		Table table;
		Column<handle> transforms;
		Column<vec3> euler;
		Column<float> sensitivities;
		Column<float> movementSpeeds;
		BitArray allocation;

		void init()
		{
			table.init(10, transforms + euler + sensitivities + movementSpeeds);
			allocation.init(10);
		}

		handle add(handle transform, float sensitivity, float movementSpeed)
		{
			handle result = {allocation.allocate()};
			TableElement element = table.element(result.id);
			element << transform << glm::eulerAngles(transform::rotations[transform]) << sensitivity << movementSpeed;
			return result;
		}

		void remove(handle id)
		{
			allocation.free(id.id);
		}
		
		void update(float deltaTime)
		{
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				quat& rotation = transform::rotations[transforms[*it]];
				euler[*it] -= vec3(input::mousePosOffset.y * sensitivities[*it], input::mousePosOffset.x * sensitivities[*it], 0.0f);
				if (euler[*it].x > glm::pi<float>() / 2 - 0.001f)
					euler[*it].x = glm::pi<float>() / 2 - 0.001f;
				if (euler[*it].x < -glm::pi<float>() / 2 + 0.001f)
					euler[*it].x = -glm::pi<float>() / 2 + 0.001f;
				rotation = quat(euler[*it]);
				vec3& position = transform::positions[transforms[*it]];
				position -= rotation * (input::cameraInput * deltaTime * movementSpeeds[*it]);
			}
		}
	}
}