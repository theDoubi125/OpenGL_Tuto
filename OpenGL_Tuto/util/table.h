#pragma once
#include <vector>

#define MAX_ELEMENT_SIZE 128
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

struct TableElement
{
	Table* table;
	int elementIndex;
	int columnCursor = 0;
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
	TableElement& operator+=(const T& data)
	{
		TableElement result = {this, count};
		return *this;
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

	TableElement push()
	{
		return { this, count++ };
	}

	TableElement element(int index)
	{
		return { this, index };
	}

	template<typename T>
	T* column(int columnIndex)
	{
		return (T*)columns[columnIndex];
	}

	template<typename T>
	Table& operator>>(Column<T>& column)
	{
		column = addColumn<T>();
		return *this;
	}

	void remove(int index);

	void swap(int indexA, int indexB);
};

template<typename T>
TableElement& operator<<(TableElement& tableElement, const T& data)
{
	Table& table = *tableElement.table;
	if (table.columnElementSize[tableElement.columnCursor] != sizeof(data))
	{
		std::cerr << "Error on table push : element size mismatch" << std::endl;
	}
	T* column = table.column<T>(tableElement.columnCursor);
	column[tableElement.elementIndex] = data;
	tableElement.columnCursor++;
	return tableElement;
}