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
		voxel::Chunk& getChunk(handle chunkId);
		ivec3 getChunkOffset(handle chunkId);

		// chunks added this frame. Set count to 0 when treatment finished
		extern handle addedChunks[100];
		extern int addedChunksCount;
	}
}