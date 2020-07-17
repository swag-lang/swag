#include <stdint.h>
#include <windows.h>

#ifdef _MSC_VER
#define SWAG_IMPORT __declspec(dllimport)
#define SWAG_EXPORT extern "C" __declspec(dllexport)
#endif

SWAG_EXPORT void swag_runtime_print_n(const char* message, int len)
{
    if (!message)
        message = "<null>";
#ifdef _WIN32
    WriteFile(GetStdHandle(-11), (void*) message, len, 0, 0);
#else
#endif
}

SWAG_EXPORT int32_t __strlen(const char* message)
{
    int32_t len = 0;
    while (*message++)
        len++;
    return len;
}

SWAG_EXPORT void __print(const char* message)
{
    if (!message)
        message = "<null>";
    swag_runtime_print_n(message, __strlen(message));
}

SWAG_EXPORT char* __itoa(char* result, int64_t value)
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

SWAG_EXPORT void __ftoa(char* result, double value)
{
    int64_t ipart  = (int64_t) value;
    double  fpart  = value - (double) ipart;
    char*   n      = __itoa(result, ipart);
    *n++           = '.';
    int afterPoint = 5;
    if (fpart < 0)
        fpart = -fpart;
    while (afterPoint--)
        fpart *= 10;
    __itoa(n, (int64_t) fpart);
}

SWAG_EXPORT void __print_i64(int64_t value)
{
    char buf[100];
    __itoa(buf, value);
    __print(buf);
}

SWAG_EXPORT void __print_f64(double value)
{
    char buf[100];
    __ftoa(buf, value);
    __print(buf);
}

SWAG_EXPORT void __assert(bool expr, const char* file, int line, const char* msg)
{
    if (expr)
        return;

    __print("error: [backend] ");
    __print(file);
    __print(":");
    __print_i64(line);
    if (msg)
    {
        __print(": ");
        __print(msg);
        __print("\n");
    }
    else
        __print(": native code assertion failed\n");

#ifdef _WIN32
#ifdef SWAG_DEVMODE
    MessageBoxA(0, "Native assertion failed !", "[Developer Mode]", 0x10);
#endif
    RaiseException(0x666, 0, 0, 0);
#endif
    exit(-1);
}

SWAG_EXPORT bool __strcmp(const char* str1, const char* str2, uint32_t num)
{
    if (!str1 || !str2)
        return str1 == str2;
    return !memcmp((void*) str1, (void*) str2, num);
}