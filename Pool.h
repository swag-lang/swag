#pragma once
struct PoolElem
{
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
    T* alloc()
    {
        unique_lock lk(lock);
        if (!lastBucket || lastBucket->maxUsed == S)
        {
            lastBucket = new PoolSlot<T, S>();
            return &lastBucket->buffer[0];
        }

        return &lastBucket->buffer[lastBucket->maxUsed++];
    }

    mutex           lock;
    T*              firstFree  = nullptr;
    PoolSlot<T, S>* lastBucket = nullptr;
};
