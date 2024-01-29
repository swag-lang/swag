#include "pch.h"
#include "TypeManager.h"
#include "Ast.h"
#include "AstNode.h"
#include "Job.h"
#include "Mutex.h"
#include "Semantic.h"
#include "Tokenizer.h"
#include "TypeInfo.h"
#include "Workspace.h"

TypeManager* g_TypeMgr = nullptr;

static TypeInfo*                            g_LiteralTypeToType[(int) LiteralType::TT_MAX];
thread_local Map<uint32_t, TypeInfoNative*> g_MapUntypedValuesI;
thread_local Map<uint32_t, TypeInfoNative*> g_MapUntypedValuesB;
thread_local Map<uint32_t, TypeInfoNative*> g_MapUntypedValuesF;

void TypeManager::setup()
{
    typeInfoS8             = new TypeInfoNative(NativeTypeKind::S8, "s8", 1, TYPEINFO_INTEGER | TYPEINFO_CAN_PROMOTE_816);
    typeInfoS16            = new TypeInfoNative(NativeTypeKind::S16, "s16", 2, TYPEINFO_INTEGER | TYPEINFO_CAN_PROMOTE_816);
    typeInfoS32            = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER);
    typeInfoS64            = new TypeInfoNative(NativeTypeKind::S64, "s64", 8, TYPEINFO_INTEGER);
    typeInfoU8             = new TypeInfoNative(NativeTypeKind::U8, "u8", 1, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_CAN_PROMOTE_816);
    typeInfoU16            = new TypeInfoNative(NativeTypeKind::U16, "u16", 2, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_CAN_PROMOTE_816);
    typeInfoU32            = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoU64            = new TypeInfoNative(NativeTypeKind::U64, "u64", 8, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED);
    typeInfoBool           = new TypeInfoNative(NativeTypeKind::Bool, "bool", 1, 0);
    typeInfoF32            = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT);
    typeInfoF64            = new TypeInfoNative(NativeTypeKind::F64, "f64", 8, TYPEINFO_FLOAT);
    typeInfoRune           = new TypeInfoNative(NativeTypeKind::Rune, "rune", 4, 0);
    typeInfoVoid           = new TypeInfoNative(NativeTypeKind::Void, "void", 0, 0);
    typeInfoString         = new TypeInfoNative(NativeTypeKind::String, "string", 2 * sizeof(Register), 0);
    typeInfoAny            = new TypeInfoNative(NativeTypeKind::Any, "any", 2 * sizeof(Register), 0);
    typeInfoUndefined      = new TypeInfoNative(NativeTypeKind::Undefined, "?", 0, 0);
    typeInfoUntypedInt     = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER | TYPEINFO_UNTYPED_INTEGER);
    typeInfoUntypedBinHexa = new TypeInfoNative(NativeTypeKind::U32, "u32", 4, TYPEINFO_INTEGER | TYPEINFO_UNSIGNED | TYPEINFO_UNTYPED_BINHEXA);
    typeInfoUntypedFloat   = new TypeInfoNative(NativeTypeKind::F32, "f32", 4, TYPEINFO_FLOAT | TYPEINFO_UNTYPED_FLOAT);
    typeInfoCharacter      = new TypeInfoNative(NativeTypeKind::S32, "s32", 4, TYPEINFO_INTEGER | TYPEINFO_UNTYPED_INTEGER | TYPEINFO_CHARACTER);

    typeInfoVariadic = new TypeInfoVariadic();
    typeInfoVariadic->name.setView("...", 3);
    typeInfoVariadic->sizeOf = 2 * sizeof(Register);

    typeInfoCVariadic       = new TypeInfoVariadic();
    typeInfoCVariadic->kind = TypeInfoKind::CVariadic;
    typeInfoCVariadic->name.setView("cvarargs", 8);
    typeInfoCVariadic->flags |= TYPEINFO_C_VARIADIC;
    typeInfoCVariadic->sizeOf = 0;

    // Some default pointers
    typeInfoNull = new TypeInfoPointer();
    typeInfoNull->name.setView("null", 4);
    typeInfoNull->sizeOf = sizeof(Register);

    typeInfoCString         = new TypeInfoPointer();
    typeInfoCString->sizeOf = sizeof(Register);
    typeInfoCString->setConst();
    typeInfoCString->flags |= TYPEINFO_C_STRING;
    typeInfoCString->pointedType = typeInfoU8;
    typeInfoCString->name.setView("cstring", 7);

    // The rest
    typeInfoCode = new TypeInfoCode();

    typeInfoOpCall             = new TypeInfoFuncAttr();
    typeInfoOpCall->returnType = typeInfoVoid;
    typeInfoOpCall->parameters.push_back(new TypeInfoParam());
    auto typePtr         = new TypeInfoPointer();
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
    typeInfoSliceRunes->flags |= TYPEINFO_CONST;
    typeInfoSliceRunes->computeName();

    memset(g_LiteralTypeToType, 0, sizeof(g_LiteralTypeToType));
    g_LiteralTypeToType[(int) LiteralType::TT_U8]                      = typeInfoU8;
    g_LiteralTypeToType[(int) LiteralType::TT_U16]                     = typeInfoU16;
    g_LiteralTypeToType[(int) LiteralType::TT_U32]                     = typeInfoU32;
    g_LiteralTypeToType[(int) LiteralType::TT_U64]                     = typeInfoU64;
    g_LiteralTypeToType[(int) LiteralType::TT_S8]                      = typeInfoS8;
    g_LiteralTypeToType[(int) LiteralType::TT_S16]                     = typeInfoS16;
    g_LiteralTypeToType[(int) LiteralType::TT_S32]                     = typeInfoS32;
    g_LiteralTypeToType[(int) LiteralType::TT_S64]                     = typeInfoS64;
    g_LiteralTypeToType[(int) LiteralType::TT_F32]                     = typeInfoF32;
    g_LiteralTypeToType[(int) LiteralType::TT_F64]                     = typeInfoF64;
    g_LiteralTypeToType[(int) LiteralType::TT_BOOL]                    = typeInfoBool;
    g_LiteralTypeToType[(int) LiteralType::TT_RUNE]                    = typeInfoRune;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING]                  = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING_RAW]              = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING_MULTILINE]        = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING_ESCAPE]           = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_STRING_MULTILINE_ESCAPE] = typeInfoString;
    g_LiteralTypeToType[(int) LiteralType::TT_CHARACTER]               = typeInfoCharacter;
    g_LiteralTypeToType[(int) LiteralType::TT_CHARACTER_ESCAPE]        = typeInfoCharacter;
    g_LiteralTypeToType[(int) LiteralType::TT_VOID]                    = typeInfoVoid;
    g_LiteralTypeToType[(int) LiteralType::TT_NULL]                    = typeInfoNull;
    g_LiteralTypeToType[(int) LiteralType::TT_ANY]                     = typeInfoAny;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_INT]             = typeInfoUntypedInt;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_FLOAT]           = typeInfoUntypedFloat;
    g_LiteralTypeToType[(int) LiteralType::TT_UNTYPED_BINHEXA]         = typeInfoUntypedBinHexa;
    g_LiteralTypeToType[(int) LiteralType::TT_CSTRING]                 = typeInfoCString;
    g_LiteralTypeToType[(int) LiteralType::TT_TYPE]                    = nullptr; // will be done with registerTypeType

    // Promotion matrix
