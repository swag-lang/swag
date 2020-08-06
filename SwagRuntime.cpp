#include <stdint.h>
#include "SwagRuntime.h"
#include "libc/libc.h"

static char* __itoa(char* result, int64_t value)
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

static void __ftoa(char* result, double value)
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
        n = __itoa(result, ipart);
    *n++ = '.';

    int afterPoint = 5;
    if (fpart < 0)
        fpart = -fpart;
    while (afterPoint--)
        fpart *= 10;
    __itoa(n, (int64_t) fpart);
}

static void __print(const void* __msg)
{
    swag_runtime_print_n((const char*) __msg, (int) strlen((const char*) __msg));
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_print_n(const void* message, int len)
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
EXTERN_C void swag_runtime_print_i64(int64_t value)
{
    char buf[100];
    __itoa(buf, (int) value);
    __print(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_print_f64(double value)
{
    char buf[100];
    __ftoa(buf, value);
    __print(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_assert(bool expr, const void* file, int line, const void* message)
{
    if (expr)
        return;

    __print("error: ");
    __print(file);
    __print(":");
    swag_runtime_print_i64(line);

    const char* msg = (const char*) message;
    if (msg && msg[0])
    {
        __print(": ");
        __print(msg);
        __print("\n");
    }
    else
    {
        __print(": assertion failed\n");
    }

#ifdef _WIN32
#ifdef _DEBUG
    //MessageBoxA(0, "Native assertion failed !", "[Developer Mode]", 0x10);
#endif
    RaiseException(0x666, 0, 0, 0);
#endif
    exit(-1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* swag_runtime_loadDynamicLibrary(const void* name)
{
#ifdef _WIN32
    return LoadLibraryA((const char*) name);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" uint32_t swag_runtime_tlsAlloc()
{
#ifdef _WIN32
    return TlsAlloc();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_tlsSetValue(uint32_t id, void* value)
{
#ifdef _WIN32
    TlsSetValue(id, value);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* swag_runtime_tlsGetValue(uint32_t id)
{
#ifdef _WIN32
    auto  result = TlsGetValue(id);
    return result;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_convertArgcArgv(void* dest, int argc, void* argv[])
{
    uint64_t argumentsStr[MAX_COMMAND_ARGUMENTS];
    swag_runtime_assert(argc <= MAX_COMMAND_ARGUMENTS, __FILE__, __LINE__, "too many application arguments");

    for (int i = 0; i < argc; i++)
    {
        argumentsStr[i * 2]       = (int64_t) argv[i];
        argumentsStr[(i * 2) + 1] = (int64_t) strlen((const char*) argv[i]);
    }

    void** p = (void**) dest;
    p[0]     = &argumentsStr[0];
    p[1]     = (void*) (uint64_t) argc;
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C bool swag_runtime_comparestring(const void* str1, const void* str2, uint32_t num)
{
    if (!str1 || !str2)
        return str1 == str2;
    return !memcmp((void*) str1, (void*) str2, num);
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C bool swag_runtime_comparetype(const void* type1, const void* type2)
{
    if (type1 == type2)
        return true;
    if (!type1 && !type2)
        return true;
    if (!type1 || !type2)
        return false;

    auto ctype1 = (ConcreteTypeInfo*) type1;
    auto ctype2 = (ConcreteTypeInfo*) type2;
    if (ctype1->name.count != ctype2->name.count)
        return false;

    return swag_runtime_comparestring(ctype1->name.buffer, ctype2->name.buffer, (uint32_t) ctype1->name.count);
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* swag_runtime_interfaceof(const void* structType, const void* itfType)
{
    auto ctype  = (ConcreteTypeInfoStruct*) structType;
    auto itype  = (ConcreteTypeInfoStruct*) itfType;
    auto buffer = (ConcreteTypeInfoParam*) ctype->interfaces.buffer;

    for (int i = 0; i < ctype->interfaces.count; i++)
    {
        if (buffer[i].name.count != itype->base.name.count)
            continue;
        if (swag_runtime_comparestring(buffer[i].name.buffer, itype->base.name.buffer, (uint32_t) itype->base.name.count))
            return buffer[i].value;
    }

    return nullptr;
}
