#include "page.h"
#include "buffer.h"

namespace SE
{
	// Get a new page from buffer pool.
	// Init the level of btree as part of it.
	//
	Buf* BufferPool::GetNewPage(
		unsigned int level)
	{
		m_lock.Lock();
		PageId pageId = m_nextPageID;
		Page* page = new Page(m_nextPageID, level);
		Buf* buf = new Buf(page);
		m_buffers[m_nextPageID] = buf;
		m_nextPageID++;
		m_lock.Unlock();

		// Ex latch the buffer page
		//
		buf->FixPage(EX_LATCH);
		return buf;
	}

	// Find the buffer containing page structure given the page ID.
	//
	Buf* BufferPool::FindPage(
		PageId pageID)
	{
		assert(pageID < m_nextPageID);
		return m_buffers[pageID];
	}

	// Singleton buffer pool.
	//
	static BufferPool bufferPool;

	BufferPool* GetGlobalBufferPool()
	{
		return &bufferPool;
	}

	// Latch the page
	//
	void Buf::FixPage(LatchType type)
	{
		m_latch.AcquireLatch(type);
	}

	// Release the latch
	//
	void Buf::Release()
	{
		m_latch.Release();
	}
}