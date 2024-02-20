#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Job.h"
#include "Scope.h"
#include "SourceFile.h"
#include "Workspace.h"

TypeManager* g_TypeMgr = nullptr;
TypeInfo*    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeMax)];

thread_local Map<uint32_t, TypeInfoNative*> g_MapUntypedValuesI;
thread_local Map<uint32_t, TypeInfoNative*> g_MapUntypedValuesB;
thread_local Map<uint32_t, TypeInfoNative*> g_MapUntypedValuesF;

void TypeManager::setup()
{
    typeInfoS8            = new TypeInfoNative(NativeTypeKind::S8, "s8", 1, TYPEINFO_INTEGER | TYPEINFO_CAN_PROMOTE_816);
    typeInfoS16           = new TypeInfoNative(NativeTypeKind::S16, "s16", 2, TYPEINFO_INTEGER | TYPEINFO_CAN_PROMOTE_816);
    typeInfoS32           = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER);
    typeInfoS64           = new TypeInfoNative(NativeTypeKind::S64, "s64", 8, TYPEINFO_INTEGER);
    typeInfoU8            = new TypeInfoNative(NativeTypeKind::U8, "u8", 1, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_CAN_PROMOTE_816);
    typeInfoU16           = new TypeInfoNative(NativeTypeKind::U16, "u16", 2, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_CAN_PROMOTE_816);
    typeInfoU32           = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU64           = new TypeInfoNative(NativeTypeKind::U64, "u64", 8, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoBool          = new TypeInfoNative(NativeTypeKind::Bool, "bool", 1, 0);
    typeInfoF32           = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT);
    typeInfoF64           = new TypeInfoNative(NativeTypeKind::F64, "f64", 8, TYPEINFO_FLOAT);
    typeInfoRune          = new TypeInfoNative(NativeTypeKind::Rune, "rune", 4, 0);
    typeInfoVoid          = new TypeInfoNative(NativeTypeKind::Void, "void", 0, 0);
    typeInfoString        = new TypeInfoNative(NativeTypeKind::String, "string", 2 * sizeof(Register), 0);
    typeInfoAny           = new TypeInfoNative(NativeTypeKind::Any, "any", 2 * sizeof(Register), 0);
    typeInfoUndefined     = new TypeInfoNative(NativeTypeKind::Undefined, "?", 0, 0);
    typeInfoUntypedInt    = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER | TYPEINFO_UNTYPED_INTEGER);
    typeInfoUntypedBinHex = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_UNTYPED_BIN_HEX);
    typeInfoUntypedFloat  = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT | TYPEINFO_UNTYPED_FLOAT);
    typeInfoCharacter     = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER | TYPEINFO_UNTYPED_INTEGER | TYPEINFO_CHARACTER);

    typeInfoVariadic = new TypeInfoVariadic();
    typeInfoVariadic->name.setView("...", 3);
    typeInfoVariadic->sizeOf = 2 * sizeof(Register);

    typeInfoCVariadic       = new TypeInfoVariadic();
    typeInfoCVariadic->kind = TypeInfoKind::CVariadic;
    typeInfoCVariadic->name.setView("cvarargs", 8);
    typeInfoCVariadic->addFlag(TYPEINFO_C_VARIADIC);
    typeInfoCVariadic->sizeOf = 0;

    // Some default pointers
    typeInfoNull = new TypeInfoPointer();
    typeInfoNull->name.setView("null", 4);
    typeInfoNull->sizeOf = sizeof(Register);

    typeInfoCString         = new TypeInfoPointer();
    typeInfoCString->sizeOf = sizeof(Register);
    typeInfoCString->setConst();
    typeInfoCString->addFlag(TYPEINFO_C_STRING);
    typeInfoCString->pointedType = typeInfoU8;
    typeInfoCString->name.setView("cstring", 7);

    // The rest
    typeInfoCode = new TypeInfoCode();

    typeInfoOpCall             = new TypeInfoFuncAttr();
    typeInfoOpCall->returnType = typeInfoVoid;
    typeInfoOpCall->parameters.push_back(new TypeInfoParam());
    const auto typePtr   = new TypeInfoPointer();
    typePtr->pointedType = typeInfoVoid;
    typePtr->sizeOf      = sizeof(void*);
    typePtr->computeName();
    typeInfoOpCall->parameters[0]->typeInfo = typePtr;
    typeInfoOpCall->computeName();

    typeInfoOpCall2             = new TypeInfoFuncAttr();
    typeInfoOpCall2->returnType = typeInfoVoid;
    typeInfoOpCall2->parameters.push_back(new TypeInfoParam());
    typeInfoOpCall2->parameters.push_back(new TypeInfoParam());
    typeInfoOpCall2->parameters[0]->typeInfo = typePtr;
    typeInfoOpCall2->parameters[1]->typeInfo = typePtr;
    typeInfoOpCall2->computeName();

    typeInfoModuleCall             = new TypeInfoFuncAttr();
    typeInfoModuleCall->returnType = typeInfoVoid;
    typeInfoModuleCall->parameters.push_back(new TypeInfoParam());
    typeInfoModuleCall->parameters[0]->typeInfo = typePtr;
    typeInfoModuleCall->computeName();

    typeInfoSliceRunes              = new TypeInfoSlice();
    typeInfoSliceRunes->pointedType = typeInfoRune;
    typeInfoSliceRunes->addFlag(TYPEINFO_CONST);
    typeInfoSliceRunes->computeName();

    memset(g_LiteralTypeToType, 0, sizeof(g_LiteralTypeToType));
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUnsigned8)]             = typeInfoU8;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUnsigned16)]            = typeInfoU16;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUnsigned32)]            = typeInfoU32;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUnsigned64)]            = typeInfoU64;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeSigned8)]               = typeInfoS8;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeSigned16)]              = typeInfoS16;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeSigned32)]              = typeInfoS32;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeSigned64)]              = typeInfoS64;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeFloat32)]               = typeInfoF32;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeFloat64)]               = typeInfoF64;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeBool)]                  = typeInfoBool;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeRune)]                  = typeInfoRune;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeString)]                = typeInfoString;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeStringRaw)]             = typeInfoString;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeStringMultiLine)]       = typeInfoString;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeStringEscape)]          = typeInfoString;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeStringMultiLineEscape)] = typeInfoString;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeCharacter)]             = typeInfoCharacter;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeCharacterEscape)]       = typeInfoCharacter;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeVoid)]                  = typeInfoVoid;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeNull)]                  = typeInfoNull;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeAny)]                   = typeInfoAny;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUntypedInt)]            = typeInfoUntypedInt;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUntypedFloat)]          = typeInfoUntypedFloat;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeUntypedBinHexa)]        = typeInfoUntypedBinHex;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeCString)]               = typeInfoCString;
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeType)]                  = nullptr; // will be done with registerTypeType

    // Promotion matrix
