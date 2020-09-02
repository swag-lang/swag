#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C void* swag_runtime_interfaceof(const void* structType, const void* itfType)
{
    auto ctype  = (ConcreteTypeInfoStruct*) structType;
    auto itype  = (ConcreteTypeInfoStruct*) itfType;
    auto buffer = (ConcreteTypeInfoParam*) ctype->interfaces.buffer;

    for (SwagS32 i = 0; i < ctype->interfaces.count; i++)
    {
        if (swag_runtime_compareString(buffer[i].name.buffer,
                                       itype->base.name.buffer,
                                       (SwagU32) buffer[i].name.count,
                                       (SwagU32) itype->base.name.count))
            return buffer[i].value;
    }

    return nullptr;
}
