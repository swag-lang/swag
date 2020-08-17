#include "swag_runtime.h"
#include "libc/libc.h"

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* swag_runtime_loadDynamicLibrary(const void* name)
{
#ifdef _WIN32
    return LoadLibraryA((const char*) name);
#endif
}
