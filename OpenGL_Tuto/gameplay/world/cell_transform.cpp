#include "cell_transform.h"
#include "util/bit_array.h"
#include "util/alloc_table.h"
#include <glm/gtc/quaternion.hpp>
#include "gameplay/entities/turning.h"
#include "gameplay/entities/walker.h"

namespace world
{
	quat dirToQuatArray[] = {
		quat(),//East = 0,
		quat(vec3(0, glm::pi<float>() / 2, 0)),//South = 1,
		quat(vec3(0, glm::pi<float>(), 0)),//West = 2,
		quat(vec3(0, 3 * glm::pi<float>() / 2, 0)),//North = 3,
		quat(vec3(glm::pi<float>() / 2, 0, 0)),//Up = 4,
		quat(vec3(- glm::pi<float>() / 2, 0, 0)),//Down = 5
	};

	Direction turnDir(Direction dir, int offset)
	{
		return (Direction)(((int)dir + offset) % 4);
	}

	quat dirToQuat(Direction direction)
	{
		return dirToQuatArray[(int)direction];
	}

	namespace cellTransform
	{

		AllocTable table;
		Column<handle> transformIds;
		Column<ivec3> cells;
		Column<Direction> directions;

		BitArray idle;

		float time;
		
		namespace moving
		{
			AllocTable table;
			Column<handle> elementIds;
			Column<ivec3> startCells;
			Column<ivec3> targetCells;
		}
		
		namespace turning
		{
			AllocTable table;
			Column<handle> elementIds;
			Column<Direction> startDirections;
			Column<Direction> targetDirections;
		}

		float speed = 1;
		float threshold = 0.01f;

		void init()
		{
			table.init(500, transformIds + cells + directions);
			moving::table.init(500, moving::elementIds + moving::startCells + moving::targetCells);
			turning::table.init(500, turning::elementIds + turning::startDirections + turning::targetDirections);
			idle.init(500);
		}

		handle add(handle transformId, ivec3 cell, Direction direction)
		{
			TableElement element = table.allocate();
			element << transformId << cell << direction;
			idle.allocate(element.elementIndex);
			return { element.elementIndex };
		}

		void move(handle toMove, ivec3 offset)
		{
			TableElement element = moving::table.allocate();
			element << toMove << cells[toMove] << cells[toMove] + offset;
			idle.free(toMove.id);
		}

		void turn(handle toTurn, int directionOffset)
		{
			TableElement element = turning::table.allocate();
			element << toTurn << directions[toTurn] << turnDir(directions[toTurn], directionOffset);
			idle.free(toTurn.id);
		}

		void remove(handle elementId)
		{
			table.free(elementId);
		}

		void playStep()
		{
			entities::turning::update();
			entities::walker::update();
			time = 0;
		}

		void update(float deltaTime)
		{
			if (time < 1)
			{
				time += deltaTime;
				for (auto it = moving::table.begin(); it.isValid(); it++)
				{
					handle elementId = moving::elementIds[*it];
					vec3& position = transform::positions[transformIds[elementId]];
					float f = time;
					position = (1 - f) * (vec3)moving::startCells[*it] + f * (vec3)moving::targetCells[*it];
				}
				for (auto it = turning::table.begin(); it.isValid(); it++)
				{
					handle elementId = turning::elementIds[*it];
					quat& rotation = transform::rotations[transformIds[elementId]];
					float f = time;
					quat startRotation = dirToQuat(turning::startDirections[*it]);
					quat targetRotation = dirToQuat(turning::targetDirections[*it]);
					if(time <= 1)
						rotation = glm::normalize(glm::lerp(startRotation, targetRotation, time));
				}
				if (time >= 1)
				{
					for (auto it = moving::table.begin(); it.isValid(); it++)
					{
						idle.allocate(moving::elementIds[*it].id);
						cells[moving::elementIds[*it]] = moving::targetCells[*it];
					}
					for (auto it = turning::table.begin(); it.isValid(); it++)
					{
						idle.allocate(turning::elementIds[*it].id);
						directions[turning::elementIds[*it]] = turning::targetDirections[*it];
					}
					moving::table.clear();
					turning::table.clear();
				}
			}
		}
	}
}