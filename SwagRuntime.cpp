#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "SwagRuntime.h"

/////////////////////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////////////////////
static void __ftoa(char* result, double value)
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

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void* swag_runtime_malloc(uint64_t size)
{
    auto result = malloc(size);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void* swag_runtime_realloc(void* addr, uint64_t size)
{
    auto result = realloc(addr, size);
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_free(void* addr)
{
    free(addr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void* swag_runtime_memcpy(void* mem1, const void* mem2, uint64_t size)
{
    return memcpy(mem1, mem2, size);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void* swag_runtime_memset(void* mem, int32_t value, uint64_t size)
{
    return memset(mem, value, size);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" int32_t swag_runtime_memcmp(const void* mem1, const void* mem2, uint64_t size)
{
    return memcmp(mem1, mem2, size);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_exit(int32_t exitCode)
{
    exit(exitCode);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" int32_t swag_runtime_strlen(const char* message)
{
    int32_t len = 0;
    while (*message++)
        len++;
    return len;
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" bool swag_runtime_strcmp(const char* str1, const char* str2, uint32_t num)
{
    if (!str1 || !str2)
        return str1 == str2;
    return !memcmp((void*) str1, (void*) str2, num);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_print_n(const char* message, int len)
{
    if (!message)
    {
        message = "<null>";
        len     = 6;
    }
#ifdef _WIN32
    WriteFile(GetStdHandle(-11), (void*) message, len, 0, 0);
#else
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_print(const char* message)
{
    if (!message)
        message = "<null>";
    swag_runtime_print_n(message, swag_runtime_strlen(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_print_i64(int64_t value)
{
    char buf[100];
    __itoa(buf, value);
    swag_runtime_print(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_print_f64(double value)
{
    char buf[100];
    __ftoa(buf, value);
    swag_runtime_print(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_assert(bool expr, const char* file, int line, const char* msg, uint32_t flags)
{
    if (expr)
        return;

    swag_runtime_print("error: ");
    swag_runtime_print(file);
    swag_runtime_print(":");
    swag_runtime_print_i64(line);
    if (msg && msg[0])
    {
        swag_runtime_print(": ");
        swag_runtime_print(msg);
        swag_runtime_print("\n");
    }
    else
        swag_runtime_print(": assertion failed\n");

#ifdef _WIN32
    if (flags & SWAG_ASSERT_DEVMODE)
        MessageBoxA(0, "Native assertion failed !", "[Developer Mode]", 0x10);
    if (flags & SWAG_ASSERT_RETURN)
        return;
    RaiseException(0x666, 0, 0, 0);
#endif
    exit(-1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void* swag_runtime_loadDynamicLibrary(const char* name)
{
#ifdef _WIN32
    return LoadLibraryA(name);
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
extern "C" void swag_runtime_tlsSetValue(uint32_t id, void* value)
{
#ifdef _WIN32
    TlsSetValue(id, value);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void* swag_runtime_tlsGetValue(uint32_t id)
{
#ifdef _WIN32
    auto result = TlsGetValue(id);
    return result;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
extern "C" void swag_runtime_convertArgcArgv(void* dest, int argc, char* argv[])
{
    uint64_t argumentsStr[64];
    swag_runtime_assert(argc <= 64, __FILE__, __LINE__, "too many application arguments", 0);

    for (int i = 0; i < argc; i++)
    {
        argumentsStr[i * 2]       = (int64_t) argv[i];
        argumentsStr[(i * 2) + 1] = (int64_t) swag_runtime_strlen(argv[i]);
    }

    void** p = (void**) dest;
    p[0]     = &argumentsStr[0];
    p[1]     = (void*) (uint64_t) argc;
}
