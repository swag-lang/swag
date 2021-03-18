#pragma once
#include "Register.h"
struct TypeInfo;

static const uint64_t ATTRIBUTE_CONSTEXPR           = 0x0000000000000001;
static const uint64_t ATTRIBUTE_PRINT_BC            = 0x0000000000000002;
static const uint64_t ATTRIBUTE_TEST_FUNC           = 0x0000000000000004;
static const uint64_t ATTRIBUTE_COMPILER            = 0x0000000000000008;
static const uint64_t ATTRIBUTE_PUBLIC              = 0x0000000000000010;
static const uint64_t ATTRIBUTE_FOREIGN             = 0x0000000000000020;
static const uint64_t ATTRIBUTE_INIT_FUNC           = 0x0000000000000040;
static const uint64_t ATTRIBUTE_DROP_FUNC           = 0x0000000000000080;
static const uint64_t ATTRIBUTE_RUN_FUNC            = 0x0000000000000100;
static const uint64_t ATTRIBUTE_MAIN_FUNC           = 0x0000000000000200;
static const uint64_t ATTRIBUTE_COMPLETE            = 0x0000000000000400;
static const uint64_t ATTRIBUTE_INLINE              = 0x0000000000000800;
static const uint64_t ATTRIBUTE_SELECTIF_FUNC       = 0x0000000000001000;
static const uint64_t ATTRIBUTE_ENUM_FLAGS          = 0x0000000000002000;
static const uint64_t ATTRIBUTE_PRIVATE             = 0x0000000000004000;
static const uint64_t ATTRIBUTE_MACRO               = 0x0000000000008000;
static const uint64_t ATTRIBUTE_MIXIN               = 0x0000000000010000;
static const uint64_t ATTRIBUTE_PROTECTED           = 0x0000000000020000;
static const uint64_t ATTRIBUTE_STRICT              = 0x0000000000040000;
static const uint64_t ATTRIBUTE_PROPERTY            = 0x0000000000080000;
static const uint64_t ATTRIBUTE_NO_BSS              = 0x0000000000100000;
static const uint64_t ATTRIBUTE_ENUM_INDEX          = 0x0000000000200000;
static const uint64_t ATTRIBUTE_AST_FUNC            = 0x0000000000400000;
static const uint64_t ATTRIBUTE_NO_RETURN           = 0x0000000000800000;
static const uint64_t ATTRIBUTE_CALLBACK            = 0x0000000001000000;
static const uint64_t ATTRIBUTE_OPTIM_BK_ON         = 0x0000000002000000;
static const uint64_t ATTRIBUTE_OPTIM_BK_OFF        = 0x0000000004000000;
static const uint64_t ATTRIBUTE_GLOBAL              = 0x0000000008000000;
static const uint64_t ATTRIBUTE_COMPILER_FUNC       = 0x0000000010000000;
static const uint64_t ATTRIBUTE_GENERATED_FUNC      = 0x0000000020000000;
static const uint64_t ATTRIBUTE_OPTIM_BC_ON         = 0x0000000040000000;
static const uint64_t ATTRIBUTE_OPTIM_BC_OFF        = 0x0000000080000000;
static const uint64_t ATTRIBUTE_SELECTIF_ON         = 0x0000000100000000;
static const uint64_t ATTRIBUTE_SELECTIF_OFF        = 0x0000000200000000;
static const uint64_t ATTRIBUTE_SHARP_FUNC          = 0x0000000400000000;
static const uint64_t ATTRIBUTE_SAFETY_OFF_OPERATOR = 0x0000000800000000;
static const uint64_t ATTRIBUTE_NO_COPY             = 0x0000001000000000;
static const uint64_t ATTRIBUTE_IMPLICIT            = 0x0000002000000000;
static const uint64_t ATTRIBUTE_AUTO_DISCARD          = 0x0000004000000000;

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// MUST BE IN SYNC IN BOOTSTRAP.SWG
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static const uint64_t ATTRIBUTE_SAFETY_NP_ON    = 0x0000100000000000;
static const uint64_t ATTRIBUTE_SAFETY_BC_ON    = 0x0000200000000000;
static const uint64_t ATTRIBUTE_SAFETY_OF_ON    = 0x0000400000000000;
static const uint64_t ATTRIBUTE_SAFETY_MT_ON    = 0x0000800000000000;
static const uint64_t ATTRIBUTE_SAFETY_AN_ON    = 0x0001000000000000;
static const uint64_t ATTRIBUTE_SAFETY_NP_OFF   = 0x0010000000000000;
static const uint64_t ATTRIBUTE_SAFETY_BC_OFF   = 0x0020000000000000;
static const uint64_t ATTRIBUTE_SAFETY_OF_OFF   = 0x0040000000000000;
static const uint64_t ATTRIBUTE_SAFETY_MT_OFF   = 0x0080000000000000;
static const uint64_t ATTRIBUTE_SAFETY_AN_OFF   = 0x0100000000000000;
static const uint64_t ATTRIBUTE_SAFETY_MASK_ON  = ATTRIBUTE_SAFETY_NP_ON | ATTRIBUTE_SAFETY_BC_ON | ATTRIBUTE_SAFETY_OF_ON | ATTRIBUTE_SAFETY_MT_ON | ATTRIBUTE_SAFETY_AN_ON;
static const uint64_t ATTRIBUTE_SAFETY_MASK_OFF = ATTRIBUTE_SAFETY_NP_OFF | ATTRIBUTE_SAFETY_BC_OFF | ATTRIBUTE_SAFETY_OF_OFF | ATTRIBUTE_SAFETY_MT_OFF | ATTRIBUTE_SAFETY_AN_OFF;
static const uint64_t ATTRIBUTE_SAFETY_MASK     = ATTRIBUTE_SAFETY_MASK_ON | ATTRIBUTE_SAFETY_MASK_OFF;

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
    VectorNative<TypeInfoFuncAttr*> isHere;
    vector<OneAttribute>            attributes;

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
