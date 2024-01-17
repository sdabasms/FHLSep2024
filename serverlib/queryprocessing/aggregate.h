#pragma once

#include "interfaces/ioperator.h"
#include "interfaces/iexpression.h"

namespace Qp
{
	// An aggregate aggregates rows using the aggregateExpr
	// grouped by the column value in the matchColumn.
	// A stream aggregate requires the rows to be sorted on matchColumn.
	//
	class StreamAggregate : public IOperator
	{
	public:
		StreamAggregate(IOperator* child, unsigned int nvals, unsigned int groupByColumn, AggregateExpression aggExpression);

		void Open() override;
		bool GetRow(Value* rgvals) override;
		void Close() override;

	private:
		IOperator* child;
		Value* rgvalsChild;
		unsigned int nvals;
		unsigned int groupByColumn;
		AggregateExpression aggExpression;
		bool pendingChildRow;
		bool childDone;
	};
}