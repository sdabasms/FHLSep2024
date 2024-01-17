#pragma once

#include "interfaces/ioperator.h"
#include "interfaces/iexpression.h"

namespace Qp
{
	// This operator joins rows from two inputs on two given columns.
	// Matching rows are determined by the JoinExpression; if that returns true, the rows are joined.
	//
	// Since joins operate on two tables, you'll need to specify 
	// how many columns the left and right side returns.
	// The join will keep separate Value arrays for each child internally.
	//
	class Join : public IOperator
	{
	public:
		Join(IOperator* left, IOperator* right, unsigned int clvals, unsigned int rcvals, JoinExpression expr);

		void Open() override;
		bool GetRow(Value* rgvals) override;
		void Close() override;

	private:
		IOperator* left;
		IOperator* right;
		unsigned int clvals;
		unsigned int crvals;
		JoinExpression expr;
		Value* lvals;
		Value* rvals;

		bool gotlrow;
		bool ropen;
	};
}