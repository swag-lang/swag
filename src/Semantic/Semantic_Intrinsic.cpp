#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

bool Semantic::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node)
{
    auto       first     = node->firstChild();
    const auto typeFirst = TypeManager::concreteType(first->typeInfo);

    // Check first parameter
    if (!typeFirst->isLambdaClosure())
        return context->report({first, formErr(Err0560, typeFirst->getDisplayNameC())});

    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeFirst, TypeInfoKind::LambdaClosure);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
    {
        const Diagnostic err{first, formErr(Err0498, SWAG_LIMIT_CB_MAX_PARAMS, typeFunc->parameters.size())};
        return context->report(err, Diagnostic::hereIs(typeFunc->declNode));
    }

    if (typeFunc->numReturnRegisters() > 1)
    {
        const Diagnostic err{first, formErr(Err0605, typeFunc->returnType->getDisplayNameC())};
        return context->report(err, Diagnostic::hereIs(typeFunc->declNode));
    }

    node->typeInfo    = first->typeInfo;
    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeCallback;
    return true;
}

bool Semantic::resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, const char* name)
{
    auto       first  = node->firstChild();
    const auto second = node->lastChild();

    // Must start with a pointer of the same type as the slice
    if (!first->typeInfo->isPointer())
        return context->report({first, formErr(Err0557, name, first->typeInfo->getDisplayNameC())});
    if (!first->typeInfo->isPointerArithmetic() && !first->typeInfo->isCString())
        return context->report({first, formErr(Err0556, name, first->typeInfo->getDisplayNameC())});

    const auto ptrPointer = castTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    SWAG_ASSERT(ptrPointer->pointedType);

    // Slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, second->typeInfo, nullptr, second, CAST_FLAG_TRY_COERCE));

    // Create slice type
    const auto ptrSlice   = makeType<TypeInfoSlice>();
    ptrSlice->pointedType = ptrPointer->pointedType;
    if (ptrPointer->isConst())
        ptrSlice->addFlag(TYPEINFO_CONST);
    ptrSlice->computeName();
    node->typeInfo = ptrSlice;

    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeSlice;
    return true;
}

bool Semantic::resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node)
{
    auto first  = node->firstChild();
    auto second = node->lastChild();

    // Check first parameter
    if (!first->typeInfo->isPointer())
        return context->report({first, formErr(Err0557, node->token.cstr(), first->typeInfo->getDisplayNameC())});

    const auto ptrPointer = castTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, toErr(Err0156)});

    // Check second parameter
    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (second->isConstantGenTypeInfo())
    {
        const auto genType  = second->getConstantGenTypeInfo();
        const auto realType = context->sourceFile->module->typeGen.getRealType(second->computedValue()->storageSegment, genType);

        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, realType, nullptr, second, CAST_FLAG_JUST_CHECK))
        {
            const auto typePointer = castTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
            Diagnostic err{first, formErr(Err0539, first->typeInfo->getDisplayNameC(), realType->getDisplayNameC(), realType->getDisplayNameC(), typePointer->pointedType->getDisplayNameC())};
            err.addNote(second->token, Diagnostic::isType(realType));
            return context->report(err);
        }
    }

    YIELD();

    if (!second->typeInfo->isPointerToTypeInfo())
        return context->report({second, formErr(Err0552, second->typeInfo->getDisplayNameC())});

    node->typeInfo    = g_TypeMgr->typeInfoAny;
    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeAny;
    return true;
}

