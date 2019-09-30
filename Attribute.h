#pragma once
#include "Register.h"

static const uint64_t ATTRIBUTE_CONSTEXPR     = 0x00000000'00000001;
static const uint64_t ATTRIBUTE_PRINTBYTECODE = 0x00000000'00000002;
static const uint64_t ATTRIBUTE_TEST          = 0x00000000'00000004;
static const uint64_t ATTRIBUTE_COMPILER      = 0x00000000'00000008;
static const uint64_t ATTRIBUTE_PUBLIC        = 0x00000000'00000010;
static const uint64_t ATTRIBUTE_FOREIGN       = 0x00000000'00000020;

struct TypeInfoFuncAttr;
struct SymbolAttributes
{
    set<TypeInfoFuncAttr*>     attributes;
    map<string, ComputedValue> values;

    bool getValue(const string& fullName, ComputedValue& value);

    void reset()
    {
        attributes.clear();
        values.clear();
    }
};
