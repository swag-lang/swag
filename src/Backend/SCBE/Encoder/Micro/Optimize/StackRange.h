// ReSharper disable CppInconsistentNaming
#pragma once

struct StackRange
{
    void clear()
    {
        ranges.clear();
    }

    void push_back(uint32_t start, uint32_t length = sizeof(uint64_t))
    {
        if (start == UINT32_MAX)
            return;

        uint32_t newStart = start;
        uint32_t newEnd   = start + length;

        Vector<std::pair<uint32_t, uint32_t>> newRanges;
        bool                                  inserted = false;

        for (const auto& [curStart, curEnd] : ranges)
        {
            if (newEnd < curStart)
            {
                // No overlap and before current range
                if (!inserted)
                {
                    newRanges.emplace_back(newStart, newEnd);
                    inserted = true;
                }

                newRanges.emplace_back(curStart, curEnd);
            }
            else if (newStart > curEnd)
            {
                // No overlap and after current range
                newRanges.emplace_back(curStart, curEnd);
            }
            else
            {
                // Overlapping or adjacent ranges – merge
                newStart = std::min(newStart, curStart);
                newEnd   = std::max(newEnd, curEnd);
            }
        }

        if (!inserted)
            newRanges.emplace_back(newStart, newEnd);
        ranges = std::move(newRanges);
    }

    bool contains(uint32_t offset, uint32_t length = 1) const
    {
        if (ranges.empty())
            return false;

        const uint32_t rangeStart = offset;
        const uint32_t rangeEnd   = offset + length;

        // Binary search for potential overlapping range
        uint32_t left  = 0;
        uint32_t right = ranges.size() - 1;

        while (left <= right)
        {
            const uint32_t mid          = left + (right - left) / 2;
            const auto&    [start, end] = ranges[mid];

            if (rangeEnd < start && mid == 0)
                return false;
            if (rangeEnd < start)
                right = mid - 1;
            else if (rangeStart >= end)
                left = mid + 1;
            else
                return true;
        }

        return false;
    }

    Vector<std::pair<uint32_t, uint32_t>> ranges;
};
