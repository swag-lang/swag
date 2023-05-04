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
    switch (node->tokenId)
    {
    case TokenId::IntrinsicSafety:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(checkIsConstExpr(context, front->flags & AST_VALUE_COMPUTED, front, Err(Err0248), node->token.text));
        SWAG_VERIFY(front->typeInfo->isString(), context->report({front, Fmt(Err(Err0249), node->token.ctext(), front->typeInfo->getDisplayNameC())}));
        node->typeInfo = g_TypeMgr->typeInfoBool;
        node->setFlagsValueIsComputed();

        const auto& w      = front->computedValue->text;
        auto        module = node->sourceFile->module;
        bool        done   = false;

#define CHECK_SAFETY_NAME(__name, __flag)                                  \
    if (w == g_LangSpec->__name)                                           \
    {                                                                      \
        done                       = true;                                 \
        node->computedValue->reg.b = module->mustEmitSafety(node, __flag); \
    }

        CHECK_SAFETY_NAME(name_boundcheck, SAFETY_BOUNDCHECK);
        CHECK_SAFETY_NAME(name_overflow, SAFETY_OVERFLOW);
        CHECK_SAFETY_NAME(name_math, SAFETY_MATH);
        CHECK_SAFETY_NAME(name_switch, SAFETY_SWITCH);
        CHECK_SAFETY_NAME(name_unreachable, SAFETY_UNREACHABLE);
        CHECK_SAFETY_NAME(name_any, SAFETY_ANY);
        CHECK_SAFETY_NAME(name_bool, SAFETY_BOOL);
        CHECK_SAFETY_NAME(name_nan, SAFETY_NAN);
        CHECK_SAFETY_NAME(name_sanity, SAFETY_SANITY);

        if (!done)
        {
            auto note = Diagnostic::help(Hlp(Hlp0011));
            return context->report({front, front->token, Fmt(Err(Err0593), w.c_str())}, note);
        }

        return true;
    }

    case TokenId::IntrinsicHasTag:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(checkIsConstExpr(context, front->flags & AST_VALUE_COMPUTED, front, Err(Err0248), node->token.text));
        SWAG_VERIFY(front->typeInfo->isString(), context->report({front, Fmt(Err(Err0249), node->token.ctext(), front->typeInfo->getDisplayNameC())}));
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

        SWAG_CHECK(checkIsConstExpr(context, nameNode->flags & AST_VALUE_COMPUTED, nameNode, Err(Err0248), node->token.text));
        SWAG_VERIFY(!(nameNode->flags & AST_VALUE_IS_TYPEINFO), context->report({nameNode, Err(Err0245)}));
        SWAG_VERIFY(nameNode->typeInfo->isString(), context->report({nameNode, Fmt(Err(Err0249), node->token.ctext(), nameNode->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(!(defaultVal->flags & AST_VALUE_IS_TYPEINFO), context->report({defaultVal, Err(Err0283)}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, defaultVal->typeInfo, nullptr, defaultVal, CASTFLAG_DEFAULT));

        node->typeInfo = typeNode->typeInfo;
        node->setFlagsValueIsComputed();

        auto tag = g_Workspace->hasTag(nameNode->computedValue->text);
        if (tag)
        {
            if (!TypeManager::makeCompatibles(context, typeNode->typeInfo, tag->type, nullptr, typeNode, CASTFLAG_JUST_CHECK))
            {
                Diagnostic diag{typeNode, Fmt(Err(Err0252), typeNode->typeInfo->getDisplayNameC(), tag->type->getDisplayNameC(), tag->name.c_str())};
                auto       note      = Diagnostic::note(typeNode, Fmt(Nte(Nte0038), tag->cmdLine.c_str()));
                note->sourceFile     = nullptr;
                note->showSourceCode = false;
                return context->report(diag);
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
    default:
        break;
    }
    return true;
}

bool SemanticJob::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node)
{
    auto first     = node->childs.front();
    auto typeFirst = TypeManager::concreteType(first->typeInfo);

    // Check first parameter
    if (!typeFirst->isLambdaClosure())
        return context->report({node, Err(Err0784)});

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeFirst, TypeInfoKind::LambdaClosure);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
        return context->report({first, Fmt(Err(Err0785), SWAG_LIMIT_CB_MAX_PARAMS, typeFunc->parameters.size()), Diagnostic::isType(typeFunc)});
    if (typeFunc->numReturnRegisters() > 1)
        return context->report({first, Fmt(Err(Err0786), typeFunc->returnType->getDisplayNameC()), Diagnostic::isType(typeFunc)});

    node->typeInfo    = first->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeCallback;
    return true;
}

bool SemanticJob::resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* name)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Must start with a pointer of the same type as the slice
    if (!first->typeInfo->isPointer())
        return context->report({first, Fmt(Err(Err0787), name), Diagnostic::isType(first->typeInfo)});
    if (!first->typeInfo->isPointerArithmetic() && !first->typeInfo->isCString())
        return context->report({first, Fmt(Err(Err0711), name), Diagnostic::isType(first->typeInfo)});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, Fmt(Err(Err0788), name)});

    // Slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, second->typeInfo, nullptr, second, CASTFLAG_TRY_COERCE));

    // Create slice type
    auto ptrSlice         = makeType<TypeInfoSlice>();
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
    if (!first->typeInfo->isPointer())
        return context->report({first, Err(Err0789)});

    auto ptrPointer = CastTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, Err(Err0790)});

    // Check second parameter
    if (second->flags & AST_VALUE_IS_TYPEINFO)
    {
        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, second->typeInfo, nullptr, second, CASTFLAG_JUST_CHECK))
        {
            Diagnostic diag{first, Err(Err0791)};
            diag.hint = Diagnostic::isType(first->typeInfo);
            diag.addRange(second->token, Diagnostic::isType(second->typeInfo));
            return context->report(diag);
        }
    }

    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (context->result != ContextResult::Done)
        return true;
    if (!second->typeInfo->isPointerToTypeInfo())
        return context->report({node, Fmt(Err(Err0792), second->typeInfo->getDisplayNameC())});

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
    auto& typeGen    = module->typeGen;

    SWAG_CHECK(checkIsConcrete(context, first));

    third->allocateComputedValue();
    third->computedValue->storageSegment = getConstantSegFromContext(third);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, third->typeInfo, third->computedValue->storageSegment, &third->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    if (context->result != ContextResult::Done)
        return true;

    auto firstTypeInfo = TypeManager::concreteType(first->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(firstTypeInfo->isPointer() || firstTypeInfo->isStruct(), context->report({first, Err(Err0793)}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({second, Err(Err0794)}));
    auto thirdTypeInfo = TypeManager::concreteType(third->typeInfo, CONCRETE_ALIAS);
    SWAG_VERIFY(thirdTypeInfo->isInterface(), context->report({third, Err(Err0795)}));

    node->typeInfo = third->typeInfo;
    third->flags |= AST_NO_BYTECODE;

    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicMakeInterface;
    return true;
}

bool SemanticJob::resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = TypeManager::concreteType(expression->typeInfo);
    if (typeInfo->isString())
    {
        node->typeInfo    = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoU8, TYPEINFO_CONST | TYPEINFO_POINTER_ARITHMETIC);
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isSlice())
    {
        auto ptrSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto ptrFlags = TYPEINFO_POINTER_ARITHMETIC;
        if (ptrSlice->isConst())
            ptrFlags |= TYPEINFO_CONST;
        node->typeInfo = g_TypeMgr->makePointerTo(ptrSlice->pointedType, ptrFlags);
        node->typeInfo->forceComputeName();
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isArray())
    {
        auto ptrArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto ptrFlags = TYPEINFO_POINTER_ARITHMETIC;
        if (ptrArray->isConst())
            ptrFlags |= TYPEINFO_CONST;
        node->typeInfo = g_TypeMgr->makePointerTo(ptrArray->pointedType, ptrFlags);
        node->typeInfo->forceComputeName();
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isAny() || typeInfo->isInterface())
    {
        node->typeInfo    = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!(typeInfo->isTuple()), context->report({expression, Err(Err0796), Diagnostic::isType(typeInfo)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opData, nullptr, nullptr, node, nullptr));
        if (context->result == ContextResult::Pending)
            return true;
        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicDataOf;
    }
    else
    {
        return context->report({node, Fmt(Err(Err0797), typeInfo->getDisplayNameC())});
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
        else if (typeInfo->isString())
            node->computedValue->text = expr->computedValue->text;
        else if (typeInfo->isNative())
            node->computedValue->text = Ast::literalToString(typeInfo, *expr->computedValue);
        else if (typeInfo->isEnum())
            node->computedValue->text = Ast::enumToString(typeInfo, expr->computedValue->text, expr->computedValue->reg);
        else
            return context->report({expr, Err(Err0799)});
    }
    else if (expr->typeInfo->isCode())
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
        return context->report({expr, Err(Err0799)});
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
        return context->report({expr, Err(Err0803)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool SemanticJob::resolveIntrinsicRunes(SemanticContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    SWAG_CHECK(checkIsConstExpr(context, expr->flags & AST_VALUE_COMPUTED, expr));
    SWAG_VERIFY(typeInfo->isString(), context->report({expr, Fmt(Err(Err0084), typeInfo->getDisplayNameC())}));
    node->setFlagsValueIsComputed();

    // Convert
    Vector<uint32_t> runes;
    const char*      pz  = expr->computedValue->text.buffer;
    uint32_t         cpt = 0;
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
    auto typeInfo = TypeManager::concretePtrRef(expression->typeInfo);
    typeInfo      = TypeManager::concreteType(typeInfo, CONCRETE_FORCEALIAS);

    if (expression->resolvedSymbolName && expression->resolvedSymbolName->kind == SymbolKind::EnumValue)
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);

    if (typeInfo->isEnum())
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = typeEnum->values.size();
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        return true;
    }

    typeInfo = TypeManager::concretePtrRefType(typeInfo);
    if (typeInfo->isString())
    {
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, 0);

        node->typeInfo = g_TypeMgr->typeInfoU64;
        if (expression->flags & AST_VALUE_COMPUTED)
        {
            node->setFlagsValueIsComputed();
            node->computedValue->reg.u64 = expression->computedValue->text.length();
            if (node->computedValue->reg.u64 > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        }
    }
    else if (typeInfo->isArray())
    {
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, 0);

        node->setFlagsValueIsComputed();
        auto typeArray               = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue->reg.u64 = typeArray->count;
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    }
    else if (typeInfo->isSlice())
    {
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, 0);

        // :SliceLiteral
        // Slice literal. This can happen for enum values
        if (expression->flags & AST_VALUE_COMPUTED)
        {
            node->computedValue->reg.u64 = node->computedValue->reg.u64;
            if (node->computedValue->reg.u64 > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        }
        else
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
            node->typeInfo    = g_TypeMgr->typeInfoU64;
        }
    }
    else if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, 0);

        auto typeList = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = typeList->subTypes.size();
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    }
    else if (typeInfo->isVariadic() || typeInfo->isTypedVariadic())
    {
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
        node->typeInfo    = g_TypeMgr->typeInfoU64;
    }
    else if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!(typeInfo->isTuple()), context->report({expression, Err(Err0800), Diagnostic::isType(typeInfo)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, nullptr));
        if (context->result != ContextResult::Done)
            return true;
        node->typeInfo = g_TypeMgr->typeInfoU64;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCountOf;
    }
    else
    {
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, 0);
        node->inheritComputedValue(expression);
        node->typeInfo = expression->typeInfo;

        SWAG_VERIFY(typeInfo->isNativeInteger(), context->report({expression, Fmt(Err(Err0801), typeInfo->getDisplayNameC()), Diagnostic::isType(typeInfo)}));
        if (expression->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (expression->computedValue->reg.s8 < 0)
                        return context->report({expression, Fmt(Err(Err0802), node->computedValue->reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (expression->computedValue->reg.s16 < 0)
                        return context->report({expression, Fmt(Err(Err0802), node->computedValue->reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (expression->computedValue->reg.s32 < 0)
                        return context->report({expression, Fmt(Err(Err0802), node->computedValue->reg.s32)});
                    break;
                case NativeTypeKind::S64:
                    if (expression->computedValue->reg.s64 < 0)
                        return context->report({expression, Fmt(Err(Err0805), node->computedValue->reg.s64)});
                    break;
                default:
                    break;
                }
            }
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, typeInfo, nullptr, node, CASTFLAG_TRY_COERCE));
    }

    return true;
}

bool SemanticJob::resolveIntrinsicSpread(SemanticContext* context)
{
    auto node         = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr         = node->childs.front();
    auto typeInfo     = TypeManager::concreteType(expr->typeInfo);
    node->byteCodeFct = ByteCodeGenJob::emitIntrinsicSpread;

    SWAG_VERIFY(node->parent && node->parent->parent && node->parent->parent->kind == AstNodeKind::FuncCallParam, context->report({node, Err(Err0806)}));

    if (typeInfo->isArray())
    {
        auto typeArr   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->typeInfo = typeArr->pointedType;
    }
    else if (typeInfo->isSlice())
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        node->typeInfo = typeSlice->pointedType;
    }
    else if (typeInfo->isListArray())
    {
        auto typeList  = CastTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListArray);
        node->typeInfo = typeList->subTypes[0]->typeInfo;

        // Need to be sure that the expression list can be casted to the equivalent array
        auto typeArr         = makeType<TypeInfoArray>();
        typeArr->count       = (uint32_t) typeList->subTypes.size();
        typeArr->pointedType = typeList->subTypes[0]->typeInfo;
        typeArr->finalType   = typeArr->pointedType;
        typeArr->sizeOf      = typeArr->count * typeArr->finalType->sizeOf;
        typeArr->totalCount  = typeArr->count;
        typeArr->computeName();

        SWAG_CHECK(TypeManager::makeCompatibles(context, typeArr, typeList, nullptr, expr));

        Allocator::free(typeArr, sizeof(TypeInfoArray));
    }
    else
    {
        return context->report({expr, Fmt(Err(Err0807), typeInfo->getDisplayNameC())});
    }

    auto typeVar     = makeType<TypeInfoVariadic>(TypeInfoKind::TypedVariadic);
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
    auto& typeGen    = sourceFile->module->typeGen;

    // Will be runtime for an 'any' type
    if (expr->typeInfo->isAny() || expr->typeInfo->isInterface())
    {
        SWAG_CHECK(checkIsConcrete(context, expr));
        node->allocateComputedValue();
        node->computedValue->storageSegment = getConstantSegFromContext(node);
        SWAG_CHECK(typeGen.genExportedTypeInfo(context, expr->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->byteCodeFct = ByteCodeGenJob::emitIntrinsicKindOf;
        node->flags |= AST_R_VALUE;
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    // For a function, this is the unscoped type
    if (expr->typeInfo->isFuncAttr())
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo, GEN_EXPORTED_TYPE_FORCE_NO_SCOPE));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    // For an enum, this is the raw type
    if (expr->typeInfo->isEnum())
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(resolveTypeAsExpression(context, expr, typeEnum->rawType, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    // For an alias, this is the raw type
    if (expr->typeInfo->isAlias())
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(expr->typeInfo, TypeInfoKind::Alias);
        SWAG_CHECK(resolveTypeAsExpression(context, expr, typeAlias->rawType, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    SWAG_CHECK(resolveIntrinsicTypeOf(context));
    return true;
}

bool SemanticJob::resolveIntrinsicDeclType(SemanticContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, Err(Err0810)}));

    if (expr->flags & AST_CONST_EXPR)
    {
        SWAG_CHECK(executeCompilerNode(context, expr, true));
        if (context->result != ContextResult::Done)
            return true;
    }

    expr->flags |= AST_NO_BYTECODE;

    // @mktype on a typeinfo will give back the original compiler type
    if (typeInfo->isPointerToTypeInfo() &&
        expr->computedValue &&
        expr->computedValue->storageOffset != UINT32_MAX &&
        expr->computedValue->storageSegment != nullptr)
    {
        auto storageSegment = expr->computedValue->storageSegment;
        auto addr           = storageSegment->address(expr->computedValue->storageOffset);
        auto newTypeInfo    = context->sourceFile->module->typeGen.getRealType(storageSegment, (ExportedTypeInfo*) addr);
        if (newTypeInfo)
            typeInfo = newTypeInfo;
    }

    // Should be a lambda
    if (typeInfo->isFuncAttr() && !(typeInfo->flags & TYPEINFO_FUNC_IS_ATTR))
    {
        typeInfo         = typeInfo->clone();
        typeInfo->kind   = TypeInfoKind::LambdaClosure;
        typeInfo->sizeOf = sizeof(void*);
    }

    node->typeInfo = typeInfo;
    return true;
}

bool SemanticJob::resolveIntrinsicTypeOf(SemanticContext* context)
{
    auto node     = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto expr     = node->childs.front();
    auto typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, Err(Err0810)}));

    expr->flags |= AST_NO_BYTECODE;

    SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo));
    if (context->result != ContextResult::Done)
        return true;
    node->inheritComputedValue(expr);
    SWAG_CHECK(setupIdentifierRef(context, node));
    return true;
}

