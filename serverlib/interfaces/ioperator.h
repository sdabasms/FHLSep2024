#include "common/value.h"

#pragma once


namespace Qp
{
	// This is the interface for a QP operator.
	//
	class IOperator
	{
	public:
		// Opens the operator and initialized internal state.
		//
		virtual void Open() = 0;

		// Gets a single row and writes the row values into rgvals.
		// Returns true if a row was fetched successfully, and false if no more rows are available.
		//
		virtual bool GetRow(Value *rgvals) = 0;

		// Closes the operator and releases resources.
		//
		virtual void Close() = 0;
	};
}
