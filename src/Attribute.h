#pragma once
#include "Register.h"
#include "Runtime.h"
#include "Tokenizer.h"
#include "RaceCondition.h"
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
static const uint64_t ATTRIBUTE_NOT_GENERIC         = 0x0000000000004000;
static const uint64_t ATTRIBUTE_MACRO               = 0x0000000000008000;
static const uint64_t ATTRIBUTE_MIXIN               = 0x0000000000010000;
static const uint64_t ATTRIBUTE_PRIVATE             = 0x0000000000020000;
static const uint64_t ATTRIBUTE_STRICT              = 0x0000000000040000;
static const uint64_t ATTRIBUTE_TLS                 = 0x0000000000080000;
static const uint64_t ATTRIBUTE_OPAQUE              = 0x0000000000100000;
static const uint64_t ATTRIBUTE_ENUM_INDEX          = 0x0000000000200000;
static const uint64_t ATTRIBUTE_AST_FUNC            = 0x0000000000400000;
static const uint64_t ATTRIBUTE_NO_RETURN           = 0x0000000000800000;
static const uint64_t ATTRIBUTE_PREMAIN_FUNC        = 0x0000000001000000;
static const uint64_t ATTRIBUTE_OPTIM_BACKEND_ON    = 0x0000000002000000;
static const uint64_t ATTRIBUTE_OPTIM_BACKEND_OFF   = 0x0000000004000000;
static const uint64_t ATTRIBUTE_GLOBAL              = 0x0000000008000000;
static const uint64_t ATTRIBUTE_COMPILER_FUNC       = 0x0000000010000000;
static const uint64_t ATTRIBUTE_GENERATED_FUNC      = 0x0000000020000000;
static const uint64_t ATTRIBUTE_OPTIM_BYTECODE_ON   = 0x0000000040000000;
static const uint64_t ATTRIBUTE_OPTIM_BYTECODE_OFF  = 0x0000000080000000;
static const uint64_t ATTRIBUTE_SELECTIF_ON         = 0x0000000100000000;
static const uint64_t ATTRIBUTE_SELECTIF_OFF        = 0x0000000200000000;
static const uint64_t ATTRIBUTE_SHARP_FUNC          = 0x0000000400000000;
static const uint64_t ATTRIBUTE_SAFETY_OVERFLOW_OFF = 0x0000000800000000;
static const uint64_t ATTRIBUTE_NO_COPY             = 0x0000001000000000;
static const uint64_t ATTRIBUTE_IMPLICIT            = 0x0000002000000000;
static const uint64_t ATTRIBUTE_DISCARDABLE         = 0x0000004000000000;
static const uint64_t ATTRIBUTE_DEPRECATED          = 0x0000008000000000;
static const uint64_t ATTRIBUTE_EXPORT_TYPE_METHODS = 0x0000010000000000;
static const uint64_t ATTRIBUTE_EXPORT_TYPE_NOZERO  = 0x0000020000000000;
static const uint64_t ATTRIBUTE_RUN_GENERATED_FUNC  = 0x0000040000000000;
static const uint64_t ATTRIBUTE_RUN_GENERATED_EXP   = 0x0000080000000000;
static const uint64_t ATTRIBUTE_INCOMPLETE          = 0x0000100000000000;
static const uint64_t ATTRIBUTE_GEN                 = 0x0000200000000000;
static const uint64_t ATTRIBUTE_NO_INLINE           = 0x0000400000000000;

static const uint64_t ATTRIBUTE_OPTIM_MASK    = (ATTRIBUTE_OPTIM_BACKEND_ON | ATTRIBUTE_OPTIM_BACKEND_OFF | ATTRIBUTE_OPTIM_BYTECODE_ON | ATTRIBUTE_OPTIM_BYTECODE_OFF);
static const uint64_t ATTRIBUTE_SELECTIF_MASK = (ATTRIBUTE_SELECTIF_ON | ATTRIBUTE_SELECTIF_OFF);
static const uint64_t ATTRIBUTE_EXPOSE_MASK   = ATTRIBUTE_PUBLIC | ATTRIBUTE_PRIVATE;

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
    const AttributeParameter* getParam(const Utf8& paramName) const;
    const ComputedValue*      getValue(const Utf8& paramName) const;

    Utf8                       name;
    vector<AttributeParameter> parameters;
    AstNode*                   node     = nullptr;
    TypeInfoFuncAttr*          typeFunc = nullptr;
};

struct AttributeList
{
    OneAttribute*             getAttribute(const Utf8& fullName);
    void                      getAttributes(VectorNative<const OneAttribute*>& res, const Utf8& fullName);
    const AttributeParameter* getParam(const Utf8& fullName, const Utf8& parameter);
    const ComputedValue*      getValue(const Utf8& fullName, const Utf8& parameter);
    bool                      hasAttribute(const Utf8& fullName);
    void                      emplace(OneAttribute& other);
    void                      add(AttributeList& other);

    void reset()
    {
        allAttributes.clear();
    }

    bool empty() const
    {
        return allAttributes.empty();
    }

    uint32_t size() const
    {
        return (uint32_t) allAttributes.size();
    }

    void operator=(const AttributeList& other)
    {
        SWAG_RACE_CONDITION_WRITE(raceCond);
        SWAG_RACE_CONDITION_READ1(other.raceCond);
        allAttributes = other.allAttributes;
    }

    vector<OneAttribute> allAttributes;
    SWAG_RACE_CONDITION_INSTANCE(raceCond);
};
