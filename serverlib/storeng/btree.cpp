#include "btree.h"

#include "page.h"
#include "buffer.h"
#include <cassert>

namespace SE
{
	// Contructs basic tree structure with one page.
	//
	BTree::BTree()
	{
		m_numCols = 1;
		m_rootLevel = 0;
		Buf* buf = GetGlobalBufferPool()->GetNewPage(m_rootLevel);
		m_rootPageID = buf->GetPage()->GetPageId();
		buf->Release();
	}

	BTree::BTree(unsigned int numCols)
	{
		m_numCols = numCols;
		m_rootLevel = 0;
		Buf* buf = GetGlobalBufferPool()->GetNewPage(m_rootLevel);
		m_rootPageID = buf->GetPage()->GetPageId();
		buf->Release();
	}
	
	// Get the desired latch on the root page.
	//
	Buf* BTree::LatchRoot(LatchType type)
	{
		Buf* buf = nullptr;
		bool rootChanged = true;

		// If root changes by the time we get the latch, retry
		//
		do
		{
			m_lock.Lock();
			buf = GetGlobalBufferPool()->FindPage(m_rootPageID);
			m_lock.Unlock();
			buf->FixPage(type);
			m_lock.Lock();
			rootChanged = (m_rootPageID != buf->GetPage()->GetPageId());
			m_lock.Unlock();
			if (rootChanged)
			{
				buf->Release();
			}
		} while (rootChanged);

		return buf;
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
		std::deque<Buf*> latchedBufs;
		// Find the insert point for the given value.
		//
		Buf* buf = Position(*val, true, latchedBufs);

		Page* leafPage = buf->GetPage();

		// Split the page if it is full.
		// In worst case, it may happen that everytime we do fresh positioning,
		// the page again needs split because of other insert threads.
		//
		while (leafPage->IsFull(m_numCols))
		{
			Split(*val, latchedBufs);

			assert(latchedBufs.empty());

			// Reposition for insert
			//
			buf = Position(*val, true, latchedBufs);
			leafPage = buf->GetPage();
		}

		// At this point, there should be space on the page.
		// Insert values onto page.
		//
		leafPage->InsertRow(val, m_numCols);

		// Only the page we have inserted on should be latched.
		// Release latch on the page.
		//
		assert((latchedBufs.size() == 1) && (latchedBufs.front() == buf));
	}

	// Get the next row of the tree given a key.
	// null vals indicate end of scan.
	//
	Value* BTree::GetNextRow(Value lastKey, std::deque<Buf*> &latchedBufs, Buf** buf)
	{
		// If the buffer is null, this is the start of the scan.
		//
		Buf* buffer = *buf;

		if (buffer == nullptr)
		{
			assert(latchedBufs.empty());
			buffer = Position(lastKey, false, latchedBufs);
		}

		assert(latchedBufs.size() == 1);

		Page* page = buffer->GetPage();
		assert(page != nullptr);

		// Find the row with lastKey
		//
		unsigned int slotCount = page->GetSlotCount();
		unsigned int slot = 0;
		Value* row = page->GetRow(slot, m_numCols);
		while (slot < slotCount)
		{
			if (lastKey < row[0])
			{
				break;
			}

			slot++;
			row = page->GetRow(slot, m_numCols);
		}

		// If the next row resides on the next page, then move to the next page
		// Unlatch the current page.
		//
		if (slot == slotCount)
		{
			slot = 0;
			PageId pageId = page->GetNextPageId();
			buffer = nullptr;
			page = nullptr;

			if (pageId != 0)
			{
				buffer = GetGlobalBufferPool()->FindPage(pageId);
				buffer->FixPage(SH_LATCH);
				latchedBufs.push_back(buffer);
				page = buffer->GetPage();
			}
			
			// Unlatch the previous page
			//
			Buf* previousBuf = latchedBufs.front();
			latchedBufs.pop_front();
			previousBuf->Release();
		}

		*buf = buffer;

		// If there are no more pages, then return null to signal end of scan.
		//
		if (page == nullptr)
		{
			assert(latchedBufs.empty());
			return nullptr;
		}

		assert(slot < page->GetSlotCount());

		row = page->GetRow(slot, m_numCols);
		return row;
	}

