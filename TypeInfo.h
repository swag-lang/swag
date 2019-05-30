#pragma once
#include "Pool.h"
#include "Utf8.h"
#include "TypeManager.h"
struct Scope;
struct TypeInfo;

enum class TypeInfoKind
{
    Native,
    Namespace,
    Enum,
    Function,
    Attribute,
};

enum class NativeType
{
    SX,
    UX,
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
    FX,
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
static const uint32_t TYPEINFO_ATTRIBUTE_FUNC = 0x00000000'00000008;
static const uint32_t TYPEINFO_ATTRIBUTE_VAR  = 0x00000000'00000010;

struct TypeInfo : public PoolElement
{
    virtual bool isSame(TypeInfo* from)
    {
        return kind == from->kind;
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

struct TypeInfoAttr : public TypeInfo
{
    TypeInfoAttr()
    {
        kind = TypeInfoKind::Attribute;
    }

    bool isSame(TypeInfo* from) override
    {
        if (kind != from->kind)
            return false;
        auto fromFunc = CastTypeInfo<TypeInfoAttr>(from, TypeInfoKind::Attribute);
        return TypeManager::match(parameters, fromFunc->parameters);
    }

    vector<TypeInfo*> parameters;
};

struct TypeInfoFunc : public TypeInfo
{
    TypeInfoFunc()
    {
        kind = TypeInfoKind::Function;
    }

    bool isSame(TypeInfo* from) override
    {
        if (kind != from->kind)
            return false;
        auto fromFunc = CastTypeInfo<TypeInfoFunc>(from, TypeInfoKind::Function);
        return TypeManager::match(parameters, fromFunc->parameters);
    }

    vector<TypeInfo*>  parameters;
    TypeInfo*          returnType;
    set<TypeInfoAttr*> attributes;
};
