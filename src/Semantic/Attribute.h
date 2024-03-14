#pragma once
#include "Syntax/ComputedValue.h"
#include "Core/Flags.h"
#include "Threading/RaceCondition.h"
#include "Syntax/Tokenizer/Token.h"

struct AstNode;
struct TypeInfo;
struct TypeInfoFuncAttr;

using AttributeFlags = Flags<uint64_t>;

constexpr AttributeFlags ATTRIBUTE_CONSTEXPR           = 0x0000000000000001;
constexpr AttributeFlags ATTRIBUTE_PRINT_BC            = 0x0000000000000002;
constexpr AttributeFlags ATTRIBUTE_TEST_FUNC           = 0x0000000000000004;
constexpr AttributeFlags ATTRIBUTE_COMPILER            = 0x0000000000000008;
constexpr AttributeFlags ATTRIBUTE_PUBLIC              = 0x0000000000000010;
constexpr AttributeFlags ATTRIBUTE_INTERNAL            = 0x0000000000000020;
constexpr AttributeFlags ATTRIBUTE_PRIVATE             = 0x0000000000000040;
constexpr AttributeFlags ATTRIBUTE_DROP_FUNC           = 0x0000000000000080;
constexpr AttributeFlags ATTRIBUTE_RUN_FUNC            = 0x0000000000000100;
constexpr AttributeFlags ATTRIBUTE_MAIN_FUNC           = 0x0000000000000200;
constexpr AttributeFlags ATTRIBUTE_COMPLETE            = 0x0000000000000400;
constexpr AttributeFlags ATTRIBUTE_INLINE              = 0x0000000000000800;
constexpr AttributeFlags ATTRIBUTE_MATCH_VALIDIF_FUNC  = 0x0000000000001000;
constexpr AttributeFlags ATTRIBUTE_ENUM_FLAGS          = 0x0000000000002000;
constexpr AttributeFlags ATTRIBUTE_NOT_GENERIC         = 0x0000000000004000;
constexpr AttributeFlags ATTRIBUTE_MACRO               = 0x0000000000008000;
constexpr AttributeFlags ATTRIBUTE_MIXIN               = 0x0000000000010000;
constexpr AttributeFlags ATTRIBUTE_FOREIGN             = 0x0000000000020000;
constexpr AttributeFlags ATTRIBUTE_STRICT              = 0x0000000000040000;
constexpr AttributeFlags ATTRIBUTE_TLS                 = 0x0000000000080000;
constexpr AttributeFlags ATTRIBUTE_OPAQUE              = 0x0000000000100000;
constexpr AttributeFlags ATTRIBUTE_ENUM_INDEX          = 0x0000000000200000;
constexpr AttributeFlags ATTRIBUTE_AST_FUNC            = 0x0000000000400000;
constexpr AttributeFlags ATTRIBUTE_CALLEE_RETURN       = 0x0000000000800000;
constexpr AttributeFlags ATTRIBUTE_PREMAIN_FUNC        = 0x0000000001000000;
constexpr AttributeFlags ATTRIBUTE_OPTIM_BACKEND_ON    = 0x0000000002000000;
constexpr AttributeFlags ATTRIBUTE_OPTIM_BACKEND_OFF   = 0x0000000004000000;
constexpr AttributeFlags ATTRIBUTE_GLOBAL              = 0x0000000008000000;
constexpr AttributeFlags ATTRIBUTE_MESSAGE_FUNC        = 0x0000000010000000;
constexpr AttributeFlags ATTRIBUTE_GENERATED_FUNC      = 0x0000000020000000;
constexpr AttributeFlags ATTRIBUTE_OPTIM_BYTECODE_ON   = 0x0000000040000000;
constexpr AttributeFlags ATTRIBUTE_OPTIM_BYTECODE_OFF  = 0x0000000080000000;
constexpr AttributeFlags ATTRIBUTE_MATCH_VALIDIF_OFF   = 0x0000000100000000;
constexpr AttributeFlags ATTRIBUTE_MATCH_SELF_OFF      = 0x0000000200000000;
constexpr AttributeFlags ATTRIBUTE_SHARP_FUNC          = 0x0000000400000000;
constexpr AttributeFlags ATTRIBUTE_NO_DOC              = 0x0000000800000000;
constexpr AttributeFlags ATTRIBUTE_NO_COPY             = 0x0000001000000000;
constexpr AttributeFlags ATTRIBUTE_IMPLICIT            = 0x0000002000000000;
constexpr AttributeFlags ATTRIBUTE_DISCARDABLE         = 0x0000004000000000;
constexpr AttributeFlags ATTRIBUTE_DEPRECATED          = 0x0000008000000000;
constexpr AttributeFlags ATTRIBUTE_EXPORT_TYPE_METHODS = 0x0000010000000000;
constexpr AttributeFlags ATTRIBUTE_EXPORT_TYPE_NO_ZERO = 0x0000020000000000;
constexpr AttributeFlags ATTRIBUTE_RUN_GENERATED_FUNC  = 0x0000040000000000;
constexpr AttributeFlags ATTRIBUTE_RUN_GENERATED_EXP   = 0x0000080000000000;
constexpr AttributeFlags ATTRIBUTE_INCOMPLETE          = 0x0000100000000000;
constexpr AttributeFlags ATTRIBUTE_GEN                 = 0x0000200000000000;
constexpr AttributeFlags ATTRIBUTE_NO_INLINE           = 0x0000400000000000;
constexpr AttributeFlags ATTRIBUTE_MATCH_VALIDIFX_FUNC = 0x0000800000000000;
constexpr AttributeFlags ATTRIBUTE_CAN_OVERFLOW_ON     = 0x0001000000000000;
constexpr AttributeFlags ATTRIBUTE_CAN_OVERFLOW_OFF    = 0x0002000000000000;
constexpr AttributeFlags ATTRIBUTE_PRINT_GEN_BC        = 0x0004000000000000;
constexpr AttributeFlags ATTRIBUTE_INIT_FUNC           = 0x0008000000000000;
constexpr AttributeFlags ATTRIBUTE_NO_DUPLICATE        = 0x0010000000000000;
constexpr AttributeFlags ATTRIBUTE_OVERLOAD            = 0x0020000000000000;

