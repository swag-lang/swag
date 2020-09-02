#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C bool swag_runtime_compareType(const void* type1, const void* type2)
{
    if (type1 == type2)
        return true;
    if (!type1 && !type2)
        return true;
    if (!type1 || !type2)
        return false;

    auto ctype1 = (ConcreteTypeInfo*) type1;
    auto ctype2 = (ConcreteTypeInfo*) type2;
    return swag_runtime_compareString(ctype1->name.buffer, ctype2->name.buffer, (SwagU32) ctype1->name.count, (SwagU32) ctype2->name.count);
}
