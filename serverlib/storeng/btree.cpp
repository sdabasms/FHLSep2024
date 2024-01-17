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
		Page* page = Position(*val, true);
		assert(page->IsLeafLevel());

		// Split the page if it is full.
		//
		if (page->IsFull(m_numCols))
		{
			Split(*val);

			// Find new insert point after split.
			//
			page = Position(*val, true);
			assert(page->IsLeafLevel());
			
		}

		// At this point, there should be space on the page.
		//
		assert(!page->IsFull(m_numCols));

		page->InsertRow(val, m_numCols);
	}

	// Split the tree based on the value provided.
	//
	void BTree::Split(Value val)
	{
retrySplit:
		// Traverse the tree from the root and split along the way.
		//
		Page* page = GetGlobalBufferPool()->FindPage(m_rootPageID);
		Page* parentPage = nullptr;

traverse:
		// Traverse the tree till we reach leaf level.
		//
		if (!page->IsLeafLevel())
		{
			if (page->IsFull(m_numCols))
			{
				// We first need to split the internal tree and then retry splitting leaf level.
				//
				IndexPagePayload* firstVal = page->GetIndexRow(0);

				// If this is the initial root page case, increase the level of the tree.
				//
				if (IsRootLevel(page->GetLevel()))
				{
					m_rootLevel++;
					parentPage = GetGlobalBufferPool()->GetNewPage(m_rootLevel);
					m_rootPageID = parentPage->GetPageId();
					parentPage->InsertIndexRow(firstVal->beginKey, page->GetPageId());
				}
				else
				{
					assert(parentPage != nullptr);
				}

				// Create a new index page.
				//
				Page* newIndexPage = GetGlobalBufferPool()->GetNewPage(page->GetLevel());
				PageId newIndexPageId = newIndexPage->GetPageId();

				// Transfer some rows from old page to the newly created page.
				//
				Value splitVal = TransferIndexRows(page, newIndexPage);

				// Insert the newly created page into the tree structure.
				// This involves fixing the linkages.
				//
				parentPage->InsertIndexRow(splitVal + 1, newIndexPage->GetPageId());
				newIndexPage->SetPrevPageId(page->GetPageId());
				page->SetNextPageId(newIndexPage->GetPageId());

				goto retrySplit;
			}
			else
			{
				// Find the child page.
				//
				parentPage = page;
				
				page = FindChildPage(page, val);

				// Continue traversing the tree.
				//
				goto traverse;
			}
		}
		else
		{
			assert(page->IsFull(m_numCols));

			Value firstVal = *(page->GetRow(0, m_numCols));

			// If this is the initial root page case, increase the level of the tree.
			//
			if (IsRootLevel(page->GetLevel()))
			{
				m_rootLevel++;
				parentPage = GetGlobalBufferPool()->GetNewPage(m_rootLevel);
				m_rootPageID = parentPage->GetPageId();
				parentPage->InsertIndexRow(firstVal, page->GetPageId());
			}
			else
			{
				assert(parentPage != nullptr);
			}

			// Create a new leaf page.
			//
			Page* newLeafPage = GetGlobalBufferPool()->GetNewPage(0);
			PageId newLeafPageId = newLeafPage->GetPageId();

			// Transfer some rows from old page to the newly created page.
			//
			Value splitVal = TransferRows(page, newLeafPage);

			// Insert the newly created page into the tree structure.
			// This involves fixing the linkages.
			//
			parentPage->InsertIndexRow(splitVal+1, newLeafPage->GetPageId());
			newLeafPage->SetPrevPageId(page->GetPageId());
			page->SetNextPageId(newLeafPage->GetPageId());
		}
	}

	// Given a index page, find the child page for the given value.
	//
	Page* BTree::FindChildPage(Page* page, Value val)
	{
		PageId childPageId = page->GetIndexRow(0)->pageID;

		for (unsigned int slot = 0; slot < page->GetSlotCount(); slot++)
		{
			IndexPagePayload* payload = page->GetIndexRow(slot);
			if (val > payload->beginKey)
			{
				childPageId = payload->pageID;
			}
			else
			{
				break;
			}
		}

		return GetGlobalBufferPool()->FindPage(childPageId);
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferRows(Page* leftPage, Page* rightPage)
	{
		// This implementation chooses mid-point for split. Other points can be chosen.
		//
		unsigned int splitPoint = leftPage->GetSlotCount()/2;
		Value splitVal = *(leftPage->GetRow(splitPoint, m_numCols));

		// Transfer rows from left page to right from the split point.
		//
		for (unsigned int slot = splitPoint + 1; slot < leftPage->GetSlotCount(); slot++)
		{
			Value* slotVal = leftPage->GetRow(slot, m_numCols);
			rightPage->InsertRow(slotVal, m_numCols);
		}

		// Set the number of rows on the old page.
		//
		leftPage->SetSlotCount(splitPoint+1);

		return splitVal;
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferIndexRows(Page* leftPage, Page* rightPage)
	{
		// This implementation chooses mid-point for split. Other points can be chosen.
		//
		unsigned int splitPoint = leftPage->GetSlotCount() / 2;
		Value splitVal = leftPage->GetIndexRow(splitPoint)->beginKey;

		// Transfer rows from left page to right from the split point.
		//
		for (unsigned int slot = splitPoint + 1; slot < leftPage->GetSlotCount(); slot++)
		{
			IndexPagePayload* slotVal = leftPage->GetIndexRow(slot);
			rightPage->InsertIndexRow(slotVal->beginKey, slotVal->pageID);
		}

		// Set the number of rows on the old page.
		//
		leftPage->SetSlotCount(splitPoint + 1);

		return splitVal;
	}

	// Get the first key of the BTree.
	//
	Page* BTree::GetFirstLeafPage()
	{
		return Position(0, false);
	}

	// Traverse the tree to find the leaf page either for read or write.
	//
	Page* BTree::Position(Value val, bool forInsert)
	{
		Page* page = GetGlobalBufferPool()->FindPage(m_rootPageID);

traverse:
		if (!page->IsLeafLevel())
		{
			// Traverse the tree.
			//
			page = FindChildPage(page, val);
			goto traverse;
		}

		return page;
	}

	// Get the next key of the tree given a key.
	// null vals indicate end of scan.
	//
	Value* BTree::GetRow(PageId* pageId, unsigned int* slot)
	{
		Page* page = GetGlobalBufferPool()->FindPage(*pageId);
		assert(page->IsLeafLevel());

		Value* slotVal = nullptr;

nextPage:
		// Provide the next row on the page if we're not at the end of it.
		//
		if (*slot < page->GetSlotCount())
		{
			slotVal = page->GetRow(*slot, m_numCols);
			(*slot)++;
		}
		// Move to the next leaf page.
		//
		else if (page->GetNextPageId() != 0)
		{
			page = GetGlobalBufferPool()->FindPage(page->GetNextPageId());
			*pageId = page->GetPageId();
			*slot = 0;

			goto nextPage;
		}
		
		return slotVal;
	}
}