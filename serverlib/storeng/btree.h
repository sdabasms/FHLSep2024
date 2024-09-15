#include <deque>
#include "common/value.h"
#include "buffer.h"

#pragma once

namespace SE
{
	enum Operation
	{
		READ_OPERATION,
		WRITE_OPERATION,
		INVALID_OPERATION
	};

	// This class implements a BTree structure. It is currently in-memory.
	// Each row contains one 'Value' which is also the key.
	//
	class BTree
	{
	public:
		BTree();
		BTree(unsigned int numCols);

		// Insert rows into the tree.
		//
		void InsertRow(Value val);
		void InsertRow(Value* val);

		unsigned int GetNumCols()
		{
			return m_numCols;
		}

		Value* GetNextRow(Value lastKey, std::deque<Buf*> &latchedBufs, Buf** buf);

	private:

		// Check if a page's level is the root level of this tree.
		//
		bool IsRootLevel(unsigned int level)
		{
			return (level == m_rootLevel);
		}

		// Get the desired latch on the root page.
		//
		Buf* LatchRoot(LatchType type);

		// Update root page id and level
		//
		void UpdateRoot(PageId newRootPageId, unsigned int level)
		{
			m_lock.Lock();
			m_rootPageID = newRootPageId;
			m_rootLevel = level;
			m_lock.Unlock();
		}

		// Split the page that would contain the Value val.
		//
		void Split(Value val, std::deque<Buf*> &latchedBufs);

		// Transfer the rows from one page to another during split.
		//
		Value TransferRows(Page* leftPage, Page* rightPage) const;
		Value TransferIndexRows(Page* leftPage, Page* rightPage);

		// Find the child page within an internal page during traversal.
		//
		Buf* FindChildPage(Page* page, Value val);

		// Find the page into which a scan needs to go.
		//
		Buf* Position(Value val, bool forInsert, std::deque<Buf*> &latchedBufs);

		// Find the page into which a insert needs to go.
		//
		Buf* PositionForInsert(Value val, std::deque<Buf*> &latchedBufs);

		// Metadata about the BTree which help traverse the tree.
		//
		PageId m_rootPageID;
		unsigned int m_rootLevel;

		// Number of 'Value' columns present in each row.
		//
		unsigned int m_numCols;

		// spinLock to control access to root page
		//
		SpinLock m_lock;
	};
}