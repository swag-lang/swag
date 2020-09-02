#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C bool swag_runtime_compareString(const void* str1, const void* str2, SwagU32 num1, SwagU32 num2)
{
    if (num1 != num2)
        return false;
    if (!str1 || !str2)
        return str1 == str2;
    return !memcmp((void*) str1, (void*) str2, num1);
}
