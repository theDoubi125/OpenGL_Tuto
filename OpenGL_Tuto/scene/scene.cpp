#include "../mesh_render.h"
#include "../render/render_pipeline.h"
#include "../point_light.h"
#include "../shadow.h"
#include "../gameplay/rotation.h"
#include "../texture.h"
#include "../camera.h"

namespace scene
{
	MeshRenderer boxRenderer, lampRenderer;
	MeshLibrary meshLibrary;
	PointLightManager pointLights;
	ShadowRenderManager shadowRenderer;
	AnchoredRotationTable rotationTable;
	AnimatedRotationTable animRotationTable;


	Shader *lightingShader;
	Shader *lampShader;
	Shader *shadowShader;
	Shader *pointLightShader;
	Shader *directionalLightShader;

	unsigned int testTexture;
	unsigned int specularMap;

	vec3 sunDirection(1, 0.7f, 0.45f);

	int SCR_WIDTH, SCR_HEIGHT;

	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

	float vertices[] = {
		// positions			// normals				// texture coords
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f,  0.0f, -1.0f,		0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,		0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,		1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,		0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,		1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f,  0.0f,  0.0f,		0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f,  0.0f,  0.0f,		1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, -1.0f,  0.0f,		0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f,  0.0f,		0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f,  1.0f,  0.0f,		0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,  1.0f,  0.0f,		0.0f,  1.0f
	};

	void spawnEntities()
	{

	}

	void init(int screenWidth, int screenHeight)
	{
		SCR_WIDTH = screenWidth;
		SCR_HEIGHT = screenHeight;

		MeshData cubeMesh = meshLibrary.loadMesh("cube", vertices, sizeof(vertices));
		handle transformId = transform::add(vec3(0, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transform::add(vec3(2, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transform::add(vec3(-2, 0, 0), quat(), vec3(1, 1, 1));
		boxRenderer.add(transformId, cubeMesh);

		transformId = transform::add(vec3(0, -1, 0), quat(), vec3(10, 1, 10));
		boxRenderer.add(transformId, cubeMesh);

		handle characterTransformId = transform::add(vec3(0, 0, 2), quat(), vec3(1, 1, 1));
		boxRenderer.add(characterTransformId, cubeMesh);
		rotationTable.add(characterTransformId, vec3(0.5, -0.5, 0), vec3(0.5, -0.5, 0));
		animRotationTable.add(characterTransformId, 10, quat(), quat(vec3(0, 0, -glm::pi<float>() / 2)));

		handle lampId = transform::add(vec3(0, 2, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(lampId, cubeMesh);
		//pointLights.add(lampId, 1, vec3(1), vec3(1));

		handle sunTransformId = transform::add(vec3(0, 1, 0), quat(vec3(1, 1, 0)), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(sunTransformId, cubeMesh);

		handle testTransformId = transform::add(vec3(0, 0, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(testTransformId, cubeMesh);

		vec3 sunDirection(1, 0.7f, 0.45f);

		shadowRenderer.shadowCasters = &boxRenderer;
		shadowRenderer.init();

		testTexture = loadTexture("./textures/container2.png");
		specularMap = loadTexture("./textures/container_specular.png");

		lightingShader = new Shader("./shaders/color.vert", "./shaders/color.frag");
		lampShader = new Shader("./shaders/color.vert", "./shaders/lamp.frag");
		shadowShader = new Shader("./shaders/shadows.vert", "./shaders/shadows.frag");
		pointLightShader = new Shader("./shaders/point_light.vert", "./shaders/point_light.frag");
		directionalLightShader = new Shader("./shaders/directional_light.vert", "./shaders/directional_light.frag");
	}

	void update(float deltaTime)
	{

	}

	void render()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		int diffuseAttr = glGetUniformLocation(render::currentShader, "diffuse");
		int specularAttr = glGetUniformLocation(render::currentShader, "specular");
		int modelAttr = glGetUniformLocation(render::currentShader, "model");
		glUniform1i(diffuseAttr, 0);
		glUniform1i(specularAttr, 1);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		glUniformMatrix4fv(modelAttr, 1, false, (float*)&model);

		boxRenderer.render(render::currentShader);

		shadowRenderer.render(shadowShader->ID, getLightMatrix(normalize(sunDirection)));
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// Lighting pass
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
		directionalLightShader->setMat4("lightMatrix", getLightMatrix(normalize(sunDirection)));
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