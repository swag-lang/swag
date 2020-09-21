#include "swag_runtime.h"
#include "libc/libc.h"

EXTERN_C bool swag_runtime_compareType(const void* type1, const void* type2, SwagU32 flags)
{
    if (type1 == type2)
        return true;
    if (!type1 && !type2)
        return true;
    if (!type1 || !type2)
        return false;

    auto ctype1 = (ConcreteTypeInfo*) type1;
    auto ctype2 = (ConcreteTypeInfo*) type2;

    // Fine to convert from concrete to ref, or the other way
    if (flags & COMPARE_CAST_ANY)
    {
        if (ctype1->kind == TypeInfoKind::Reference && ctype2->kind != TypeInfoKind::Reference)
        {
            ConcreteTypeInfoReference* ref = (ConcreteTypeInfoReference*) ctype1;
            return swag_runtime_compareType(ref->pointedType, ctype2, flags);
        }

        if (ctype1->kind != TypeInfoKind::Reference && ctype2->kind == TypeInfoKind::Reference)
        {
            ConcreteTypeInfoReference* ref = (ConcreteTypeInfoReference*) ctype2;
            return swag_runtime_compareType(ctype1, ref->pointedType, flags);
        }
    }

    if ((ctype1->kind != ctype2->kind) || (ctype1->sizeOf != ctype2->sizeOf) || (ctype1->flags != ctype2->flags))
        return false;
    return swag_runtime_compareString(ctype1->name.buffer, ctype2->name.buffer, (SwagU32) ctype1->name.count, (SwagU32) ctype2->name.count);
}
