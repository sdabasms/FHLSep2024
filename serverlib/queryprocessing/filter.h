#pragma once

#include "interfaces/ioperator.h"
#include "interfaces/iexpression.h"

namespace Qp
{
	// This operator only returns rows that match the predicate
	// and discards all other rows.
	//
	class Filter : public IOperator
	{
	public:
		Filter(IOperator *child, BooleanExpression expr);

		void Open() override;
		bool GetRow(Value* rgvals) override;
		void Close() override;

	private:
		IOperator* child;
		BooleanExpression expr;
	};
}