bool Semantic::resolveIntrinsicMakeInterface(SemanticContext* context)
{
    const auto node   = context->node;
    const auto params = node->firstChild();

    auto       first      = params->firstChild();
    auto       second     = params->secondChild();
    auto       third      = params->children[2];
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;
    auto&      typeGen    = module->typeGen;

    SWAG_CHECK(checkIsConcrete(context, first));
    SWAG_CHECK(checkIsConcreteOrType(context, second));
    YIELD();

    third->allocateComputedValue();
    third->computedValue()->storageSegment = getConstantSegFromContext(third);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, third->typeInfo, third->computedValue()->storageSegment, &third->computedValue()->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    YIELD();

    const auto firstTypeInfo = first->typeInfo->getConcreteAlias();
    SWAG_VERIFY(firstTypeInfo->isPointer() || firstTypeInfo->isStruct(), context->report({first, formErr(Err0558, firstTypeInfo->getDisplayNameC())}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({second, formErr(Err0553, second->typeInfo->getDisplayNameC())}));
    const auto thirdTypeInfo = third->typeInfo->getConcreteAlias();
    SWAG_VERIFY(thirdTypeInfo->isInterface(), context->report({third, formErr(Err0564, thirdTypeInfo->getDisplayNameC())}));

    node->typeInfo = third->typeInfo;
    third->addAstFlag(AST_NO_BYTECODE);

    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeInterface;
    return true;
}

bool Semantic::resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = TypeManager::concretePtrRef(expression->typeInfo);
    typeInfo      = typeInfo->getConcreteAlias();

    const auto symbolName = expression->resolvedSymbolName();
    if (symbolName && symbolName->is(SymbolKind::EnumValue))
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);
    else if (symbolName && symbolName->is(SymbolKind::Variable))
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);

    if (typeInfo->isEnum())
    {
        const auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        node->setFlagsValueIsComputed();
        node->computedValue()->reg.u64 = typeEnum->values.size();
        if (node->computedValue()->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedBinHex;
        return true;
    }

    typeInfo = TypeManager::concretePtrRefType(typeInfo);
    if (typeInfo->isString())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);
        node->typeInfo       = g_TypeMgr->typeInfoU64;
        if (expression->hasFlagComputedValue())
        {
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = expression->computedValue()->text.length();
            if (node->computedValue()->reg.u64 > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedBinHex;
        }
        else
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
        }
    }
    else if (typeInfo->isArray())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);
        node->setFlagsValueIsComputed();
        const auto typeArray           = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue()->reg.u64 = typeArray->count;
        if (node->computedValue()->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedBinHex;
    }
    else if (typeInfo->isSlice())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        // @SliceLiteral
        // Slice literal. This can happen for enum values
        if (expression->hasFlagComputedValue())
        {
            const auto slice = static_cast<SwagSlice*>(node->computedValue()->getStorageAddr());
            if (slice->count > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedBinHex;
            node->computedValue()->reg.u64 = slice->count;
        }
        else
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
            node->typeInfo    = g_TypeMgr->typeInfoU64;
        }
    }
    else if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);
        const auto typeList  = castTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListTuple, TypeInfoKind::TypeListArray);
        node->setFlagsValueIsComputed();
        node->computedValue()->reg.u64 = typeList->subTypes.size();
        if (node->computedValue()->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedBinHex;
    }
    else if (typeInfo->isVariadic() || typeInfo->isTypedVariadic())
    {
        node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
        node->typeInfo    = g_TypeMgr->typeInfoU64;
    }
    else if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({expression, toErr(Err0545)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opCount, nullptr, nullptr, node, nullptr));
        YIELD();
        node->typeInfo = g_TypeMgr->typeInfoU64;
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
    }
    else
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);
        node->inheritComputedValue(expression);
        node->typeInfo = expression->typeInfo;

        SWAG_VERIFY(typeInfo->isNativeInteger(), context->report({expression, formErr(Err0562, typeInfo->getDisplayNameC())}));
        if (expression->hasFlagComputedValue())
        {
            if (!typeInfo->hasFlag(TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                    case NativeTypeKind::S8:
                        if (expression->computedValue()->reg.s8 < 0)
                            return context->report({expression, formErr(Err0160, node->computedValue()->reg.s8)});
                        break;
                    case NativeTypeKind::S16:
                        if (expression->computedValue()->reg.s16 < 0)
                            return context->report({expression, formErr(Err0160, node->computedValue()->reg.s16)});
                        break;
                    case NativeTypeKind::S32:
                        if (expression->computedValue()->reg.s32 < 0)
                            return context->report({expression, formErr(Err0160, node->computedValue()->reg.s32)});
                        break;
                    case NativeTypeKind::S64:
                        if (expression->computedValue()->reg.s64 < 0)
                            return context->report({expression, formErr(Err0159, node->computedValue()->reg.s64)});
                        break;
                    default:
                        break;
                }
            }
        }
        else if (!node->byteCodeFct)
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, typeInfo, nullptr, node, CAST_FLAG_TRY_COERCE));
    }

    return true;
}

