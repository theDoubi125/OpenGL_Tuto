#pragma once
#include <vector>
#include <iostream>

#define MAX_ELEMENT_SIZE 128
#define MAX_COLUMN_COUNT 10

struct handle
{
	int id;

	void operator=(const handle& handle);
	bool operator<(const handle& handle) const;
	bool operator==(const handle& handle) const;
};

struct ColumnTypeList
{
	unsigned int sizes[MAX_COLUMN_COUNT];
	void* outputColumns[MAX_COLUMN_COUNT];
	unsigned int count;
};

struct AnonymousColumn
{
	int columnIndex;
	struct Table* table;
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

	void init(int allocSize, ColumnTypeList columnList)
	{
		addColumns(columnList);
		allocate(allocSize);
	}

	void addColumns(ColumnTypeList typeList)
	{
		for (int i = 0; i < typeList.count; i++)
		{
			AnonymousColumn addedColumn = addColumn(typeList.sizes[i]);
			((AnonymousColumn*)typeList.outputColumns[i])->columnIndex = addedColumn.columnIndex;
			((AnonymousColumn*)typeList.outputColumns[i])->table = addedColumn.table;
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

	AnonymousColumn addColumn(unsigned int size)
	{
		int columnIndex = columnCount;
		columnElementSize[columnCount] = size;
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

	template<typename T>
	Table& operator+=(Column<T>& column)
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

template<typename T, typename U>
ColumnTypeList operator+(Column<T>& col1, Column<U>& col2)
{
	ColumnTypeList result;
	result.count = 2;
	result.sizes[0] = sizeof(T);
	result.sizes[1] = sizeof(U);
	result.outputColumns[0] = &col1;
	result.outputColumns[1] = &col2;
	return result;
}

template<typename T>
ColumnTypeList operator+(ColumnTypeList list, Column<T>& col)
{
	list.sizes[list.count] = sizeof(T);
	list.outputColumns[list.count] = &col;
	list.count++;
	return list;
}