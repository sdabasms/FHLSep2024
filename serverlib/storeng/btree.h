#include "common/value.h"

#pragma once

namespace SE
{
	typedef unsigned int PageId;
	class Page;
	const PageId NULL_PAGE_ID = 0;

	// This class implements a BTree structure. It is currently in-memory.
	// Each row contains one 'Value' which is also the key.
	//
	class BTree
	{
	public:
		BTree();
		BTree(unsigned int numCols);

		// Implement accessors to get rows from the tree.
		//
		Page* GetFirstLeafPage();
		Value* GetRow(PageId* pageId, unsigned int* slot);

		// Insert rows into the tree.
		//
		void InsertRow(Value val);
		void InsertRow(Value* val);

		unsigned int GetNumCols()
		{
			return m_numCols;
		}

	private:

		// Check if a page's level is the root level of this tree.
		//
		bool IsRootLevel(unsigned int level)
		{
			return (level == m_rootLevel);
		}

		// Split the page that would contain the Value val.
		//
		void Split(Value val);

		// Transfer the rows from one page to another during split.
		//
		Value TransferRows(Page* leftPage, Page* rightPage);
		Value TransferIndexRows(Page* leftPage, Page* rightPage);

		// Find the child page within an internal page during traversal.
		//
		Page* FindChildPage(Page* page, Value val);

		// Find the page into which a scan or insert needs to go.
		//
		Page* Position(Value val);

		// Metadata about the BTree which help traverse the tree.
		//
		unsigned int m_rootPageID;
		unsigned int m_rootLevel;

		// Number of 'Value' columns present in each row.
		//
		unsigned int m_numCols;
	};
}