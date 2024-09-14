#include "interfaces/istorage.h"

#include "common/value.h"
#include "btree.h"

#pragma once

namespace SE
{
	// A session operates on an index and stores progress of a scan.
	//
	class BTreeSession : public IStorage
	{
	public:
		BTreeSession(BTree* btree);

		// Implement storage interfaces for an index.
		//
		void Open() override;
		bool GetRow(Value* val)override;
		void Close() override;

	private:

		// Store progress of the scan.
		//
		Value m_lastKey;
		Buf* m_buf;

		// Maintain the queue of pages which has been latched,
		//
		std::deque<Buf*> m_LatchedBufs;
		BTree* m_btree;
	};
}