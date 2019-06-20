#include "voxel.h"
#include <cstring>
#include <glad/glad.h>
#include "transform.h"

#include "imgui.h"
#include "mesh_render.h"

#include "util/bit_array.h"

namespace world
{
	namespace manager
	{
		Table regionTable;
		Column<ivec3> offsets;
		Column<voxel::Chunk> chunks;
		BitArray allocation;

		// chunks added this frame
		handle addedChunks[100];
		int addedChunksCount = 0;

		void init()
		{
			regionTable.init(100, offsets + chunks);
			allocation.init(100);
		}

		void loadChunk(ivec3 offset, char data[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE])
		{
			handle id = { allocation.allocate() };
			TableElement element = regionTable.element(id.id);
			element << offset << voxel::Chunk(data);
			addedChunks[addedChunksCount] = id;
			addedChunksCount++;
		}

		char getCell(ivec3 cell)
		{
			ivec3 chunkOffset = cell / CHUNK_SIZE - ivec3(cell.x < 0 ? 1 : 0, cell.y < 0 ? 1 : 0, cell.z < 0 ? 1 : 0);
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				if (chunkOffset == offsets[*it])
				{
					return chunks[*it][cell - chunkOffset * CHUNK_SIZE];
				}
			}
			return 0;
		}

		char getCell(ivec3 cell, ivec3 chunkOffset)
		{
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				if (chunkOffset == offsets[*it])
				{
					return chunks[*it][cell - chunkOffset * CHUNK_SIZE];
				}
			}
			return 0;
		}

		void setCell(ivec3 cell, char content)
		{
			ivec3 chunkOffset = cell / CHUNK_SIZE - ivec3(cell.x < 0 ? 1 : 0, cell.y < 0 ? 1 : 0, cell.z < 0 ? 1 : 0);
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				if (chunkOffset == offsets[*it])
				{
					chunks[*it][cell - chunkOffset * CHUNK_SIZE] = content;
					return;
				}
			}
			voxel::Chunk newChunk((char)0);
			newChunk[cell - chunkOffset * CHUNK_SIZE] = content;
			handle id = { allocation.allocate() };
			TableElement element = regionTable.element(id.id);
			element << chunkOffset << newChunk;
			addedChunks[addedChunksCount] = id;
			addedChunksCount++;
		}

		void computeChunksMesh(char* outData, int& outDataSize)
		{
			int dataCursor = 0;
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				int dataSize = 0;
				world::voxel::computeChunkMesh(chunks[*it], &outData[dataCursor], dataSize, offsets[*it] * CHUNK_SIZE);
				dataCursor += dataSize;
			}
			outDataSize = dataCursor;
		}

		voxel::Chunk& getChunk(ivec3 chunk)
		{
			for (auto it = allocation.begin(); it.isValid(); it++)
			{
				if (offsets[*it] == chunk)
					return chunks[*it];
			}
			voxel::Chunk newChunk((char)0);
			handle id = { allocation.allocate() };
			TableElement element = regionTable.element(id.id);
			element << chunk << newChunk;
			addedChunks[addedChunksCount] = id;
			addedChunksCount++;
			return chunks[id];
		}

		voxel::Chunk& getChunk(handle chunkId)
		{
			return chunks[chunkId];
		}

		ivec3 getChunkOffset(handle chunkId)
		{
			return offsets[chunkId];
		}
	}

}