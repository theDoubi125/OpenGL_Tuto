#pragma once

#include "util/table.h"

struct JoinIterator
{
	Column<handle> mainColumn;
	Column<handle> secondaryColumn;
};