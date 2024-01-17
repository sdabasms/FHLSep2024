#include "project.h"

#include <algorithm>

namespace Qp
{
	Project::Project(IOperator* child, ProjectExpression expr)
		: child(child)
		, expr(expr)
	{
	}

	void Project::Open()
	{
		child->Open();
	}

	bool Project::GetRow(Value* rgvals)
	{
		bool ret = child->GetRow(rgvals);

		if (ret)
		{
			expr(rgvals);
		}

		return ret;
	}

	void Project::Close()
	{
		child->Close();
	}
}