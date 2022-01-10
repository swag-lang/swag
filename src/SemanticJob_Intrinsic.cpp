#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "AstOutput.h"
#include "Module.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Workspace.h"

bool SemanticJob::resolveIntrinsicTag(SemanticContext* context)
{
    auto node = context->node;
    switch (node->token.id)
    {
    case TokenId::IntrinsicHasTag:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(checkIsConstExpr(context, front->flags & AST_VALUE_COMPUTED, front, Err(Err0248)));
        SWAG_VERIFY(front->typeInfo->isNative(NativeTypeKind::String), context->report(front, Fmt(Err(Err0249), front->typeInfo->getDisplayNameC())));
        auto tag       = g_Workspace->hasTag(front->computedValue->text);
        node->typeInfo = g_TypeMgr->typeInfoBool;
        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = tag ? true : false;
        return true;
    }

    case TokenId::IntrinsicGetTag:
    {
        auto nameNode   = node->childs[0];
        auto typeNode   = node->childs[1];
        auto defaultVal = node->childs[2];
        SWAG_CHECK(evaluateConstExpression(context, nameNode));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(evaluateConstExpression(context, defaultVal));
        if (context->result == ContextResult::Pending)
            return true;

        SWAG_CHECK(checkIsConstExpr(context, nameNode->flags & AST_VALUE_COMPUTED, nameNode, Err(Err0250)));
        SWAG_VERIFY(!(nameNode->flags & AST_VALUE_IS_TYPEINFO), context->report(nameNode, Err(Err0245)));
        SWAG_VERIFY(nameNode->typeInfo->isNative(NativeTypeKind::String), context->report(nameNode, Fmt(Err(Err0251), nameNode->typeInfo->getDisplayNameC())));
        SWAG_VERIFY(!(defaultVal->flags & AST_VALUE_IS_TYPEINFO), context->report(defaultVal, Err(Err0283)));
        SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, defaultVal->typeInfo, nullptr, defaultVal, CASTFLAG_DEFAULT));

        node->typeInfo = typeNode->typeInfo;
        node->setFlagsValueIsComputed();

        auto tag = g_Workspace->hasTag(nameNode->computedValue->text);
        if (tag)
        {
            if (!TypeManager::makeCompatibles(context, typeNode->typeInfo, tag->type, nullptr, typeNode, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            {
                Diagnostic diag{typeNode, Fmt(Err(Err0252), typeNode->typeInfo->getDisplayNameC(), tag->type->getDisplayNameC(), tag->name.c_str())};
                Diagnostic note{typeNode, Fmt(Nte(Nte0038), tag->cmdLine.c_str()), DiagnosticLevel::Note};
                note.hasFile     = false;
                note.printSource = false;
                return context->report(diag, &note);
            }

            node->typeInfo       = tag->type;
            *node->computedValue = tag->value;
        }
        else
        {
            node->computedValue = defaultVal->computedValue;
        }

        return true;
    }
    }
    return true;
}