#define PR(__a, __b, __c)                                                          \
    do                                                                             \
    {                                                                              \
        promoteMatrix[(int) NativeTypeKind::__a][(int) NativeTypeKind::__b] = __c; \
        promoteMatrix[(int) NativeTypeKind::__b][(int) NativeTypeKind::__a] = __c; \
    } while (0)

    PR(U8, U8, typeInfoU8);
    PR(U8, U16, typeInfoU16);
    PR(U8, U32, typeInfoU32);
    PR(U8, U64, typeInfoU64);
    PR(U8, S8, typeInfoS8);
    PR(U8, S16, typeInfoS16);
    PR(U8, S32, typeInfoS32);
    PR(U8, S64, typeInfoS64);
    PR(U8, F32, typeInfoF32);
    PR(U8, F64, typeInfoF64);

    PR(U16, U16, typeInfoU16);
    PR(U16, U32, typeInfoU32);
    PR(U16, U64, typeInfoU64);
    PR(U16, S8, typeInfoU16);
    PR(U16, S16, typeInfoS16);
    PR(U16, S32, typeInfoS32);
    PR(U16, S64, typeInfoS64);
    PR(U16, F32, typeInfoF32);
    PR(U16, F64, typeInfoF64);

    PR(U32, U32, typeInfoU32);
    PR(U32, U64, typeInfoU64);
    PR(U32, S8, typeInfoU32);
    PR(U32, S16, typeInfoU32);
    PR(U32, S32, typeInfoS32);
    PR(U32, S64, typeInfoS64);
    PR(U32, F32, typeInfoF32);
    PR(U32, F64, typeInfoF64);

    PR(U64, U64, typeInfoU64);
    PR(U64, S8, typeInfoU64);
    PR(U64, S16, typeInfoU64);
    PR(U64, S32, typeInfoU64);
    PR(U64, S64, typeInfoS64);
    PR(U64, F32, typeInfoF64);
    PR(U64, F64, typeInfoF64);

    PR(S8, S8, typeInfoS8);
    PR(S8, S16, typeInfoS16);
    PR(S8, S32, typeInfoS32);
    PR(S8, S64, typeInfoS64);
    PR(S8, F32, typeInfoF32);
    PR(S8, F64, typeInfoF64);

    PR(S16, S16, typeInfoS16);
    PR(S16, S32, typeInfoS32);
    PR(S16, S64, typeInfoS64);
    PR(S16, F32, typeInfoF32);
    PR(S16, F64, typeInfoF64);

    PR(S32, S32, typeInfoS32);
    PR(S32, S64, typeInfoS64);
    PR(S32, F32, typeInfoF32);
    PR(S32, F64, typeInfoF64);

    PR(S64, S64, typeInfoS64);
    PR(S64, F32, typeInfoF64);
    PR(S64, F64, typeInfoF64);

    PR(F32, F32, typeInfoF32);
    PR(F32, F64, typeInfoF64);

    PR(F64, F64, typeInfoF64);
}

