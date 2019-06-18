#include "transform.h"
#include "mesh_render.h"
#include "gameplay/world/voxel.h"
#include "gameplay/world/world.h"
#include "util/bit_array.h"
#include "mesh_render.h"

namespace render
{
	namespace voxel
	{
		ivec3 inormalVecs[] = { ivec3(1, 0, 0), ivec3(-1, 0, 0), ivec3(0, 1, 0), ivec3(0, -1, 0), ivec3(0, 0, 1), ivec3(0, 0, -1) };

		Table table;
		Column<handle> chunkIds;
		Column<handle> meshIds;
		Column<handle> transformIds;
		BitArray allocation;

		void init()
		{
			table.init(100, chunkIds + meshIds);
		}

		void computeChunkMesh(const world::voxel::Chunk& chunk, char* outData, int& outDataSize)
		{
			int count = 0;
			int dataCursor = 0;
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				for (int j = 0; j < CHUNK_SIZE; j++)
				{
					for (int k = 0; k < CHUNK_SIZE; k++)
					{
						if (chunk[ivec3(i, j, k)] > 0)
						{
							for (int l = 0; l < 6; l++) {
								ivec3 neighbour = ivec3(i, j, k) + inormalVecs[l];
								if (neighbour.x < 0 || neighbour.y < 0 || neighbour.z < 0 || neighbour.x >= CHUNK_SIZE || neighbour.y >= CHUNK_SIZE || neighbour.z >= CHUNK_SIZE || chunk[neighbour] == 0)
								{
									count++;
									world::voxel::computeFaceMesh(vec3((float)i, (float)j, (float)k), (world::voxel::FaceDir)l, outData, dataCursor + 6 * 8 * l * sizeof(float), 1, vec3(0, 0, 0));
								}
							}
							dataCursor += 6 * 8 * 6 * sizeof(float);
							outDataSize += 6 * 8 * 6 * sizeof(float);
						}
					}
				}
			}
		}

		void update()
		{
			char meshDataBuffer[500000];
			int meshDataSize = 500000;
			for (int i = 0; i < world::manager::addedChunksCount; i++)
			{
				handle id = { allocation.allocate() };
				TableElement element = table.element(id.id);
				handle chunkId = world::manager::addedChunks[i];
				computeChunkMesh(world::manager::getChunk(chunkId), meshDataBuffer, meshDataSize);
				handle meshHandle = mesh::library::loadMesh(meshDataBuffer, meshDataSize);

				handle transformId = transform::add((vec3)(world::manager::getChunkOffset(chunkId) * CHUNK_SIZE), quat(), vec3(1, 1, 1));
				//TODO : refactor the mesh render system to allow it to be used from here
				
				element << world::manager::addedChunks[i] << meshHandle << transformId;

			}
			world::manager::addedChunksCount = 0;
		}
	}
}