bool SemanticJob::resolveIntrinsicMakeForeign(SemanticContext* context)
{
    auto node  = context->node;
    auto first = node->childs.front();
    auto expr  = node->childs.back();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Lambda)
        return context->report(first, Err(Err0782));
    first->flags |= AST_NO_BYTECODE;

    // Check expression
    SWAG_CHECK(checkIsConcrete(context, expr));
    if (!expr->typeInfo->isSame(g_TypeMgr->typeInfoConstPointers[(int) NativeTypeKind::Void], ISSAME_CAST))
        return context->report(expr, Err(Err0783));

    node->typeInfo    = first->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeForeign;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first = node->childs.front();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Lambda)
        return context->report(node, Err(Err0784));

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(first->typeInfo, TypeInfoKind::Lambda);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
        return context->report(Hint::isType(typeFunc), {first, Fmt(Err(Err0785), SWAG_LIMIT_CB_MAX_PARAMS, typeFunc->parameters.size())});
    if (typeFunc->numReturnRegisters() > 1)
        return context->report(Hint::isType(typeFunc), {first, Fmt(Err(Err0786), typeFunc->returnType->getDisplayNameC())});

    node->typeInfo    = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeCallback;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* name)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Must start with a pointer of the same type as the slice
    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report(first, Fmt(Err(Err0787), name));

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report(first, Fmt(Err(Err0788), name));

    // Slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, second->typeInfo, nullptr, second, CASTFLAG_TRY_COERCE));

    // Create slice type
    auto ptrSlice         = allocType<TypeInfoSlice>();
    ptrSlice->pointedType = ptrPointer->pointedType;
    if (ptrPointer->isConst())
        ptrSlice->flags |= TYPEINFO_CONST;
    ptrSlice->computeName();
    node->typeInfo = ptrSlice;

    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeSlice;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Check first parameter
    if (first->typeInfo->kind != TypeInfoKind::Pointer)
        return context->report(first, Err(Err0789));

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report(first, Err(Err0790));

    // Check second parameter
    if (second->flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, second->typeInfo, nullptr, second, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR))
            return context->report(node, Fmt(Err(Err0791), ptrPointer->pointedType->getDisplayNameC(), second->typeInfo->getDisplayNameC()));
    }

    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (context->result != ContextResult::Done)
        return true;
    if (!(second->typeInfo->isPointerToTypeInfo()))
        return context->report(node, Fmt(Err(Err0792), second->typeInfo->getDisplayNameC()));

    node->typeInfo    = g_TypeMgr->typeInfoAny;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeAny;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeInterface(SemanticContext* context)
{
    auto node   = context->node;
    auto params = node->childs.front();

    auto  first      = params->childs[0];
    auto  second     = params->childs[1];
    auto  third      = params->childs[2];
    auto  sourceFile = context->sourceFile;
    auto  module     = sourceFile->module;
    auto& typeTable  = module->typeTable;

    SWAG_CHECK(checkIsConcrete(context, first));

    third->allocateComputedValue();
    third->computedValue->storageSegment = getConstantSegFromContext(third);
    SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, third->typeInfo, third->computedValue->storageSegment, &third->computedValue->storageOffset, MAKE_CONCRETE_SHOULD_WAIT));
    if (context->result != ContextResult::Done)
        return true;

    auto firstTypeInfo = TypeManager::concreteReferenceType(first->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(firstTypeInfo->kind == TypeInfoKind::Pointer || firstTypeInfo->kind == TypeInfoKind::Struct, context->report(first, Err(Err0793)));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report(second, Err(Err0794)));
    auto thirdTypeInfo = TypeManager::concreteReferenceType(third->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(thirdTypeInfo->kind == TypeInfoKind::Interface, context->report(third, Err(Err0795)));

    node->typeInfo = third->typeInfo;
    third->flags |= AST_NO_BYTECODE;

    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeInterface;
    return true;
}

