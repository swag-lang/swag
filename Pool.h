#pragma once
struct PoolElem
{
    PoolElem*    nextFree  = nullptr;
    virtual void reset()   = 0;
    virtual void release() = 0;
};

template<typename T, int S>
struct PoolSlot
{
    T   buffer[S];
    int maxUsed = 1;
};

template<typename T, int S = 32>
struct Pool
{
    void release(T* elem)
    {
        scoped_lock lk(mutexFree);
        elem->nextFree = firstFree;
        firstFree      = elem;
    }

    T* alloc()
    {
        if (mutexFree.try_lock())
        {
            if (firstFree)
            {
                auto result = firstFree;
                firstFree   = (T*) firstFree->nextFree;
                mutexFree.unlock();
                result->reset();
                return result;
            }

            mutexFree.unlock();
        }

        if (!lastBucket || lastBucket->maxUsed == S)
        {
            lastBucket = new PoolSlot<T, S>();
            return &lastBucket->buffer[0];
        }

        return &lastBucket->buffer[lastBucket->maxUsed++];
    }

    mutex           mutexFree;
    T*              firstFree  = nullptr;
    PoolSlot<T, S>* lastBucket = nullptr;
};
