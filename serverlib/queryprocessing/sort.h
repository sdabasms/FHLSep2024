#pragma once

#include "interfaces/ioperator.h"
#include "interfaces/iexpression.h"

#include <deque>

namespace Qp
{
	// Sort operator that orders all data using a compare expression.
	//
	class Sort : public IOperator
	{
	public:
		Sort(IOperator* child, unsigned int nvals, CompareExpression cmp);

		void Open() override;
		bool GetRow(Value* rgvals) override;
		void Close() override;

	private:
		IOperator* child;
		unsigned int nvals;
		CompareExpression cmp;

		std::deque<Value*> childRows;
		bool firstGetRow;
	};
}