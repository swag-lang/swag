#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C void swag_runtime_convertArgcArgv(void* dest, swag_runtime_int32_t argc, void* argv[])
{
    static swag_runtime_uint64_t argumentsStr[MAX_COMMAND_ARGUMENTS];
    swag_runtime_assert(argc <= MAX_COMMAND_ARGUMENTS, __FILE__, __LINE__, "too many application arguments");

    for (swag_runtime_int32_t i = 0; i < argc; i++)
    {
        argumentsStr[i * 2]       = (swag_runtime_int64_t) argv[i];
        argumentsStr[(i * 2) + 1] = (swag_runtime_int64_t) strlen((const char*) argv[i]);
    }

    void** p = (void**) dest;
    p[0]     = &argumentsStr[0];
    p[1]     = (void*) (swag_runtime_uint64_t) argc;
}
