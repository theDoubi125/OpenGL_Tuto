#include <glad/glad.h>
#include "gamemode_manager.h"
#include "render/render_pipeline.h"
#include "gameplay/input.h"
#include "gameplay/movement/cube_movement.h"
#include "gameplay/movement/first_person.h"
#include "gameplay/movement/third_person.h"
#include "camera.h"
#include "mesh_render.h"
#include "gameplay/world/raytracing.h"
#include "gameplay/world/world.h"
#include "gameplay/world/cell_transform.h"

namespace gamemode
{
	namespace manager
	{
		GameMode currentMode = GameMode::INGAME;
		handle spaceInputHandle;
		handle altInputHandle;
		handle cameraModeHandle;
		handle cameraTransform;
		handle characterTransformId;
		handle cubeMovementId;

		void init(vec3 startPos)
		{

			spaceInputHandle = input::registerKey(GLFW_KEY_SPACE);
			altInputHandle = input::registerKey(GLFW_KEY_LEFT_ALT);
			camera::init();
			cameraTransform = transform::add(vec3(-10, 5, 0), quat(), vec3(1, 1, 1));
			camera::mainCamera = camera::add(cameraTransform, 45.0f);

			movement::third_person::init();
			movement::first_person::init();


			characterTransformId = transform::add(startPos, quat(), vec3(1, 1, 1));
			cameraModeHandle = movement::third_person::add(cameraTransform, characterTransformId, 0.001f, 10);
			mesh::render::add(characterTransformId, mesh::library::findMesh("cube"), render::shaders::gBuffer::shader);
			cubeMovementId = movement::cube::add(characterTransformId, 0.5f);
		}
		
		void update(float deltaTime)
		{
			if (input::getState(spaceInputHandle) == input::KeyState::PRESSED)
			{
     			switch (currentMode)
				{
				case GameMode::EDITOR:
					movement::first_person::remove(cameraModeHandle);
					movement::third_person::add(cameraTransform, characterTransformId, 0.001f, 10);
					currentMode = GameMode::INGAME;
					break;
				case GameMode::INGAME:
					input::setCursorVisibility(false);
					movement::third_person::remove(cameraModeHandle);
					movement::first_person::add(cameraTransform, 0.001f, 10);
					currentMode = GameMode::EDITOR;
					break;
				}
			}
			if (input::getState(altInputHandle) == input::KeyState::PRESSED)
			{
				input::setCursorVisibility(true);
				switch (currentMode)
				{
				case GameMode::EDITOR:
					movement::first_person::remove(cameraModeHandle);
					break;
				case GameMode::INGAME:
					movement::third_person::remove(cameraModeHandle);
					break;
				}
			}
			else if (input::getState(altInputHandle) == input::KeyState::RELEASED)
			{
				input::setCursorVisibility(false);
				switch (currentMode)
				{
				case GameMode::EDITOR:
					cameraModeHandle = movement::first_person::add(cameraTransform, 0.001f, 10);
					break;
				case GameMode::INGAME:
					cameraModeHandle = movement::third_person::add(cameraTransform, characterTransformId, 0.001f, 10);
					break;
				}
			}
			switch (currentMode)
			{
			case GameMode::INGAME:
				quat camRotation = camera::getCameraRot(camera::mainCamera);
				movement::cube::cubeInput[cubeMovementId.id] = -(camRotation * vec3(0, 0, 1)) * input::movementInput.z + (camRotation * vec3(1, 0, 0)) * input::movementInput.x;
				if (input::getButtonState(1) == input::KeyState::PRESSED)
				{
					world::cellTransform::playStep();
				}
				break;
			case GameMode::EDITOR:
				vec3 direction = camera::getCameraRot(camera::mainCamera) * vec3(0, 0, 1);
				vec3 position = camera::getCameraPos(camera::mainCamera);

				world::raytracing::ray ray = { position, direction };
				if (input::getButtonState(0) == input::KeyState::PRESSED)
				{
					for (int i = 0; i < 30; i++)
					{
						ivec3 lastCell = ray.getCell();
						ivec3 cell = world::raytracing::raytraceNext(ray);
						if (world::manager::getCell(cell))
						{
							world::manager::setCell(lastCell, 1);
							break;
						}
					}
				}if (input::getButtonState(1) == input::KeyState::PRESSED)
				{
					for (int i = 0; i < 50; i++)
					{
						ivec3 lastCell = ray.getCell();
						ivec3 cell = world::raytracing::raytraceNext(ray);
						if (world::manager::getCell(cell))
						{
							world::manager::setCell(cell, 0);
							break;
						}
					}
				}
			}
			movement::third_person::update(deltaTime);
			movement::first_person::update(deltaTime);
		}
	}
}