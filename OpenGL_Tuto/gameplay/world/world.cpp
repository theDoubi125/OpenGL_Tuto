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
		}

		char getCell(ivec3 cell)
		{
			ivec3 chunkOffset = cell / CHUNK_SIZE - ivec3(cell.x < 0 ? 1:0, cell.y < 0 ? 1:0, cell.z < 0 ? 1:0);
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
			return chunks[id];
		}
	}

}