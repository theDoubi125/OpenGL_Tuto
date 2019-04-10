#include "table.h"

void Table::remove(int index)
{
	char buffer[MAX_ELEMENT_SIZE];
	for (int i = 0; i < columnCount; i++)
	{
		memcpy(buffer, &columns[i][index], columnElementSize[i]);
		memcpy(&columns[i][index], &columns[i][count - 1], columnElementSize[i]);
	}
	count--;
}

void Table::swap(int indexA, int indexB)
{
	char buffer[MAX_ELEMENT_SIZE];
	for (int i = 0; i < columnCount; i++)
	{
		memcpy(buffer, &columns[i][indexA], columnElementSize[i]);
		memcpy(&columns[i][indexA], &columns[i][indexB], columnElementSize[i]);
		memcpy(&columns[i][indexB], buffer, columnElementSize[i]);
	}
}