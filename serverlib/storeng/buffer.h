#include <vector>
#include <map>
#include <assert.h>
#include "common/value.h"
#include "latch.h"
#include "page.h"

#pragma once

namespace SE
{
	// Implementation of buffer. Buffer is the container for the page in memory and controls access to it.
	//
	class Buf
	{
	public:
		Buf(Page* page)
		{
			m_pPage = page;
		}

		Page* GetPage()
		{
			return m_pPage;
		}

		void FixPage(LatchType type);
		void Release();

	private:
		Page* m_pPage;
		Latch m_latch;
	};

	// Implementation of buffer pool to provide list of pages.
	//
	class BufferPool
	{
	public:
		BufferPool()
			:
			m_nextPageID(1)
		{}

		Buf* GetNewPage(unsigned int level);
		Buf* FindPage(PageId pageID);

	private:
		SpinLock m_lock;
		PageId m_nextPageID;
		std::map<int, Buf*> m_buffers;
	};

	// Singleton buffer pool.
	//
	BufferPool* GetGlobalBufferPool();
}