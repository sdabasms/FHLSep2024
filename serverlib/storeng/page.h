#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include "common/value.h"
//#include "btreesession.h"

#pragma once

namespace SE
{
	typedef unsigned int PageId;
	class Page;
	const PageId NULL_PAGE_ID = 0;

	// Record structure for index page.
	//
	struct IndexPagePayload
	{
		Value beginKey;
		PageId pageID;
	};

	// Page header stores metadata about the page.
	//
	class PageHeader
	{
	public:
		PageHeader(
			PageId pageId,
			unsigned int level)
			:
			m_pageId(pageId),
			m_level(level),
			m_slotCount(0),
			m_prevPageId(0),
			m_nextPageId(0)
		{}

		unsigned int GetSlotCount()
		{
			return m_slotCount;
		}

		void SetSlotCount(unsigned int slotCount)
		{
			m_slotCount = slotCount;
		}

		void SetPrevPageId(PageId prevPageId)
		{
			m_prevPageId = prevPageId;
		}

		void SetNextPageId(PageId nextPageId)
		{
			m_nextPageId = nextPageId;
		}

		PageId GetNextPageId()
		{
			return m_nextPageId;
		}

	protected:
		PageId m_pageId;

		// 0 is leaf.
		//
		unsigned int m_level;

		unsigned int m_slotCount;

		PageId m_prevPageId;

		PageId m_nextPageId;
	};

	// This size can be modified. Use smaller sizes to trigger splits easily.
	//
	const int PAGE_SIZE = 500;
	const int PAGE_DATA_SIZE = PAGE_SIZE - sizeof(PageHeader);

	// Structure of a page includes the header and the data section.
	//
	class Page : public PageHeader
	{
	public:
		Page(
			PageId pageId,
			unsigned int level)
			:
			PageHeader(pageId, level)
		{
			// Zero out the page.
			//
			memset(m_data, 0, PAGE_DATA_SIZE);
		}

		PageId GetPageId()
		{
			return m_pageId;
		}

		bool IsLeafLevel()
		{
			return m_level == 0;
		}

		unsigned int GetLevel()
		{
			return m_level;
		}

		// Page interfaces to read/write rows.
		//
		Value* GetRow(unsigned int slot, unsigned int numCols);
		IndexPagePayload* GetIndexRow(unsigned int slot);
		void InsertRow(Value* val, unsigned int numCols);
		void InsertIndexRow(Value beginVal, PageId pageId);
		bool IsFull(unsigned int numCols);

	private:
		unsigned char m_data[PAGE_DATA_SIZE];
	};
}