	// Split the tree based on the value provided.
	//
	//
	// TODO: Add appropriate latches at different places
	//
	//
	void BTree::Split(Value val, std::deque<Buf*> &latchedBufs)
	{
		// We have the queue of buffers EX latched which needs to be split.
		// The first page in the queue doesn't need split, split from next page onwards.
		//
		Buf* parentBuf = nullptr;

		while (!latchedBufs.empty())
		{
			Buf* buf = latchedBufs.front();
			latchedBufs.pop_front();
			Page* page = buf->GetPage();
			Page* parentPage = (parentBuf != nullptr) ? parentBuf->GetPage() : nullptr;

			if (!page->IsFull(m_numCols))
			{
				// This can happen only once
				//
				assert(parentBuf == nullptr);
				parentBuf = buf;
				parentPage = page;
				continue;
			}

			assert(page->IsFull(m_numCols));

			if (!page->IsLeafLevel()) // split internal page
			{
				Buf* newBuf = GetGlobalBufferPool()->GetNewPage(page->GetLevel());
				Page* newPage = newBuf->GetPage();

				// Transfer data rows
				//
				Value leftVal = page->GetIndexRow(0)->beginKey;
				Value rightVal = TransferIndexRows(page, newPage);

				// If root also needs to split, add a new root page.
				//
				if (parentPage == nullptr)
				{
					assert((page->GetPageId() == m_rootPageID) && (page->GetLevel() == m_rootLevel));

					int newRootLevel = page->GetLevel() + 1;
					Buf* newRootBuf = GetGlobalBufferPool()->GetNewPage(newRootLevel);
					Page* newRootPage = newRootBuf->GetPage();
					newRootPage->InsertIndexRow(leftVal, page->GetPageId());
					newRootPage->InsertIndexRow(rightVal, newPage->GetPageId());
					UpdateRoot(newRootPage->GetPageId(), newRootLevel);
					parentBuf = newRootBuf;
				}
				else
				{
					// Intermediate nodes' parent need to have a new IndexRow
					// IndexRow for left page should already be there, just add right page's entry
					//
					parentPage->InsertIndexRow(rightVal, newPage->GetPageId());
				}

				// Move to next level
				//
				if (val < rightVal)
				{
					parentBuf = buf;
				}
				else
				{
					parentBuf = newBuf;
				}
			}
			else // split leaf page
			{
				assert(page->IsLeafLevel() && page->IsFull(m_numCols));

				Buf* newBuf = GetGlobalBufferPool()->GetNewPage(0);
				Page* newPage = newBuf->GetPage();
				newPage->SetNextPageId(page->GetNextPageId());

				Buf* nextBuf = (page->GetNextPageId() == 0) ? nullptr : GetGlobalBufferPool()->FindPage(page->GetNextPageId());

				if (nullptr != nextBuf)
				{
					Page* nextPage = nextBuf->GetPage();
					nextPage->SetPrevPageId(newPage->GetPageId());
				}
				newPage->SetPrevPageId(page->GetPageId());
				page->SetNextPageId(newPage->GetPageId());

				// Transfer data rows
				//
				Value leftVal = *(page->GetRow(0, m_numCols));
				Value rightVal = TransferRows(page, newPage);

				// If there's only one buffer in the queue, then that is the only page,
				// root as well as leaf, We need to create a new root in that case.
				//
				if (parentPage == nullptr)
				{
					assert((page->GetPageId() == m_rootPageID) && (page->GetLevel() == m_rootLevel));

					int newRootLevel = page->GetLevel() + 1;
					Buf* newRootBuf = GetGlobalBufferPool()->GetNewPage(newRootLevel);
					Page* newRootPage = newRootBuf->GetPage();
					newRootPage->InsertIndexRow(leftVal, page->GetPageId());
					newRootPage->InsertIndexRow(rightVal, newPage->GetPageId());
					UpdateRoot(newRootPage->GetPageId(), newRootLevel);
					parentBuf = newRootBuf;
				}
				else
				{
					// parent page need to have a new IndexRow
					// IndexRow for left page should already be there
					// just add right page's entry
					//
					parentPage->InsertIndexRow(rightVal, newPage->GetPageId());
				}

				// Only parentBuf, newBuf & buf must be latched
				// Rest all the buffers must have been released by this time.
				//
				assert(latchedBufs.empty());
			}
		}
	}

	// Given a index page, find the child page for the given value.
	//
	Buf* BTree::FindChildPage(Page* page, Value val)
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
		Buf* childPageBuf = GetGlobalBufferPool()->FindPage(childPageId);
		assert(childPageBuf != nullptr);
		return childPageBuf;
	}

	// Transfer rows from one page to another by choosing a split point.
	//
	Value BTree::TransferRows(Page* leftPage, Page* rightPage) const
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

	// Find the page into which a scan needs to go.
	// This will return a page whose buf container is SH latched.
	//
	//
	// TODO: Add appropriate latches at different places
	//
	//
	Buf* BTree::Position(Value val, bool forInsert, std::deque<Buf*> &latchedBufs)
	{
		// We must not hold any latch when starting the crab walk.
		//
		assert(latchedBufs.empty());

		// Start traversing from root page, take SH latch on it and
		// then move on to the next page until reached to leaf page.
		//
		BufferPool* bufPool = GetGlobalBufferPool();
		Buf* buf = LatchRoot(SH_LATCH);
		buf->Release();

		Page* page = buf->GetPage();

		assert((page != nullptr) && (page->GetLevel() == m_rootLevel));

		// If root page is a leaf page
		// Use revert to unoptimized crab walk.
		//
		if (forInsert && page->IsLeafLevel())
		{
			return PositionForInsert(val, latchedBufs);
		}

		while (!page->IsLeafLevel())
		{
			buf = FindChildPage(page, val);
			page = buf->GetPage();
			assert(page != nullptr);
		}

		// If positioning for read or inserting on non-full page, return the buffer
		//
		if (!forInsert || !page->IsFull(m_numCols))
		{
			return buf;
		}

		assert(latchedBufs.empty());

		return PositionForInsert(val, latchedBufs);
	}

	// Find the page into which a insert needs to go.
	// This will return the page whose buf is EX latched.
	// The ancestors of the page could also be latched if there are chances of split.
	//
	//
	// TODO: Add appropriate latches at different places
	//
	//
	Buf* BTree::PositionForInsert(Value val, std::deque<Buf*> &latchedBufs)
	{
		// We must not hold any latch when starting the crab walk.
		//
		assert(latchedBufs.empty());
		// Start traversing from root page, take EX latch on it and
		// then move on to the next page until reached to leaf page.
		//
		Buf* buf = LatchRoot(EX_LATCH);
		Page* page = buf->GetPage();
		buf->Release();

		assert((page != nullptr) && (page->GetLevel() == m_rootLevel));

		while (!page->IsLeafLevel())
		{
			buf = FindChildPage(page, val);
			page = buf->GetPage();
			assert(page != nullptr);

			// If split cannot occur, unlatch all ancestors
			//
			if (!page->IsFull(m_numCols))
			{
			}
		}

		return buf;
	}
}