#define PR(__a, __b, __c)                                                      \
    promoteMatrix[(int) NativeTypeKind::__a][(int) NativeTypeKind::__b] = __c; \
    promoteMatrix[(int) NativeTypeKind::__b][(int) NativeTypeKind::__a] = __c;

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

TypeInfoArray* TypeManager::convertTypeListToArray(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto      typeArray    = makeType<TypeInfoArray>();
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

        if (typeArray->pointedType->isListTuple())
        {
            typeList               = CastTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListTuple);
            typeArray->pointedType = convertTypeListToStruct(context, typeList, isCompilerConstant);
            finalType              = typeArray->pointedType;
            break;
        }

        if (typeArray->pointedType->isListArray())
        {
            typeList               = CastTypeInfo<TypeInfoList>(typeArray->pointedType, TypeInfoKind::TypeListArray);
            typeArray->pointedType = makeType<TypeInfoArray>();
            typeArray              = (TypeInfoArray*) typeArray->pointedType;
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
        typeArray = CastTypeInfo<TypeInfoArray>(typeArray->pointedType, TypeInfoKind::Array);
    }

    orgTypeArray->sizeOf = orgTypeArray->finalType->sizeOf * orgTypeArray->totalCount;
    return orgTypeArray;
}

TypeInfoStruct* TypeManager::convertTypeListToStruct(JobContext* context, TypeInfoList* typeList, bool isCompilerConstant)
{
    auto typeStruct = makeType<TypeInfoStruct>();

    Utf8 name = context->sourceFile->scopeFile->name + "_tpl4_";
    name += Fmt("%d", g_UniqueID.fetch_add(1));
    typeStruct->name = name;

    typeStruct->flags |= TYPEINFO_STRUCT_IS_TUPLE;

    typeStruct->fields.reserve((int) typeList->subTypes.size());
    for (size_t idx = 0; idx < typeList->subTypes.size(); idx++)
    {
        auto one       = typeList->subTypes[idx];
        auto typeParam = (TypeInfoParam*) one->clone();
        if (!typeParam->name.empty())
        {
            typeStruct->fields.push_back(typeParam);
            typeParam = (TypeInfoParam*) one->clone();
            typeParam->flags |= TYPEINFOPARAM_HIDE_FROM_NAME;
        }

        typeParam->name = Fmt("item%u", idx);
        typeParam->flags |= TYPEINFOPARAM_AUTO_NAME;
        typeStruct->fields.push_back(typeParam);
    }

    typeStruct->structName = typeStruct->name;

    // Generate some fake nodes
    // This peace of code is necessary to solve something like :
    // let s = [{1, 2}, {3, 4}]
    auto structDecl = Ast::newStructDecl(context->sourceFile, nullptr);
    structDecl->allocateExtension(ExtensionKind::Misc);
    structDecl->extMisc()->exportNode = typeList->declNode;
    typeStruct->declNode              = structDecl;
    typeStruct->declNode->flags |= AST_GENERATED;
    typeStruct->declNode->typeInfo   = typeStruct;
    typeStruct->declNode->ownerScope = context->sourceFile->scopeFile;
    typeStruct->flags |= TYPEINFO_GHOST_TUPLE;
    typeStruct->scope        = Ast::newScope(typeStruct->declNode, "", ScopeKind::Struct, context->sourceFile->scopeFile);
    typeStruct->scope->owner = typeStruct->declNode;
    typeStruct->alignOf      = 1;
    structDecl->scope        = typeStruct->scope;
    for (auto f : typeStruct->fields)
    {
        f->declNode           = Ast::newVarDecl(context->sourceFile, f->name, typeStruct->declNode);
        f->declNode->typeInfo = f->typeInfo;
        f->declNode->flags |= AST_GENERATED;
        f->offset = typeStruct->sizeOf;

        AddSymbolTypeInfo toAdd;
        toAdd.kind                          = SymbolKind::Variable;
        toAdd.node                          = f->declNode;
        toAdd.typeInfo                      = f->typeInfo;
        toAdd.storageOffset                 = f->offset;
        toAdd.aliasName                     = &f->name;
        toAdd.flags                         = OVERLOAD_VAR_STRUCT;
        f->declNode->resolvedSymbolOverload = typeStruct->scope->symTable.addSymbolTypeInfo((ErrorContext*) context, toAdd);
        f->declNode->resolvedSymbolName     = f->declNode->resolvedSymbolOverload->symbol;

        typeStruct->sizeOf += f->typeInfo->sizeOf;
    }

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
        auto it = g_MapUntypedValuesI.find(value);
        if (it != g_MapUntypedValuesI.end())
        {
            SWAG_ASSERT(it->second->isUntypedInteger());
            SWAG_ASSERT(it->second->valueInteger == *(int32_t*) &value);
            return it->second;
        }

        TypeInfoNative* newType    = (TypeInfoNative*) typeInfo->clone();
        newType->valueInteger      = *(int32_t*) &value;
        typeInfo                   = newType;
        g_MapUntypedValuesI[value] = newType;
    }
    else if (typeInfo->isUntypedBinHex())
    {
        auto it = g_MapUntypedValuesB.find(value);
        if (it != g_MapUntypedValuesB.end())
        {
            SWAG_ASSERT(it->second->isUntypedBinHex());
            SWAG_ASSERT(it->second->valueInteger == *(int32_t*) &value);
            return it->second;
        }

        TypeInfoNative* newType    = (TypeInfoNative*) typeInfo->clone();
        newType->valueInteger      = *(int32_t*) &value;
        typeInfo                   = newType;
        g_MapUntypedValuesB[value] = newType;
    }
    else if (typeInfo->isUntypedFloat())
    {
        auto it = g_MapUntypedValuesF.find(value);
        if (it != g_MapUntypedValuesF.end())
        {
            SWAG_ASSERT(it->second->isUntypedFloat());
            SWAG_ASSERT(it->second->valueFloat == *(float*) &value);
            return it->second;
        }

        TypeInfoNative* newType    = (TypeInfoNative*) typeInfo->clone();
        newType->valueFloat        = *(float*) &value;
        typeInfo                   = newType;
        g_MapUntypedValuesF[value] = newType;
    }

    return typeInfo;
}

