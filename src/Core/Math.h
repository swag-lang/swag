#pragma once

namespace Math
{
    inline bool isPowerOfTwo(size_t v)
    {
        if (!v)
            return false;
        return (v & v - 1) == 0;
    }

    inline bool isAligned(uint32_t v, uint32_t align)
    {
        return (v & (align - 1)) == 0;
    }

    inline uint32_t align(uint32_t v, uint32_t align)
    {
        return (v + align - 1) & ~(align - 1);
    }

    inline bool addWillOverflow(int8_t x, int8_t y)
    {
        const int32_t result = static_cast<int32_t>(x) + static_cast<int32_t>(y);
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
        return false;
    }

    inline bool addWillOverflow(int16_t x, int16_t y)
    {
        const int32_t result = static_cast<int32_t>(x) + static_cast<int32_t>(y);
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
        return false;
    }

    inline bool addWillOverflow(int32_t x, int32_t y)
    {
        const int64_t result = static_cast<int64_t>(x) + static_cast<int64_t>(y);
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
        return false;
    }

    inline bool addWillOverflow(int64_t x, int64_t y)
    {
        if (y < 0 && x < INT64_MIN - y)
            return true;
        if (y > 0 && x > INT64_MAX - y)
            return true;
        return false;
    }

    inline bool addWillOverflow(uint8_t x, uint8_t y)
    {
        const uint32_t result = static_cast<uint32_t>(x) + static_cast<uint32_t>(y);
        if (result > UINT8_MAX)
            return true;
        return false;
    }

    inline bool addWillOverflow(uint16_t x, uint16_t y)
    {
        const uint32_t result = static_cast<uint32_t>(x) + static_cast<uint32_t>(y);
        if (result > UINT16_MAX)
            return true;
        return false;
    }

    inline bool addWillOverflow(uint32_t x, uint32_t y)
    {
        const uint64_t result = static_cast<uint64_t>(x) + static_cast<uint64_t>(y);
        if (result > UINT32_MAX)
            return true;
        return false;
    }

    inline bool addWillOverflow(uint64_t x, uint64_t y)
    {
        if (y > UINT64_MAX - x)
            return true;
        return false;
    }

    inline bool subWillOverflow(int8_t x, int8_t y)
    {
        const int32_t result = static_cast<int32_t>(x) - static_cast<int32_t>(y);
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
        return false;
    }

    inline bool subWillOverflow(int16_t x, int16_t y)
    {
        const int32_t result = static_cast<int32_t>(x) - static_cast<int32_t>(y);
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
        return false;
    }

    inline bool subWillOverflow(int32_t x, int32_t y)
    {
        const int64_t result = static_cast<int64_t>(x) - static_cast<int64_t>(y);
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
        return false;
    }

    inline bool subWillOverflow(int64_t x, int64_t y)
    {
        if (-y < 0 && x < INT64_MIN + y)
            return true;
        if (-y > 0 && x > INT64_MAX + y)
            return true;
        return false;
    }

    inline bool subWillOverflow(uint8_t x, uint8_t y)
    {
        const uint32_t result = static_cast<uint32_t>(x) - static_cast<uint32_t>(y);
        if (result > UINT8_MAX)
            return true;
        return false;
    }

    inline bool subWillOverflow(uint16_t x, uint16_t y)
    {
        const uint32_t result = static_cast<uint32_t>(x) - static_cast<uint32_t>(y);
        if (result > UINT16_MAX)
            return true;
        return false;
    }

    inline bool subWillOverflow(uint32_t x, uint32_t y)
    {
        const uint64_t result = static_cast<uint64_t>(x) - static_cast<uint64_t>(y);
        if (result > UINT32_MAX)
            return true;
        return false;
    }

    inline bool subWillOverflow(uint64_t x, uint64_t y)
    {
        if (y > x)
            return true;
        return false;
    }

    inline bool mulWillOverflow(int8_t x, int8_t y)
    {
        const int32_t result = static_cast<int32_t>(x) * static_cast<int32_t>(y);
        if (result < INT8_MIN || result > INT8_MAX)
            return true;
        return false;
    }

    inline bool mulWillOverflow(int16_t x, int16_t y)
    {
        const int32_t result = static_cast<int32_t>(x) * static_cast<int32_t>(y);
        if (result < INT16_MIN || result > INT16_MAX)
            return true;
        return false;
    }

    inline bool mulWillOverflow(int32_t x, int32_t y)
    {
        const int64_t result = static_cast<int64_t>(x) * static_cast<int64_t>(y);
        if (result < INT32_MIN || result > INT32_MAX)
            return true;
        return false;
    }

    inline bool mulWillOverflow(int64_t x, int64_t y)
    {
        if ((x > 0 && y > 0 && x > INT64_MAX / y) ||
            (x < 0 && y > 0 && x < INT64_MIN / y) ||
            (x > 0 && y < 0 && y < INT64_MIN / x) ||
            (x < 0 && y < 0 && (x <= INT64_MIN || y <= INT64_MIN || -x > INT64_MAX / -y)))
            return true;
        return false;
    }

    inline bool mulWillOverflow(uint8_t x, uint8_t y)
    {
        const uint32_t result = static_cast<uint32_t>(x) * static_cast<uint32_t>(y);
        if (result > UINT8_MAX)
            return true;
        return false;
    }

    inline bool mulWillOverflow(uint16_t x, uint16_t y)
    {
        const uint32_t result = static_cast<uint32_t>(x) * static_cast<uint32_t>(y);
        if (result > UINT16_MAX)
            return true;
        return false;
    }

    inline bool mulWillOverflow(uint32_t x, uint32_t y)
    {
        const uint64_t result = static_cast<uint64_t>(x) * static_cast<uint64_t>(y);
        if (result > UINT32_MAX)
            return true;
        return false;
    }

    inline bool mulWillOverflow(uint64_t x, uint64_t y)
    {
        const auto res = x * y;
        if (res > 0 && UINT64_MAX / y < x)
            return true;
        return false;
    }
}
