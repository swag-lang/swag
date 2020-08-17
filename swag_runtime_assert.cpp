#include "swag_runtime.h"
#include "libc/libc.h"

static void __print(const void* __msg)
{
    swag_runtime_print_n((const char*) __msg, (swag_runtime_int32_t) strlen((const char*) __msg));
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_assert(bool expr, const void* file, swag_runtime_int32_t line, const void* message)
{
    if (expr)
        return;

    __print("error: ");
    __print(file ? file : "<unknown file>");
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
