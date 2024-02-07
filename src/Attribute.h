#pragma once
#include "ComputedValue.h"
#include "RaceCondition.h"
#include "Tokenizer.h"

struct TypeInfo;

constexpr uint64_t ATTRIBUTE_CONSTEXPR           = 0x0000000000000001;
constexpr uint64_t ATTRIBUTE_PRINT_BC            = 0x0000000000000002;
constexpr uint64_t ATTRIBUTE_TEST_FUNC           = 0x0000000000000004;
constexpr uint64_t ATTRIBUTE_COMPILER            = 0x0000000000000008;
constexpr uint64_t ATTRIBUTE_PUBLIC              = 0x0000000000000010;
constexpr uint64_t ATTRIBUTE_INTERNAL            = 0x0000000000000020;
constexpr uint64_t ATTRIBUTE_PRIVATE             = 0x0000000000000040;
constexpr uint64_t ATTRIBUTE_DROP_FUNC           = 0x0000000000000080;
constexpr uint64_t ATTRIBUTE_RUN_FUNC            = 0x0000000000000100;
constexpr uint64_t ATTRIBUTE_MAIN_FUNC           = 0x0000000000000200;
constexpr uint64_t ATTRIBUTE_COMPLETE            = 0x0000000000000400;
constexpr uint64_t ATTRIBUTE_INLINE              = 0x0000000000000800;
constexpr uint64_t ATTRIBUTE_MATCH_VALIDIF_FUNC  = 0x0000000000001000;
constexpr uint64_t ATTRIBUTE_ENUM_FLAGS          = 0x0000000000002000;
constexpr uint64_t ATTRIBUTE_NOT_GENERIC         = 0x0000000000004000;
constexpr uint64_t ATTRIBUTE_MACRO               = 0x0000000000008000;
constexpr uint64_t ATTRIBUTE_MIXIN               = 0x0000000000010000;
constexpr uint64_t ATTRIBUTE_FOREIGN             = 0x0000000000020000;
constexpr uint64_t ATTRIBUTE_STRICT              = 0x0000000000040000;
constexpr uint64_t ATTRIBUTE_TLS                 = 0x0000000000080000;
constexpr uint64_t ATTRIBUTE_OPAQUE              = 0x0000000000100000;
constexpr uint64_t ATTRIBUTE_ENUM_INDEX          = 0x0000000000200000;
constexpr uint64_t ATTRIBUTE_AST_FUNC            = 0x0000000000400000;
constexpr uint64_t ATTRIBUTE_CALLEE_RETURN       = 0x0000000000800000;
constexpr uint64_t ATTRIBUTE_PREMAIN_FUNC        = 0x0000000001000000;
constexpr uint64_t ATTRIBUTE_OPTIM_BACKEND_ON    = 0x0000000002000000;
constexpr uint64_t ATTRIBUTE_OPTIM_BACKEND_OFF   = 0x0000000004000000;
constexpr uint64_t ATTRIBUTE_GLOBAL              = 0x0000000008000000;
constexpr uint64_t ATTRIBUTE_COMPILER_FUNC       = 0x0000000010000000;
constexpr uint64_t ATTRIBUTE_GENERATED_FUNC      = 0x0000000020000000;
constexpr uint64_t ATTRIBUTE_OPTIM_BYTECODE_ON   = 0x0000000040000000;
constexpr uint64_t ATTRIBUTE_OPTIM_BYTECODE_OFF  = 0x0000000080000000;
constexpr uint64_t ATTRIBUTE_MATCH_VALIDIF_OFF   = 0x0000000100000000;
constexpr uint64_t ATTRIBUTE_MATCH_SELF_OFF      = 0x0000000200000000;
constexpr uint64_t ATTRIBUTE_SHARP_FUNC          = 0x0000000400000000;
constexpr uint64_t ATTRIBUTE_NO_DOC              = 0x0000000800000000;
constexpr uint64_t ATTRIBUTE_NO_COPY             = 0x0000001000000000;
constexpr uint64_t ATTRIBUTE_IMPLICIT            = 0x0000002000000000;
constexpr uint64_t ATTRIBUTE_DISCARDABLE         = 0x0000004000000000;
constexpr uint64_t ATTRIBUTE_DEPRECATED          = 0x0000008000000000;
constexpr uint64_t ATTRIBUTE_EXPORT_TYPE_METHODS = 0x0000010000000000;
constexpr uint64_t ATTRIBUTE_EXPORT_TYPE_NO_ZERO = 0x0000020000000000;
constexpr uint64_t ATTRIBUTE_RUN_GENERATED_FUNC  = 0x0000040000000000;
constexpr uint64_t ATTRIBUTE_RUN_GENERATED_EXP   = 0x0000080000000000;
constexpr uint64_t ATTRIBUTE_INCOMPLETE          = 0x0000100000000000;
constexpr uint64_t ATTRIBUTE_GEN                 = 0x0000200000000000;
constexpr uint64_t ATTRIBUTE_NO_INLINE           = 0x0000400000000000;
constexpr uint64_t ATTRIBUTE_MATCH_VALIDIFX_FUNC = 0x0000800000000000;
constexpr uint64_t ATTRIBUTE_CAN_OVERFLOW_ON     = 0x0001000000000000;
constexpr uint64_t ATTRIBUTE_CAN_OVERFLOW_OFF    = 0x0002000000000000;
constexpr uint64_t ATTRIBUTE_PRINT_GEN_BC        = 0x0004000000000000;
constexpr uint64_t ATTRIBUTE_INIT_FUNC           = 0x0008000000000000;
constexpr uint64_t ATTRIBUTE_NO_DUPLICATE        = 0x0010000000000000;
constexpr uint64_t ATTRIBUTE_OVERLOAD            = 0x0020000000000000;

