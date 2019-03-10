#include "../mesh_render.h"
#include "../render/render_pipeline.h"
#include "../point_light.h"
#include "../shadow.h"
#include "../gameplay/rotation.h"

namespace scene
{
	MeshRenderer boxRenderer, lampRenderer;
	MeshLibrary meshLibrary;
	TransformManager transforms;
	PointLightManager pointLights;
	ShadowRenderManager shadowRenderer;
	AnchoredRotationTable rotationTable;
	AnimatedRotationTable animRotationTable;

	void spawnEntities()
	{

	}

	void init()
	{
		boxRenderer.transforms = &transforms;
		pointLights.transforms = &transforms;
		rotationTable.transforms = &transforms;
		animRotationTable.transforms = &transforms;
		lampRenderer.transforms = &transforms;

		MeshData cubeMesh = meshLibrary.loadMesh("cube", vertices, sizeof(vertices));
		handle transformId = transforms.add(vec3(0, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transforms.add(vec3(2, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transforms.add(vec3(-2, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transforms.add(vec3(0, -1, 0), quat(), vec3(10, 1, 10));
		boxRenderer.add(transformId, cubeMesh);

		handle characterTransformId = transforms.add(vec3(0, 0, 2), quat(), vec3(1, 1, 1));
		boxRenderer.add(characterTransformId, cubeMesh);
		rotationTable.add(characterTransformId, vec3(0.5, -0.5, 0), vec3(0.5, -0.5, 0));
		animRotationTable.add(characterTransformId, 10, quat(), quat(vec3(0, 0, -glm::pi<float>() / 2)));

		handle lampId = transforms.add(vec3(0, 2, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(lampId, cubeMesh);
		//pointLights.add(lampId, 1, vec3(1), vec3(1));

		handle sunTransformId = transforms.add(vec3(0, 1, 0), quat(vec3(1, 1, 0)), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(sunTransformId, cubeMesh);

		handle testTransformId = transforms.add(vec3(0, 0, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(testTransformId, cubeMesh);

		vec3 sunDirection(1, 0.7f, 0.45f);

		shadowRenderer.shadowCasters = &boxRenderer;
		shadowRenderer.init();
	}

	void update(float deltaTime)
	{

	}
};