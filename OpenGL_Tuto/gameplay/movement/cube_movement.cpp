#include <iostream>
#include "cube_movement.h"
#include "util/bit_array.h"
#include <GLFW/glfw3.h>
#include "gameplay/rotation.h"

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

		void init()
		{
			cubeDataTable >> transformIds >> cubeSizes >> cubeInput;
			bitArray.init(32);
			inactiveFlags.init(32);
			activeBitArray.init(32);
			cubeDataTable.allocate(32);

			activeTable >> activeCubeIds >> rotationAnimIds;
			activeTable.allocate(10);
		}

		handle add(handle transformId, float size)
		{
			handle result = { bitArray.allocate() };
			TableElement elt = cubeDataTable.element(result.id);
			elt << transformId << size << vec3(0, 0, 0);
			inactiveFlags.allocate(result.id);
			return result;
		}

		void update()
		{
			std::vector<int> toActivate;
			std::vector<vec3> toActivateDirection;
			std::vector<int> toDisactivate;
			for (int index : inactiveFlags)
			{
				vec3 input = cubeInput[index];
				if (input.x != 0 || input.y != 0 || input.z != 0)
				{
					toActivate.push_back(index);
					toActivateDirection.push_back(abs(input.x) > abs(input.z) ? vec3(input.x / abs(input.x), 0, 0) : vec3(0, 0, input.z / abs(input.z)));
				}
			}
			for(int index : toActivate)
			{
				inactiveFlags.free(index);
				handle activeId = { activeBitArray.allocate() };
				TableElement elt = activeTable.element(activeId.id);
				handle element = { index };
				vec3 pos = transform::positions[transformIds[index].id];
				quat rot = transform::rotations[transformIds[index].id];
				quat inverseRot = glm::inverse(normalize(rot));
				vec3 direction = toActivateDirection[index];
				vec3 anchorLocalPoint = (vec3(0, -0.5f, 0) + direction / 2.0f) * normalize(rot);
				quat targetRot = quat(glm::cross(vec3(0, 1, 0), direction) * glm::pi<float>() / 2.0f) * normalize(rot);
 				elt << element << rotation::animation::add(transformIds[index], anchorLocalPoint, pos + vec3(0, -0.5f, 0) + toActivateDirection[index]/2.0f, 1, rot, targetRot);
			}
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