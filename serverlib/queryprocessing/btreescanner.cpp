#include "btreescanner.h"

namespace Qp
{
	BTreeScanner::BTreeScanner(SE::IStorage *storage)
	{
		m_storage = storage;
	}

	void BTreeScanner::Open()
	{
		m_storage->Open();
	}

	bool BTreeScanner::GetRow(Value* rgvals)
	{
		return m_storage->GetRow(rgvals);
	}

	void BTreeScanner::Close()
	{
		m_storage->Close();
	}
}