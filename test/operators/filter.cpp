/*
	This file contains QP tests for the Filter operator.
*/

#include "pch.h"

// Tests that the filter only returns rows equal to 5
//
TEST(QpTestSuiteFilter, FilterEq5)
{
	Value rgvals[1];

	Qp::ConstScan rowGen(0, 10, 1);
	Qp::Filter filter(&rowGen, [](Value* rgvals) { return rgvals[0] == 5; });

	Value numRows = 0;

	ExecuteQuery(&filter,
		rgvals,
		[&](Value *rgvals)
		{
			EXPECT_EQ(5, rgvals[0]);
			++numRows;
		});

	EXPECT_EQ(1, numRows);
}


// Tests that the filter only returns rows greater than 5
//
TEST(QpTestSuiteFilter, FilterGt5)
{
	Value rgvals[1];

	Qp::ConstScan rowGen(0, 10, 1);
	Qp::Filter filter(&rowGen, [](Value* rgvals) { return rgvals[0] > 5; });

	Value numRows = 0;

	ExecuteQuery(&filter,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(5 + numRows, rgvals[0]);
		});

	EXPECT_EQ(5, numRows);
}

// Tests that the filter does not return any rows
//
TEST(QpTestSuiteFilter, FilterNoRows)
{
	Value rgvals[2];

	Qp::ConstScan rowGen(0, 10, 1);
	Qp::Filter filter(&rowGen, [](Value* rgvals) { return rgvals[0] == 999; });

	Value numRows = 0;

	ExecuteQuery(&filter,
		rgvals,
		[&](Value *rgvals)
		{
			// Nothing to validate here.
			//
		});

	EXPECT_EQ(0, numRows);
}
