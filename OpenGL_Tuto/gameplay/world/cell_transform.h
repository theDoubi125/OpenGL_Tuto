#include "transform.h"

namespace world
{
	enum Direction
	{
		East = 0,
		South = 1,
		West = 2,
		North = 3,
		Up = 4,
		Down = 5
	};

	quat dirToQuat(Direction direction);
	Direction turnDir(Direction dir, int offset); // direction faced after turning by %offset% * 90 degrees to the right. Works only on East, North, West and South

	namespace cellTransform
	{
		extern Column<handle> transformIds;
		extern Column<ivec3> cells;


		void init();
		handle add(handle transformId, ivec3 cell, Direction direction);
		void move(handle element, ivec3 offset);
		void turn(handle toTurn, int directionOffset);
		void remove(handle elementId);
		void update(float deltaTime);
		void playStep();
	}
}