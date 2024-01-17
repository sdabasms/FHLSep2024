#include "btree.h"

#include "page.h"
#include "buffer.h"

namespace SE
{
	// Contructs basic tree structure with one page.
	//
	BTree::BTree()
	{
		m_numCols = 1;
		m_rootLevel = 0;
		m_rootPageID = GetGlobalBufferPool()->GetNewPage(m_rootLevel)->GetPageId();
	}

	BTree::BTree(unsigned int numCols)
	{
		m_numCols = numCols;
		m_rootLevel = 0;
		m_rootPageID = GetGlobalBufferPool()->GetNewPage(m_rootLevel)->GetPageId();
	}

	// Allow insert by value for single column trees.
	//
	void BTree::InsertRow(Value val)
	{
		assert(m_numCols == 1);
		InsertRow(&val);
	}

	// Insert single row in ascending order.
	//
	void BTree::InsertRow(Value* val)
	{
		// Find the insert point for the given value.
		//
		
		// Split the page if it is full.
		//

		// At this point, there should be space on the page.
		//
		
		// Insert values onto page.
		//
	}

	// Split the tree based on the value provided.
	//
	void BTree::Split(Value val)
	{
		// Traverse the tree from the root and split along the way.
		//

		// Traverse the tree till we reach leaf level.
		// If intermediate page is full, split that first and retry traversal.
		//
	}

	// Given a index page, find the child page for the given value.
	//
	Page* BTree::FindChildPage(Page* page, Value val)
	{
		return nullptr;
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferRows(Page* leftPage, Page* rightPage)
	{
		// Choose a split point on the page.
		//

		// Transfer rows from left page to right from the split point.
		//

		// Set the number of rows on the old page.
		//

		return 0;
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferIndexRows(Page* leftPage, Page* rightPage)
	{
		// Choose a split point on the page.
		//

		// Transfer rows from left page to right from the split point.
		//

		// Set the number of rows on the old page.
		//

		return 0;
	}

	// Get the first key of the BTree.
	//
	Page* BTree::GetFirstLeafPage()
	{
		return Position(0);
	}

	// Traverse the tree to find the leaf page either for read or write.
	//
	Page* BTree::Position(Value val)
	{
		return nullptr;
	}

	// Get the next key of the tree given a key.
	// null vals indicate end of scan.
	//
	Value* BTree::GetRow(PageId* pageId, unsigned int* slot)
	{
		// Use the page provided to find the row.
		// If we've reached end of page, move to the next page.
		//
		return 0;
	}
}