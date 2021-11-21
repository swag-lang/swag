#pragma once
#include "Timer.h"
#include "Stats.h"

template<typename T>
struct ScopedLock
{
    ScopedLock(T& mtx)
        : mt{&mtx}
    {
        mt->lock();
    }

    ~ScopedLock()
    {
        mt->unlock();
    }

    T* mt = nullptr;
};

struct SharedLock
{
    SharedLock(shared_mutex& mtx)
        : mt{&mtx}
    {
        mt->lock_shared();
    }

    ~SharedLock()
    {
        mt->unlock_shared();
    }

    shared_mutex* mt = nullptr;
};
