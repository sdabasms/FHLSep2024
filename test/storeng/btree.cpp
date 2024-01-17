/*
	This file contains BTree tests.
*/

#include "pch.h"

// Tests that the scanner that projects all rows from a tree.
//
TEST(BTreeSeTestSuite, BtreeInsertScan)
{
	// Create a value array to hold data.
	//
	const int nCols = 1;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 5;

	// Create a BTree with rows.
	//
	SE::BTree btree;

	for (Value i = 0; i < totalRows; i++)
	{
		btree.InsertRow(i);
	}

	// Create a session to iterate over the BTree created above.
	//
	SE::BTreeSession btreeSession(&btree);

	// Init the scan operator.
	//
	Qp::BTreeScanner btreeScanner(&btreeSession);

	Qp::Project project(&btreeScanner, [](Value* rgval) {});
	Value numRows = 0;

	btreeScanner.Open();
	while (btreeScanner.GetRow(rgvals))
	{
		EXPECT_EQ(numRows, rgvals[0]);
		++numRows;
	}
	btreeScanner.Close();

	EXPECT_EQ(totalRows, numRows);
}


// Tests that the scanner that projects all rows from a tree with multiple columns.
//
TEST(BTreeSeTestSuite, BtreeMultipleColumns)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 5;

	// Create a BTree with rows.
	//
	SE::BTree btree(nCols);

	for (Value i = 0; i < totalRows; i++)
	{
		rgvals[0] = i;
		rgvals[1] = i*10;
		rgvals[2] = i*100;

		btree.InsertRow(rgvals);
	}

	// Create a session to iterate over the BTree created above.
	//
	SE::BTreeSession btreeSession(&btree);

	// Init the scan operator.
	//
	Qp::BTreeScanner btreeScanner(&btreeSession);

	Qp::Project project(&btreeScanner, [](Value* rgval) {});
	Value numRows = 0;

	btreeScanner.Open();
	while (btreeScanner.GetRow(rgvals))
	{
		EXPECT_EQ(numRows, rgvals[0]);
		EXPECT_EQ(numRows*10, rgvals[1]);
		EXPECT_EQ(numRows*100, rgvals[2]);
		++numRows;
	}
	btreeScanner.Close();

	EXPECT_EQ(totalRows, numRows);
}

// Test that inserts a lot of rows that will trigger a split at the leaf level.
//
TEST(BTreeSeTestSuite, BtreeLeafSplit)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 50;

	// Create a BTree with rows.
	//
	SE::BTree btree(nCols);

	for (Value i = 0; i < totalRows; i++)
	{
		rgvals[0] = i;
		rgvals[1] = i * 10;
		rgvals[2] = i * 100;

		btree.InsertRow(rgvals);
	}

	// Create a session to iterate over the BTree created above.
	//
	SE::BTreeSession btreeSession(&btree);

	// Init the scan operator.
	//
	Qp::BTreeScanner btreeScanner(&btreeSession);

	Qp::Project project(&btreeScanner, [](Value* rgval) {});
	Value numRows = 0;

	btreeScanner.Open();
	while (btreeScanner.GetRow(rgvals))
	{
		EXPECT_EQ(numRows, rgvals[0]);
		EXPECT_EQ(numRows * 10, rgvals[1]);
		EXPECT_EQ(numRows * 100, rgvals[2]);
		++numRows;
	}
	btreeScanner.Close();

	EXPECT_EQ(totalRows, numRows);
}

// Test that will insert enough rows to trigger a split at intermediate levels.
//
TEST(BTreeSeTestSuite, BtreeIndexSplit)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 500;

	// Create a BTree with rows.
	//
	SE::BTree btree(nCols);

	for (Value i = 0; i < totalRows; i++)
	{
		rgvals[0] = i;
		rgvals[1] = i * 10;
		rgvals[2] = i * 100;

		btree.InsertRow(rgvals);
	}

	// Create a session to iterate over the BTree created above.
	//
	SE::BTreeSession btreeSession(&btree);

	// Init the scan operator.
	//
	Qp::BTreeScanner btreeScanner(&btreeSession);

	Qp::Project project(&btreeScanner, [](Value* rgval) {});
	Value numRows = 0;

	btreeScanner.Open();
	while (btreeScanner.GetRow(rgvals))
	{
		EXPECT_EQ(numRows, rgvals[0]);
		EXPECT_EQ(numRows * 10, rgvals[1]);
		EXPECT_EQ(numRows * 100, rgvals[2]);
		++numRows;
	}
	btreeScanner.Close();

	EXPECT_EQ(totalRows, numRows);
}