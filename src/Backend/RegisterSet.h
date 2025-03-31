#pragma once
enum class CpuReg : uint8_t;

struct RegisterSet
{
    void push_back(CpuReg reg)
    {
        regs |= 1ULL << static_cast<uint64_t>(reg);
    }

    void append(RegisterSet other)
    {
        regs |= other.regs;
    }

    bool contains(CpuReg reg) const
    {
        return regs & (1ULL << static_cast<uint64_t>(reg));
    }

    struct Iterator
    {
        uint64_t remaining;
        int      index;

        Iterator(uint64_t regs, int startIdx = 0) :
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
        return Iterator(regs, 0);
    }

    Iterator end() const
    {
        return Iterator(0, 64); // 64 means "past the last"
    }

    uint64_t regs = 0;
};
