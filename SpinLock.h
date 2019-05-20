#pragma once
struct SpinLock
{
    void lock()
    {
        while (lck.test_and_set(memory_order_acquire)) {}
    }

    void unlock()
    {
        lck.clear(memory_order_release);
    }

    atomic_flag lck = ATOMIC_FLAG_INIT;
};
