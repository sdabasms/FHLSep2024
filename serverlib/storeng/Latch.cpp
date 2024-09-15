#include "Latch.h"
#include <cassert>

// Acquire the spinlock
//
void SpinLock::Lock()
{
    while (m_lock.test_and_set(std::memory_order_acquire)) {}
    m_ownerId = std::this_thread::get_id();
}

// Release the spinlock
//
void SpinLock::Unlock()
{
    assert(std::this_thread::get_id() == m_ownerId);
    m_lock.clear(std::memory_order_release);
}

void Latch::AcquireLatch(LatchType type)
{
    // We are not adding timeout mechanism.
    // Requesting thread will busy wait until latch is acquired.
    //
    m_lock.Lock();
    switch (type)
    {
    case SH_LATCH:
        // If there are EX latch holders or waiters, keep waiting
        //
        while (m_countExHolders > 0 || m_countExWaiters > 0)
        {
            m_lock.Unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            m_lock.Lock();
        }

        m_countShHolders++;
        break;

    case EX_LATCH:
        // If there are SH or EX latch holders, keep waiting
        //
        m_countExWaiters++;
        while (m_countExHolders > 0 || m_countShHolders > 0)
        {
            m_lock.Unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            m_lock.Lock();
        }

        m_countExHolders++;
        m_countExWaiters--;
        m_ownerId = std::this_thread::get_id();
        break;

    default:
        assert(false);
        break;
    }

    m_lock.Unlock();
}

void Latch::Release()
{
    // Acquire spin lock
    //
    m_lock.Lock();
    if (m_countExHolders > 0)
    {
        // if EX_LATCH was taken, validate that owner is trying to release it.
        //
        assert(std::this_thread::get_id() == m_ownerId);
        assert(m_countExHolders < 2);
        m_countExHolders--;
        assert(m_countExHolders > -1);
    }
    else
    {
        // For SH_LATCH, decrement the number of SH latch holder count.
        //
        assert(m_countShHolders > 0);
        m_countShHolders--;
        assert(m_countShHolders > -1);
    }

    // Release spin lock
    //
    m_lock.Unlock();
}

// Returns true if the caller holds the EX latch
//
bool Latch::IsExLatched() const
{
    return ((m_countExHolders == 1) && (std::this_thread::get_id() == m_ownerId));
}