void TypeManager::convertStructParamToRef(AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(node->kind == AstNodeKind::FuncDeclParam);

    // A struct/interface is forced to be a const reference
    if (!node->typeInfo->isGeneric() && typeInfo->isStruct())
    {
        const auto typeRef   = makeType<TypeInfoPointer>();
        typeInfo             = typeInfo->getConstAlias();
        typeRef->flags       = typeInfo->flags | TYPEINFO_CONST | TYPEINFO_POINTER_REF | TYPEINFO_POINTER_AUTO_REF;
        typeRef->pointedType = typeInfo;
        typeRef->sizeOf      = sizeof(void*);
        typeRef->computeName();
        node->typeInfo = typeRef;
    }
}

TypeInfoArray* TypeManager::convertTypeListToArray(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto       typeArray    = makeType<TypeInfoArray>();
    const auto orgTypeArray = typeArray;
    TypeInfo*  finalType    = nullptr;

    while (true)
    {
        typeArray->pointedType = solidifyUntyped(typeList->subTypes.front()->typeInfo);
        finalType              = typeArray->pointedType;
        typeArray->sizeOf      = typeList->sizeOf;
        typeArray->count       = typeList->subTypes.size();
        typeArray->totalCount  = typeArray->count;
        if (isCompilerConstant)
            typeArray->addFlag(TYPEINFO_CONST);

        if (typeArray->pointedType->isListTuple())
        {
            typeList               = castTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListTuple);
            typeArray->pointedType = convertTypeListToStruct(context, typeList, isCompilerConstant);
            finalType              = typeArray->pointedType;
            break;
        }

        if (typeArray->pointedType->isListArray())
        {
            typeList               = castTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListArray);
            typeArray->pointedType = makeType<TypeInfoArray>();
            typeArray              = castTypeInfo<TypeInfoArray>(typeArray->pointedType);
            continue;
        }

        break;
    }

    // Compute all the type names
    typeArray = orgTypeArray;
    while (typeArray)
    {
        typeArray->finalType = finalType;
        typeArray->computeName();
        if (!typeArray->pointedType->isArray())
            break;
        typeArray = castTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
    }

    orgTypeArray->sizeOf = orgTypeArray->finalType->sizeOf * orgTypeArray->totalCount;
    return orgTypeArray;
}

