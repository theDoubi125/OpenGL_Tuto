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

namespace scene
{
	voxel::Chunk chunk(0);
	MeshLibrary meshLibrary;
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
	float sunPovDistance = 1;

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

		float* dataBuffer = new float[5000000];
		for (int i = 0; i < 6; i++) {
			voxel::computeFaceMesh(vec3(0, 0, 0), (voxel::FaceDir)i, dataBuffer, 6 * 8 * i, 1);
		}

		MeshData cubeMesh = meshLibrary.loadMesh("cube", dataBuffer, 6 * 8 * 6 * sizeof(float));

		int testSize = CHUNK_SIZE;
		for (int i = 0; i < testSize; i++)
		{
			for (int j = 0; j < testSize; j++)
			{
				chunk[ivec3(i, 0, j)] = 1;
			}
			for (int j = i + 1; j < testSize; j++)
			{
				chunk[ivec3(i, 1, j)] = 1;
			}

		}
		int chunkMeshSize = 0;

		voxel::computeChunkMesh(chunk, dataBuffer, chunkMeshSize);
		MeshData chunkMesh = meshLibrary.loadMesh("chunk", dataBuffer, chunkMeshSize);

		delete[] dataBuffer;

		transform::init();
		movement::cube::init();
		rotation::anchor::init();
		rotation::animation::init();
		

		handle transformId = transform::add(vec3(0, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, chunkMesh);

		transformId = transform::add(vec3(2, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transform::add(vec3(5, 1, 3), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		handle characterTransformId = transform::add(vec3(5, 1, 5), quat(), vec3(1, 1, 1));
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
	}

	void update(float deltaTime, vec3 input, Camera& camera)
	{
		movement::cube::cubeInput[cubeMovementId.id] = -camera.Front * input.z + camera.Right * input.x;
		rotation::animation::update(deltaTime);
		rotation::anchor::update();
		movement::cube::update(chunk, deltaTime);
	}

	void render(Camera& camera)
	{
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		static glm::vec4 testVec(0, 0, 0, 1);
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		render::clear_frame();
		render::start_render(camera);

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
			pointLightShader->setVec3("viewPos", camera.Position);
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
		directionalLightShader->setVec3("viewPos", camera.Position);
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
		lampShader->setVec3("viewPos", camera.Position);

	}
};