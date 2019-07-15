#include <glad/glad.h>
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
		Column<handle> renderIds;
		BitArray allocation;

		void init()
		{
			table.init(100, chunkIds + meshIds + transformIds + renderIds);
			allocation.init(100);
		}

		void computeChunkMesh(handle chunkId, char* outData, int& outDataSize)
		{
			int count = 0;
			int dataCursor = 0;
			outDataSize = 0;
			ivec3 chunkOffset = world::manager::getChunkOffset(chunkId);
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				for (int j = 0; j < CHUNK_SIZE; j++)
				{
					for (int k = 0; k < CHUNK_SIZE; k++)
					{
						if (world::manager::getCell(chunkOffset * CHUNK_SIZE + ivec3(i, j, k)) > 0)
						{
							for (int l = 0; l < 6; l++) {
								ivec3 neighbour = ivec3(i, j, k) + inormalVecs[l];
								char neighbourCell = world::manager::getCell(chunkOffset * CHUNK_SIZE + neighbour);
								if (neighbourCell == 0)
								{
									count++;
									world::voxel::computeFaceMesh(vec3((float)i, (float)j, (float)k), (world::voxel::FaceDir)l, outData, dataCursor, 1, vec3(0, 0, 0));
									dataCursor += 6 * 8 * sizeof(float);
									outDataSize += 6 * 8 * sizeof(float);
								}
							}
						}
					}
				}
			}
		}

		void update()
		{
			char meshDataBuffer[1000000];

			for (int i = 0; i < world::manager::modifiedChunksCount; i++)
			{
				int meshDataSize = 1000000;
				handle id = { allocation.allocate() };
				TableElement element = table.element(id.id);
				handle chunkId = world::manager::modifiedChunks[i];
				computeChunkMesh(chunkId, meshDataBuffer, meshDataSize);
				mesh::library::replaceMesh(meshDataBuffer, meshDataSize, meshIds[chunkId]);

				vec3 chunkOffset = (vec3)(world::manager::getChunkOffset(chunkId) * CHUNK_SIZE);
				handle transformId = transform::add(chunkOffset, quat(), vec3(1, 1, 1));
				mesh::render::updateMesh(renderIds[chunkId], meshIds[chunkId], 3);
			}
			world::manager::modifiedChunksCount = 0;

			for (int i = 0; i < world::manager::addedChunksCount; i++)
			{
				int meshDataSize = 1000000;
				handle id = { allocation.allocate() };
				TableElement element = table.element(id.id);
				handle chunkId = world::manager::addedChunks[i];
				computeChunkMesh(chunkId, meshDataBuffer, meshDataSize);
				handle meshHandle = mesh::library::loadMesh(meshDataBuffer, meshDataSize);

				vec3 chunkOffset = (vec3)(world::manager::getChunkOffset(chunkId) * CHUNK_SIZE);
				handle transformId = transform::add(chunkOffset, quat(), vec3(1, 1, 1));
				handle renderId = mesh::render::add(transformId, meshHandle, 3);

				element << world::manager::addedChunks[i] << meshHandle << transformId << renderId;

			}
			world::manager::addedChunksCount = 0;
		}
	}
}