bool Semantic::resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = TypeManager::concretePtrRefType(expression->typeInfo);

    if (typeInfo->isListArray())
    {
        const auto typeList  = castTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListArray);
        const auto typeArray = TypeManager::convertTypeListToArray(context, typeList, expression->hasAstFlag(AST_CONST_EXPR));
        SWAG_CHECK(TypeManager::makeCompatibles(context, typeArray, nullptr, expression, CAST_FLAG_DEFAULT));
        typeInfo = typeArray;
    }

    if (typeInfo->isString())
    {
        // @ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoU8, TYPEINFO_CONST | TYPEINFO_POINTER_ARITHMETIC);
        if (expression->hasFlagComputedValue())
        {
            node->setFlagsValueIsComputed();
            if (expression->computedValue()->text.buffer == nullptr)
            {
                node->typeInfo                        = g_TypeMgr->typeInfoNull;
                node->computedValue()->storageSegment = nullptr;
                node->computedValue()->storageOffset  = UINT32_MAX;
            }
            else
            {
                node->computedValue()->storageSegment = getConstantSegFromContext(node);
                node->computedValue()->storageOffset  = node->computedValue()->storageSegment->addString(expression->computedValue()->text);
            }
        }
        else
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
        }
    }
    else if (typeInfo->isSlice())
    {
        // @ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        const auto    ptrSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        TypeInfoFlags ptrFlags = TYPEINFO_POINTER_ARITHMETIC;
        if (ptrSlice->isConst())
            ptrFlags.add(TYPEINFO_CONST);
        node->typeInfo = g_TypeMgr->makePointerTo(ptrSlice->pointedType, ptrFlags);

        if (expression->hasFlagComputedValue())
        {
            node->inheritComputedValue(expression);
            const auto slice = static_cast<SwagSlice*>(node->computedValue()->getStorageAddr());
            if (!slice->buffer)
            {
                node->typeInfo                        = g_TypeMgr->typeInfoNull;
                node->computedValue()->storageSegment = nullptr;
                node->computedValue()->storageOffset  = UINT32_MAX;
            }
            else
            {
                node->computedValue()->storageOffset = node->computedValue()->storageSegment->offset(static_cast<uint8_t*>(slice->buffer));
            }
        }
        else
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
        }
    }
    else if (typeInfo->isArray())
    {
        // @ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        const auto    ptrArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        TypeInfoFlags ptrFlags = TYPEINFO_POINTER_ARITHMETIC;
        if (ptrArray->isConst())
            ptrFlags.add(TYPEINFO_CONST);
        node->typeInfo = g_TypeMgr->makePointerTo(ptrArray->pointedType, ptrFlags);

        if (expression->hasFlagComputedValue())
        {
            node->inheritComputedValue(expression);
            if (!expression->computedValue()->storageSegment)
            {
                node->typeInfo                        = g_TypeMgr->typeInfoNull;
                node->computedValue()->storageSegment = nullptr;
                node->computedValue()->storageOffset  = UINT32_MAX;
            }
        }
        else
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
        }
    }
    else if (typeInfo->isAny())
    {
        // @ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        TypeInfoFlags ptrFlags = 0;
        if (expression->hasAstFlag(AST_CONST_EXPR))
            ptrFlags.add(TYPEINFO_CONST);
        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid, ptrFlags);

        if (expression->hasFlagComputedValue())
        {
            node->inheritComputedValue(expression);
            SWAG_ASSERT(expression->hasFlagComputedValue());

            if (!expression->computedValue()->storageSegment)
            {
                node->typeInfo                        = g_TypeMgr->typeInfoNull;
                node->computedValue()->storageSegment = nullptr;
                node->computedValue()->storageOffset  = UINT32_MAX;
            }
            else
            {
                const auto any = static_cast<SwagAny*>(expression->computedValue()->getStorageAddr());
                if (!any->value)
                {
                    node->typeInfo                        = g_TypeMgr->typeInfoNull;
                    node->computedValue()->storageSegment = nullptr;
                    node->computedValue()->storageOffset  = UINT32_MAX;
                }
                else
                {
                    node->computedValue()->storageOffset = node->computedValue()->storageSegment->offset(static_cast<uint8_t*>(any->value));
                }
            }
        }
        else
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
        }
    }
    else if (typeInfo->isInterface())
    {
        // @ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        node->typeInfo    = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
    }
    else if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({expression, toErr(Err0546)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opData, nullptr, nullptr, node, nullptr));
        YIELD();
        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
    }
    else
    {
        return context->report({expression, formErr(Err0547, typeInfo->getDisplayNameC())});
    }

    return true;
}