TypeInfo* TypeManager::literalTypeToType(LiteralType literalType)
{
    SWAG_ASSERT(literalType < LiteralType::TT_MAX);
    auto result = g_LiteralTypeToType[(int) literalType];
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
    auto alignValue = value & ~((uint64_t) align - 1);
    if (alignValue < value)
        alignValue += align;
    return alignValue;
}

uint32_t TypeManager::alignOf(TypeInfo* typeInfo)
{
    if (typeInfo->isStruct())
    {
        auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
        SWAG_ASSERT(typeStruct->sizeOf);
        SWAG_ASSERT(typeStruct->alignOf);
        return typeStruct->alignOf;
    }
    else if (typeInfo->isArray())
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        return alignOf(typeArray->finalType);
    }
    else if (typeInfo->isPointer())
    {
        return typeInfo->sizeOf;
    }
    else if (typeInfo->isSlice() ||
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
    typeInfoTypeType                                = makePointerTo(g_Workspace->swagScope.regTypeInfo, TYPEINFO_CONST);
    g_LiteralTypeToType[(int) LiteralType::TT_TYPE] = typeInfoTypeType;
}

TypeInfo* TypeManager::makeUnConst(TypeInfo* typeInfo)
{
    if (!typeInfo->isConst())
        return typeInfo;
    if (typeInfo->isConstAlias())
        return typeInfo->getConstAlias();

    ScopedLock lk(mutex);

    auto it = mapUnConst.find(typeInfo);
    if (it != mapUnConst.end())
        return it->second;

    auto typeUnConst = typeInfo->clone();
    typeUnConst->flags &= ~TYPEINFO_CONST;
    typeUnConst->forceComputeName();
    mapUnConst[typeInfo] = typeUnConst;

    return typeUnConst;
}

