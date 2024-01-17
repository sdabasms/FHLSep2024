#include "common/value.h"

#pragma once

namespace SE
{
	// This is the interface for storage engine.
	//
	class IStorage
	{
	public:
		// Initializes the storage session.
		//
		virtual void Open() = 0;

		// Gets a single row and writes the row values into rgvals.
		// Returns true if a row was fetched successfully, and false if no more rows are available.
		//
		virtual bool GetRow(Value *rgvals) = 0;

		// Closes the session and releases resources.
		//
		virtual void Close() = 0;
	};
}