bool Semantic::resolveIntrinsicKindOf(SemanticContext* context)
{
    const auto node       = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto expr       = node->firstChild();
    const auto sourceFile = context->sourceFile;
    auto&      typeGen    = sourceFile->module->typeGen;

    // Will be runtime for an 'any' type
    if (expr->typeInfo->isAny())
    {
        SWAG_CHECK(checkIsConcrete(context, expr));

        if (expr->hasFlagComputedValue())
        {
            const auto any                       = static_cast<SwagAny*>(expr->computedValue()->getStorageAddr());
            expr->computedValue()->storageOffset = expr->computedValue()->storageSegment->offset(reinterpret_cast<uint8_t*>(any->type));
            node->inheritComputedValue(expr);
            node->addAstFlag(AST_VALUE_GEN_TYPEINFO);
            node->typeInfo = g_TypeMgr->typeInfoTypeType;
            setConst(node);
            setIdentifierRefPrevious(node);
        }
        else
        {
            node->allocateComputedValue();
            node->computedValue()->storageSegment = getConstantSegFromContext(node);
            SWAG_CHECK(typeGen.genExportedTypeInfo(context, expr->typeInfo, node->computedValue()->storageSegment, &node->computedValue()->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node->typeInfo));
            YIELD();
            node->byteCodeFct = ByteCodeGen::emitIntrinsicKindOf;
            node->addAstFlag(AST_R_VALUE);
            setConst(node);
            setIdentifierRefPrevious(node);
        }

        return true;
    }

    // Will be runtime for an 'interface' type
    if (expr->typeInfo->isInterface())
    {
        SWAG_CHECK(checkIsConcrete(context, expr));
        node->allocateComputedValue();
        node->computedValue()->storageSegment = getConstantSegFromContext(node);
        SWAG_CHECK(typeGen.genExportedTypeInfo(context, expr->typeInfo, node->computedValue()->storageSegment, &node->computedValue()->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node->typeInfo));
        YIELD();
        node->byteCodeFct = ByteCodeGen::emitIntrinsicKindOf;
        node->addAstFlag(AST_R_VALUE);
        setConst(node);
        setIdentifierRefPrevious(node);
        return true;
    }

    const Diagnostic err{node, node->token, formErr(Err0318, expr->typeInfo->getDisplayNameC())};
    return context->report(err);
}

bool Semantic::resolveIntrinsicVarArgs(SemanticContext* context)
{
    auto       node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto typeInfo = node->firstChild()->typeInfo;
    typeInfo->computeScopedName();
    SWAG_VERIFY(typeInfo->scopedName == "*Swag.CVaList", context->report({node->firstChild(), formErr(Err0536, "*Swag.CVaList", typeInfo->getDisplayNameC())}));
    node->addAstFlag(AST_SIDE_EFFECTS);

    if (node->token.is(TokenId::IntrinsicCVaStart))
    {
        SWAG_VERIFY(node->ownerFct && node->ownerFct->parameters && !node->ownerFct->parameters->children.empty(), context->report({node, node->token, toErr(Err0317)}));
        const auto typeParam = node->ownerFct->parameters->lastChild()->typeInfo;
        SWAG_VERIFY(typeParam->isCVariadic(), context->report({node, node->token, toErr(Err0317)}));
        node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaStart;
    }
    else if (node->token.is(TokenId::IntrinsicCVaEnd))
    {
        node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaEnd;
    }
    else
    {
        node->typeInfo = node->secondChild()->typeInfo;

        SWAG_VERIFY(node->typeInfo->numRegisters() == 1, context->report({node->secondChild(), formErr(Err0257, node->typeInfo->getDisplayNameC())}));

        SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::F32), context->report({node->secondChild(), formErr(Err0126, node->typeInfo->getDisplayNameC(), "f64")}));
        SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S8), context->report({node->secondChild(), formErr(Err0126, node->typeInfo->getDisplayNameC(), "s32")}));
        SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S16), context->report({node->secondChild(), formErr(Err0126, node->typeInfo->getDisplayNameC(), "s32")}));
        SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U8), context->report({node->secondChild(), formErr(Err0126, node->typeInfo->getDisplayNameC(), "u32")}));
        SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U16), context->report({node->secondChild(), formErr(Err0126, node->typeInfo->getDisplayNameC(), "u32")}));
        SWAG_VERIFY(!node->typeInfo->isBool(), context->report({node->secondChild(), formErr(Err0126, node->typeInfo->getDisplayNameC(), "u32")}));

        node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaArg;
    }

    return true;
}