TypeInfoStruct* TypeManager::convertTypeListToStruct(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    const auto typeStruct = makeType<TypeInfoStruct>();

    Utf8 name = context->sourceFile->scopeFile->name + "_tpl4_";
    name += FMT("%d", g_UniqueID.fetch_add(1));
    typeStruct->name = name;

    typeStruct->addFlag(TYPEINFO_STRUCT_IS_TUPLE | TYPEINFO_GHOST_TUPLE);

    typeStruct->fields.reserve(static_cast<int>(typeList->subTypes.size()));
    for (size_t idx = 0; idx < typeList->subTypes.size(); idx++)
    {
        const auto one       = typeList->subTypes[idx];
        auto       typeParam = one->clone();
        typeParam->offset    = typeStruct->sizeOf;

        if (typeParam->name.empty())
        {
            typeParam->name = FMT("item%u", idx);
            typeParam->flags |= TYPEINFOPARAM_AUTO_NAME;
        }

        typeStruct->sizeOf += one->typeInfo->sizeOf;
        typeStruct->fields.push_back(typeParam);
    }

    typeStruct->structName = typeStruct->name;
    typeStruct->declNode   = typeList->declNode;

    Ast::convertTypeStructToStructDecl(context, typeStruct);
    return typeStruct;
}

TypeInfo* TypeManager::solidifyUntyped(TypeInfo* typeInfo)
{
    if (typeInfo->isUntypedInteger())
        return g_TypeMgr->typeInfoS32;
    if (typeInfo->isUntypedFloat())
        return g_TypeMgr->typeInfoF32;
    return typeInfo;
}

TypeInfo* TypeManager::resolveUntypedType(TypeInfo* typeInfo, uint32_t value)
{
    if (typeInfo->isUntypedInteger())
    {
        const auto it = g_MapUntypedValuesI.find(value);
        if (it != g_MapUntypedValuesI.end())
        {
            SWAG_ASSERT(it->second->isUntypedInteger());
            SWAG_ASSERT(it->second->valueInteger == *reinterpret_cast<int32_t*>(&value));
            return it->second;
        }

        const auto newType         = castTypeInfo<TypeInfoNative>(typeInfo->clone());
        newType->valueInteger      = *reinterpret_cast<int32_t*>(&value);
        typeInfo                   = newType;
        g_MapUntypedValuesI[value] = newType;
    }
    else if (typeInfo->isUntypedBinHex())
    {
        const auto it = g_MapUntypedValuesB.find(value);
        if (it != g_MapUntypedValuesB.end())
        {
            SWAG_ASSERT(it->second->isUntypedBinHex());
            SWAG_ASSERT(it->second->valueInteger == *reinterpret_cast<int32_t*>(&value));
            return it->second;
        }

        const auto newType         = castTypeInfo<TypeInfoNative>(typeInfo->clone());
        newType->valueInteger      = *reinterpret_cast<int32_t*>(&value);
        typeInfo                   = newType;
        g_MapUntypedValuesB[value] = newType;
    }
    else if (typeInfo->isUntypedFloat())
    {
        const auto it = g_MapUntypedValuesF.find(value);
        if (it != g_MapUntypedValuesF.end())
        {
            SWAG_ASSERT(it->second->isUntypedFloat());
            SWAG_ASSERT(it->second->valueFloat == *reinterpret_cast<float*>(&value));
            return it->second;
        }

        const auto newType         = castTypeInfo<TypeInfoNative>(typeInfo->clone());
        newType->valueFloat        = *reinterpret_cast<float*>(&value);
        typeInfo                   = newType;
        g_MapUntypedValuesF[value] = newType;
    }

    return typeInfo;
}

TypeInfo* TypeManager::literalTypeToType(LiteralType literalType)
{
    SWAG_ASSERT(literalType < LiteralType::TypeMax);
    const auto result = g_LiteralTypeToType[static_cast<int>(literalType)];
    SWAG_ASSERT(result);
    return result;
}