bool SemanticJob::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = CastAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);

    switch (node->tokenId)
    {
    case TokenId::IntrinsicSpread:
        SWAG_CHECK(resolveIntrinsicSpread(context));
        return true;

    case TokenId::IntrinsicIsConstExpr:
    {
        auto expr      = node->childs.front();
        node->typeInfo = g_TypeMgr->typeInfoBool;
        expr->flags |= AST_NO_BYTECODE;

        // Special case for a function parameter in a validif block, should be done at runtime
        if (expr->isValidIfParam(expr->resolvedSymbolOverload))
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicIsConstExprSI;
            break;
        }

        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = (expr->flags & AST_VALUE_COMPUTED);
        break;
    }

    case TokenId::IntrinsicSizeOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(!expr->typeInfo->isKindGeneric(), context->report({expr, Err(Err0812)}));
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = expr->typeInfo->sizeOf;
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicAlignOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(!expr->typeInfo->isKindGeneric(), context->report({expr, Err(Err0814)}));
        node->setFlagsValueIsComputed();
        node->computedValue->reg.u64 = TypeManager::alignOf(expr->typeInfo);
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
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
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
        break;
    }

    case TokenId::IntrinsicDeclType:
        SWAG_CHECK(resolveIntrinsicDeclType(context));
        return true;

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
        auto expr     = node->childs.front();
        auto typeInfo = TypeManager::concreteType(expr->typeInfo, CONCRETE_FORCEALIAS);
        if (!typeInfo->isEnum() && !typeInfo->isArray())
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
        if (!expr->typeInfo->isPointerTo(NativeTypeKind::U8))
            return context->report({expr, Err(Err0730), Diagnostic::isType(expr->typeInfo)});
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkstring"));
        node->typeInfo = g_TypeMgr->typeInfoString;
        break;
    }

    case TokenId::IntrinsicMakeCallback:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeCallback(context, node));
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
        SWAG_VERIFY(typeInfo->scopedName == "*Swag.CVaList", context->report({node, Fmt(Err(Err0048), typeInfo->getDisplayNameC())}));

        if (node->tokenId == TokenId::IntrinsicCVaStart)
        {
            SWAG_VERIFY(node->ownerFct && node->ownerFct->parameters && node->ownerFct->parameters->childs.size(), context->report({node, node->token, Err(Err0442)}));
            auto typeParam = node->ownerFct->parameters->childs.back()->typeInfo;
            SWAG_VERIFY(typeParam->isCVariadic(), context->report({node, node->token, Err(Err0442)}));
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCVaStart;
        }
        else if (node->tokenId == TokenId::IntrinsicCVaEnd)
        {
            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCVaEnd;
        }
        else
        {
            node->typeInfo = node->childs[1]->typeInfo;

            SWAG_VERIFY(node->typeInfo->numRegisters() == 1, context->report({node->childs[1], Fmt(Err(Err0443), node->typeInfo->getDisplayNameC())}));

            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::F32), context->report({node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "f64")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S8), context->report({node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "s32")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S16), context->report({node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "s32")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U8), context->report({node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "u32")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U16), context->report({node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "u32")}));
            SWAG_VERIFY(!node->typeInfo->isBool(), context->report({node->childs[1], Fmt(Err(Err0445), node->typeInfo->getDisplayNameC(), "u32")}));

            node->byteCodeFct = ByteCodeGenJob::emitIntrinsicCVaArg;
        }

        break;
    }
    default:
        break;
    }

    return true;
}
