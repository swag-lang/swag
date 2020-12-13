#include "pch.h"
#include "TypeManager.h"
#include "Tokenizer.h"
#include "Workspace.h"

TypeManager g_TypeMgr;

static TypeInfo*                            g_LiteralTypeToType[(int) LiteralType::TT_MAX];
thread_local map<uint32_t, TypeInfoNative*> mapUntypedValuesI;
thread_local map<uint32_t, TypeInfoNative*> mapUntypedValuesB;
thread_local map<uint32_t, TypeInfoNative*> mapUntypedValuesF;

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

    typeInfoUInt = new TypeInfoNative(NativeTypeKind::UInt, "uint", 8, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoInt  = new TypeInfoNative(NativeTypeKind::Int, "int", 8, TYPEINFO_INTEGER);

    typeInfoUntypedInt = (TypeInfoNative*) typeInfoS32->clone();
    typeInfoUntypedInt->flags |= TYPEINFO_UNTYPED_INTEGER;
    typeInfoUntypedBinHexa = (TypeInfoNative*) typeInfoU32->clone();
    typeInfoUntypedBinHexa->flags |= TYPEINFO_UNTYPED_BINHEXA;
    typeInfoUntypedFloat = (TypeInfoNative*) typeInfoF32->clone();
    typeInfoUntypedFloat->flags |= TYPEINFO_UNTYPED_FLOAT;

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

    typeInfoCode      = new TypeInfoCode();
    typeInfoNameAlias = new TypeInfoNameAlias();

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

    memset(g_LiteralTypeToType, 0, sizeof(g_LiteralTypeToType));
    g_LiteralTypeToType[(int) LiteralType::TT_UINT]            = typeInfoUInt;
    g_LiteralTypeToType[(int) LiteralType::TT_INT]             = typeInfoInt;
    g_LiteralTypeToType[(int) LiteralType::TT_U8]              = typeInfoU8;
    g_LiteralTypeToType[(int) LiteralType::TT_U16]             = typeInfoU16;
    g_LiteralTypeToType[(int) LiteralType::TT_U32]             = typeInfoU32;
    g_LiteralTypeToType[(int) LiteralType::TT_U64]             = typeInfoU64;
    g_LiteralTypeToType[(int) LiteralType::TT_S8]              = typeInfoS8;
    g_LiteralTypeToType[(int) LiteralType::TT_S16]             = typeInfoS16;
    g_LiteralTypeToType[(int) LiteralType::TT_S32]             = typeInfoS32;
    g_LiteralTypeToType[(int) LiteralType::TT_S64]             = typeInfoS64;
    g_LiteralTypeToType[(int) LiteralType::TT_F32]             = typeInfoF32;
    g_LiteralTypeToType[(int) LiteralType::TT_F64]             = typeInfoF64;
    g_LiteralTypeToType[(int) LiteralType::TT_BOOL]            = typeInfoBool;
    g_LiteralTypeToType[(int) LiteralType::TT_CHAR]            = typeInfoChar;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING]          = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_VOID]            = typeInfoVoid;
    g_LiteralTypeToType[(int) LiteralType::TT_NULL]            = typeInfoNull;
    g_LiteralTypeToType[(int) LiteralType::TT_ANY]             = typeInfoAny;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_INT]     = typeInfoUntypedInt;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_FLOAT]   = typeInfoUntypedFloat;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_BINHEXA] = typeInfoUntypedBinHexa;
    g_LiteralTypeToType[(int) LiteralType::TT_TYPE]            = nullptr; // will be done with registerTypeType

    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U8]   = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U16]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U32]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::U64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::UInt] = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S8]   = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S16]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S32]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::S64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::Int]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::U8][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U8]   = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U16]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U32]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::U64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::UInt] = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S8]   = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S16]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S32]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::S64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::Int]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::U16][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U8]   = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U16]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U32]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::U64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::UInt] = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S8]   = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S16]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S32]  = typeInfoU32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::S64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::Int]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::U32][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U8]   = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U16]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U32]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::U64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::UInt] = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S8]   = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S16]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S32]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::S64]  = typeInfoU64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::Int]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::F32]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::U64][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::U8]   = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::U16]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::U32]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::U64]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::UInt] = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::S8]   = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::S16]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::S32]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::S64]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::Int]  = typeInfoUInt;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::F32]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::UInt][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U8]   = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U16]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U32]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::U64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::UInt] = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S8]   = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S16]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S32]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::S64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::Int]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::S8][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U8]   = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U16]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U32]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::U64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::UInt] = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S8]   = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S16]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S32]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::S64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::Int]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::S16][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U8]   = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U16]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U32]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::U64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::UInt] = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S8]   = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S16]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S32]  = typeInfoS32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::S64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::Int]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::S32][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U8]   = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U16]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U32]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::U64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::UInt] = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S8]   = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S16]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S32]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::S64]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::Int]  = typeInfoS64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::F32]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::S64][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::U8]   = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::U16]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::U32]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::U64]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::UInt] = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::S8]   = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::S16]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::S32]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::S64]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::Int]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::F32]  = typeInfoInt;
    promoteMatrix[(int) NativeTypeKind::Int][(int) NativeTypeKind::F64]  = typeInfoInt;

    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U8]   = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U16]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::U64]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::UInt] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S8]   = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S16]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::S64]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::Int]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::F32]  = typeInfoF32;
    promoteMatrix[(int) NativeTypeKind::F32][(int) NativeTypeKind::F64]  = typeInfoF64;

    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U8]   = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U16]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U32]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::U64]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::UInt] = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S8]   = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S16]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S32]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::S64]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::Int]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::F32]  = typeInfoF64;
    promoteMatrix[(int) NativeTypeKind::F64][(int) NativeTypeKind::F64]  = typeInfoF64;
}

