#include "sort.h"

#include <algorithm>

namespace Qp
{
	Sort::Sort(IOperator* child, unsigned int nvals, CompareExpression cmp)
		: child(child)
		, nvals(nvals)
		, cmp(cmp)
	{
	}

	void Sort::Open()
	{
		firstGetRow = true;
	}

	bool Sort::GetRow(Value* rgvals)
	{
		if (firstGetRow)
		{
			child->Open();
			bool childGotRow;
			do
			{
				Value* rgvalsChild = new Value[nvals];
				childGotRow = child->GetRow(rgvalsChild);

				if (childGotRow)
				{
					childRows.push_back(rgvalsChild);
				}
				else
				{
					delete[] rgvalsChild;
				}
			} while (childGotRow);
			child->Close();

			// Now sort all the rows.
			//
			std::sort(childRows.begin(), childRows.end(), cmp);
			firstGetRow = false;
		}

		if (childRows.empty())
		{
			return false;
		}

		Value* row = childRows.front();
		std::copy(row, row + nvals, rgvals);

		childRows.pop_front();
		delete[] row;
		return true;
	}

	void Sort::Close()
	{
		for(Value * row : childRows)
		{
			delete[] row;
		}
		childRows.clear();
	}
}