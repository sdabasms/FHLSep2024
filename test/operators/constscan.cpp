/*
	This file contains test for the ConstScan.
	They should pass with the minimal implementation already.
*/

#include "pch.h"

// Tests that the const scan generates numbers 0..10
//
TEST(QpTestSuiteConstScan, ConstScan0To10)
{
	Value rgvals[1];
	Qp::ConstScan constScan(0, 10, 1);

	Value numRows = 0;

	ExecuteQuery(&constScan,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(numRows - 1, rgvals[0]);
		});

	EXPECT_EQ(11, numRows);
}

// Tests that the const scan generates even numbers between -20..20
//
TEST(QpTestSuiteConstScan, ConstScanMinus20To20)
{
	Value rgvals[1];
	Qp::ConstScan constScan(-20, 20, 2);

	Value numRows = 0;

	ExecuteQuery(&constScan,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(-22 + numRows * 2, rgvals[0]);
		});

	EXPECT_EQ(21, numRows);
}

// Tests that the const scan generates odd numbers between -19..19
//
TEST(QpTestSuiteConstScan, ConstScanMinus19To19Odd)
{
	Value rgvals[1];
	Qp::ConstScan constScan(-19, 20 /* 20 is not expected to be returned */, 2);

	Value numRows = 0;

	ExecuteQuery(&constScan,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(-21 + numRows * 2, rgvals[0]);
		});
	
	EXPECT_EQ(20, numRows);
}

// Tests that the const scan properly repeats values
//
TEST(QpTestSuiteConstScan, ConstScanRepeat5Times)
{
	Value rgvals[1];
	Qp::ConstScan constScan(0, 9, 1, /* repeat: */ 5);

	Value numRows = 0;

	ExecuteQuery(&constScan,
		rgvals,
		[&](Value *rgvals)
		{
			EXPECT_EQ(numRows / 5, rgvals[0]);
			++numRows;
		});

	EXPECT_EQ(50, numRows);
}