TypeInfo* TypeManager::concreteReferenceType(TypeInfo* typeInfo, uint32_t flags)
{
    typeInfo = concreteReference(typeInfo);
    typeInfo = concreteType(typeInfo, flags);
    typeInfo = concreteReference(typeInfo);
    return typeInfo;
}

TypeInfo* TypeManager::concreteReference(TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Reference)
        return typeInfo;
    return CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference)->pointedType;
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
            auto returnType = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr)->returnType;
            if (!returnType)
                return g_TypeMgr.typeInfoVoid;
            return concreteType(returnType, flags);
        }
        break;

    case TypeInfoKind::Enum:
        if (flags & CONCRETE_ENUM)
            return concreteType(CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum)->rawType, flags);
        break;

    case TypeInfoKind::Alias:
        if (flags & (CONCRETE_ALIAS | CONCRETE_FORCEALIAS))
        {
            auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
            if (typeAlias->flags & TYPEINFO_STRICT && !(flags & CONCRETE_FORCEALIAS))
                return typeAlias;
            return concreteType(typeAlias->rawType, flags);
        }
        break;

    case TypeInfoKind::Generic:
        if (flags & CONCRETE_GENERIC)
        {
            auto typeGeneric = CastTypeInfo<TypeInfoGeneric>(typeInfo, TypeInfoKind::Generic);
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
    auto      typeArray    = allocType<TypeInfoArray>();
    auto      orgTypeArray = typeArray;
    TypeInfo* finalType    = nullptr;

    while (true)
    {
        typeArray->pointedType = solidifyUntyped(typeList->subTypes.front()->typeInfo);
        finalType              = typeArray->pointedType;
        typeArray->sizeOf      = typeList->sizeOf;
        typeArray->count       = (uint32_t) typeList->subTypes.size();
        typeArray->totalCount  = typeArray->count;
        if (isCompilerConstant)
            typeArray->flags |= TYPEINFO_CONST;
        if (typeArray->pointedType->kind != TypeInfoKind::TypeListArray)
            break;
        typeList               = CastTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListArray);
        typeArray->pointedType = allocType<TypeInfoArray>();
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
    auto typeStruct       = allocType<TypeInfoStruct>();
    typeStruct->nakedName = typeList->computeTupleName(context);

    typeStruct->fields.reserve((int) typeList->subTypes.size());
    for (int idx = 0; idx < typeList->subTypes.size(); idx++)
    {
        auto one = typeList->subTypes[idx];
        if (one->namedParam.empty())
            one->namedParam = format("item%d", idx);
        typeStruct->fields.push_back((TypeInfoParam*) one->clone());
    }

    typeStruct->name       = typeStruct->nakedName;
    typeStruct->structName = typeStruct->name;

    return typeStruct;
}

