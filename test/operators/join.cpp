/*
	This file contains QP tests for the Join operator.
*/

#include "pch.h"

// Tests the join operator.
//
TEST(QpTestSuiteJoin, JoinSimple)
{
	Value rgvals[2];

	// 10 rows on every side, from 1 to 10.
	//
	Qp::ConstScan left(1, 10, 1);
	Qp::ConstScan right(1, 10, 1);
	
	Qp::Join join(&left, &right, 1, 1, [](Value* l, Value* r) { return l[0] == r[0]; });

	Value numRows = 0;

	ExecuteQuery(&join,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(numRows, rgvals[0]);
			EXPECT_EQ(numRows, rgvals[1]);
		});

	
	EXPECT_EQ(10, numRows);
}
// Tests the join operator with additional columns.
// These additional columns should not change when passing through the join.
//
TEST(QpTestSuiteJoin, JoinAddtlColumns)
{
	Value rgvals[4];
	rgvals[1] = 111;
	rgvals[3] = 333;

	Qp::ConstScan left(1, 10, 1);
	Qp::ConstScan right(1, 10, 1);
	Qp::Join join(&left, &right, 2, 2, [](Value* l, Value* r) { return l[0] == r[0]; });

	Value numRows = 0;


	ExecuteQuery(&join,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(numRows, rgvals[0]);
			EXPECT_EQ(numRows, rgvals[2]);
			EXPECT_EQ(111, rgvals[1]);
			EXPECT_EQ(333, rgvals[3]);
		});

	EXPECT_EQ(10, numRows);
}

// Tests the join operator with a not-equal predicate.
//
TEST(QpTestSuiteJoin, JoinNePredicate)
{
	Value rgvals[2];

	Qp::ConstScan left(1, 10, 1);
	Qp::ConstScan right(1, 10, 1);
	Qp::Join join(&left, &right, 1, 1, [](Value* l, Value* r) { return l[0] != r[0]; });

	Value numRows = 0;

	ExecuteQuery(&join,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_NE(rgvals[0], rgvals[1]);
		});

	EXPECT_EQ(90, numRows);
}

// Tests a cross product.
//
TEST(QpTestSuiteJoin, JoinCrossProduct)
{
	Value rgvals[2];

	Qp::ConstScan left(1, 10, 1);
	Qp::ConstScan right(1, 10, 1);
	Qp::Join join(&left, &right, 1, 1, [](Value* l, Value* r) { return true; });

	Value numRows = 0;

	ExecuteQuery(&join,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
		});


	EXPECT_EQ(100, numRows);
}

// Tests that the join operator works on multiple identical rows.
//
TEST(QpTestSuiteJoin, JoinOnIdenticalValues)
{
	Value rgvals[2];

	Qp::ConstScan left(1, 10, 1, /*repeat: */ 2);
	Qp::ConstScan right(1, 10, 1, /*repeat: */ 2);
	Qp::Join join(&left, &right, 1, 1, [](Value* l, Value* r) { return l[0] == r[0]; });

	Value numRows = 0;

	ExecuteQuery(&join,
		rgvals,
		[&](Value *rgvals)
		{
			++numRows;
			EXPECT_EQ(rgvals[0], rgvals[1]);
		});

	EXPECT_EQ(40, numRows);
}
