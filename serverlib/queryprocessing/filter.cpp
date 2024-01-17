#include "filter.h"

namespace Qp
{
	Filter::Filter(IOperator* child, BooleanExpression expr)
		: child(child)
		, expr(expr)
	{
	}

	void Filter::Open()
	{
		child->Open();
	}

	bool Filter::GetRow(Value* rgvals)
	{
		do
		{
			if (!child->GetRow(rgvals))
			{
				return false;
			}

			if (expr(rgvals))
			{
				return true;
			}
		} while (true);
	}

	void Filter::Close()
	{
		child->Close();
	}
}