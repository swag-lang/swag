#pragma once
struct PoolFreeElem
{
    PoolFreeElem* nextFree = nullptr;
};

template<typename T, int S = 32>
struct PoolFree
{
    void free(T* elem)
    {
        elem->nextFree = firstFree;
        firstFree      = elem;
    }

    T* alloc()
    {
        if (firstFree)
        {
            auto result = firstFree;
            firstFree   = (T*) result->nextFree;
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
