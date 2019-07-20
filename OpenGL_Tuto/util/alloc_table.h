#pragma once

#include "bit_array.h"
#include "table.h"

struct AllocTable
{
	Table table;
	BitArray allocation;

	void init(int allocSize, ColumnTypeList columnList);
	TableElement allocate();
	void free(handle element);
	BitIterator begin();
	void clear();
};