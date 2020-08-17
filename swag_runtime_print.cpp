#include "swag_runtime.h"
#include "libc/libc.h"

static char* __itoa(char* result, swag_runtime_int64_t value)
{
    char *               ptr = result, *ptr1 = result, tmp_char;
    swag_runtime_int64_t tmp_value;
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

static void __ftoa(char* result, double value)
{
    swag_runtime_int64_t ipart = (swag_runtime_int64_t) value;
    double               fpart = value - (double) ipart;

    char* n = result;
    if (ipart == 0)
    {
        if (value < 0)
            *n++ = '-';
        *n++ = '0';
    }
    else
        n = __itoa(result, ipart);
    *n++ = '.';

    swag_runtime_int32_t afterPoint = 5;
    if (fpart < 0)
        fpart = -fpart;
    while (afterPoint--)
        fpart *= 10;
    __itoa(n, (swag_runtime_int64_t) fpart);
}

static void __print(const void* __msg)
{
    swag_runtime_print_n((const char*) __msg, (swag_runtime_int32_t) strlen((const char*) __msg));
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_print_n(const void* message, swag_runtime_int32_t len)
{
    if (!message)
    {
        message = "<null>";
        len     = 6;
    }
#ifdef _WIN32
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), (void*) message, len, 0, 0);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_print_i64(swag_runtime_int64_t value)
{
    char buf[100];
    __itoa(buf, (swag_runtime_int32_t) value);
    __print(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_print_f64(double value)
{
    char buf[100];
    __ftoa(buf, value);
    __print(buf);
}
