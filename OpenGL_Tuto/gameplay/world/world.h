#pragma once

#define CHUNK_SIZE 16
#define REGION_SIZE 16
#include "types.h"

class MeshLibrary;

namespace world
{
	namespace manager
	{
		void init();
		void loadChunk(ivec3 offset, char data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE]);
		char getCell(ivec3 cell);
		void setCell(ivec3 cell, char content);
		void computeChunksMesh(char* outData, int& outDataSize);
		voxel::Chunk& getChunk(ivec3 chunk);
	}
}