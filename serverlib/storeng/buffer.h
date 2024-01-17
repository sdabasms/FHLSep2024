#include <vector>
#include <map>
#include <assert.h>
#include "common/value.h"

namespace SE
{
	// Implementation of buffer pool to provide list of pages.
	//
	class BufferPool
	{
	public:
		BufferPool()
			:
			m_nextPageID(1)
		{}

		Page* GetNewPage(unsigned int level);
		Page* FindPage(PageId pageID);

	private:
		PageId m_nextPageID;
		std::map<int, Page*> m_pages;
	};

	// Singleton buffer pool.
	//
	BufferPool* GetGlobalBufferPool();
}