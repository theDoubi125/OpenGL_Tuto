#include <glad/glad.h>
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

#include "render/voxel_render.h"

#include "util/profiler/profiler.h"

#include "gameplay/modes/gamemode_manager.h"

#include "gameplay/world/raytracing.h"
#include "imgui.h"
#include "render/debug/render_debug_line.h"


namespace scene
{
	MeshRenderer boxRenderer, lampRenderer;
	PointLightManager pointLights;
	DirectionalLightManager directionalLights;
	ShadowRenderManager shadowRenderer;

	handle cubeMovementId;

	namespace shaders
	{
		namespace gbuffer
		{
			int diffuseAttr;
			int specularAttr;
			int modelAttr;
		}
		namespace lighting
		{
			GLuint shader;
		}
		namespace lamp
		{
			GLuint shader;
			GLuint projectionAttr;
			GLuint viewAttr;
			GLuint modelAttr;
			GLuint viewPosAttr;
		}
		namespace shadow
		{
			GLuint shader;
			GLuint lightMatrixAttr;
		}
		namespace pointLight
		{
			GLuint shader;
			GLuint gPositionAttr;
			GLuint gNormalAttr;
			GLuint gAlbedoSpecAttr;
			GLuint viewPosAttr;
			GLuint lightPositionAttr;
			GLuint lightColorAttr;
		}
		namespace directionalLight
		{
			GLuint shader;
			GLuint gPositionAttr;
			GLuint gNormalAttr;
			GLuint gAlbedoSpecAttr;
			GLuint viewPosAttr;
			GLuint lightDirectionAttr;
			GLuint lightColorAttr;
			GLuint shadowMapAttr;
			GLuint lightMatrixAttr;
		}
	}

	unsigned int testTexture;
	unsigned int specularMap;

	int SCR_WIDTH, SCR_HEIGHT;

	vec3 sunDirection(1, 0.7f, 0.45f);
	float sunPovDistance = 20;

	bool debugMode = false;

	void initShaders()
	{
		shaders::lighting::shader = shader::compileShader("./shaders/color.vert", "./shaders/color.frag");
		shaders::lamp::shader = shader::compileShader("./shaders/color.vert", "./shaders/lamp.frag");
		shaders::shadow::shader = shader::compileShader("./shaders/shadows.vert", "./shaders/shadows.frag");
		shaders::pointLight::shader = shader::compileShader("./shaders/point_light.vert", "./shaders/point_light.frag");
		shaders::directionalLight::shader = shader::compileShader("./shaders/directional_light.vert", "./shaders/directional_light.frag");

		shaders::lamp::projectionAttr = glGetUniformLocation(shaders::lamp::shader, "projection");
		shaders::lamp::viewAttr = 	  glGetUniformLocation(shaders::lamp::shader, "view");
		shaders::lamp::modelAttr = 	  glGetUniformLocation(shaders::lamp::shader, "model");
		shaders::lamp::viewPosAttr =  glGetUniformLocation(shaders::lamp::shader, "viewPos");

		shaders::pointLight::gPositionAttr		= glGetUniformLocation(shaders::pointLight::shader, "gPosition");
		shaders::pointLight::gNormalAttr		= glGetUniformLocation(shaders::pointLight::shader, "gNormal");
		shaders::pointLight::gAlbedoSpecAttr	= glGetUniformLocation(shaders::pointLight::shader, "gAlbedoSpec");
		shaders::pointLight::viewPosAttr		= glGetUniformLocation(shaders::pointLight::shader, "viewPos");
		shaders::pointLight::lightPositionAttr	= glGetUniformLocation(shaders::pointLight::shader, "light.Position");
		shaders::pointLight::lightColorAttr		= glGetUniformLocation(shaders::pointLight::shader, "light.Color");

		shaders::directionalLight::gPositionAttr	 = glGetUniformLocation(shaders::directionalLight::shader, "gPosition");
		shaders::directionalLight::gNormalAttr		 = glGetUniformLocation(shaders::directionalLight::shader, "gNormal");
		shaders::directionalLight::gAlbedoSpecAttr	 = glGetUniformLocation(shaders::directionalLight::shader, "gAlbedoSpec");
		shaders::directionalLight::viewPosAttr		 = glGetUniformLocation(shaders::directionalLight::shader, "viewPos");
		shaders::directionalLight::lightDirectionAttr= glGetUniformLocation(shaders::directionalLight::shader, "light.Direction");
		shaders::directionalLight::lightColorAttr	 = glGetUniformLocation(shaders::directionalLight::shader, "light.Color");
		shaders::directionalLight::shadowMapAttr	 = glGetUniformLocation(shaders::directionalLight::shader, "shadowMap");
		shaders::directionalLight::lightMatrixAttr	 = glGetUniformLocation(shaders::directionalLight::shader, "lightMatrix");

		shaders::gbuffer::diffuseAttr = glGetUniformLocation(render::shaders::gBuffer::shader, "diffuse");
		shaders::gbuffer::specularAttr = glGetUniformLocation(render::shaders::gBuffer::shader, "specular");
		shaders::gbuffer::modelAttr = glGetUniformLocation(render::shaders::gBuffer::shader, "model");


		shaders::shadow::lightMatrixAttr = glGetUniformLocation(shaders::shadow::shader, "lightSpaceMatrix");
	}

