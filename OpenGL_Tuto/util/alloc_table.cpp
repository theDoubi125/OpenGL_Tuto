#include "alloc_table.h"


void AllocTable::init(int allocSize, ColumnTypeList columnList)
{
	table.init(allocSize, columnList);
	allocation.init(allocSize);
}

TableElement AllocTable::allocate()
{
	handle element = { allocation.allocate() };
	return table.element(element.id);
}

void AllocTable::free(handle element)
{
	allocation.free(element.id);
}

BitIterator AllocTable::begin()
{
	return allocation.begin();
}

void AllocTable::clear()
{
	allocation.clear();
}