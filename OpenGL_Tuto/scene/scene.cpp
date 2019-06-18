#include "../mesh_render.h"
#include "../render/render_pipeline.h"
#include "../point_light.h"
#include "../shadow.h"
#include "../gameplay/rotation.h"
#include "../texture.h"
#include "../camera.h"
#include "gameplay/movement/cube_movement.h"

#include "gameplay/world/voxel.h"
#include "directional_light.h"
#include "gameplay/input.h"
#include "camera.h"
#include "gameplay/world/voxel.h"
#include "gameplay/world/world.h"
#include "gameplay/movement/third_person.h"
#include "gameplay/movement/first_person.h"
#include "gameplay/input.h"


namespace scene
{
	MeshRenderer boxRenderer, lampRenderer;
	PointLightManager pointLights;
	DirectionalLightManager directionalLights;
	ShadowRenderManager shadowRenderer;

	handle cubeMovementId;

	Shader* lightingShader;
	Shader* lampShader;
	Shader* shadowShader;
	Shader* pointLightShader;
	Shader* directionalLightShader;

	unsigned int testTexture;
	unsigned int specularMap;

	int SCR_WIDTH, SCR_HEIGHT;

	vec3 sunDirection(1, 0.7f, 0.45f);
	float sunPovDistance = 20;
	handle spaceInputHandle;

	handle cameraTransform;
	handle cameraId;

	bool debugMode = false;
	handle cameraModeHandle;
	handle characterTransformId;

