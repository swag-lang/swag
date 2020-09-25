#include "pch.h"

namespace Runtime
{
    ////////////////////////////////////////////////////////////
    int memcmp(const void* b1, const void* b2, size_t n)
    {
        if (!n)
            return 0;

        auto p1 = (const int8_t*) b1;
        auto p2 = (const int8_t*) b2;
        for (size_t i = 0; i < n; i++)
        {
            if (p1[i] != p2[i])
                return p1[i] - p2[i];
        }

        return 0;
    }

    ////////////////////////////////////////////////////////////
    bool strcmp(const void* str1, const void* str2, uint32_t num1, uint32_t num2)
    {
        if (num1 != num2)
            return false;
        if (!str1 || !str2)
            return str1 == str2;
        return !memcmp((void*) str1, (void*) str2, num1);
    }

    ////////////////////////////////////////////////////////////
    float abs(float value)
    {
        return value < 0 ? -value : value;
    }

    ////////////////////////////////////////////////////////////
    void print(const void* message, uint32_t len)
    {
        if (!message || !len)
            return;
#ifdef _WIN32
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), (void*) message, len, 0, 0);
#endif
    }

} // namespace Runtime