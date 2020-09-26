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

    /////////////////////////////////////////////////////////////////////////////////////////////
    static char* itoa(char* result, int64_t value)
    {
        char *  ptr = result, *ptr1 = result, tmp_char;
        int64_t tmp_value;
        do
        {
            tmp_value = value;
            value /= 10;
            *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * 10)];
        } while (value);

        if (tmp_value < 0)
            *ptr++ = '-';
        char* retVal = ptr;
        *ptr--       = 0;
        while (ptr1 < ptr)
        {
            tmp_char = *ptr;
            *ptr--   = *ptr1;
            *ptr1++  = tmp_char;
        }

        return retVal;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    static void ftoa(char* result, double value)
    {
        int64_t ipart = (int64_t) value;
        double  fpart = value - (double) ipart;

        char* n = result;
        if (ipart == 0)
        {
            if (value < 0)
                *n++ = '-';
            *n++ = '0';
        }
        else
            n = itoa(result, ipart);
        *n++ = '.';

        int32_t afterPoint = 5;
        if (fpart < 0)
            fpart = -fpart;
        while (afterPoint--)
            fpart *= 10;
        ipart = (int64_t) fpart;
        if (fpart - ipart > 0.5)
            ipart += 1;
        itoa(n, (int64_t) fpart);
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

    ////////////////////////////////////////////////////////////
    void print(int64_t value)
    {
        char buf[100];
        itoa(buf, value);
        print(buf, (int) strlen(buf));
    }

    ////////////////////////////////////////////////////////////
    void print(double value)
    {
        char buf[100];
        ftoa(buf, value);
        print(buf, (int) strlen(buf));
    }

} // namespace Runtime