TypeInfo* TypeManager::solidifyUntyped(TypeInfo* typeInfo)
{
    if (typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
        return g_TypeMgr.typeInfoS32;
    if (typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
        return g_TypeMgr.typeInfoF32;
    return typeInfo;
}

TypeInfo* TypeManager::makeUntypedType(TypeInfo* typeInfo, uint32_t value)
{
    if (typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
    {
        auto it = mapUntypedValuesI.find(value);
        if (it != mapUntypedValuesI.end())
        {
            SWAG_ASSERT(it->second->flags & TYPEINFO_UNTYPED_INTEGER);
            SWAG_ASSERT(it->second->valueInteger == *(int32_t*) &value);
            return it->second;
        }

        TypeInfoNative* newType  = (TypeInfoNative*) typeInfo->clone();
        newType->valueInteger    = *(int32_t*) &value;
        typeInfo                 = newType;
        mapUntypedValuesI[value] = newType;
    }
    else if (typeInfo->flags & TYPEINFO_UNTYPED_BINHEXA)
    {
        auto it = mapUntypedValuesB.find(value);
        if (it != mapUntypedValuesB.end())
        {
            SWAG_ASSERT(it->second->flags & TYPEINFO_UNTYPED_BINHEXA);
            SWAG_ASSERT(it->second->valueInteger == *(int32_t*) &value);
            return it->second;
        }

        TypeInfoNative* newType  = (TypeInfoNative*) typeInfo->clone();
        newType->valueInteger    = *(int32_t*) &value;
        typeInfo                 = newType;
        mapUntypedValuesB[value] = newType;
    }
    else if (typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
    {
        auto it = mapUntypedValuesF.find(value);
        if (it != mapUntypedValuesF.end())
        {
            SWAG_ASSERT(it->second->flags & TYPEINFO_UNTYPED_FLOAT);
            SWAG_ASSERT(it->second->valueFloat == *(float*) &value);
            return it->second;
        }

        TypeInfoNative* newType  = (TypeInfoNative*) typeInfo->clone();
        newType->valueFloat      = *(float*) &value;
        typeInfo                 = newType;
        mapUntypedValuesF[value] = newType;
    }

    return typeInfo;
}

void TypeManager::registerTypeType()
{
    // Generate the alias for 'const *TypeInfo'
    typeInfoTypeType = allocType<TypeInfoPointer>();
    typeInfoTypeType->flags |= TYPEINFO_CONST;
    typeInfoTypeType->ptrCount    = 1;
    typeInfoTypeType->finalType   = g_Workspace.swagScope.regTypeInfo;
    typeInfoTypeType->pointedType = typeInfoTypeType->finalType;
    typeInfoTypeType->computeName();
    typeInfoTypeType->sizeOf                        = sizeof(void*);
    g_LiteralTypeToType[(int) LiteralType::TT_TYPE] = typeInfoTypeType;
}

TypeInfo* TypeManager::literalTypeToType(LiteralType literalType)
{
    SWAG_ASSERT(literalType < LiteralType::TT_MAX);
    auto result = g_LiteralTypeToType[(int) literalType];
    SWAG_ASSERT(result);
    return result;
}

TypeInfo* TypeManager::literalTypeToType(const Token& token)
{
    auto result = literalTypeToType(token.literalType);
    SWAG_ASSERT(result);
    result = makeUntypedType(result, token.literalValue.u32);
    return result;
}

TypeInfo* TypeManager::makeConst(TypeInfo* typeInfo)
{
    unique_lock lk(typeInfo->mutex);
    if (typeInfo->isConst())
        return typeInfo;

    TypeInfo* typeConst;
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeAlias = allocType<TypeInfoAlias>();
        typeAlias->copyFrom(typeInfo);
        typeAlias->rawType = typeInfo;
        typeAlias->flags |= TYPEINFO_CONST | TYPEINFO_FAKE_ALIAS;
        typeConst = typeAlias;
    }
    else
    {
        typeConst = typeInfo->clone();
        typeConst->setConst();
        typeConst->computeName();
    }

    return typeConst;
}

uint32_t TypeManager::alignOf(TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        return typeStruct->alignOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        return alignOf(typeArray->finalType);
    }
    else if (typeInfo->kind == TypeInfoKind::Slice ||
             typeInfo->kind == TypeInfoKind::Interface ||
             typeInfo->kind == TypeInfoKind::TypeSet ||
             typeInfo->kind == TypeInfoKind::Pointer ||
             typeInfo->kind == TypeInfoKind::Reference ||
             typeInfo->isNative(NativeTypeKind::Any) ||
             typeInfo->isNative(NativeTypeKind::String))
    {
        return sizeof(void*);
    }

    return typeInfo->sizeOf;
}