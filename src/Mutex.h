#pragma once
struct Mutex
{
    mutex mt;

    void lock()
    {
        mt.lock();
    }

    void unlock()
    {
        mt.unlock();
    }
};

struct SharedMutex
{
    shared_mutex mt;

    void lock()
    {
        mt.lock();
    }

    void unlock()
    {
        mt.unlock();
    }

    void lock_shared()
    {
        mt.lock_shared();
    }

    void unlock_shared()
    {
        mt.unlock_shared();
    }
};

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
    SharedLock(SharedMutex& mtx)
        : mt{&mtx}
    {
        mt->lock_shared();
    }

    ~SharedLock()
    {
        mt->unlock_shared();
    }

    SharedMutex* mt = nullptr;
};
