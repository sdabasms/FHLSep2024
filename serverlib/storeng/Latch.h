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
	std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
	std::thread::id m_ownerId = std::this_thread::get_id();
};

class Latch
{
public:

	void AcquireLatch(LatchType type);
	void Release();
	void UpdateLatch();
	
private:
	SpinLock m_lock;
	std::thread::id m_ownerId = std::this_thread::get_id();

	int m_countShHolders = 0;
	int m_countExHolders = 0;

	int m_countExWaiters = 0;
};