TypeInfo* TypeManager::literalTypeToType(LiteralType literalType, Register literalValue)
{
    auto result = literalTypeToType(literalType);
    SWAG_ASSERT(result);
    result = resolveUntypedType(result, literalValue.u32);
    return result;
}

uint64_t TypeManager::align(uint64_t value, uint32_t align)
{
    SWAG_ASSERT(align);
    if (align == 1)
        return value;
    auto alignValue = value & ~(static_cast<uint64_t>(align) - 1);
    if (alignValue < value)
        alignValue += align;
    return alignValue;
}

uint32_t TypeManager::alignOf(TypeInfo* typeInfo)
{
    if (typeInfo->isStruct())
    {
        const auto typeStruct = castTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct->sizeOf);
        SWAG_ASSERT(typeStruct->alignOf);
        return typeStruct->alignOf;
    }

    if (typeInfo->isArray())
    {
        const auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        return alignOf(typeArray->finalType);
    }

    if (typeInfo->isPointer())
    {
        return typeInfo->sizeOf;
    }

    if (typeInfo->isSlice() ||
        typeInfo->isInterface() ||
        typeInfo->isAny() ||
        typeInfo->isString())
    {
        return sizeof(void*);
    }

    return max(1, typeInfo->sizeOf);
}

void TypeManager::registerTypeType()
{
    typeInfoTypeType                                             = makePointerTo(g_Workspace->swagScope.regTypeInfo, TYPEINFO_CONST);
    g_LiteralTypeToType[static_cast<int>(LiteralType::TypeType)] = typeInfoTypeType;
}

TypeInfo* TypeManager::makeUnConst(TypeInfo* typeInfo)
{
    if (!typeInfo->isConst())
        return typeInfo;
    if (typeInfo->isConstAlias())
        return typeInfo->getConstAlias();

    ScopedLock lk(mutex);

    const auto it = mapUnConst.find(typeInfo);
    if (it != mapUnConst.end())
        return it->second;

    const auto typeUnConst = typeInfo->clone();
    typeUnConst->removeFlag(TYPEINFO_CONST);
    typeUnConst->forceComputeName();
    mapUnConst[typeInfo] = typeUnConst;

    return typeUnConst;
}

TypeInfo* TypeManager::makeConst(TypeInfo* typeInfo)
{
    if (typeInfo->isConst())
        return typeInfo;

    ScopedLock lk(mutex);

    const auto it = mapConst.find(typeInfo);
    if (it != mapConst.end())
        return it->second;

    if (typeInfo->isStruct())
    {
        const auto typeAlias = makeType<TypeInfoAlias>();
        typeAlias->copyFrom(typeInfo);
        typeAlias->rawType = typeInfo;
        typeAlias->addFlag(TYPEINFO_CONST | TYPEINFO_CONST_ALIAS);
        mapConst[typeInfo] = typeAlias;
        return typeAlias;
    }

    const auto typeConst = typeInfo->clone();
    typeConst->setConst();
    mapConst[typeInfo] = typeConst;
    return typeConst;
}

TypeInfoPointer* TypeManager::makePointerTo(TypeInfo* toType, TypeInfoFlags ptrFlags)
{
    ScopedLock lk(mutex);

    const auto it = mapPointers.find(toType);
    if (it != mapPointers.end())
    {
        for (const auto& it1 : it->second)
        {
            if (it1.flags == ptrFlags)
                return it1.pointerTo;
        }
    }

    const auto ptrType   = makeType<TypeInfoPointer>();
    ptrType->pointedType = toType;
    ptrType->sizeOf      = sizeof(Register);
    ptrType->flags       = ptrFlags;
    ptrType->computeName();

    if (it == mapPointers.end())
        mapPointers[toType] = {{ptrFlags, ptrType}};
    else
        mapPointers[toType].push_back({ptrFlags, ptrType});

    return ptrType;
}

TypeInfoParam* TypeManager::makeParam()
{
    const auto typeParam = Allocator::alloc<TypeInfoParam>();
#ifdef SWAG_STATS
    g_Stats.memParams += Allocator::alignSize(sizeof(TypeInfoParam));
#endif
    return typeParam;
}
