#pragma once

#include "interfaces/ioperator.h"

namespace Qp
{
	// This operator generates values between [min..max] at step interval.
	//
	class ConstScan : public IOperator
	{
	public:
		ConstScan(Value min, Value max, Value step, unsigned int repeat = 1);

		void Open() override;

		bool GetRow(Value *rgvals) override;

		void Close() override;

	private:
		Value cur;
		Value max;
		Value min;
		Value step;
		unsigned int repeat;

		unsigned int repeatCounter;
	};
}