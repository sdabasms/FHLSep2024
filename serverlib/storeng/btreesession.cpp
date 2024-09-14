#include "btreesession.h"
#include "btree.h"
#include "buffer.h"
#include "page.h"

namespace SE
{
	// Initialize a session with the BTree.
	//
	BTreeSession::BTreeSession(BTree *btree)
	{
		m_btree = btree;
		m_buf = nullptr;
		m_lastKey = -1;
	}

	// Open the session and collect the first key.
	//
	void BTreeSession::Open()
	{
		m_lastKey = -1;
	}

	// Get the next row of the tree in the session.
	//
	bool BTreeSession::GetRow(Value *rgvals)
	{
		Value* newVal = m_btree->GetNextRow(m_lastKey, m_LatchedBufs, &m_buf);

		if (newVal != nullptr)
		{
			m_lastKey = newVal[0];
			for (unsigned int i = 0; i < m_btree->GetNumCols(); i++)
			{
				rgvals[i] = newVal[i];
			}

			return true;
		}

		return false;
	}

	// Close the session and release resources.
	//
	void BTreeSession::Close()
	{
		while (!m_LatchedBufs.empty())
		{
			Buf* buf = m_LatchedBufs.front();
			m_LatchedBufs.pop_front();
			buf->Release();
		}
		m_btree = nullptr;
		m_lastKey = -1;
	}
}