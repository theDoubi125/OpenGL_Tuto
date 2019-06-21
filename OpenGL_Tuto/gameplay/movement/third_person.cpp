#include "first_person.h"
#include "util/bit_array.h"
#include "gameplay/input.h"

namespace movement
{
	namespace third_person
	{
		Table table;
		Column<handle> transforms;
		Column<handle> targetTransforms;
		Column<vec3> euler;
		Column<float> sensitivities;
		Column<float> distances;
		BitArray allocation;

		void init()
		{
			table.init(10, transforms + targetTransforms + euler + sensitivities + distances);
			allocation.init(10);
		}

		handle add(handle transform, handle targetTransform, float sensitivity, float distance)
		{
			handle result = { allocation.allocate() };
			TableElement element = table.element(result.id);
			element << transform << targetTransform << glm::eulerAngles(transform::rotations[transform]) << sensitivity << distance;
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
				vec3 targetPosition = transform::positions[targetTransforms[*it]];
				position = targetPosition - rotation * vec3(0, 0, distances[*it]);
			}
		}
	}
}