	void init(int screenWidth, int screenHeight)
	{
		SCR_WIDTH = screenWidth;
		SCR_HEIGHT = screenHeight;
		lightingShader = new Shader("./shaders/color.vert", "./shaders/color.frag");
		lampShader = new Shader("./shaders/color.vert", "./shaders/lamp.frag");
		shadowShader = new Shader("./shaders/shadows.vert", "./shaders/shadows.frag");
		pointLightShader = new Shader("./shaders/point_light.vert", "./shaders/point_light.frag");
		directionalLightShader = new Shader("./shaders/directional_light.vert", "./shaders/directional_light.frag");

		testTexture = loadTexture("./textures/container2.png");
		specularMap = loadTexture("./textures/container_specular.png");

		char* dataBuffer = new char[5000000];
		for (int i = 0; i < 6; i++) {
			world::voxel::computeFaceMesh(vec3(0, 0, 0), (world::voxel::FaceDir)i, dataBuffer, 6 * 8 * i * sizeof(float), 1, vec3(0, 0, 0));
		}

		mesh::library::init();
		mesh::render::init();
		handle cubeMesh = mesh::library::loadMesh(dataBuffer, 6 * 8 * 6 * sizeof(float));

		world::manager::init();
		int testSize = CHUNK_SIZE;
		for (int i = 0; i < testSize * 2; i++)
		{
			for (int j = 0; j < testSize * 2; j++)
			{
				world::manager::setCell(ivec3(i, 0, j), 1);
			}
			for (int j = i + 1; j < testSize * 2; j++)
			{
				world::manager::setCell(ivec3(i, 1, j), 1);
			}
		}
		int chunkMeshSize = 0;
		//world::manager::computeChunksMesh(dataBuffer, chunkMeshSize);
		//handle chunkMesh = mesh::loadMesh(dataBuffer, chunkMeshSize);

		delete[] dataBuffer;

		transform::init();
		movement::cube::init();
		rotation::anchor::init();
		rotation::animation::init();

		handle transformId = transform::add(vec3(2, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transform::add(vec3(5, 1, 3), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		characterTransformId = transform::add(vec3(5, 1, 5), quat(), vec3(1, 1, 1));
		boxRenderer.add(characterTransformId, cubeMesh);
		cubeMovementId = movement::cube::add(characterTransformId, 0.5f);
		//rotation::animation::add(characterTransformId, vec3(0.5, -0.5, 0), vec3(0.5, -0.5, 0), 3, quat(), quat(vec3(0, 0, -glm::pi<float>() / 2)));

		handle lampId = transform::add(vec3(0, 2, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(lampId, cubeMesh);
		//pointLights.add(lampId, 1, vec3(1), vec3(1));

		handle sunTransformId = transform::add(vec3(0, 1, 0), quat(vec3(1, 1, 0)), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(sunTransformId, cubeMesh);

		handle testTransformId = transform::add(vec3(0, 0, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(testTransformId, cubeMesh);
		//pointLights.add(sunTransformId, 1, vec3(1), vec3(1));


		directionalLights.add(sunTransformId, 1, vec3(1), vec3(1));
		shadowRenderer.shadowCasters = &boxRenderer;
		shadowRenderer.init();

		input::init();
		spaceInputHandle = input::registerKey(GLFW_KEY_SPACE);

		camera::init();
		cameraTransform = transform::add(vec3(-10, 5, 0), quat(), vec3(1, 1, 1));
		cameraId = camera::add(cameraTransform, 45.0f);

		movement::third_person::init();
		movement::first_person::init();
		cameraModeHandle = movement::third_person::add(cameraTransform, characterTransformId, 0.001f, 10);
	}

	void update(float deltaTime)
	{
		input::update();
		quat camRotation = camera::getCameraRot(cameraId);
		movement::cube::cubeInput[cubeMovementId.id] = -(camRotation * vec3(0, 0, 1)) * input::movementInput.z + (camRotation * vec3(1, 0, 0)) * input::movementInput.x;
		rotation::animation::update(deltaTime);
		rotation::anchor::update();
		movement::cube::update(deltaTime);
		movement::third_person::update(deltaTime);
		movement::first_person::update(deltaTime);

		if (input::getState(spaceInputHandle) == input::KeyState::PRESSED)
		{
			if (debugMode)
			{
				movement::first_person::remove(cameraModeHandle);
				movement::third_person::add(cameraTransform, characterTransformId, 0.001f, 10);
				debugMode = false;
			}
			else
			{
				movement::third_person::remove(cameraModeHandle);
				movement::first_person::add(cameraTransform, 0.001f, 10);
				debugMode = true;
			}
		}
		input::reset();
	}

	void render()
	{
		float zoom = camera::getZoom(scene::cameraId);
		vec3 cameraPosition = camera::getCameraPos(scene::cameraId);
		glm::mat4 projection = glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera::getViewMatrix(scene::cameraId);
		static glm::vec4 testVec(0, 0, 0, 1);
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		render::clear_frame();
		render::start_render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		int diffuseAttr = glGetUniformLocation(render::currentShader, "diffuse");
		int specularAttr = glGetUniformLocation(render::currentShader, "specular");
		int modelAttr = glGetUniformLocation(render::currentShader, "model");
		glUniform1i(diffuseAttr, 0);
		glUniform1i(specularAttr, 1);
		glUniformMatrix4fv(modelAttr, 1, false, (float*)&model);

		boxRenderer.render(render::currentShader);

		shadowRenderer.render(shadowShader->ID, getLightMatrix(normalize(sunDirection), sunPovDistance));
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		render::start_lighting();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for (int i = 0; i < pointLights.count; i++)
		{
			pointLightShader->use();
			pointLightShader->setInt("gPosition", 0);
			pointLightShader->setInt("gNormal", 1);
			pointLightShader->setInt("gAlbedoSpec", 2);
			pointLightShader->setVec3("viewPos", cameraPosition);
			vec3 position;
			position = transform::positions[transform::indexOf(pointLights.transformIds[i])];
			pointLightShader->setVec3("light.Position", position);
			pointLightShader->setVec3("light.Color", vec3(1, 1, 1));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		directionalLightShader->use();
		directionalLightShader->setInt("gPosition", 0);
		directionalLightShader->setInt("gNormal", 1);
		directionalLightShader->setInt("gAlbedoSpec", 2);
		directionalLightShader->setVec3("viewPos", cameraPosition);
		directionalLightShader->setVec3("light.Direction", normalize(sunDirection));
		directionalLightShader->setVec3("light.Color", vec3(1, 1, 1));

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowRenderer.depthMap);
		directionalLightShader->setInt("shadowMap", 4);
		directionalLightShader->setMat4("lightMatrix", getLightMatrix(normalize(sunDirection), sunPovDistance));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);


		render::render_deferred();

		lampShader->use();
		lampShader->setMat4("projection", projection);
		lampShader->setMat4("view", view);
		lampShader->setMat4("model", model);
		lampShader->setVec3("viewPos", cameraPosition);

	}
};