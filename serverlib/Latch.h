#pragma once

#include <atomic>
#include <thread>

enum LatchType
{
	SH_LATCH = 1,
	EX_LATCH,
	INVALID_LATCH
};

class SpinLock
{
public:
	void Lock();
	void Unlock();

private:
	std::atomic_flag m_lock;
	std::thread::id m_ownerId;
};

class Latch
{
public:
	void AcquireLatch(LatchType type);
	void Release();
	void UpdateLatch();
	
private:
	SpinLock m_lock;
	std::thread::id m_ownerId;

	int m_countShHolders;
	int m_countExHolders;

	int m_countExWaiters;
};
