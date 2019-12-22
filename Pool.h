#pragma once
struct PoolElem
{
    PoolElem*    nextFree;
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
        elem->nextFree = firstFree;
        firstFree      = elem;
    }

    T* alloc()
    {
        if (firstFree)
        {
            auto result = firstFree;
            firstFree   = (T*) firstFree->nextFree;
            result->reset();
            return result;
        }

        if (!lastBucket || lastBucket->maxUsed == S)
        {
            lastBucket = new PoolSlot<T, S>();
            return &lastBucket->buffer[0];
        }

        return &lastBucket->buffer[lastBucket->maxUsed++];
    }

    T*              firstFree  = nullptr;
    PoolSlot<T, S>* lastBucket = nullptr;
};
