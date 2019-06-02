#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "SpinLock.h"
struct Scope;
struct TypeInfo;
struct SymbolMatchContext;
struct Job;
enum class Intrisic;

enum class TypeInfoKind
{
    Native,
    Namespace,
    Enum,
    FunctionAttribute,
};

enum class NativeType
{
    S8,
    S16,
    S32,
    S64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    Bool,
    Char,
    String,
    Void,
};

template<typename T>
inline T* CastTypeInfo(TypeInfo* ptr, TypeInfoKind kind)
{
    T* casted = static_cast<T*>(ptr);
    assert(casted->kind == kind);
    return casted;
}

static const uint64_t TYPEINFO_INT_SIGNED     = 0x00000000'00000001;
static const uint64_t TYPEINFO_INT_UNSIGNED   = 0x00000000'00000002;
static const uint64_t TYPEINFO_FLOAT          = 0x00000000'00000004;
static const uint64_t TYPEINFO_ATTRIBUTE_FUNC = 0x00000000'00000008;
static const uint64_t TYPEINFO_ATTRIBUTE_VAR  = 0x00000000'00000010;

struct TypeInfo : public PoolElement
{
    virtual bool isSame(TypeInfo* from)
    {
        return this == from;
    }

    uint64_t     flags;
    TypeInfoKind kind;
    NativeType   nativeType;
    Utf8         name;
};

struct TypeInfoNative : public TypeInfo
{
    TypeInfoNative(NativeType type, const char* tname, uint64_t tflags)
    {
        kind       = TypeInfoKind::Native;
        nativeType = type;
        name       = tname;
        flags      = tflags;
    }
};

struct TypeInfoNamespace : public TypeInfo
{
    Scope* scope;

    TypeInfoNamespace()
    {
        kind = TypeInfoKind::Namespace;
    }
};

struct TypeInfoEnum : public TypeInfo
{
    Scope*    scope;
    TypeInfo* rawType = nullptr;

    TypeInfoEnum()
    {
        kind = TypeInfoKind::Enum;
    }
};

struct TypeInfoFuncAttrParam : public PoolElement
{
    Utf8      name;
    TypeInfo* typeInfo;
};

enum MatchResult
{
    Ok,
    TooManyParameters,
    NotEnoughParameters,
    BadSignature
};

struct SymbolMatchParameter : public PoolElement
{
    Utf8      name;
    TypeInfo* typeInfo;
};

struct SymbolMatchContext
{
    int                           badSignatureParameterIdx  = 0;
    TypeInfo*                     basSignatureRequestedType = nullptr;
    TypeInfo*                     basSignatureGivenType     = nullptr;
    MatchResult                   result                    = MatchResult::Ok;
    vector<SymbolMatchParameter*> parameters;

    ~SymbolMatchContext()
    {
        for (auto param : parameters)
            param->release();
    }
};

struct TypeInfoFuncAttr : public TypeInfo
{
    TypeInfoFuncAttr()
    {
        kind = TypeInfoKind::FunctionAttribute;
    }

    void match(SymbolMatchContext& context);
    bool isSame(TypeInfoFuncAttr* from);
    bool isSame(TypeInfo* from) override;

    int                            firstDefaultValueIdx = -1;
    vector<TypeInfoFuncAttrParam*> parameters;
    TypeInfo*                      returnType;
    set<TypeInfoFuncAttr*>         attributes;
    Intrisic                       intrinsic;
};
