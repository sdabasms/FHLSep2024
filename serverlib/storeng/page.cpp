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

		if (m_slotCount > 0)
		{
			Value* prevVal = (Value*)((m_data)+((m_slotCount-1) * sizeof(Value) * numCols));
			assert(val[0] > prevVal[0]);
		}

		Value* newVal = (Value*)((m_data)+(m_slotCount * sizeof(Value) * numCols));
		
		for (unsigned int i = 0; i < numCols; i++)
		{
			newVal[i] = val[i];
		}

		m_slotCount++;
	}

	void Page::InsertIndexRow(Value beginVal, PageId pageId)
	{
		assert(!IsLeafLevel());

		IndexPagePayload* newRecord = (IndexPagePayload *)(m_data + (m_slotCount * sizeof(IndexPagePayload)));

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