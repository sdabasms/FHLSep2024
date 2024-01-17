#pragma once

#include "value.h"

void DumpValues(std::ostream& out, Value* rgvals, unsigned int cvals)
{
	for (unsigned int i = 0; i < cvals; ++i)
	{
		if (i > 0)
		{
			out << ", ";
		}

		out << rgvals[i];
	}

	out << '\n';
}