TypeInfo* TypeManager::makeConst(TypeInfo* typeInfo)
{
    if (typeInfo->isConst())
        return typeInfo;

    ScopedLock lk(mutex);

    auto it = mapConst.find(typeInfo);
    if (it != mapConst.end())
        return it->second;

    if (typeInfo->isStruct())
    {
        auto typeAlias = makeType<TypeInfoAlias>();
        typeAlias->copyFrom(typeInfo);
        typeAlias->rawType = typeInfo;
        typeAlias->flags |= TYPEINFO_CONST | TYPEINFO_CONST_ALIAS;
        mapConst[typeInfo] = typeAlias;
        return typeAlias;
    }
    else
    {
        auto typeConst = typeInfo->clone();
        typeConst->setConst();
        mapConst[typeInfo] = typeConst;
        return typeConst;
    }
}

TypeInfoPointer* TypeManager::makePointerTo(TypeInfo* toType, uint64_t ptrFlags)
{
    ScopedLock lk(mutex);

    auto it = mapPointers.find(toType);
    if (it != mapPointers.end())
    {
        for (auto& it1 : it->second)
        {
            if (it1.flags == ptrFlags)
                return it1.pointerTo;
        }
    }

    auto ptrType         = makeType<TypeInfoPointer>();
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
    auto typeParam = Allocator::alloc<TypeInfoParam>();
#ifdef SWAG_STATS
    g_Stats.memParams += Allocator::alignSize(sizeof(TypeInfoParam));
#endif
    return typeParam;
}

void TypeManager::convertStructParamToRef(AstNode* node, TypeInfo* typeInfo)
{
    SWAG_ASSERT(node->kind == AstNodeKind::FuncDeclParam);

    // A struct/interface is forced to be a const reference
    if (!node->typeInfo->isGeneric() && typeInfo->isStruct())
    {
        auto typeRef         = makeType<TypeInfoPointer>();
        typeInfo             = typeInfo->getConstAlias();
        typeRef->flags       = typeInfo->flags | TYPEINFO_CONST | TYPEINFO_POINTER_REF | TYPEINFO_POINTER_AUTO_REF;
        typeRef->pointedType = typeInfo;
        typeRef->sizeOf      = sizeof(void*);
        typeRef->computeName();
        node->typeInfo = typeRef;
    }
}
