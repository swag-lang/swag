#include "pch.h"
#include "TypeManager.h"

TypeManager g_TypeMgr;

void TypeManager::setup()
{
    typeInfoS8     = new TypeInfoNative(NativeTypeKind::S8, "s8", 1, TYPEINFO_INTEGER);
    typeInfoS16    = new TypeInfoNative(NativeTypeKind::S16, "s16", 2, TYPEINFO_INTEGER);
    typeInfoS32    = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER);
    typeInfoS64    = new TypeInfoNative(NativeTypeKind::S64, "s64", 8, TYPEINFO_INTEGER);
    typeInfoU8     = new TypeInfoNative(NativeTypeKind::U8, "u8", 1, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU16    = new TypeInfoNative(NativeTypeKind::U16, "u16", 2, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU32    = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU64    = new TypeInfoNative(NativeTypeKind::U64, "u64", 8, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoBool   = new TypeInfoNative(NativeTypeKind::Bool, "bool", 1, 0);
    typeInfoF32    = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT);
    typeInfoF64    = new TypeInfoNative(NativeTypeKind::F64, "f64", 8, TYPEINFO_FLOAT);
    typeInfoChar   = new TypeInfoNative(NativeTypeKind::Char, "char", 4, 0);
    typeInfoVoid   = new TypeInfoNative(NativeTypeKind::Void, "void", 0, 0);
    typeInfoString = new TypeInfoNative(NativeTypeKind::String, "string", 2 * sizeof(Register), 0);

    typeInfoVariadic = new TypeInfoVariadic();
    typeInfoVariadic->reset();
    typeInfoVariadic->name   = "...";
    typeInfoVariadic->sizeOf = 2 * sizeof(Register);

    typeInfoVariadicValue = new TypeInfoVariadic();
    typeInfoVariadicValue->reset();
    typeInfoVariadicValue->kind   = TypeInfoKind::VariadicValue;
    typeInfoVariadicValue->name   = "...";
    typeInfoVariadicValue->sizeOf = 0;

    typeInfoNull = new TypeInfoPointer();
    typeInfoNull->reset();
    typeInfoNull->name   = "null";
    typeInfoNull->sizeOf = sizeof(Register);

    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U8]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U16] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U32] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S8]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S16] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S32] = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U8]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U16] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U32] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S8]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S16] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S32] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S64] = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U8]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U16] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U32] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S8]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S16] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S32] = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U8]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U16] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U32] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S8]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S16] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S32] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S64] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U8]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U16] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U64] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S8]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S16] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S64] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::F32] = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::F64] = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U8]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U16] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U32] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U64] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S8]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S16] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S32] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S64] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::F32] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::F64] = typeInfoF64;
}

TypeInfo* TypeManager::concreteType(TypeInfo* typeInfo, MakeConcrete flags)
{
    if (!typeInfo)
        return typeInfo;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        return typeInfo;

    case TypeInfoKind::FuncAttr:
        if (flags & MakeConcrete::FlagFunc)
        {
            auto returnType = static_cast<TypeInfoFuncAttr*>(typeInfo)->returnType;
            if (!returnType)
                return g_TypeMgr.typeInfoVoid;
            return concreteType(returnType, flags);
        }
        break;

    case TypeInfoKind::Enum:
        if (flags & MakeConcrete::FlagEnum)
            return concreteType(static_cast<TypeInfoEnum*>(typeInfo)->rawType, flags);
        break;

    case TypeInfoKind::Alias:
        if (flags & MakeConcrete::FlagAlias)
            return concreteType(static_cast<TypeInfoAlias*>(typeInfo)->rawType, flags);
        break;

    case TypeInfoKind::Generic:
        if (flags & MakeConcrete::FlagGeneric)
        {
            auto typeGeneric = static_cast<TypeInfoGeneric*>(typeInfo);
            if (!typeGeneric->rawType)
                return typeGeneric;
            return concreteType(typeGeneric->rawType, flags);
        }
        break;
    }

    return typeInfo;
}
