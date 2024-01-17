/*
	This file contains QP tests for the BTreeScanner operator.
*/

#include "pch.h"

// Tests that the scanner reads all the rows from the tree.
//
TEST(QpTestSuiteBTreeScanner, ReadAllRows)
{
	// First, create a BTree to scan.
	//
	const int nCols = 1;
	Value rgvals[nCols] = {};
	const unsigned int totalRows = 2000;
	SE::BTree btree;

	for (Value i = 0; i < totalRows; i++)
	{
		btree.InsertRow(i);
	}

	// Now create a scanner that reads the rows.
	//
	SE::BTreeSession session(&btree);
	Qp::BTreeScanner scanner(&session);

	Value numRows = 0;

	ExecuteQuery(&scanner,
		rgvals,
		[&](Value *rgvals)
		{
			EXPECT_EQ(numRows, rgvals[0]);
			++numRows;
		});

	EXPECT_EQ(totalRows, numRows);

	// If we re-open the scan, it should return the same result again.
	//
	numRows = 0;
	ExecuteQuery(&scanner,
		rgvals,
		[&](Value *rgvals)
		{
			EXPECT_EQ(numRows, rgvals[0]);
			++numRows;
		});

	EXPECT_EQ(totalRows, numRows);
}
