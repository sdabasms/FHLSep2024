#include "page.h"

namespace SE
{
	// Check if page has no more space left.
	//
	bool Page::IsFull(unsigned int numCols)
	{
		if (IsLeafLevel())
		{
			return (sizeof(Value) * (m_slotCount + 1) * numCols >= PAGE_DATA_SIZE);
		}
		else
		{
			return (sizeof(IndexPagePayload) * (m_slotCount + 1) >= PAGE_DATA_SIZE);
		}
	}

	// Insert a key on the page.
	// It assumes rows are inserted in sorted order.
	//
	void Page::InsertRow(Value* val, unsigned int numCols)
	{
		assert (IsLeafLevel());

		unsigned int slot = m_slotCount;

		while (slot > 0)
		{
			Value* prevVal = (Value*)((m_data)+((slot-1) * sizeof(Value) * numCols));
			if (val[0] > prevVal[0])
			{
				break;
			}
			Value* nextVal = (Value*)((m_data)+(slot * sizeof(Value) * numCols));
				
			for (unsigned int i = 0; i < numCols; i++)
			{
				nextVal[i] = prevVal[i];
			}
			slot--;
		}

		Value* newVal = (Value*)((m_data)+(slot * sizeof(Value) * numCols));
		for (unsigned int i = 0; i < numCols; i++)
		{
			newVal[i] = val[i];
		}

		m_slotCount++;
	}

	void Page::InsertIndexRow(Value beginVal, PageId pageId)
	{
		assert(!IsLeafLevel());

		unsigned int slot = m_slotCount;
		while (slot > 0)
		{
			IndexPagePayload* prevRecord = (IndexPagePayload*)(m_data + ((slot-1) * sizeof(IndexPagePayload)));
			if (beginVal > prevRecord->beginKey)
			{
				break;
			}

			IndexPagePayload* nextRecord = (IndexPagePayload*)(m_data + (slot * sizeof(IndexPagePayload)));
			nextRecord->beginKey = prevRecord->beginKey;
			nextRecord->pageID = prevRecord->pageID;

			slot--;
		}

		IndexPagePayload* newRecord = (IndexPagePayload *)(m_data + (slot * sizeof(IndexPagePayload)));

		newRecord->beginKey = beginVal;
		newRecord->pageID = pageId;

		m_slotCount++;
	}

	// Get the row on the page given a slot.
	//
	Value* Page::GetRow(unsigned int slot, unsigned int numCols)
	{
		return (Value*)((m_data)+(slot * sizeof(Value) * numCols));
	}

	// Get index row payload.
	//
	IndexPagePayload* Page::GetIndexRow(unsigned int slot)
	{
		assert(!IsLeafLevel());

		return (IndexPagePayload*)(m_data + (slot * sizeof(IndexPagePayload)));
	}
}