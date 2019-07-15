#include <iostream>
#include "cube_movement.h"
#include "util/bit_array.h"
#include <GLFW/glfw3.h>
#include "gameplay/rotation.h"
#include "../world/voxel.h"
#include <cmath>
#define GRAVITY 10
#include "gameplay/world/world.h"

namespace movement
{
	namespace cube
	{
		Column<handle> transformIds;
		Column<float> cubeSizes;
		Column<vec3> cubeInput;
		Table cubeDataTable;
		BitArray bitArray;

		BitArray inactiveFlags;

		Table activeTable;
		BitArray activeBitArray;
		Column<handle> activeCubeIds;
		Column<handle> rotationAnimIds;

		Table fallingTable;
		BitArray fallingBitArray;
		Column<int> fallingIndexes;
		Column<float> fallingSpeeds;


		void init()
		{
			cubeDataTable.init(32, transformIds + cubeSizes + cubeInput);
			bitArray.init(32);
			inactiveFlags.init(32);
			activeBitArray.init(32);

			activeTable.init(10, activeCubeIds + rotationAnimIds);

			fallingBitArray.init(32);
			fallingTable.init(32, fallingIndexes + fallingSpeeds);
		}

		handle add(handle transformId, float size)
		{
			handle result = { bitArray.allocate() };
			TableElement elt = cubeDataTable.element(result.id);
			elt << transformId << size << vec3(0, 0, 0);
			inactiveFlags.allocate(result.id);
			return result;
		}

		void handleActivations(const std::vector<int>& toActivate, const std::vector<vec3>& toActivateDirection)
		{
			for (int index : toActivate)
			{
				vec3 pos = transform::positions[transformIds[index].id];
				vec3 direction = toActivateDirection[index];
				vec3 targetPos = pos + direction;
				ivec3 itargetPos(round(targetPos.x), round(targetPos.y), round(targetPos.z));

				if (world::manager::getCell(itargetPos) == 0)
				{
					handle activeId = { activeBitArray.allocate() };
					TableElement elt = activeTable.element(activeId.id);
					handle element = { index };
					quat rot = transform::rotations[transformIds[index].id];
					quat inverseRot = glm::inverse(normalize(rot));
					vec3 anchorLocalPoint = (vec3(0, -0.5f, 0) + direction / 2.0f) * normalize(rot);
					quat targetRot = quat(glm::cross(vec3(0, 1, 0), direction) * glm::pi<float>() / 2.0f) * normalize(rot);
					elt << element << rotation::animation::add(transformIds[index], anchorLocalPoint, pos + vec3(0, -0.5f, 0) + toActivateDirection[index] / 2.0f, 0.5f, rot, targetRot);
				}
				else
				{
					handle activeId = { activeBitArray.allocate() };
					TableElement elt = activeTable.element(activeId.id);
					handle element = { index };
					quat rot = transform::rotations[transformIds[index].id];
					quat inverseRot = glm::inverse(normalize(rot));
					vec3 anchorLocalPoint = (vec3(0, 0.5f, 0) + direction / 2.0f) * normalize(rot);
					quat targetRot = quat(glm::cross(vec3(0, 1, 0), direction) * glm::pi<float>()) * normalize(rot);
					elt << element << rotation::animation::add(transformIds[index], anchorLocalPoint, pos + vec3(0, 0.5f, 0) + toActivateDirection[index] / 2.0f, 1, rot, targetRot);
				}
			}
		}

		void handleFalling(float deltaTime) 
		{
			std::vector<int> toRemove;
			for (int index : fallingBitArray)
			{
				int fallingIndex = fallingIndexes[index];
				vec3& pos = transform::positions[transformIds[fallingIndex].id];
				fallingSpeeds[fallingIndex] = fallingSpeeds[fallingIndex] + deltaTime * GRAVITY;
				float deltaY = fallingSpeeds[fallingIndex] * deltaTime;
				bool cellChange = floor(pos.y) != floor(pos.y - deltaY);
				pos.y -= deltaY;
				if (cellChange && world::manager::getCell(pos - vec3(0, deltaY, 0)) != 0)
				{
					toRemove.push_back(index);
					pos.y = round(pos.y);
				}
			}
			for (int index : toRemove)
			{
				fallingBitArray.free(index);
				inactiveFlags.allocate(index);
			}
		}

		void update(float deltaTime)
		{
			std::vector<int> toActivate;
			std::vector<int> toMove;
			std::vector<vec3> toMoveDirection;
			std::vector<int> toDisactivate;

			for (int index : inactiveFlags)
			{
				vec3 pos = transform::positions[transformIds[index].id];
				vec3 input = cubeInput[index];
				ivec3 igroundPos(round(pos.x), round(pos.y) - 1, round(pos.z));

				if (world::manager::getCell(igroundPos) == 0)
				{
					int fallingElement = fallingBitArray.allocate();
					TableElement elt = fallingTable.element(fallingElement);
					elt << index << 0;
					toActivate.push_back(index);
				}
				else if (input.x != 0 || input.y != 0 || input.z != 0)
				{
					toActivate.push_back(index);
					toMove.push_back(index);
					toMoveDirection.push_back(abs(input.x) > abs(input.z) ? vec3(input.x / abs(input.x), 0, 0) : vec3(0, 0, input.z / abs(input.z)));
				}
			}
			for(int toActivateIndex : toActivate)
			{
				inactiveFlags.free(toActivateIndex);
			}
			handleActivations(toMove, toMoveDirection);
			handleFalling(deltaTime);

			for (int index : activeBitArray)
			{
				for (handle removedRotation : rotation::animation::removed)
				{
					if (rotationAnimIds[index] == removedRotation)
					{
						toDisactivate.push_back(index);
					}
				}
			}
			for (int index : toDisactivate)
			{
				inactiveFlags.allocate(activeCubeIds[index].id);
				activeBitArray.free(index);
			}
		}
	}
}