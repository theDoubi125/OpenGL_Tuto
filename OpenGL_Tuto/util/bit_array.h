#pragma once

#include "transform.h"

class BitIterator
{
public:
	int index = 0;
	const struct BitArray& bitArray;

	BitIterator(int element, const BitArray& bitArray) : index(element), bitArray(bitArray) {}

	BitIterator& operator++(int i);
	BitIterator& operator++();
	bool operator!=(const BitIterator& other) const;
	int operator*();
	bool isValid() const;

};

struct BitArray
{
	void init(int flagCount);
	int allocate();
	void allocate(int index);
	void free(int index);
	void clear();

	unsigned int* data;
	int arraySize;

	int flagCount() const;
	bool value(int flag) const;
	void updateBounds();

	BitIterator begin();
	BitIterator iterator(handle element);
	BitIterator end();

	int beginIndex = 0, endIndex = -1;
};