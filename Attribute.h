#pragma once
#include "Register.h"
struct TypeInfo;

static const uint64_t ATTRIBUTE_CONSTEXPR = 0x0000000000000001;
static const uint64_t ATTRIBUTE_PRINT_BC  = 0x0000000000000002;
static const uint64_t ATTRIBUTE_TEST_FUNC = 0x0000000000000004;
static const uint64_t ATTRIBUTE_COMPILER  = 0x0000000000000008;
static const uint64_t ATTRIBUTE_PUBLIC    = 0x0000000000000010;
static const uint64_t ATTRIBUTE_FOREIGN   = 0x0000000000000020;
static const uint64_t ATTRIBUTE_INIT_FUNC = 0x0000000000000040;
static const uint64_t ATTRIBUTE_DROP_FUNC = 0x0000000000000080;
static const uint64_t ATTRIBUTE_RUN_FUNC  = 0x0000000000000100;
static const uint64_t ATTRIBUTE_MAIN_FUNC = 0x0000000000000200;
static const uint64_t ATTRIBUTE_COMPLETE  = 0x0000000000000400;
static const uint64_t ATTRIBUTE_INLINE    = 0x0000000000000800;
//static const uint64_t ATTRIBUTE_PACK          = 0x0000000000001000;
static const uint64_t ATTRIBUTE_ENUM_FLAGS = 0x0000000000002000;
static const uint64_t ATTRIBUTE_PRIVATE    = 0x0000000000004000;
static const uint64_t ATTRIBUTE_MACRO      = 0x0000000000008000;
static const uint64_t ATTRIBUTE_MIXIN      = 0x0000000000010000;
static const uint64_t ATTRIBUTE_PROTECTED  = 0x0000000000020000;
//static const uint64_t ATTRIBUTE_READWRITE     = 0x0000000000040000;
static const uint64_t ATTRIBUTE_PROPERTY  = 0x0000000000080000;
static const uint64_t ATTRIBUTE_NO_BSS    = 0x0000000000100000;
static const uint64_t ATTRIBUTE_INDEX     = 0x0000000000200000;
static const uint64_t ATTRIBUTE_AST_FUNC  = 0x0000000000400000;
static const uint64_t ATTRIBUTE_NO_RETURN = 0x0000000000800000;
//static const uint64_t ATTRIBUTE_INTERNAL       = 0x0000000001000000;
static const uint64_t ATTRIBUTE_SAFETY_ON      = 0x0000000002000000;
static const uint64_t ATTRIBUTE_SAFETY_OFF     = 0x0000000004000000;
static const uint64_t ATTRIBUTE_GLOBAL         = 0x0000000008000000;
static const uint64_t ATTRIBUTE_COMPILER_FUNC  = 0x0000000010000000;
static const uint64_t ATTRIBUTE_NO_OPTIM       = 0x0000000020000000;
static const uint64_t ATTRIBUTE_GENERATED_FUNC = 0x0000000100000000;

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
