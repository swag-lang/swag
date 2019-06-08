#include "pch.h"
#include "TypeManager.h"

TypeManager g_TypeMgr;

void TypeManager::setup()
{
    typeInfoS8     = new TypeInfoNative(NativeType::S8, "s8", 1);
    typeInfoS16    = new TypeInfoNative(NativeType::S16, "s16", 2);
    typeInfoS32    = new TypeInfoNative(NativeType::S32, "s32", 4);
    typeInfoS64    = new TypeInfoNative(NativeType::S64, "s64", 8);
    typeInfoU8     = new TypeInfoNative(NativeType::U8, "u8", 1);
    typeInfoU16    = new TypeInfoNative(NativeType::U16, "u16", 2);
    typeInfoU32    = new TypeInfoNative(NativeType::U32, "u32", 4);
    typeInfoU64    = new TypeInfoNative(NativeType::U64, "u64", 8);
    typeInfoBool   = new TypeInfoNative(NativeType::Bool, "bool", 1);
    typeInfoF32    = new TypeInfoNative(NativeType::F32, "f32", 4);
    typeInfoF64    = new TypeInfoNative(NativeType::F64, "f64", 8);
    typeInfoChar   = new TypeInfoNative(NativeType::Char, "char", 1);
    typeInfoString = new TypeInfoNative(NativeType::String, "string", 4);
    typeInfoVoid   = new TypeInfoNative(NativeType::Void, "void", 0);

    promoteMatrix[(int) NativeType::U8][(int) NativeType::U8]  = typeInfoU32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::U16] = typeInfoU32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::U32] = typeInfoU32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::U64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::S8]  = typeInfoU32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::S16] = typeInfoU32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::S32] = typeInfoU32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::S64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::U8][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::U16][(int) NativeType::U8]  = typeInfoU32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::U16] = typeInfoU32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::U32] = typeInfoU32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::U64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::S8]  = typeInfoU32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::S16] = typeInfoU32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::S32] = typeInfoU32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::S64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::U16][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::U32][(int) NativeType::U8]  = typeInfoU32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::U16] = typeInfoU32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::U32] = typeInfoU32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::U64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::S8]  = typeInfoU32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::S16] = typeInfoU32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::S32] = typeInfoU32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::S64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::U32][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::U64][(int) NativeType::U8]  = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::U16] = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::U32] = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::U64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::S8]  = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::S16] = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::S32] = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::S64] = typeInfoU64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::F32] = typeInfoF64;
    promoteMatrix[(int) NativeType::U64][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::S8][(int) NativeType::U8]  = typeInfoS32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::U16] = typeInfoS32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::U32] = typeInfoS32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::U64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::S8]  = typeInfoS32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::S16] = typeInfoS32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::S32] = typeInfoS32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::S64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::S8][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::S16][(int) NativeType::U8]  = typeInfoS32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::U16] = typeInfoS32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::U32] = typeInfoS32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::U64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::S8]  = typeInfoS32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::S16] = typeInfoS32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::S32] = typeInfoS32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::S64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::S16][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::S32][(int) NativeType::U8]  = typeInfoS32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::U16] = typeInfoS32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::U32] = typeInfoS32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::U64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::S8]  = typeInfoS32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::S16] = typeInfoS32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::S32] = typeInfoS32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::S64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::S32][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::S64][(int) NativeType::U8]  = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::U16] = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::U32] = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::U64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::S8]  = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::S16] = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::S32] = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::S64] = typeInfoS64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::F32] = typeInfoF64;
    promoteMatrix[(int) NativeType::S64][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::F32][(int) NativeType::U8]  = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::U16] = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::U32] = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::U64] = typeInfoF64;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::S8]  = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::S16] = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::S32] = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::S64] = typeInfoF64;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::F32] = typeInfoF32;
    promoteMatrix[(int) NativeType::F32][(int) NativeType::F64] = typeInfoF64;

    promoteMatrix[(int) NativeType::F64][(int) NativeType::U8]  = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::U16] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::U32] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::U64] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::S8]  = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::S16] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::S32] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::S64] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::F32] = typeInfoF64;
    promoteMatrix[(int) NativeType::F64][(int) NativeType::F64] = typeInfoF64;
}

TypeInfo* TypeManager::flattenType(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Enum)
        return typeInfo;
    return static_cast<TypeInfoEnum*>(typeInfo)->rawType;
}

TypeInfo* TypeManager::concreteType(TypeInfo* typeInfo)
{
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        return typeInfo;
    case TypeInfoKind::FuncAttr:
        return concreteType(static_cast<TypeInfoFuncAttr*>(typeInfo)->returnType);
    case TypeInfoKind::Enum:
        return concreteType(static_cast<TypeInfoEnum*>(typeInfo)->rawType);
    }

    return typeInfo;
}