#include "bit_array.h"

#define FLAG_PER_INT (sizeof(unsigned int) * 8)
#define FLAG_PER_CHAR (sizeof(char) * 8)

void BitArray::init(int capacity)
{
	arraySize = capacity / FLAG_PER_INT + ((capacity % FLAG_PER_INT) > 0 ? 1 : 0);
	data = new unsigned int[arraySize];
	for (int i = 0; i < arraySize; i++)
	{
		data[i] = 0;
	}
}

void BitArray::clear()
{
	for (int i = 0; i < arraySize; i++)
	{
		data[i] = 0;
	}
}

int BitArray::allocate()
{
	for (int i = 0; i < arraySize; i++)
	{
		if (~data[i] > 0)
		{
			unsigned int lastData = data[i];
			data[i] |= (lastData + 1);
			int j;
			unsigned int insertPosition = data[i] - lastData;
			for (j = 0; j < FLAG_PER_INT && insertPosition > 0; j++)
			{
				insertPosition = insertPosition >> 1;
			}
			if (j - 1 < beginIndex || j - 1 > endIndex)
				updateBounds();
			return j - 1 + i * FLAG_PER_INT;
		}
	}
	return -1;
}

void BitArray::allocate(int index)
{
	unsigned int x = 1 << (index % FLAG_PER_INT);
	data[index / FLAG_PER_INT] |= x;
	if (index < beginIndex || index > endIndex)
		updateBounds();
}

void BitArray::free(int index)
{
	unsigned int x = ~(1 << (index % FLAG_PER_INT));
	data[index / FLAG_PER_INT] &= x;
	if (index == beginIndex || index == endIndex)
		updateBounds();
}

void BitArray::updateBounds()
{
	for (beginIndex = 0; beginIndex < flagCount() && !value(beginIndex); beginIndex++);
	for (endIndex = flagCount() - 1; endIndex > beginIndex && !value(endIndex); endIndex--);
}

int BitArray::flagCount() const
{
	return arraySize * FLAG_PER_INT;
}

bool BitArray::value(int index) const
{
	return data[index / FLAG_PER_INT] & (1 << (index % FLAG_PER_INT));
}

BitIterator BitArray::begin()
{
	return { beginIndex, *this };
}

BitIterator BitArray::iterator(handle element)
{
	return { element.id, *this };
}

BitIterator BitArray::end()
{
	return { endIndex + 1, *this };
}

BitIterator& BitIterator::operator++(int i)
{
	index++;
	while (!bitArray.value(index) && index < bitArray.endIndex)
	{
		index++;
	}
	return *this;
}

BitIterator& BitIterator::operator++()
{
	index++;
	while (!bitArray.value(index) && index < bitArray.endIndex)
	{
		index++;
	}
	return *this;
}

bool BitIterator::isValid() const
{
	return index < bitArray.flagCount() && bitArray.value(index);
}

int BitIterator::operator*()
{
	return index;
}


bool BitIterator::operator!=(const BitIterator& other) const
{
	return &other.bitArray != &bitArray || other.index != index;
}