#include "camera.h"
#include "util/bit_array.h"
#include "transform.h"

namespace camera
{
	Table table;
	BitArray allocations;
	Column<handle> transformIds;
	Column<float> zooms;

	handle mainCamera = { -1 };

	void init()
	{
		table.init(10, transformIds + zooms);
		allocations.init(10);
	}

	handle add(handle transformId, float zoom)
	{
		handle result = { allocations.allocate() };
		TableElement element = table.element(result.id);
		element << transformId << zoom;
		return result;
	}

	void remove(handle cameraHandle)
	{
		allocations.free(cameraHandle.id);
	}
	
	glm::mat4 getViewMatrix(handle cameraHandle)
	{
		handle transformId = transformIds[cameraHandle.id];
		vec3 position = transform::positions[transformId.id];
		vec3 forward = glm::normalize(transform::rotations[transformId.id]) * vec3(0, 0, 1);
		return glm::lookAt(position, position + forward, vec3(0, 1, 0));
	}

	float getZoom(handle cameraHandle)
	{
		return zooms[cameraHandle.id];
	}

	vec3 getCameraPos(handle cameraHandle)
	{
		handle transformId = transformIds[cameraHandle.id];
		return transform::positions[transformId.id];
	}

	quat getCameraRot(handle cameraHandle)
	{
		handle transformId = transformIds[cameraHandle.id];
		return transform::rotations[transformId.id];
	}
}