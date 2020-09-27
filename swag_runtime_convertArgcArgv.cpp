#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C void swag_runtime_convertArgcArgv(void* dest, SwagS32 argc, void* argv[])
{
    static SwagU64 argumentsStr[MAX_COMMAND_ARGUMENTS];

    for (SwagS32 i = 0; i < argc; i++)
    {
        argumentsStr[i * 2]       = (SwagS64) argv[i];
        argumentsStr[(i * 2) + 1] = (SwagS64) strlen((const char*) argv[i]);
    }

    void** p = (void**) dest;
    p[0]     = &argumentsStr[0];
    p[1]     = (void*) (SwagU64) argc;
}
