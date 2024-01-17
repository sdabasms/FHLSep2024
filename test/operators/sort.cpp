/*
	This file contains QP tests for the Sort operator.
*/

#include "pch.h"

// Tests the sort operator.
//
TEST(QpTestSuiteSort, SortDescending)
{
	Qp::ConstScan rowGen(1, 100, 1, /*repeat: */ 3);
	Qp::Sort sort(&rowGen, 1,
		[](Value* left, Value* right)
		{
			// sort descending
			//
			return left[0] > right[0];
		});

	Value rgvals[1];

	Value numRows = 0;
	Value lastRow = 999;

	ExecuteQuery(&sort,
		rgvals,
		[&](Value *rgvals)
		{
			// next row should be <= last row
			//
			EXPECT_LE(rgvals[0], lastRow);
			lastRow = rgvals[0];
			++numRows;
		});
	
	EXPECT_EQ(300, numRows);
}

// Tests the sort operator.
//
TEST(QpTestSuiteSort, SortAscending)
{
	Qp::ConstScan rowGen(1, 100, 1, /*repeat: */ 3);
	Qp::Sort sort(&rowGen, 1,
		[](Value* in, Value* out)
		{
			// sort ascending
			//
			return in[0] < out[0];
		});

	Value rgvals[1];

	Value numRows = 0;
	Value lastRow = 0;

	ExecuteQuery(&sort,
		rgvals,
		[&](Value *rgvals)
		{
			// next row should be >= last row
			//
			EXPECT_GE(rgvals[0], lastRow);
			lastRow = rgvals[0];
			++numRows;
		});

	EXPECT_EQ(300, numRows);
}