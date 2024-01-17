#pragma once

#include "interfaces/ioperator.h"
#include "interfaces/istorage.h"

namespace Qp
{
	// BTree scanner operator which iterates over the entire tree
	// left to right.
	//
	class BTreeScanner : public IOperator
	{
	public:
		BTreeScanner(SE::IStorage* storage);

		void Open() override;
		bool GetRow(Value* rgvals) override;
		void Close() override;

	private:
		SE::IStorage * m_storage;
	};
}