bool Semantic::resolveIntrinsicProperty(SemanticContext* context)
{
    const auto node = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    switch (node->token.id)
    {
        case TokenId::CompilerIsConstExpr:
            SWAG_CHECK(resolveCompilerIsConstExpr(context));
            return true;

        case TokenId::CompilerSizeOf:
            SWAG_CHECK(resolveCompilerSizeOf(context));
            return true;

        case TokenId::CompilerAlignOf:
            SWAG_CHECK(resolveCompilerAlignOf(context));
            return true;

        case TokenId::CompilerOffsetOf:
            SWAG_CHECK(resolveCompilerOffsetOf(context));
            return true;

        case TokenId::CompilerDeclType:
            SWAG_CHECK(resolveCompilerDeclType(context));
            return true;

        case TokenId::CompilerTypeOf:
            SWAG_CHECK(resolveCompilerTypeOf(context));
            return true;

        case TokenId::CompilerStringOf:
            SWAG_CHECK(resolveCompilerStringOf(context));
            return true;
        case TokenId::CompilerNameOf:
            SWAG_CHECK(resolveCompilerNameOf(context));
            return true;

        case TokenId::CompilerRunes:
            SWAG_CHECK(resolveCompilerRunes(context));
            return true;

        case TokenId::IntrinsicKindOf:
            SWAG_CHECK(resolveIntrinsicKindOf(context));
            return true;

        case TokenId::IntrinsicCountOf:
        {
            const auto expr     = node->firstChild();
            const auto typeInfo = expr->typeInfo->getConcreteAlias();
            if (!typeInfo->isEnum() && !typeInfo->isArray())
                SWAG_CHECK(checkIsConcrete(context, expr));
            node->inheritComputedValue(expr);
            SWAG_CHECK(resolveIntrinsicCountOf(context, node, expr));
            break;
        }

        case TokenId::IntrinsicDataOf:
        {
            const auto expr = node->firstChild();
            SWAG_CHECK(checkIsConcrete(context, expr));
            SWAG_CHECK(resolveIntrinsicDataOf(context, node, expr));
            break;
        }

        case TokenId::IntrinsicMakeAny:
        {
            const auto expr = node->firstChild();
            SWAG_CHECK(checkIsConcrete(context, expr));
            SWAG_CHECK(resolveIntrinsicMakeAny(context, node));
            break;
        }

        case TokenId::IntrinsicMakeSlice:
        {
            const auto expr = node->firstChild();
            SWAG_CHECK(checkIsConcrete(context, expr));
            SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, "@mkslice"));
            break;
        }

        case TokenId::IntrinsicMakeString:
        {
            auto expr = node->firstChild();
            SWAG_CHECK(checkIsConcrete(context, expr));
            if (!expr->typeInfo->isPointerTo(NativeTypeKind::U8))
                return context->report({expr, formErr(Err0559, expr->typeInfo->getDisplayNameC())});
            SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, "@mkstring"));
            node->typeInfo = g_TypeMgr->typeInfoString;
            break;
        }

        case TokenId::IntrinsicMakeCallback:
        {
            const auto expr = node->firstChild();
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
            SWAG_CHECK(resolveIntrinsicVarArgs(context));
            break;
    }

    return true;
}
