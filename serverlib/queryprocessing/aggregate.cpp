#include "aggregate.h"

#include <algorithm>

namespace Qp
{
	StreamAggregate::StreamAggregate(IOperator* child, unsigned int nvals, unsigned int groupByColumn, AggregateExpression aggExpression)
		: child(child)
		, nvals(nvals)
		, groupByColumn(groupByColumn)
		, aggExpression(aggExpression)
	{
		rgvalsChild = new Value[nvals];
	}

	void StreamAggregate::Open()
	{
		child->Open();
		pendingChildRow = false;
		childDone = false;
	}

	bool StreamAggregate::GetRow(Value* rgvals)
	{
		if(childDone)
		{
			return false;
		}
		bool newGroup = true;
		Value groupByValue;

		while (true)
		{
			if (pendingChildRow)
			{
				pendingChildRow = false;
			}
			else if (!child->GetRow(rgvalsChild))
			{
				childDone = true;
				return true;
			}

			if (newGroup)
			{
				groupByValue = rgvalsChild[groupByColumn];
			}

			if (rgvalsChild[groupByColumn] == groupByValue)
			{
				aggExpression(rgvalsChild, rgvals, newGroup);
				newGroup = false;
			}
			else
			{
				pendingChildRow = true;
				return true;
			}
		}
	}

	void StreamAggregate::Close()
	{
		child->Close();
	}
}