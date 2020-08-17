#include "swag_runtime.h"
#include "libc/libc.h"

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C swag_runtime_uint64_t swag_runtime_tlsAlloc()
{
#ifdef _WIN32
    return TlsAlloc();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_tlsSetValue(swag_runtime_uint64_t id, void* value)
{
#ifdef _WIN32
    TlsSetValue((swag_runtime_uint32_t) id, value);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void* swag_runtime_tlsGetValue(swag_runtime_uint64_t id)
{
#ifdef _WIN32
    return TlsGetValue((swag_runtime_uint32_t) id);
#endif
}
