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

    bool try_lock()
    {
        return mt.try_lock();
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

    bool try_lock()
    {
        return mt.try_lock();
    }

    bool try_lock_shared()
    {
        return mt.try_lock_shared();
    }
};

template<typename T>
struct ScopedLock
{
    explicit ScopedLock(T& mtx)
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
    explicit SharedLock(SharedMutex& mtx)
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
