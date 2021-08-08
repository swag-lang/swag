#pragma once

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
