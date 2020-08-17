#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C void* swag_runtime_interfaceof(const void* structType, const void* itfType)
{
    auto ctype  = (ConcreteTypeInfoStruct*) structType;
    auto itype  = (ConcreteTypeInfoStruct*) itfType;
    auto buffer = (ConcreteTypeInfoParam*) ctype->interfaces.buffer;

    for (swag_runtime_int32_t i = 0; i < ctype->interfaces.count; i++)
    {
        if (swag_runtime_compareString(buffer[i].name.buffer, itype->base.name.buffer, (swag_runtime_uint32_t) buffer[i].name.count, (swag_runtime_uint32_t) itype->base.name.count))
            return buffer[i].value;
    }

    return nullptr;
}
