#include "page.h"
#include "buffer.h"

namespace SE
{
	// Get a new page from buffer pool.
	// Init the level of btree as part of it.
	//
	Page* BufferPool::GetNewPage(
		unsigned int level)
	{
		Page* page = new Page(m_nextPageID, level);
		m_pages[m_nextPageID] = page;
		m_nextPageID++;

		return page;
	}

	// Find the page structure given the page ID.
	//
	Page* BufferPool::FindPage(
		PageId pageID)
	{
		assert(pageID < m_nextPageID);
		return m_pages[pageID];
	}

	// Singleton buffer pool.
	//
	static BufferPool bufferPool;

	BufferPool* GetGlobalBufferPool()
	{
		return &bufferPool;
	}
}