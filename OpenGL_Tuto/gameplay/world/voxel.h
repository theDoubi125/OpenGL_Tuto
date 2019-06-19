#pragma once

#define CHUNK_SIZE 32
#define REGION_SIZE 16
#include "types.h"

class MeshLibrary;

namespace world
{
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
			Chunk(char data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE]);

			char data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

			char& operator[](ivec3 cell);
			char operator[](ivec3 cell) const;

		};

		void computeFaceMesh(vec3 center, FaceDir direction, char* outData, int dataCursor, float cellSize, vec3 offset);
		void computeChunkMesh(const Chunk& chunk, char* outData, int& outDataSize, vec3 offset);

		void chunkEditor(Chunk& chunk, MeshLibrary& meshLibrary);

	}
}