bool SemanticJob::resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = TypeManager::concreteReferenceType(expression->typeInfo);
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        node->typeInfo    = g_TypeMgr->typeInfoConstPointers[(int) NativeTypeKind::U8];
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto ptrSlice     = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        node->typeInfo    = g_TypeMgr->makePointerTo(ptrSlice->pointedType, ptrSlice->isConst());
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto ptrArray     = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->typeInfo    = g_TypeMgr->makePointerTo(ptrArray->pointedType, ptrArray->isConst());
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isNative(NativeTypeKind::Any) || typeInfo->kind == TypeInfoKind::Interface)
    {
        node->typeInfo    = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_VERIFY(!(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE), context->report(Hint::isType(typeInfo), {expression, Err(Err0796)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opData, nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else
    {
        return context->report(node, Fmt(Err(Err0797), typeInfo->getDisplayNameC()));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicStringOf(SemanticContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    node->setFlagsValueIsComputed();
    if (expr->computedValue)
    {
        if (expr->flags & AST_VALUE_IS_TYPEINFO)
            node->computedValue->text = ((TypeInfo*) expr->computedValue->reg.pointer)->scopedName;
        else if (typeInfo->isNative(NativeTypeKind::String))
            node->computedValue->text = expr->computedValue->text;
        else if (typeInfo->kind == TypeInfoKind::Native)
            node->computedValue->text = Ast::literalToString(typeInfo, *expr->computedValue);
        else if (typeInfo->kind == TypeInfoKind::Enum)
            node->computedValue->text = Ast::enumToString(typeInfo, expr->computedValue->text, expr->computedValue->reg);
        else
            return context->report(expr, Err(Err0799));
    }
    else if (expr->typeInfo->kind == TypeInfoKind::Code)
    {
        Concat concat;
        concat.init(4 * 1024);

        AstOutput::OutputContext outputContext;
        auto                     typeCode = CastTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
        AstOutput::outputNode(outputContext, concat, typeCode->content);
        for (auto b = concat.firstBucket; b; b = b->nextBucket)
            node->computedValue->text.append((const char*) b->datas, concat.bucketCount(b));
    }
    else if (expr->resolvedSymbolName)
    {
        node->computedValue->text = expr->resolvedSymbolName->getFullName();
    }
    else if (expr->resolvedSymbolOverload)
    {
        node->computedValue->text = expr->resolvedSymbolOverload->symbol->getFullName();
    }
    else
    {
        return context->report(expr, Err(Err0799));
    }

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool SemanticJob::resolveIntrinsicNameOf(SemanticContext* context)
{
    auto node = context->node;
    auto expr = node->childs.front();

    node->setFlagsValueIsComputed();
    if (expr->computedValue && expr->flags & AST_VALUE_IS_TYPEINFO)
        node->computedValue->text = ((TypeInfo*) expr->computedValue->reg.pointer)->name;
    else if (expr->resolvedSymbolName)
        node->computedValue->text = expr->resolvedSymbolName->name;
    else if (expr->resolvedSymbolOverload)
        node->computedValue->text = expr->resolvedSymbolOverload->symbol->name;
    else
        return context->report(expr, Err(Err0803));

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool SemanticJob::resolveIntrinsicRunes(SemanticContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    SWAG_CHECK(checkIsConstExpr(context, expr->flags & AST_VALUE_COMPUTED, expr));
    SWAG_VERIFY(typeInfo->isNative(NativeTypeKind::String), context->report(expr, Fmt(Err(Err0084), typeInfo->getDisplayNameC())));
    node->setFlagsValueIsComputed();

    // Convert
    vector<uint32_t> runes;
    const char*      pz  = expr->computedValue->text.c_str();
    int              cpt = 0;
    while (cpt < expr->computedValue->text.count)
    {
        uint32_t c;
        uint32_t offset;
        pz = Utf8::decodeUtf8(pz, c, offset);
        runes.push_back(c);
        cpt += offset;
    }

    // :SliceLiteral
    uint8_t* addrDst;
    auto     storageSegment             = SemanticJob::getConstantSegFromContext(context->node);
    node->computedValue->storageSegment = storageSegment;
    node->computedValue->storageOffset  = storageSegment->reserve((uint32_t) runes.size() * sizeof(uint32_t), &addrDst);
    node->computedValue->reg.u64        = (uint64_t) runes.size();

    // Setup array
    memcpy(addrDst, &runes[0], runes.size() * sizeof(uint32_t));

    node->typeInfo = g_TypeMgr->typeInfoSliceRunes;
    return true;
}

bool SemanticJob::resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = expression->typeInfo;
    if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = typeEnum->values.size();
        node->typeInfo               = g_TypeMgr->typeInfoUInt;
        return true;
    }

    typeInfo = TypeManager::concreteReferenceType(typeInfo);
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        node->typeInfo = g_TypeMgr->typeInfoUInt;
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->setFlagsValueIsComputed();
            node->computedValue->reg.u64 = node->computedValue->text.length();
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->setFlagsValueIsComputed();
        auto typeArray               = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue->reg.u64 = typeArray->count;
        node->typeInfo               = g_TypeMgr->typeInfoUInt;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        // :SliceLiteral
        // Slice literal. This can happen for enum values
        if (node->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue->reg.u64 = node->computedValue->reg.u64;
            node->typeInfo               = g_TypeMgr->typeInfoUInt;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
            node->typeInfo    = g_TypeMgr->typeInfoUInt;
        }
    }
    else if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = typeList->subTypes.size();
        node->typeInfo               = g_TypeMgr->typeInfoUInt;
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        node->typeInfo    = g_TypeMgr->typeInfoUInt;
    }
    else if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_VERIFY(!(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE), context->report(Hint::isType(typeInfo), {expression, Err(Err0800)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, nullptr, false));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr->typeInfoUInt;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
    }
    else
    {
        SWAG_VERIFY(typeInfo->isNativeInteger(), context->report(expression, Fmt(Err(Err0801), typeInfo->getDisplayNameC())));
        if (node->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (node->computedValue->reg.s8 < 0)
                        return context->report(expression, Fmt(Err(Err0802), node->computedValue->reg.s8));
                    break;
                case NativeTypeKind::S16:
                    if (node->computedValue->reg.s16 < 0)
                        return context->report(expression, Fmt(Err(Err0802), node->computedValue->reg.s16));
                    break;
                case NativeTypeKind::S32:
                    if (node->computedValue->reg.s32 < 0)
                        return context->report(expression, Fmt(Err(Err0802), node->computedValue->reg.s32));
                    break;
                case NativeTypeKind::S64:
                case NativeTypeKind::Int:
                    if (node->computedValue->reg.s64 < 0)
                        return context->report(expression, Fmt(Err(Err0805), node->computedValue->reg.s64));
                    break;
                }
            }
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, typeInfo, nullptr, node, CASTFLAG_TRY_COERCE));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicSpread(SemanticContext* context)
{
    auto node         = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr         = node->childs.front();
    auto typeInfo     = TypeManager::concreteReferenceType(expr->typeInfo);
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicSpread;

    SWAG_VERIFY(node->parent && node->parent->parent && node->parent->parent->kind == AstNodeKind::FuncCallParam, context->report(node, Err(Err0806)));

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArr   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->typeInfo = typeArr->pointedType;
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        node->typeInfo = typeSlice->pointedType;
    }
    else if (typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto typeList  = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListArray);
        node->typeInfo = typeList->subTypes[0]->typeInfo;

        // Need to be sure that the expression list can be casted to the equivalent array
        auto typeArr         = allocType<TypeInfoArray>();
        typeArr->count       = (uint32_t) typeList->subTypes.size();
        typeArr->pointedType = typeList->subTypes[0]->typeInfo;
        typeArr->finalType   = typeArr->pointedType;
        typeArr->sizeOf      = typeArr->count * typeArr->finalType->sizeOf;
        typeArr->totalCount  = typeArr->count;
        typeArr->computeName();

        SWAG_CHECK(TypeManager::makeCompatibles(context, typeArr, typeList, nullptr, expr));

        g_Allocator.free(typeArr, sizeof(TypeInfoArray));
    }
    else
    {
        return context->report(expr, Fmt(Err(Err0807), typeInfo->getDisplayNameC()));
    }

    auto typeVar     = allocType<TypeInfoVariadic>(TypeInfoKind::TypedVariadic);
    typeVar->rawType = node->typeInfo;
    typeVar->computeName();
    node->typeInfo = typeVar;
    node->typeInfo->flags |= TYPEINFO_SPREAD;

    return true;
}

