#pragma once
#include "Register.h"
struct TypeInfo;

static const uint32_t ATTRIBUTE_CONSTEXPR     = 0x00000001;
static const uint32_t ATTRIBUTE_PRINTBYTECODE = 0x00000002;
static const uint32_t ATTRIBUTE_TEST_FUNC     = 0x00000004;
static const uint32_t ATTRIBUTE_COMPILER      = 0x00000008;
static const uint32_t ATTRIBUTE_PUBLIC        = 0x00000010;
static const uint32_t ATTRIBUTE_FOREIGN       = 0x00000020;
static const uint32_t ATTRIBUTE_INIT_FUNC     = 0x00000040;
static const uint32_t ATTRIBUTE_DROP_FUNC     = 0x00000080;
static const uint32_t ATTRIBUTE_RUN_FUNC      = 0x00000100;
static const uint32_t ATTRIBUTE_MAIN_FUNC     = 0x00000200;
static const uint32_t ATTRIBUTE_COMPLETE      = 0x00000400;
static const uint32_t ATTRIBUTE_INLINE        = 0x00000800;
static const uint32_t ATTRIBUTE_PACK          = 0x00001000;
static const uint32_t ATTRIBUTE_FLAGS         = 0x00002000;
static const uint32_t ATTRIBUTE_PRIVATE       = 0x00004000;
static const uint32_t ATTRIBUTE_MACRO         = 0x00008000;
static const uint32_t ATTRIBUTE_MIXIN         = 0x00010000;
static const uint32_t ATTRIBUTE_READONLY      = 0x00020000;
static const uint32_t ATTRIBUTE_INTERNAL      = 0x00040000;
static const uint32_t ATTRIBUTE_PROPERTY      = 0x00080000;
static const uint32_t ATTRIBUTE_NOBSS         = 0x00100000;
static const uint32_t ATTRIBUTE_NODOC         = 0x00200000;

struct TypeInfoFuncAttr;
struct AstNode;

struct AttributeParameter
{
    Utf8          name;
    TypeInfo*     typeInfo;
    ComputedValue value;
};

struct OneAttribute
{
    Utf8                       name;
    vector<AttributeParameter> parameters;
    AstNode*                   node = nullptr;
};

struct SymbolAttributes
{
    set<TypeInfoFuncAttr*> isHere;
    vector<OneAttribute>   attributes;

    OneAttribute* getAttribute(const Utf8& fullName);
    bool          getValue(const Utf8& fullName, const Utf8& parameter, ComputedValue& value);
    bool          hasAttribute(const Utf8& fullName);

    void reset()
    {
        isHere.clear();
        attributes.clear();
    }

    bool empty() const
    {
        return attributes.empty();
    }

    uint32_t size() const
    {
        return (uint32_t) attributes.size();
    }
};
