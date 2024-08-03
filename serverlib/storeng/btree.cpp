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
		Page* leafPage = Position(*val);

		// Split the page if it is full.
		//
		if (leafPage->IsFull(m_numCols))
		{
			Split(*val);
			leafPage = Position(*val);
		}

		// At this point, there should be space on the page.
		//

		// Insert values onto page.
		//
		leafPage->InsertRow(val, m_numCols);
	}

	// Split the tree based on the value provided.
	//
	void BTree::Split(Value val)
	{
		// Traverse the tree from the root and split along the way.
		//
		BufferPool *bufPool = GetGlobalBufferPool();
		Page* page = bufPool->FindPage(m_rootPageID);
		Page* parentPage = nullptr;

		assert(page->GetLevel() == m_rootLevel);

		// Traverse the tree till we reach leaf level.
		// If intermediate page is full, split that first and retry traversal.
		//
		while (!page->IsLeafLevel())
		{
			if (page->IsFull(m_numCols))
			{
				Page *newPage = bufPool->GetNewPage(page->GetLevel());
				Value leftVal = page->GetIndexRow(0)->beginKey;
				Value rightVal = TransferIndexRows(page, newPage);
				if (parentPage == nullptr)
				{
					// Need to update the root
					//
					int newRootLevel = page->GetLevel() + 1;
					Page* newRootPage = bufPool->GetNewPage(newRootLevel);
					newRootPage->InsertIndexRow(leftVal, page->GetPageId());
					newRootPage->InsertIndexRow(rightVal, newPage->GetPageId());
					m_rootPageID = newRootPage->GetPageId();
					m_rootLevel = newRootLevel;
				}
				else
				{
					// Intermediate nodes' parent need to have a new IndexRow
					// IndexRow for left page should already be there, just add right page's entry
					//
					parentPage->InsertIndexRow(rightVal, newPage->GetPageId());
				}

				// Restart traversal from root again
				//
				page = bufPool->FindPage(m_rootPageID);
				parentPage = nullptr;

				assert(page->GetLevel() == m_rootLevel);
			}

			// Current intermediate page is not full, continue
			// Note: It is okay if we just resetted to root as part of split on the older root node
			// as the new root node will have only 2 entries in that case and thus won't be full.
			//
			parentPage = page;
			page = FindChildPage(page, val);
			assert(page != nullptr);
		}

		// page points to a leaf page.
		//
		if (page->IsFull(m_numCols))
		{
			Page *newPage = bufPool->GetNewPage(0);

			// Update leaf page links
			//
			Page* nextPage = bufPool->FindPage(page->GetNextPageId());
			newPage->SetNextPageId(page->GetNextPageId());
			if (nullptr != nextPage)
			{
				nextPage->SetPrevPageId(newPage->GetPageId());
			}
			newPage->SetPrevPageId(page->GetPageId());
			page->SetNextPageId(newPage->GetPageId());

			// Transfer rows
			Value leftVal = *(page->GetRow(0, m_numCols));
			Value rightVal = TransferRows(page, newPage);

			if (parentPage == nullptr)
			{
				// Need to update the root
				//
				int newRootLevel = page->GetLevel() + 1;
				Page* newRootPage = bufPool->GetNewPage(newRootLevel);
				newRootPage->InsertIndexRow(leftVal, page->GetPageId());
				newRootPage->InsertIndexRow(rightVal, newPage->GetPageId());
				m_rootPageID = newRootPage->GetPageId();
				m_rootLevel = newRootLevel;
			}
			else
			{
				// parent page need to have a new IndexRow
				// IndexRow for left page should already be there
				// just add right page's entry
				//
				parentPage->InsertIndexRow(rightVal, newPage->GetPageId());
			}
		}

		// Split has been completed for both intermediate pages as well as leaf page
		//
		return;
	}

	// Given a index page, find the child page for the given value.
	//
	Page* BTree::FindChildPage(Page* page, Value val)
	{
		assert(page->GetLevel() > 0);
		int slotCount = page->GetSlotCount();

		// Assuming atleast one Index entry in the index page
		//
		PageId childPageId = page->GetIndexRow(0)->pageID;
		for (int slot = 1; slot < slotCount; slot++)
		{
			IndexPagePayload *indexEntry = page->GetIndexRow(slot);

			// If this is the next entry's key is larger than the query key, then the last page is the correct page.
			//
			if (indexEntry->beginKey > val)
			{
				break;
			}
			childPageId = indexEntry->pageID;
		}
		Page* childPage = GetGlobalBufferPool()->FindPage(childPageId);
		assert(childPage != nullptr);
		return childPage;
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferRows(Page* leftPage, Page* rightPage)
	{
		// Choose a split point on the page.
		//
		int slotCount = leftPage->GetSlotCount();
		int splitIndex = (slotCount + 1) / 2;

		// Transfer rows from left page to right from the split point.
		//
		Value splitVal = *(leftPage->GetRow(splitIndex, m_numCols));
		for (int slot = splitIndex; slot < slotCount; slot++)
		{
			Value* val = leftPage->GetRow(slot, m_numCols);
			rightPage->InsertRow(val, m_numCols);
		}

		// Set the number of rows on the old page.
		//
		leftPage->SetSlotCount(splitIndex);

		return splitVal;
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferIndexRows(Page* leftPage, Page* rightPage)
	{
		// Choose a split point on the page.
		//
		int slotCount = leftPage->GetSlotCount();
		int splitIndex = (slotCount + 1) / 2;

		// Transfer rows from left page to right from the split point.
		//
		Value splitVal = leftPage->GetIndexRow(splitIndex)->beginKey;
		for (int slot = splitIndex; slot < slotCount; slot++)
		{
			IndexPagePayload* val = leftPage->GetIndexRow(slot);
			rightPage->InsertIndexRow(val->beginKey, val->pageID);
		}

		// Set the number of rows on the old page.
		//
		leftPage->SetSlotCount(splitIndex);

		return splitVal;
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
		BufferPool* bufPool = GetGlobalBufferPool();
		Page* page = bufPool->FindPage(m_rootPageID);

		assert(page->GetLevel() == m_rootLevel);

		while (!page->IsLeafLevel())
		{
			page = FindChildPage(page, val);
			assert(page != nullptr);
		}

		return page;
	}

	// Get the next key of the tree given a key.
	// null vals indicate end of scan.
	//
	Value* BTree::GetRow(PageId* pageId, unsigned int* slot)
	{
		// Use the page provided to find the row.
		// If we've reached end of page, move to the next page.
		//
		BufferPool *bufPool = GetGlobalBufferPool();
		Page* page = bufPool->FindPage(*pageId);

		assert(page != nullptr);

		// If the next row resides on the next page, then update the pageId and slot
		//
		if (page->GetSlotCount() <= *slot)
		{
			*pageId = page->GetNextPageId();
			*slot -= page->GetSlotCount();

			// If this is the last page, return nullptr signalling end of records
			//
			if (*pageId == 0)
			{
				return nullptr;
			}

			page = bufPool->FindPage(*pageId);
			assert(page != nullptr);
		}

		assert(*slot < page->GetSlotCount());

		Value *val = page->GetRow(*slot, m_numCols);
		*slot = (*slot + 1);
		return val;
	}
}