constexpr AttributeFlags ATTRIBUTE_OPTIM_MASK    = ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF | ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF;
constexpr AttributeFlags ATTRIBUTE_ACCESS_MASK   = ATTRIBUTE_PUBLIC | ATTRIBUTE_INTERNAL | ATTRIBUTE_PRIVATE;
constexpr AttributeFlags ATTRIBUTE_MATCH_MASK    = ATTRIBUTE_MATCH_VALIDIF_OFF | ATTRIBUTE_MATCH_SELF_OFF;
constexpr AttributeFlags ATTRIBUTE_OVERFLOW_MASK = ATTRIBUTE_CAN_OVERFLOW_ON | ATTRIBUTE_CAN_OVERFLOW_OFF;

struct AttributeParameter
{
    Token         token;
    ComputedValue value;
    TypeInfo*     typeInfo;
};

struct OneAttribute
{
    const AttributeParameter* getParam(const Utf8& paramName) const;
    const ComputedValue*      getValue(const Utf8& paramName) const;

    Utf8                       name;
    Vector<AttributeParameter> parameters;
    AstNode*                   node  = nullptr;
    AstNode*                   child = nullptr;
    TypeInfoFuncAttr*          type  = nullptr;
};

struct AttributeList
{
    const OneAttribute*       getAttribute(const Utf8& fullName) const;
    void                      getAttributes(VectorNative<const OneAttribute*>& res, const Utf8& fullName) const;
    const AttributeParameter* getParam(const Utf8& fullName, const Utf8& parameter) const;
    const ComputedValue*      getValue(const Utf8& fullName, const Utf8& parameter) const;
    bool                      hasAttribute(const Utf8& fullName) const;
    void                      emplace(OneAttribute& other);
    void                      add(const AttributeList& other);

    void     reset() { allAttributes.clear(); }
    bool     empty() const { return allAttributes.empty(); }
    uint32_t size() const { return allAttributes.size(); }

    AttributeList& operator=(const AttributeList& other)
    {
        SWAG_RACE_CONDITION_WRITE(raceCond);
        SWAG_RACE_CONDITION_READ1(other.raceCond);
        allAttributes = other.allAttributes;
        return *this;
    }

    Vector<OneAttribute> allAttributes;

    SWAG_RACE_CONDITION_INSTANCE(raceCond);
};
