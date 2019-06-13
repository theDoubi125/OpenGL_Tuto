#include "first_person.h"
#include "util/bit_array.h"
#include "gameplay/input.h"

namespace movement
{
	namespace first_person
	{
		Table table;
		Column<handle> transforms;
		Column<float> sensitivities;
		Column<float> movementSpeeds;
		BitArray allocation;

		void init()
		{
			table.init(10, transforms + sensitivities + movementSpeeds);
			allocation.init(10);
		}

		handle add(handle transform, float sensitivity, float movementSpeed)
		{
			handle result = {allocation.allocate()};
			TableElement element = table.element(result.id);
			element << transform << sensitivity << movementSpeed;
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
				rotation *= quat(vec3(input::mousePosOffset.x * sensitivities[*it], input::mousePosOffset.y * sensitivities[*it], 0.0f));
				vec3& position = transform::positions[transforms[*it]];
				position += input::movementInput * deltaTime * movementSpeeds[*it];
			}
		}
	}
}