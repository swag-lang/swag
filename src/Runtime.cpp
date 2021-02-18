#include "pch.h"
#include "runtime.h"
#include "assert.h"
#include "context.h"
#include "TypeTable.h"

namespace Runtime
{
    ////////////////////////////////////////////////////////////
    static bool strcmp(const void* str1, uint32_t num1, const void* str2, uint32_t num2)
    {
        if (num1 != num2)
            return false;
        if (!str1 || !str2)
            return str1 == str2;
        return !memcmp((void*) str1, (void*) str2, num1);
    }

    static ConcreteTypeInfo* concreteAlias(ConcreteTypeInfo* type1)
    {
        if (type1->kind != TypeInfoKind::Alias || (type1->flags & (uint16_t) TypeInfoFlags::Strict))
            return type1;
        auto typeAlias = (const ConcreteTypeInfoAlias*) type1;
        return concreteAlias(RELATIVE_PTR(&typeAlias->rawType));
    }

    bool compareType(const void* type1, const void* type2, uint32_t flags)
    {
        if (type1 == type2)
            return true;
        if (!type1 && !type2)
            return true;
        if (!type1 || !type2)
            return false;

        auto ctype1 = concreteAlias((ConcreteTypeInfo*) type1);
        auto ctype2 = concreteAlias((ConcreteTypeInfo*) type2);

        // Fine to convert from concrete to ref, or the other way
        if (flags & COMPARE_CAST_ANY)
        {
            if (ctype1->kind == TypeInfoKind::Reference && ctype2->kind != TypeInfoKind::Reference)
            {
                ConcreteTypeInfoReference* ref = (ConcreteTypeInfoReference*) ctype1;
                return compareType(RELATIVE_PTR(&ref->pointedType), ctype2, flags);
            }

            if (ctype1->kind != TypeInfoKind::Reference && ctype2->kind == TypeInfoKind::Reference)
            {
                ConcreteTypeInfoReference* ref = (ConcreteTypeInfoReference*) ctype2;
                return compareType(ctype1, RELATIVE_PTR(&ref->pointedType), flags);
            }
        }

        if ((ctype1->kind != ctype2->kind) || (ctype1->sizeOf != ctype2->sizeOf) || (ctype1->flags != ctype2->flags))
            return false;
        return strcmp(ctype1->name.buffer, (uint32_t) ctype1->name.count, ctype2->name.buffer, (uint32_t) ctype2->name.count);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    uint64_t tlsAlloc()
    {
#ifdef _WIN32
        return TlsAlloc();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    void tlsSetValue(uint64_t id, void* value)
    {
#ifdef _WIN32
        TlsSetValue((uint32_t) id, value);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    void* tlsGetValue(uint64_t id)
    {
#ifdef _WIN32
        return TlsGetValue((uint32_t) id);
#endif
    }

} // namespace Runtime