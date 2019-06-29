#include <glad/glad.h>
#include "gamemode_manager.h"
#include "render/render_pipeline.h"
#include "gameplay/input.h"
#include "gameplay/movement/cube_movement.h"
#include "gameplay/movement/first_person.h"
#include "gameplay/movement/third_person.h"
#include "camera.h"
#include "mesh_render.h"

namespace gamemode
{
	namespace manager
	{
		GameMode currentMode = GameMode::INGAME;
		handle spaceInputHandle;
		handle cameraModeHandle;
		handle cameraTransform;
		handle characterTransformId;
		handle cubeMovementId;

		void init(vec3 startPos)
		{

			spaceInputHandle = input::registerKey(GLFW_KEY_SPACE);
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
					movement::third_person::remove(cameraModeHandle);
					currentMode = GameMode::MENU;
					input::setCursorVisibility(true);
					break;
				case GameMode::MENU:
					input::setCursorVisibility(false);
					movement::first_person::add(cameraTransform, 0.001f, 10);
					currentMode = GameMode::EDITOR;
					break;
				}
			}
			switch (currentMode)
			{
			case GameMode::INGAME:
				quat camRotation = camera::getCameraRot(camera::mainCamera);
				movement::cube::cubeInput[cubeMovementId.id] = -(camRotation * vec3(0, 0, 1)) * input::movementInput.z + (camRotation * vec3(1, 0, 0)) * input::movementInput.x;
				break;
			}
			movement::third_person::update(deltaTime);
			movement::first_person::update(deltaTime);
		}
	}
}