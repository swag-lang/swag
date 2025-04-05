// ReSharper disable CppInconsistentNaming
#pragma once

struct StackRange
{
    void clear()
    {
        ranges.clear();
    }

    void addRange(uint32_t start, uint32_t length)
    {
        if (start == UINT32_MAX)
            return;

        uint32_t newStart = start;
        uint32_t newEnd   = start + length;

        Vector<std::pair<uint32_t, uint32_t>> newRanges;
        bool                                  inserted = false;

        for (const auto& [currStart, currEnd] : ranges)
        {
            if (newEnd < currStart)
            {
                // No overlap and before current range
                if (!inserted)
                {
                    newRanges.emplace_back(newStart, newEnd);
                    inserted = true;
                }

                newRanges.emplace_back(currStart, currEnd);
            }
            else if (newStart > currEnd)
            {
                // No overlap and after current range
                newRanges.emplace_back(currStart, currEnd);
            }
            else
            {
                // Overlapping or adjacent ranges – merge
                newStart = std::min(newStart, currStart);
                newEnd   = std::max(newEnd, currEnd);
            }
        }

        if (!inserted)
            newRanges.emplace_back(newStart, newEnd);
        ranges = std::move(newRanges);
    }

    bool isInRange(uint32_t offset) const
    {
        if (ranges.empty())
            return false;

        uint32_t left  = 0;
        uint32_t right = ranges.size() - 1;

        while (left <= right)
        {
            const uint32_t mid       = left + (right - left) / 2;
            const auto& [start, end] = ranges[mid];
            if (offset < start && mid == 0)
                return false;
            if (offset < start)
                right = mid - 1;
            else if (offset >= end)
                left = mid + 1;
            else
                return true;
        }

        return false;
    }

    Vector<std::pair<uint32_t, uint32_t>> ranges;
};