	void init(int screenWidth, int screenHeight)
	{
		P_INIT;
		P_START("init scene");
		SCR_WIDTH = screenWidth;
		SCR_HEIGHT = screenHeight;

		initShaders();

		testTexture = loadTexture("./textures/container2.png");
		specularMap = loadTexture("./textures/container_specular.png");

		char* dataBuffer = new char[5000000];
		for (int i = 0; i < 6; i++)
		{
			world::voxel::computeFaceMesh(vec3(0, 0, 0), (world::voxel::FaceDir)i, dataBuffer, 6 * 8 * i * sizeof(float), 1, vec3(0, 0, 0));
		}

		mesh::library::init();
		mesh::render::init();
		handle cubeMesh = mesh::library::loadMesh("cube", dataBuffer, 6 * 8 * 6 * sizeof(float));

		world::manager::init();
		int testSize = 10;
		for (int i = 0; i < testSize; i++)
		{
			for (int j = 0; j < testSize; j++)
			{
				world::manager::setCell(ivec3(i, 0, j), 1);
			}
			for (int j = 0; j < testSize; j++)
			{
				float height = 5.0f + cos(j / 13.0f) * cos(i/15.0f) * cos(i / 22.0f) * sin(j / 11.0f) * 5.0f + j/20;
				for (int k = 0; (float)k < height; k++)
				{
					world::manager::setCell(ivec3(i, k, j), 1);
				}
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
		mesh::render::add(transformId, cubeMesh, render::shaders::gBuffer::shader);

		transformId = transform::add(vec3(5, 1, 3), quat(), vec3(1, 1, 1));
		mesh::render::add(transformId, cubeMesh, render::shaders::gBuffer::shader);
		//rotation::animation::add(characterTransformId, vec3(0.5, -0.5, 0), vec3(0.5, -0.5, 0), 3, quat(), quat(vec3(0, 0, -glm::pi<float>() / 2)));

		handle lampId = transform::add(vec3(0, 2, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(lampId, cubeMesh);

		handle sunTransformId = transform::add(vec3(0, 1, 0), quat(vec3(1, 1, 0)), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(sunTransformId, cubeMesh);

		handle testTransformId = transform::add(vec3(0, 0, 0), quat(), vec3(0.1f, 0.1f, 0.1f));
		lampRenderer.add(testTransformId, cubeMesh);


		directionalLights.add(sunTransformId, 1, vec3(1), vec3(1));
		shadowRenderer.shadowCasters = &boxRenderer;
		shadowRenderer.init();

		render::debug::init(screenWidth, screenHeight);

		input::init();
		
		render::voxel::init();
		render::voxel::update();

		gamemode::manager::init(vec3(5, 10, 5));
		P_END;
	}

	void update(float deltaTime)
	{
		P_START("update scene");
		input::update();
		rotation::animation::update(deltaTime);
		rotation::anchor::update();
		movement::cube::update(deltaTime);

		render::voxel::update();

		gamemode::manager::update(deltaTime);
		input::reset();
		vec3 direction = camera::getCameraRot(camera::mainCamera) * vec3(0, 0, 1);
		vec3 position = camera::getCameraPos(camera::mainCamera);
		ImGui::DragFloat3("Raytrace Direction", (float*)&direction, 0.1f);
		ImGui::DragFloat3("Raytrace Position", (float*)&position, 0.1f);

		world::raytracing::ray ray = { position, direction };
		//world::raytracing::raytrace(position, direction, raytraceCells, 9);
		//render::debug::drawLine(camera::getCameraPos(camera::mainCamera), direction * 100.0f, vec4(1, 0, 0, 0.5f));
		char buffer[100];
		for (int i = 0; i < 30; i++)
		{
			ivec3 lastCell = ray.getCell();
			ivec3 cell = world::raytracing::raytraceNext(ray);
			if (world::manager::getCell(cell))
			{
				render::debug::drawCube((vec3)cell, vec3(1, 1, 1), vec4(1, 0, 0, 0.5f));
				render::debug::drawCube((vec3)lastCell, vec3(1, 1, 1), vec4(0, 0, 1, 0.5f));
				sprintf_s(buffer, "%i, %i, %i (%i)", cell.x, cell.y, cell.z, i);
				ImGui::Text(buffer);
				break;
			}
			else
			{
				//render::debug::drawCube((vec3)cell, vec3(1, 1, 1), vec4(0, 1, 0, 0.2f));
				sprintf_s(buffer, "%i, %i, %i", cell.x, cell.y, cell.z, i);
				ImGui::Text(buffer);
			}
		}
		P_END;
	}

	void render()
	{
		P_START("render scene");

		float zoom = camera::getZoom(camera::mainCamera);
		vec3 cameraPosition = camera::getCameraPos(camera::mainCamera);
		glm::mat4 projection = glm::perspective(glm::radians(zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 500.0f);
		glm::mat4 view = camera::getViewMatrix(camera::mainCamera);
		static glm::vec4 testVec(0, 0, 0, 1);
		// world transformation
		glm::mat4 model = glm::mat4(1.0f);

		render::clear_frame();
		render::start_render();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, testTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		
		glUniform1i(shaders::gbuffer::diffuseAttr, 0);
		glUniform1i(shaders::gbuffer::specularAttr, 1);
		glUniformMatrix4fv(shaders::gbuffer::modelAttr, 1, false, (float*)&model);

		mesh::render::render();

		shadowRenderer.render(shaders::shadow::shader, getLightMatrix(normalize(sunDirection), sunPovDistance), shaders::shadow::lightMatrixAttr);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		render::start_lighting();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for (int i = 0; i < pointLights.count; i++)
		{
			glUseProgram(shaders::pointLight::shader);
			glUniform1i(shaders::pointLight::gPositionAttr, 0);
			glUniform1i(shaders::pointLight::gNormalAttr, 1);
			glUniform1i(shaders::pointLight::gAlbedoSpecAttr, 2);
			glUniform3f(shaders::pointLight::viewPosAttr, cameraPosition.x, cameraPosition.y, cameraPosition.z);
			vec3 position;
			position = transform::positions[transform::indexOf(pointLights.transformIds[i])];
			glUniform3f(shaders::pointLight::lightPositionAttr, position.x, position.y, position.z);
			glUniform3f(shaders::pointLight::lightColorAttr, 1, 1, 1);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glUseProgram(shaders::directionalLight::shader);
		glUniform1i(shaders::directionalLight::gPositionAttr, 0);
		glUniform1i(shaders::directionalLight::gNormalAttr, 1);
		glUniform1i(shaders::directionalLight::gAlbedoSpecAttr, 2);
		glUniform3f(shaders::directionalLight::viewPosAttr, cameraPosition.x, cameraPosition.y, cameraPosition.z);
		vec3 nSunDir = normalize(sunDirection);
		glUniform3f(shaders::directionalLight::lightDirectionAttr, nSunDir.x, nSunDir.y, nSunDir.z);
		glUniform3f(shaders::directionalLight::lightColorAttr, 1, 1, 1);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowRenderer.depthMap);
		glUniform1i(shaders::directionalLight::shadowMapAttr, 4);
		glm::mat4 lightMatrix = getLightMatrix(normalize(sunDirection), sunPovDistance);
		glUniformMatrix4fv(shaders::directionalLight::lightMatrixAttr, 1, false, (float*)&lightMatrix);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisable(GL_BLEND);

		render::render_deferred();

		glUseProgram(shaders::lamp::shader);
		glUniform4fv(shaders::lamp::projectionAttr, 1, (float*)&projection);
		glUniform4fv(shaders::lamp::viewAttr, 1, (float*)&view);
		glUniform4fv(shaders::lamp::modelAttr, 1, (float*)&model);
		glUniform4fv(shaders::lamp::viewPosAttr, 1, (float*)&cameraPosition);

		P_END;
	}

	void showDebug()
	{
		profiler::display();
		P_FRAME_END;
	}
};