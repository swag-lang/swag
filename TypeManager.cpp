#include "pch.h"
#include "TypeManager.h"

TypeManager g_TypeMgr;

void TypeManager::setup()
{
    typeInfoS8        = new TypeInfoNative(NativeTypeKind::S8, "s8", 1, TYPEINFO_INTEGER);
    typeInfoS16       = new TypeInfoNative(NativeTypeKind::S16, "s16", 2, TYPEINFO_INTEGER);
    typeInfoS32       = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER);
    typeInfoS64       = new TypeInfoNative(NativeTypeKind::S64, "s64", 8, TYPEINFO_INTEGER);
    typeInfoU8        = new TypeInfoNative(NativeTypeKind::U8, "u8", 1, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU16       = new TypeInfoNative(NativeTypeKind::U16, "u16", 2, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU32       = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU64       = new TypeInfoNative(NativeTypeKind::U64, "u64", 8, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoBool      = new TypeInfoNative(NativeTypeKind::Bool, "bool", 1, 0);
    typeInfoF32       = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT);
    typeInfoF64       = new TypeInfoNative(NativeTypeKind::F64, "f64", 8, TYPEINFO_FLOAT);
    typeInfoChar      = new TypeInfoNative(NativeTypeKind::Char, "char", 4, 0);
    typeInfoVoid      = new TypeInfoNative(NativeTypeKind::Void, "void", 0, 0);
    typeInfoString    = new TypeInfoNative(NativeTypeKind::String, "string", 2 * sizeof(Register), 0);
    typeInfoAny       = new TypeInfoNative(NativeTypeKind::Any, "any", 2 * sizeof(Register), 0);
    typeInfoUndefined = new TypeInfoNative(NativeTypeKind::Undefined, "?", 0, 0);

    typeInfoUntypedU64 = (TypeInfoNative*) typeInfoU64->clone();
    typeInfoUntypedU64->flags |= TYPEINFO_UNTYPED_INTEGER;

    typeInfoVariadic            = new TypeInfoVariadic();
    typeInfoVariadic->nakedName = "...";
    typeInfoVariadic->name      = "...";
    typeInfoVariadic->sizeOf    = 2 * sizeof(Register);

    typeInfoNull            = new TypeInfoPointer();
    typeInfoNull->nakedName = "null";
    typeInfoNull->name      = "null";
    typeInfoNull->sizeOf    = sizeof(Register);

    typeInfoPVoid              = new TypeInfoPointer();
    typeInfoPVoid->finalType   = typeInfoVoid;
    typeInfoPVoid->pointedType = typeInfoVoid;
    typeInfoPVoid->ptrCount    = 1;
    typeInfoPVoid->computeName();
    typeInfoPVoid->sizeOf = sizeof(Register);

    typeInfoConstPVoid              = new TypeInfoPointer();
    typeInfoConstPVoid->finalType   = typeInfoVoid;
    typeInfoConstPVoid->pointedType = typeInfoVoid;
    typeInfoConstPVoid->ptrCount    = 1;
    typeInfoConstPVoid->computeName();
    typeInfoConstPVoid->setConst();
    typeInfoConstPVoid->sizeOf = sizeof(Register);

    typeInfoCode = new TypeInfoCode();

    typeInfoOpCall             = new TypeInfoFuncAttr();
    typeInfoOpCall->returnType = typeInfoVoid;
    typeInfoOpCall->parameters.push_back(new TypeInfoParam());
    auto typePtr         = new TypeInfoPointer();
    typePtr->ptrCount    = 1;
    typePtr->finalType   = typeInfoVoid;
    typePtr->pointedType = typeInfoVoid;
    typePtr->sizeOf      = sizeof(void*);
    typePtr->computeName();
    typeInfoOpCall->parameters[0]->typeInfo = typePtr;
    typeInfoOpCall->computeName();

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

TypeInfo* TypeManager::concreteReferenceType(TypeInfo* typeInfo, uint32_t flags)
{
    typeInfo = concreteReference(typeInfo);
    typeInfo = concreteType(typeInfo, flags);
    return typeInfo;
}

TypeInfo* TypeManager::concreteReference(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Reference)
        return typeInfo;
    return static_cast<TypeInfoReference*>(typeInfo)->pointedType;
}

TypeInfo* TypeManager::concreteType(TypeInfo* typeInfo, uint32_t flags)
{
    if (!typeInfo)
        return typeInfo;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Native:
        return typeInfo;

    case TypeInfoKind::FuncAttr:
        if (flags & CONCRETE_FUNC)
        {
            auto returnType = static_cast<TypeInfoFuncAttr*>(typeInfo)->returnType;
            if (!returnType)
                return g_TypeMgr.typeInfoVoid;
            return concreteType(returnType, flags);
        }
        break;

    case TypeInfoKind::Enum:
        if (flags & CONCRETE_ENUM)
            return concreteType(static_cast<TypeInfoEnum*>(typeInfo)->rawType, flags);
        break;

    case TypeInfoKind::Alias:
        if (flags & CONCRETE_ALIAS)
            return concreteType(static_cast<TypeInfoAlias*>(typeInfo)->rawType, flags);
        break;

    case TypeInfoKind::Generic:
        if (flags & CONCRETE_GENERIC)
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

TypeInfoArray* TypeManager::convertTypeListToArray(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto      typeArray    = g_Allocator.alloc0<TypeInfoArray>();
    auto      orgTypeArray = typeArray;
    TypeInfo* finalType    = nullptr;

    while (true)
    {
        typeArray->pointedType = typeList->subTypes.front()->typeInfo;
        finalType              = typeArray->pointedType;
        typeArray->sizeOf      = typeList->sizeOf;
        typeArray->count       = (uint32_t) typeList->subTypes.size();
        typeArray->totalCount  = typeArray->count;
        if (isCompilerConstant)
            typeArray->flags |= TYPEINFO_CONST;
        if (typeArray->pointedType->kind != TypeInfoKind::TypeListArray)
            break;
        typeList               = CastTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListArray);
        typeArray->pointedType = g_Allocator.alloc0<TypeInfoArray>();
        typeArray              = (TypeInfoArray*) typeArray->pointedType;
    }

    // Compute all the type names
    typeArray = orgTypeArray;
    while (typeArray)
    {
        typeArray->finalType = finalType;
        typeArray->computeName();
        if (typeArray->pointedType->kind != TypeInfoKind::Array)
            break;
        typeArray = CastTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
    }

    orgTypeArray->sizeOf = orgTypeArray->finalType->sizeOf * orgTypeArray->totalCount;
    return orgTypeArray;
}

TypeInfoStruct* TypeManager::convertTypeListToStruct(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto typeStruct       = g_Allocator.alloc0<TypeInfoStruct>();
    typeStruct->nakedName = typeList->computeTupleName(context);

    typeStruct->fields.reserve((int) typeList->subTypes.size());
    for (int idx = 0; idx < typeList->subTypes.size(); idx++)
    {
        auto one = typeList->subTypes[idx];
        if (one->namedParam.empty())
            one->namedParam = format("item%d", idx);
        typeStruct->fields.push_back((TypeInfoParam*) one->clone());
    }

    typeStruct->name = typeStruct->nakedName;

    return typeStruct;
}