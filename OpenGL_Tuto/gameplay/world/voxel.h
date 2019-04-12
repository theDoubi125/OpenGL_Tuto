#pragma once

#define CHUNK_SIZE 16
#include "types.h"

namespace voxel
{
	enum class FaceDir
	{
		RIGHT = 0, 
		LEFT = 1, 
		FORWARD = 2,
		BACK = 3,
		TOP = 4,
		BOTTOM = 5
	};

	struct Chunk
	{
		Chunk(char defaultVoxel);
		char data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

		char& operator[](ivec3 cell);
		char operator[](ivec3 cell) const;

	};

	void computeFaceMesh(vec3 center, FaceDir direction, float* outData, int dataCursor, float cellSize);
	void computeChunkMesh(const Chunk& chunk, float* outData, int& outDataSize);

}