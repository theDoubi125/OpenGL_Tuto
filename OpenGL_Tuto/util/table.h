#pragma once
#include <vector>

#define MAX_COLUMN_COUNT 10

struct handle
{
	int id;

	void operator=(const handle& handle);
	bool operator<(const handle& handle) const;
	bool operator==(const handle& handle) const;
};

template<typename T>
struct Column
{
	int columnIndex;
	struct Table* table;

	T& operator[](int index)
	{
		return table->column<T>(columnIndex)[index];
	}
	const T& operator[](int index) const
	{
		return table->column<T>(columnIndex)[index];
	}

	void operator=(const Column<T>& other)
	{
		columnIndex = other.columnIndex;
		table = other.table;
	}
};

struct Table
{
	char* columns[MAX_COLUMN_COUNT];
	int columnElementSize[MAX_COLUMN_COUNT];
	int count = 0;
	int columnCount = 0;

	int allocatedSize = 0;

	void allocate(int size)
	{
		allocatedSize = size;
		for (int i = 0; i < columnCount; i++)
		{
			columns[i] = new char[allocatedSize * columnElementSize[i]];
		}
	}

	void reallocate(int allocSize)
	{
		char* columnsBackup[MAX_COLUMN_COUNT];
		memcpy(columnsBackup, columns, sizeof(columns));
		allocate(allocSize);
		for (int i = 0; i < columnCount; i++)
		{
			memcpy(columns[i], columnsBackup[i], count * columnElementSize[i]);
		}
		for (int i = 0; i < columnCount; i++)
		{
			delete columnsBackup[i];
		}
	}

	template<typename T>
	Column<T> addColumn()
	{
		int columnIndex = columnCount;
		columnElementSize[columnCount] = sizeof(T);
		columnCount++;
		return { columnIndex, this };
	}

	template<typename T>
	T& element(int column, int index)
	{
		return ((T*)columns[column])[index];
	}

	template<typename T>
	handle push(const T& data)
	{
		if (count >= allocatedSize)
		{
			reallocate(allocatedSize * 2);
		}
		char* dataCursor = (char*)&data;
		for (int i = 0; i < columnCount; i++)
		{
			memcpy(columns[i], dataCursor, columnElementSize[i]);
			dataCursor += columnElementSize[i];
		}
		handle result = { count };
		count++;
		return result;
	}

	template<typename T>
	T* column(int columnIndex)
	{
		return (T*)columns[columnIndex];
	}
};