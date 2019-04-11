#include "voxel.h"
#include <cstring>
#include <glad/glad.h>
#include "transform.h"


namespace voxel
{
	Chunk::Chunk(char voxel)
	{
		memset(data, voxel, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	}

	char& Chunk::operator[](ivec3 cell)
	{
		return data[cell.x + cell.y * CHUNK_SIZE + cell.z * CHUNK_SIZE * CHUNK_SIZE];
	}

	char Chunk::operator[](ivec3 cell) const
	{
		return data[cell.x + cell.y * CHUNK_SIZE + cell.z * CHUNK_SIZE * CHUNK_SIZE];
	}

	vec3 normalVecs[] = { vec3(1, 0, 0), vec3(-1, 0, 0), vec3(0, 1, 0), vec3(0, -1, 0), vec3(0, 0, 1), vec3(0, 0, -1) };
	vec3 facesX[] = { vec3(0, 1, 0), vec3(0, -1, 0), vec3(1, 0, 0), vec3(-1, 0, 0), vec3(1, 0, 0), vec3(-1, 0, 0) };
	vec3 factors[] = { vec3(-1, -1, 1), vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, 1, 1) };
	float uvs[] = { 0, 0,  1.0f, 1.0f,  1.0f, 0.0f,  0, 0,  0, 1.0f,  1.0f, 1.0f };

	void computeFaceMesh(vec3 center, FaceDir direction, float* outData, float cellSize)
	{
		vec3 x = facesX[(int)direction];
		vec3 z = normalVecs[(int)direction];
		vec3 y = glm::cross(x, z);

		for (int i = 0; i < 6; i++)
		{
			vec3 test = factors[i].y * y;
			vec3 vertex = center + (factors[i].x * x + factors[i].y * y + factors[i].z * z) * cellSize / 2.0f;
			vec3 normal = z;
			memcpy(&(outData[8 * i]), &vertex, sizeof(vec3));
			memcpy(&(outData[8 * i + 3]), &normal, sizeof(vec3));
			memcpy(&(outData[8 * i + 6]), &(uvs[i * 2]), sizeof(float) * 2);
		}
	}


	void computeChunkMesh(const Chunk& chunk, float* outData, int& outDataSize)
	{
		float* dataCursor = outData;
		for (int i = 0; i < CHUNK_SIZE; i++)
		{
			for (int j = 0; j < CHUNK_SIZE; j++)
			{
				for (int k = 0; k < CHUNK_SIZE; k++)
				{
					if (chunk[ivec3(i, j, k)] > 0)
					{
						for (int l = 0; l < 6; l++) {
							voxel::computeFaceMesh(vec3((float)i, (float)j, (float)k), (voxel::FaceDir)l, &(dataCursor[6 * 8 * l]), 1);
						}
						dataCursor = &(dataCursor[6 * 8 * 6]);
						outDataSize += 6 * 8 * 6 * sizeof(float);
					}
				}
			}
		}
	}
}