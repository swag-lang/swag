#pragma once
#include "Pool.h"
struct Scope;

enum class TypeInfoKind
{
    NativeType,
    Namespace,
    Enum,
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
};

static const uint64_t TYPEINFO_INT_SIGNED   = 0x00000000'00000001;
static const uint64_t TYPEINFO_INT_UNSIGNED = 0x00000000'00000002;
static const uint64_t TYPEINFO_FLOAT        = 0x00000000'00000004;

struct TypeInfo : public PoolElement
{
    uint64_t     flags;
    TypeInfoKind kind;
    NativeType   nativeType;
};

struct TypeInfoNative : public TypeInfo
{
    TypeInfoNative(NativeType type, uint64_t tflags)
    {
        kind       = TypeInfoKind::NativeType;
        nativeType = type;
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

extern TypeInfoNative g_TypeInfoSX;
extern TypeInfoNative g_TypeInfoS8;
extern TypeInfoNative g_TypeInfoS16;
extern TypeInfoNative g_TypeInfoS32;
extern TypeInfoNative g_TypeInfoS64;
extern TypeInfoNative g_TypeInfoUX;
extern TypeInfoNative g_TypeInfoU8;
extern TypeInfoNative g_TypeInfoU16;
extern TypeInfoNative g_TypeInfoU32;
extern TypeInfoNative g_TypeInfoU64;
extern TypeInfoNative g_TypeInfoBool;
extern TypeInfoNative g_TypeInfoF32;
extern TypeInfoNative g_TypeInfoF64;
extern TypeInfoNative g_TypeInfoFX;
extern TypeInfoNative g_TypeInfoChar;
extern TypeInfoNative g_TypeInfoString;