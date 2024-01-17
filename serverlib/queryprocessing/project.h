#pragma once

#include "interfaces/ioperator.h"
#include "interfaces/iexpression.h"

namespace Qp
{
	// This operator calls an expression on every row received from its child.
	//
	class Project : public IOperator
	{
	public:
		Project(IOperator* child, ProjectExpression expr);

		void Open() override;
		bool GetRow(Value* rgvals) override;
		void Close() override;

	private:
		IOperator* child;
		ProjectExpression expr;
	};
	}