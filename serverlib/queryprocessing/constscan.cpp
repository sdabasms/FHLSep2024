#include "constscan.h"

namespace Qp
{
	ConstScan::ConstScan(Value min, Value max, Value step, unsigned int repeat)
		: min(min)
		, max(max)
		, step(step)
		, cur(min)
		, repeat(repeat)
		, repeatCounter(0)
	{}

	void ConstScan::Open()
	{
		cur = min;
		repeatCounter = 1;
	}

	bool ConstScan::GetRow(Value* rgvals)
	{
		if (cur <= max)
		{
			rgvals[0] = cur;

			if (repeatCounter < repeat)
			{
				++repeatCounter;
				return true;
			}

			repeatCounter = 1;
			cur += step;
			return true;
		}
		else
		{
			return false;
		}
	}

	void ConstScan::Close()
	{
	}
}