constexpr uint64_t ATTRIBUTE_OPTIM_MASK    = (ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF | ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
constexpr uint64_t ATTRIBUTE_ACCESS_MASK   = (ATTRIBUTE_PUBLIC | ATTRIBUTE_INTERNAL | ATTRIBUTE_PRIVATE);
constexpr uint64_t ATTRIBUTE_MATCH_MASK    = (ATTRIBUTE_MATCH_VALIDIF_OFF | ATTRIBUTE_MATCH_SELF_OFF);
constexpr uint64_t ATTRIBUTE_OVERFLOW_MASK = (ATTRIBUTE_CAN_OVERFLOW_ON | ATTRIBUTE_CAN_OVERFLOW_OFF);

struct TypeInfoFuncAttr;
struct AstNode;

struct AttributeParameter
{
    Token         token;
    ComputedValue value;
    TypeInfo*     typeInfo;
};

struct OneAttribute
{
    [[nodiscard]] const AttributeParameter* getParam(const Utf8& paramName) const;
    [[nodiscard]] const ComputedValue*      getValue(const Utf8& paramName) const;

    Utf8                       name;
    Vector<AttributeParameter> parameters;
    AstNode*                   node     = nullptr;
    AstNode*                   child    = nullptr;
    TypeInfoFuncAttr*          typeFunc = nullptr;
};

struct AttributeList
{
    OneAttribute*             getAttribute(const Utf8& fullName);
    void                      getAttributes(VectorNative<const OneAttribute*>& res, const Utf8& fullName) const;
    const AttributeParameter* getParam(const Utf8& fullName, const Utf8& parameter) const;
    const ComputedValue*      getValue(const Utf8& fullName, const Utf8& parameter) const;
    bool                      hasAttribute(const Utf8& fullName) const;
    void                      emplace(OneAttribute& other);
    void                      add(AttributeList& other);

    void reset()
    {
        allAttributes.clear();
    }

    [[nodiscard]] bool empty() const
    {
        return allAttributes.empty();
    }

    [[nodiscard]] uint32_t size() const
    {
        return (uint32_t) allAttributes.size();
    }

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
