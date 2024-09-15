/*
	This file contains BTree tests.
*/

#include "pch.h"
#include <thread>

// Helper reader function
//
static void RunReader(SE::BTree* btree, int totalRows)
{
	// Create a session to iterate over the BTree created above.
	//
	SE::BTreeSession btreeSession(btree);

	// Init the scan operator.
	//
	Qp::BTreeScanner btreeScanner(&btreeSession);

	Qp::Project project(&btreeScanner, [](Value* rgval) {});
	Value numRows = 0;
	Value rgvals[3] = {};

	btreeScanner.Open();
	while (btreeScanner.GetRow(rgvals))
	{
		EXPECT_EQ(numRows, rgvals[0]);
		EXPECT_EQ(numRows * 10, rgvals[1]);
		EXPECT_EQ(numRows * 100, rgvals[2]);
		++numRows;

		// sleep for sometime
		//
		if (numRows % 10 == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	btreeScanner.Close();

	EXPECT_EQ(totalRows, numRows);
}

// Helper writer function
//
static void RunWriter(SE::BTree* btree, int totalRows, int id, int numThreads)
{
	Value rgvals[3] = {};
	int count = 0;
	for (Value i = id; i < totalRows; i += numThreads)
	{
		rgvals[0] = i;
		rgvals[1] = i * 10;
		rgvals[2] = i * 100;

		btree->InsertRow(rgvals);
		++count;
		// sleep for sometime
		//
		if (count % 10 == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

// Helper writer function
//
static void RunRangedWriter(SE::BTree* btree, int totalRows, int id, int numThreads)
{
	Value rgvals[3] = {};
	int count = 0;
	int rowsToInsert = totalRows / numThreads;
	int rangeStart = rowsToInsert * id;
	int rangeEnd = rangeStart + rowsToInsert;
	for (Value i = rangeStart; i < rangeEnd; i++)
	{
		rgvals[0] = i;
		rgvals[1] = i * 10;
		rgvals[2] = i * 100;

		btree->InsertRow(rgvals);
		++count;
		// sleep for sometime
		//
		if (count % 10 == 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

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

// Test to cover multiple threads read
//
TEST(BTreeSeTestSuite, BtreeIndexMultipleReaders)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 2000;

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

	const int numThreads = 10;
	std::vector<std::thread> threads;
	
	for (int i = 0; i < numThreads; i++)
	{
		std::thread t(RunReader, &btree, totalRows);
		threads.push_back(move(t));
	}
	
	for (int i = 0; i < numThreads; i++)
	{
		threads[i].join();
	}
}

// Test to cover two parallel writers
//
TEST(BTreeSeTestSuite, BtreeIndexTwoWriters)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 500;

	// Create a BTree with rows.
	//
	SE::BTree btree(nCols);

	const int numThreads = 2;
	std::vector<std::thread> threads;

	for (int i = 0; i < numThreads; i++)
	{
		std::thread t(RunWriter, &btree, totalRows, i, numThreads);
		threads.push_back(move(t));
	}

	for (int i = 0; i < numThreads; i++)
	{
		threads[i].join();
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

// Test to cover writers along with readers in parallel
//
TEST(BTreeSeTestSuite, BtreeIndexMultipleWriters)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 5000;

	// Create a BTree with rows.
	//
	SE::BTree btree(nCols);

	const int numThreads = 10;
	std::vector<std::thread> threads;

	for (int i = 0; i < numThreads; i++)
	{
		std::thread t(RunWriter, &btree, totalRows, i, numThreads);
		threads.push_back(move(t));
	}

	for (int i = 0; i < numThreads; i++)
	{
		threads[i].join();
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

// Test to cover multiple writers in different ranges.
//
TEST(BTreeSeTestSuite, BtreeIndexMultipleRangedWriters)
{
	// Create a value array to hold data.
	//
	const int nCols = 3;
	Value rgvals[nCols] = {};
	unsigned int totalRows = 5000;

	// Create a BTree with rows.
	//
	SE::BTree btree(nCols);

	const int numThreads = 10;
	std::vector<std::thread> threads;

	for (int i = 0; i < numThreads; i++)
	{
		std::thread t(RunRangedWriter, &btree, totalRows, i, numThreads);
		threads.push_back(move(t));
	}

	for (int i = 0; i < numThreads; i++)
	{
		threads[i].join();
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
