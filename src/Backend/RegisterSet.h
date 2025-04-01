#pragma once
#include "Os/Os.h"

enum class CpuReg : uint8_t;

struct RegisterSet
{
    void add(CpuReg reg)
    {
        regs |= 1ULL << static_cast<uint64_t>(reg);
    }

    void erase(CpuReg reg)
    {
        regs &= ~(1ULL << static_cast<uint64_t>(reg));
    }

    void append(RegisterSet other)
    {
        regs |= other.regs;
    }

    bool contains(CpuReg reg) const
    {
        return regs & (1ULL << static_cast<uint64_t>(reg));
    }

    void clear()
    {
        regs = 0;
    }

    bool empty() const
    {
        return regs == 0;
    }

    struct Iterator
    {
        uint64_t remaining;
        int      index;

        explicit Iterator(uint64_t regs, int startIdx = 0) :
            remaining(regs),
            index(startIdx)
        {
            advanceToNext();
        }

        CpuReg operator*() const
        {
            return static_cast<CpuReg>(index);
        }

        Iterator& operator++()
        {
            remaining &= ~(1ULL << index); // Clear current bit
            advanceToNext();
            return *this;
        }

        bool operator!=(const Iterator& other) const
        {
            return remaining != other.remaining;
        }

        void advanceToNext()
        {
            while (remaining && !(remaining & (1ULL << index)))
                ++index;
        }
    };

    Iterator begin() const
    {
        return Iterator{regs, 0};
    }

    static Iterator end()
    {
        return Iterator{0, 64};
    }

    CpuReg first() const
    {
        SWAG_ASSERT(!empty());
        return static_cast<CpuReg>(OS::bitCountTz(regs));
    }    

    uint64_t regs = 0;
};
