#pragma once
#include "transform.h"

namespace gamemode
{
	namespace manager
	{
		enum class GameMode
		{
			INGAME, MENU, EDITOR
		};

		void init(vec3 characterPos);
		void update(float deltaTime);
	}
}