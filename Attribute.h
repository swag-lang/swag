#pragma once
#include "Register.h"
struct TypeInfo;

static const uint32_t ATTRIBUTE_CONSTEXPR     = 0x00000001;
static const uint32_t ATTRIBUTE_PRINTBYTECODE = 0x00000002;
static const uint32_t ATTRIBUTE_TEST          = 0x00000004;
static const uint32_t ATTRIBUTE_COMPILER      = 0x00000008;
static const uint32_t ATTRIBUTE_PUBLIC        = 0x00000010;
static const uint32_t ATTRIBUTE_FOREIGN       = 0x00000020;

struct TypeInfoFuncAttr;
struct SymbolAttributes
{
    set<TypeInfoFuncAttr*>                    attributes;
    map<Utf8, pair<TypeInfo*, ComputedValue>> values;

    bool getValue(const Utf8& fullName, ComputedValue& value);

    void reset()
    {
        attributes.clear();
        values.clear();
    }
};
