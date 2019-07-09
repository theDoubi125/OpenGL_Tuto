#include "voxel.h"
#include <cstring>
#include <glad/glad.h>
#include "transform.h"

#include "imgui.h"
#include "mesh_render.h"

namespace world
{
	namespace voxel
	{
		// A region is a x*x*x chunks zone that is save in a single file
		// File structure : list of headers (coords of the chunk) + data array

		Chunk::Chunk(char voxel)
		{
			memset(data, voxel, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
		}

		Chunk::Chunk(char voxels[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE])
		{
			memcpy(data, voxels, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
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
		ivec3 inormalVecs[] = { ivec3(1, 0, 0), ivec3(-1, 0, 0), ivec3(0, 1, 0), ivec3(0, -1, 0), ivec3(0, 0, 1), ivec3(0, 0, -1) };
		vec3 facesX[] = { vec3(0, 1, 0), vec3(0, -1, 0), vec3(1, 0, 0), vec3(-1, 0, 0), vec3(1, 0, 0), vec3(-1, 0, 0) };
		vec3 factors[] = { vec3(-1, -1, 1), vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, 1, 1) };
		float uvs[] = { 0, 0,  1.0f, 1.0f,  1.0f, 0.0f,  0, 0,  0, 1.0f,  1.0f, 1.0f };

		void computeFaceMesh(vec3 center, FaceDir direction, char* outData, int dataCursor, float cellSize, vec3 offset)
		{
			vec3 x = facesX[(int)direction];
			vec3 z = normalVecs[(int)direction];
			vec3 y = glm::cross(x, z);

			for (int i = 0; i < 6; i++)
			{
				vec3 test = factors[i].y * y;
				vec3 vertex = center + (factors[i].x * x + factors[i].y * y + factors[i].z * z) * cellSize / 2.0f + offset;
				vec3 normal = z;
				memcpy(&(outData[dataCursor + 8 * i * sizeof(float)]), &vertex, sizeof(vec3));
				memcpy(&(outData[dataCursor + (8 * i + 3) * sizeof(float)]), &normal, sizeof(vec3));
				memcpy(&(outData[dataCursor + (8 * i + 6) * sizeof(float)]), &(uvs[i * 2]), sizeof(float) * 2);
			}
		}


		void computeChunkMesh(const Chunk& chunk, char* outData, int& outDataSize, vec3 offset)
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
									voxel::computeFaceMesh(vec3((float)i, (float)j, (float)k), (voxel::FaceDir)l, outData, dataCursor + 6 * 8 * l * sizeof(float), 1, offset);
								}
							}
							dataCursor += 6 * 8 * 6 * sizeof(float);
							outDataSize += 6 * 8 * 6 * sizeof(float);
						}
					}
				}
			}
		}

		void chunkEditor(Chunk& chunk, MeshLibrary& meshLibrary)
		{
			static int layer = 0;
			ImGui::SliderInt("layer", &layer, 0, CHUNK_SIZE);

			bool mustUpdateDisplay = false;
			char nameBuffer[500];
			ImGui::Columns(CHUNK_SIZE);
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_SIZE; y++)
				{
					int voxelValue = chunk[vec3(x, layer, y)];
					sprintf_s(nameBuffer, "voxel %d %d", x, y);
					if (ImGui::SliderInt(nameBuffer, &voxelValue, 0, 1))
					{
						chunk[vec3(x, layer, y)] = voxelValue;
						mustUpdateDisplay = true;
					}
					ImGui::NextColumn();
				}
			}
			ImGui::Columns(1);
			if (mustUpdateDisplay)
			{
				char *dataBuffer = new char[5000000];
				int chunkMeshSize = 0;
				voxel::computeChunkMesh(chunk, dataBuffer, chunkMeshSize, vec3(0, 0, 0));
				//mesh::replaceMesh(, dataBuffer, chunkMeshSize);
				delete[] dataBuffer;
			}
		}
	}
}