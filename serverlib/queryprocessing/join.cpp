#include "join.h"

#include <algorithm>

namespace Qp
{
	Join::Join(IOperator* left, IOperator* right, unsigned int clvals, unsigned int crvals, JoinExpression expr)
		: left(left)
		, right(right)
		, clvals(clvals)
		, crvals(crvals)
		, expr(expr)
		, gotlrow(false)
		, ropen(false)
	{
		lvals = new Value[clvals];
		rvals = new Value[crvals];
	}

	void Join::Open()
	{
		left->Open();
	}

	bool Join::GetRow(Value* rgvals)
	{
		while (true)
		{
			if (!gotlrow)
			{
				std::copy(rgvals, rgvals + clvals, lvals);
				gotlrow = left->GetRow(lvals);

				if (!gotlrow)
				{
					// No more rows from left, we're done.
					//
					return false;
				}
			}

			if (!ropen)
			{
				right->Open();
				std::copy(rgvals + clvals, rgvals + clvals + crvals, rvals);
				ropen = true;
			}

			while (right->GetRow(rvals))
			{
				// Got a row from left and right, join.
				//
				if (expr(lvals, rvals))
				{
					// We found matching rows, copy to output.
					//
					std::copy(lvals, lvals + clvals, rgvals);
					std::copy(rvals, rvals + crvals, rgvals + clvals);
					return true;
				}
			}

			// Right side is out of rows, move to next left row.
			//
			right->Close();
			ropen = false;
			gotlrow = false;
		}
	}

	void Join::Close()
	{
		if (ropen)
		{
			right->Close();
		}

		left->Close();
	}
}