bool SemanticJob::resolveIntrinsicKindOf(SemanticContext* context)
{
    auto  node       = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto  expr       = node->childs.front();
    auto  sourceFile = context->sourceFile;
    auto& typeTable  = sourceFile->module->typeTable;

    SWAG_CHECK(checkIsConstExpr(context, expr->typeInfo, expr));

    // Will be runtime for an 'any' type
    if (expr->typeInfo->isNative(NativeTypeKind::Any) || expr->typeInfo->kind == TypeInfoKind::Interface)
    {
        SWAG_CHECK(checkIsConcrete(context, expr));
        node->allocateComputedValue();
        node->computedValue->storageSegment = getConstantSegFromContext(node);
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, expr->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, MAKE_CONCRETE_SHOULD_WAIT, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicKindOf;
        node->flags |= AST_R_VALUE;
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    // For a function, this is the unscoped type
    if (expr->typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo, MAKE_CONCRETE_FORCE_NO_SCOPE));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    // For an enum, this is the raw type
    if (expr->typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(resolveTypeAsExpression(context, expr, typeEnum->rawType, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
        return true;
    }

    SWAG_CHECK(resolveIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::makeIntrinsicTypeOf(SemanticContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    // A @typeof/@kindof as a type in a declaration
    if (node->specFlags & AST_SPEC_INTRINSIC_TYPEOF_AS_TYPE)
    {
        // @kindof on a typeinfo will give back the original compiler type
        if (node->token.id == TokenId::IntrinsicKindOf &&
            typeInfo->isPointerToTypeInfo() &&
            expr->computedValue &&
            expr->computedValue->storageOffset != UINT32_MAX &&
            expr->computedValue->storageSegment != nullptr)
        {
            auto storageSegment = expr->computedValue->storageSegment;
            auto addr           = storageSegment->address(expr->computedValue->storageOffset);
            auto newTypeInfo    = context->sourceFile->module->typeTable.getRealType(storageSegment, (ConcreteTypeInfo*) addr);
            if (newTypeInfo)
                typeInfo = newTypeInfo;
        }

        node->typeInfo = typeInfo;
    }

    // A @typeof to get a typeinfo
    else
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicTypeOf(SemanticContext* context)
{
    auto node = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr = node->childs.front();

    SWAG_CHECK(checkIsConstExpr(context, expr->typeInfo, expr));
    SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report(expr, Err(Err0810)));

    // If we have a function, then we transform it to a lambda type, as this makes no sens to
    // get the real function type with @typeof
    // i.e.
    // x := @typeof(func) is equivalent to x := @typeof(&func)
    if (expr->typeInfo->kind == TypeInfoKind::FuncAttr)
    {
        expr->typeInfo         = expr->typeInfo->clone();
        expr->typeInfo->kind   = TypeInfoKind::Lambda;
        expr->typeInfo->sizeOf = sizeof(void*);
    }

    expr->flags |= AST_NO_BYTECODE;
    SWAG_CHECK(makeIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);

    switch (node->token.id)
    {
    case TokenId::IntrinsicSpread:
        SWAG_CHECK(resolveIntrinsicSpread(context));
        return true;

    case TokenId::IntrinsicIsConstExpr:
    {
        auto expr      = node->childs.front();
        node->typeInfo = g_TypeMgr->typeInfoBool;
        expr->flags |= AST_NO_BYTECODE;

        // Special case for a function parameter in a selectif block, should be done at runtime
        if (expr->isSelectIfParam(expr->resolvedSymbolOverload))
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicIsConstExpr;
            break;
        }

        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = (expr->flags & AST_VALUE_COMPUTED);
        break;
    }

    case TokenId::IntrinsicSizeOf:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConstExpr(context, expr->typeInfo, expr));
        SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report(expr, Err(Err0812)));
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = expr->typeInfo->sizeOf;
        if (node->computedValue->reg.u64 > 0xFFFFFFFF)
            node->typeInfo = g_TypeMgr->typeInfoUInt;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicAlignOf:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConstExpr(context, expr->typeInfo, expr));
        SWAG_VERIFY(expr->typeInfo->kind != TypeInfoKind::Generic, context->report(expr, Err(Err0814)));
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = TypeManager::alignOf(expr->typeInfo);
        if (node->computedValue->reg.u64 > 0xFFFFFFFF)
            node->typeInfo = g_TypeMgr->typeInfoUInt;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicOffsetOf:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConstExpr(context, expr->resolvedSymbolOverload, expr));
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = expr->resolvedSymbolOverload->computedValue.storageOffset;
        if (node->computedValue->reg.u64 > 0xFFFFFFFF)
            node->typeInfo = g_TypeMgr->typeInfoUInt;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicTypeOf:
        SWAG_CHECK(resolveIntrinsicTypeOf(context));
        return true;

    case TokenId::IntrinsicKindOf:
        SWAG_CHECK(resolveIntrinsicKindOf(context));
        return true;

    case TokenId::IntrinsicStringOf:
        SWAG_CHECK(resolveIntrinsicStringOf(context));
        return true;
    case TokenId::IntrinsicNameOf:
        SWAG_CHECK(resolveIntrinsicNameOf(context));
        return true;

    case TokenId::IntrinsicRunes:
        SWAG_CHECK(resolveIntrinsicRunes(context));
        return true;

    case TokenId::IntrinsicCountOf:
    {
        auto expr = node->childs.front();
        if (expr->typeInfo->kind != TypeInfoKind::Enum)
            SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        SWAG_CHECK(resolveIntrinsicCountOf(context, node, expr));
        break;
    }

    case TokenId::IntrinsicDataOf:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicDataOf(context, node, expr));
        break;
    }

    case TokenId::IntrinsicMakeAny:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeAny(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeSlice:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkslice"));
        break;
    }

    case TokenId::IntrinsicMakeString:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkstring"));
        node->typeInfo = g_TypeMgr->typeInfoString;
        break;
    }

    case TokenId::IntrinsicMakeCallback:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeCallback(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeForeign:
    {
        SWAG_CHECK(resolveIntrinsicMakeForeign(context));
        break;
    }

    case TokenId::IntrinsicMakeInterface:
    {
        SWAG_CHECK(resolveIntrinsicMakeInterface(context));
        break;
    }

    case TokenId::IntrinsicCVaStart:
    case TokenId::IntrinsicCVaEnd:
    case TokenId::IntrinsicCVaArg:
    {
        auto typeInfo = node->childs[0]->typeInfo;
        typeInfo->computeScopedName();
        SWAG_VERIFY(typeInfo->scopedName == "*Swag.CVaList", context->report(node, Fmt(Err(Err0048), typeInfo->getDisplayNameC())));

        if (node->token.id == TokenId::IntrinsicCVaStart)
        {
            SWAG_VERIFY(node->ownerFct && node->ownerFct->parameters && node->ownerFct->parameters->childs.size(), context->report(node, Err(Err0442)));
            auto typeParam = node->ownerFct->parameters->childs.back()->typeInfo;
            SWAG_VERIFY(typeParam->kind == TypeInfoKind::CVariadic, context->report(node, Err(Err0442)));
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCVaStart;
        }
        else if (node->token.id == TokenId::IntrinsicCVaEnd)
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCVaEnd;
        }
        else
        {
            node->typeInfo = node->childs[1]->typeInfo;

            SWAG_VERIFY(node->typeInfo->numRegisters() == 1, context->report(node->childs[1], Fmt(Err(Err0443), node->typeInfo->getDisplayNameC())));

            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::F32), context->report(node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "f64")));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S8), context->report(node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "s32")));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S16), context->report(node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "s32")));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U8), context->report(node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "u32")));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U16), context->report(node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "u32")));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::Bool), context->report(node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "u32")));

            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCVaArg;
        }

        break;
    }
    }

    return true;
}
