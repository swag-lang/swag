#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"

#define CHECK_SAFETY_NAME(__name, __flag)                                        \
    do                                                                           \
    {                                                                            \
        if (w == g_LangSpec->__name)                                             \
        {                                                                        \
            done                         = true;                                 \
            node->computedValue()->reg.b = module->mustEmitSafety(node, __flag); \
        }                                                                        \
    } while (0)

bool Semantic::resolveIntrinsicTag(SemanticContext* context)
{
    const auto node = context->node;
    switch (node->token.id)
    {
        case TokenId::IntrinsicSafety:
        {
            auto front = node->firstChild();
            SWAG_CHECK(evaluateConstExpression(context, front));
            YIELD();
            SWAG_CHECK(checkIsConstExpr(context, front->hasFlagComputedValue(), front, toErr(Err0031), node->token.text));
            SWAG_VERIFY(front->typeInfo->isString(), context->report({front, formErr(Err0200, node->token.c_str(), front->typeInfo->getDisplayNameC())}));
            node->typeInfo = g_TypeMgr->typeInfoBool;
            node->setFlagsValueIsComputed();

            const auto& w      = front->computedValue()->text;
            const auto  module = node->token.sourceFile->module;
            bool        done   = false;

            CHECK_SAFETY_NAME(name_boundcheck, SAFETY_BOUND_CHECK);
            CHECK_SAFETY_NAME(name_overflow, SAFETY_OVERFLOW);
            CHECK_SAFETY_NAME(name_math, SAFETY_MATH);
            CHECK_SAFETY_NAME(name_switch, SAFETY_SWITCH);
            CHECK_SAFETY_NAME(name_unreachable, SAFETY_UNREACHABLE);
            CHECK_SAFETY_NAME(name_any, SAFETY_ANY);
            CHECK_SAFETY_NAME(name_bool, SAFETY_BOOL);
            CHECK_SAFETY_NAME(name_nan, SAFETY_NAN);
            CHECK_SAFETY_NAME(name_sanity, SAFETY_SANITY);
            CHECK_SAFETY_NAME(name_null, SAFETY_NULL);

            if (!done)
            {
                return context->report({front, front->token, formErr(Err0373, w.c_str())});
            }

            return true;
        }

        case TokenId::IntrinsicHasTag:
        {
            auto front = node->firstChild();
            SWAG_CHECK(evaluateConstExpression(context, front));
            YIELD();
            SWAG_CHECK(checkIsConstExpr(context, front->hasFlagComputedValue(), front, toErr(Err0031), node->token.text));
            SWAG_VERIFY(front->typeInfo->isString(), context->report({front, formErr(Err0200, node->token.c_str(), front->typeInfo->getDisplayNameC())}));
            const auto tag = g_Workspace->hasTag(front->computedValue()->text);
            node->typeInfo = g_TypeMgr->typeInfoBool;
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.b = tag ? true : false;
            return true;
        }

        case TokenId::IntrinsicGetTag:
        {
            auto       nameNode   = node->firstChild();
            const auto typeNode   = node->secondChild();
            auto       defaultVal = node->children[2];
            SWAG_CHECK(evaluateConstExpression(context, nameNode));
            YIELD();
            SWAG_CHECK(evaluateConstExpression(context, defaultVal));
            YIELD();

            SWAG_CHECK(checkIsConstExpr(context, nameNode->hasFlagComputedValue(), nameNode, toErr(Err0031), node->token.text));
            SWAG_VERIFY(nameNode->typeInfo->isString(), context->report({nameNode, formErr(Err0200, node->token.c_str(), nameNode->typeInfo->getDisplayNameC())}));
            SWAG_VERIFY(defaultVal->hasComputedValue(), context->report({defaultVal, formErr(Err0201, typeNode->typeInfo->getDisplayNameC())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, defaultVal->typeInfo, nullptr, defaultVal, CAST_FLAG_DEFAULT));

            node->setFlagsValueIsComputed();
            node->typeInfo = typeNode->typeInfo;

            const auto tag = g_Workspace->hasTag(nameNode->computedValue()->text);
            if (tag)
            {
                if (!TypeManager::makeCompatibles(context, typeNode->typeInfo, tag->type, nullptr, typeNode, CAST_FLAG_JUST_CHECK))
                {
                    Diagnostic err{typeNode, formErr(Err0659, typeNode->typeInfo->getDisplayNameC(), tag->type->getDisplayNameC(), tag->name.c_str())};
                    err.addNote(formNte(Nte0022, tag->cmdLine.c_str()));
                    return context->report(err);
                }

                node->typeInfo         = tag->type;
                *node->computedValue() = tag->value;
            }
            else
            {
                node->extSemantic()->computedValue = defaultVal->extSemantic()->computedValue;
            }

            return true;
        }
    }
    return true;
}

bool Semantic::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node)
{
    auto       first     = node->firstChild();
    const auto typeFirst = TypeManager::concreteType(first->typeInfo);

    // Check first parameter
    if (!typeFirst->isLambdaClosure())
        return context->report({first, formErr(Err0204, typeFirst->getDisplayNameC())});

    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeFirst, TypeInfoKind::LambdaClosure);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
    {
        const Diagnostic err{first, formErr(Err0745, SWAG_LIMIT_CB_MAX_PARAMS, typeFunc->parameters.size())};
        return context->report(err, Diagnostic::hereIs(typeFunc->declNode));
    }

    if (typeFunc->numReturnRegisters() > 1)
    {
        const Diagnostic err{first, formErr(Err0744, typeFunc->returnType->getDisplayNameC())};
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
        return context->report({first, formErr(Err0198, name, first->typeInfo->getDisplayNameC())});
    if (!first->typeInfo->isPointerArithmetic() && !first->typeInfo->isCString())
        return context->report({first, formErr(Err0197, name, first->typeInfo->getDisplayNameC())});

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
        return context->report({first, formErr(Err0198, node->token.c_str(), first->typeInfo->getDisplayNameC())});

    const auto ptrPointer = castTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, toErr(Err0202)});

    // Check second parameter
    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (second->isConstantGenTypeInfo())
    {
        const auto genType  = second->getConstantGenTypeInfo();
        const auto realType = context->sourceFile->module->typeGen.getRealType(second->computedValue()->storageSegment, genType);

        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, realType, nullptr, second, CAST_FLAG_JUST_CHECK))
        {
            Diagnostic err{first, formErr(Err0006, first->typeInfo->getDisplayNameC(), realType->getDisplayNameC())};
            err.addNote(second->token, Diagnostic::isType(realType));
            return context->report(err);
        }
    }

    YIELD();

    if (!second->typeInfo->isPointerToTypeInfo())
        return context->report({second, formErr(Err0203, second->typeInfo->getDisplayNameC())});

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
    SWAG_VERIFY(firstTypeInfo->isPointer() || firstTypeInfo->isStruct(), context->report({first, formErr(Err0205, firstTypeInfo->getDisplayNameC())}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({second, formErr(Err0206, second->typeInfo->getDisplayNameC())}));
    const auto thirdTypeInfo = third->typeInfo->getConcreteAlias();
    SWAG_VERIFY(thirdTypeInfo->isInterface(), context->report({third, formErr(Err0207, thirdTypeInfo->getDisplayNameC())}));

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
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
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
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
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
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    }
    else if (typeInfo->isSlice())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        // :SliceLiteral
        // Slice literal. This can happen for enum values
        if (expression->hasFlagComputedValue())
        {
            const auto slice = static_cast<SwagSlice*>(node->computedValue()->getStorageAddr());
            if (slice->count > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
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
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    }
    else if (typeInfo->isVariadic() || typeInfo->isTypedVariadic())
    {
        node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
        node->typeInfo    = g_TypeMgr->typeInfoU64;
    }
    else if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({expression, toErr(Err0749)}));
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

        SWAG_VERIFY(typeInfo->isNativeInteger(), context->report({expression, formErr(Err0149, typeInfo->getDisplayNameC())}));
        if (expression->hasFlagComputedValue())
        {
            if (!typeInfo->hasFlag(TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                    case NativeTypeKind::S8:
                        if (expression->computedValue()->reg.s8 < 0)
                            return context->report({expression, formErr(Err0148, node->computedValue()->reg.s8)});
                        break;
                    case NativeTypeKind::S16:
                        if (expression->computedValue()->reg.s16 < 0)
                            return context->report({expression, formErr(Err0148, node->computedValue()->reg.s16)});
                        break;
                    case NativeTypeKind::S32:
                        if (expression->computedValue()->reg.s32 < 0)
                            return context->report({expression, formErr(Err0148, node->computedValue()->reg.s32)});
                        break;
                    case NativeTypeKind::S64:
                        if (expression->computedValue()->reg.s64 < 0)
                            return context->report({expression, formErr(Err0147, node->computedValue()->reg.s64)});
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
        // :ConcreteRef
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
        // :ConcreteRef
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
        // :ConcreteRef
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
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
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
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        node->typeInfo    = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
    }
    else if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({expression, toErr(Err0750)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opData, nullptr, nullptr, node, nullptr));
        YIELD();
        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
    }
    else
    {
        return context->report({expression, formErr(Err0751, typeInfo->getDisplayNameC())});
    }

    return true;
}

bool Semantic::resolveIntrinsicStringOf(SemanticContext* context)
{
    const auto node     = context->node;
    auto       expr     = node->firstChild();
    const auto typeInfo = expr->typeInfo;

    node->setFlagsValueIsComputed();
    SWAG_CHECK(checkIsConcreteOrType(context, expr, true));
    YIELD();

    if (expr->hasFlagComputedValue())
    {
        if (expr->isConstantGenTypeInfo())
            node->computedValue()->text = Utf8{expr->getConstantGenTypeInfo()->fullName};
        else if (typeInfo->isString())
            node->computedValue()->text = expr->computedValue()->text;
        else if (typeInfo->isNative())
            node->computedValue()->text = Ast::literalToString(typeInfo, *expr->computedValue());
        else if (typeInfo->isEnum())
            node->computedValue()->text = Ast::enumToString(typeInfo, expr->computedValue()->text, expr->computedValue()->reg);
        else
            return context->report({expr, toErr(Err0742)});
    }
    else if (expr->typeInfo->isCode())
    {
        FormatAst     fmtAst;
        FormatContext fmtCxt;
        const auto    typeCode = castTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
        fmtAst.outputNode(fmtCxt, typeCode->content);
        node->computedValue()->text = fmtAst.getUtf8();
    }
    else if (expr->resolvedSymbolName())
        node->computedValue()->text = expr->resolvedSymbolName()->getFullName();
    else if (expr->resolvedSymbolOverload())
        node->computedValue()->text = expr->resolvedSymbolOverload()->symbol->getFullName();
    else
        return context->report({expr, toErr(Err0742)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveIntrinsicNameOf(SemanticContext* context)
{
    const auto node = context->node;
    auto       expr = node->firstChild();

    node->setFlagsValueIsComputed();
    SWAG_CHECK(checkIsConcreteOrType(context, expr, true));
    YIELD();

    if (expr->isConstantGenTypeInfo())
        node->computedValue()->text = Utf8{expr->getConstantGenTypeInfo()->name};
    else if (expr->resolvedSymbolName())
        node->computedValue()->text = expr->resolvedSymbolName()->name;
    else if (expr->resolvedSymbolOverload())
        node->computedValue()->text = expr->resolvedSymbolOverload()->symbol->name;
    else
        return context->report({expr, toErr(Err0741)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveIntrinsicRunes(SemanticContext* context)
{
    const auto node     = context->node;
    auto       expr     = node->firstChild();
    const auto typeInfo = expr->typeInfo;

    SWAG_CHECK(checkIsConstExpr(context, expr->hasFlagComputedValue(), expr));
    SWAG_VERIFY(typeInfo->isString(), context->report({expr, formErr(Err0209, typeInfo->getDisplayNameC())}));
    node->setFlagsValueIsComputed();

    // Convert
    Vector<uint32_t> runes;
    const char*      pz  = expr->computedValue()->text.buffer;
    uint32_t         cpt = 0;
    while (cpt < expr->computedValue()->text.count)
    {
        uint32_t c;
        uint32_t offset;
        pz = Utf8::decodeUtf8(pz, c, offset);
        runes.push_back(c);
        cpt += offset;
    }

    // :SliceLiteral
    const auto storageSegment             = getConstantSegFromContext(context->node);
    node->computedValue()->storageSegment = storageSegment;

    SwagSlice* slice;
    node->computedValue()->storageOffset = storageSegment->reserve(sizeof(SwagSlice), reinterpret_cast<uint8_t**>(&slice));
    slice->count                         = runes.size();

    uint8_t* addrDst;
    storageSegment->reserve(runes.size() * sizeof(uint32_t), &addrDst);
    slice->buffer = addrDst;

    // Setup array
    std::copy_n(runes.data(), runes.size(), reinterpret_cast<uint32_t*>(addrDst));

    node->typeInfo = g_TypeMgr->typeInfoSliceRunes;
    return true;
}

bool Semantic::resolveIntrinsicSpread(SemanticContext* context)
{
    auto       node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->firstChild();
    const auto typeInfo = TypeManager::concreteType(expr->typeInfo);
    node->byteCodeFct   = ByteCodeGen::emitIntrinsicSpread;

    if (!node->parent || !node->parent->parent || node->parent->parent->isNot(AstNodeKind::FuncCallParam))
    {
        if (node->parent->parent->is(AstNodeKind::Cast) || node->parent->parent->is(AstNodeKind::AutoCast))
        {
            const Diagnostic err{node->parent->parent, node->parent->parent->token, toErr(Err0498)};
            return context->report(err);
        }

        const Diagnostic err{node, node->token, toErr(Err0454)};
        return context->report(err);
    }

    if (typeInfo->isArray())
    {
        const auto typeArr = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->typeInfo     = typeArr->pointedType;
    }
    else if (typeInfo->isSlice())
    {
        const auto typeSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        node->typeInfo       = typeSlice->pointedType;
    }
    else if (typeInfo->isListArray())
    {
        const auto typeList = castTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListArray);
        node->typeInfo      = typeList->subTypes[0]->typeInfo;

        // Need to be sure that the expression list can be casted to the equivalent array
        const auto typeArr   = makeType<TypeInfoArray>();
        typeArr->count       = typeList->subTypes.size();
        typeArr->pointedType = typeList->subTypes[0]->typeInfo;
        typeArr->finalType   = typeArr->pointedType;
        typeArr->sizeOf      = typeArr->count * typeArr->finalType->sizeOf;
        typeArr->totalCount  = typeArr->count;
        typeArr->setConst();
        typeArr->computeName();

        SWAG_CHECK(TypeManager::makeCompatibles(context, typeArr, typeList, nullptr, expr));

        Allocator::free(typeArr, sizeof(TypeInfoArray));
    }
    else
    {
        return context->report({expr, formErr(Err0390, typeInfo->getDisplayNameC())});
    }

    const auto typeVar = makeType<TypeInfoVariadic>(TypeInfoKind::TypedVariadic);
    typeVar->rawType   = node->typeInfo;
    typeVar->computeName();
    node->typeInfo = typeVar;
    node->typeInfo->addFlag(TYPEINFO_SPREAD);

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
            SWAG_CHECK(setupIdentifierRef(context, node));
        }
        else
        {
            node->allocateComputedValue();
            node->computedValue()->storageSegment = getConstantSegFromContext(node);
            SWAG_CHECK(typeGen.genExportedTypeInfo(context, expr->typeInfo, node->computedValue()->storageSegment, &node->computedValue()->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node->typeInfo));
            YIELD();
            node->byteCodeFct = ByteCodeGen::emitIntrinsicKindOf;
            node->addAstFlag(AST_R_VALUE);
            SWAG_CHECK(setupIdentifierRef(context, node));
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
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    // For a function, this is the unscoped type
    if (expr->typeInfo->isFuncAttr())
    {
        SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo, GEN_EXPORTED_TYPE_FORCE_NO_SCOPE));
        YIELD();
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    // For an enum, this is the raw type
    if (expr->typeInfo->isEnum())
    {
        const auto typeEnum = castTypeInfo<TypeInfoEnum>(expr->typeInfo, TypeInfoKind::Enum);
        SWAG_CHECK(resolveTypeAsExpression(context, expr, typeEnum->rawType, &node->typeInfo));
        YIELD();
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    // For an alias, this is the raw type
    if (expr->typeInfo->isAlias())
    {
        const auto typeAlias = castTypeInfo<TypeInfoAlias>(expr->typeInfo, TypeInfoKind::Alias);
        SWAG_CHECK(resolveTypeAsExpression(context, expr, typeAlias->rawType, &node->typeInfo));
        YIELD();
        node->inheritComputedValue(expr);
        SWAG_CHECK(setupIdentifierRef(context, node));
        return true;
    }

    SWAG_CHECK(resolveIntrinsicTypeOf(context));
    return true;
}

bool Semantic::resolveIntrinsicDeclType(SemanticContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->firstChild();
    auto       typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, toErr(Err0399)}));

    if (expr->hasAstFlag(AST_CONST_EXPR))
    {
        SWAG_CHECK(executeCompilerNode(context, expr, true));
        YIELD();
    }

    expr->addAstFlag(AST_NO_BYTECODE);

    // @mktype on a typeinfo will give back the original compiler type
    if (typeInfo->isPointerToTypeInfo() &&
        expr->hasFlagComputedValue() &&
        expr->computedValue()->storageOffset != UINT32_MAX &&
        expr->computedValue()->storageSegment != nullptr)
    {
        const auto addr        = expr->computedValue()->getStorageAddr();
        const auto newTypeInfo = context->sourceFile->module->typeGen.getRealType(expr->computedValue()->storageSegment, static_cast<ExportedTypeInfo*>(addr));
        if (newTypeInfo)
            typeInfo = newTypeInfo;
    }

    // Should be a lambda
    if (typeInfo->isFuncAttr() && !typeInfo->hasFlag(TYPEINFO_FUNC_IS_ATTR))
    {
        typeInfo         = typeInfo->clone();
        typeInfo->kind   = TypeInfoKind::LambdaClosure;
        typeInfo->sizeOf = sizeof(void*);
    }

    node->typeInfo = typeInfo;
    return true;
}

bool Semantic::resolveIntrinsicTypeOf(SemanticContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->firstChild();
    const auto typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, toErr(Err0399)}));

    expr->addAstFlag(AST_NO_BYTECODE);

    SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo));
    YIELD();
    node->inheritComputedValue(expr);
    SWAG_CHECK(setupIdentifierRef(context, node));
    return true;
}

bool Semantic::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);

    switch (node->token.id)
    {
        case TokenId::IntrinsicSpread:
            SWAG_CHECK(resolveIntrinsicSpread(context));
            return true;

        case TokenId::IntrinsicIsConstExpr:
        {
            const auto expr = node->firstChild();
            node->typeInfo  = g_TypeMgr->typeInfoBool;
            expr->addAstFlag(AST_NO_BYTECODE);

            // Special case for a function parameter in a where block, should be done at runtime
            if (expr->isWhereParam(expr->resolvedSymbolOverload()))
            {
                node->byteCodeFct = ByteCodeGen::emitIntrinsicIsConstExprSI;
                break;
            }

            node->setFlagsValueIsComputed();
            node->computedValue()->reg.b = expr->hasFlagComputedValue();
            break;
        }

        case TokenId::IntrinsicSizeOf:
        {
            auto expr = node->firstChild();
            SWAG_VERIFY(!expr->typeInfo->isGeneric(), context->report({expr, toErr(Err0151)}));
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = expr->typeInfo->sizeOf;
            if (node->computedValue()->reg.u64 > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
            break;
        }

        case TokenId::IntrinsicAlignOf:
        {
            auto expr = node->firstChild();
            SWAG_VERIFY(!expr->typeInfo->isGeneric(), context->report({expr, toErr(Err0146)}));
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = TypeManager::alignOf(expr->typeInfo);
            if (node->computedValue()->reg.u64 > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
            break;
        }

        case TokenId::IntrinsicOffsetOf:
        {
            const auto expr = node->firstChild();
            SWAG_CHECK(checkIsConstExpr(context, expr->resolvedSymbolOverload(), expr));
            node->setFlagsValueIsComputed();
            node->computedValue()->reg.u64 = expr->resolvedSymbolOverload()->computedValue.storageOffset;
            if (node->computedValue()->reg.u64 > UINT32_MAX)
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
                return context->report({expr, formErr(Err0208, expr->typeInfo->getDisplayNameC())});
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
        {
            const auto typeInfo = node->firstChild()->typeInfo;
            typeInfo->computeScopedName();
            SWAG_VERIFY(typeInfo->scopedName == "*Swag.CVaList", context->report({node, formErr(Err0657, typeInfo->getDisplayNameC())}));

            if (node->token.is(TokenId::IntrinsicCVaStart))
            {
                SWAG_VERIFY(node->ownerFct && node->ownerFct->parameters && !node->ownerFct->parameters->children.empty(), context->report({node, node->token, toErr(Err0453)}));
                const auto typeParam = node->ownerFct->parameters->lastChild()->typeInfo;
                SWAG_VERIFY(typeParam->isCVariadic(), context->report({node, node->token, toErr(Err0453)}));
                node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaStart;
            }
            else if (node->token.is(TokenId::IntrinsicCVaEnd))
            {
                node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaEnd;
            }
            else
            {
                node->typeInfo = node->secondChild()->typeInfo;

                SWAG_VERIFY(node->typeInfo->numRegisters() == 1, context->report({node->secondChild(), formErr(Err0397, node->typeInfo->getDisplayNameC())}));

                SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::F32), context->report({node->secondChild(), formErr(Err0150, node->typeInfo->getDisplayNameC(), "f64")}));
                SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S8), context->report({node->secondChild(), formErr(Err0150, node->typeInfo->getDisplayNameC(), "s32")}));
                SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S16), context->report({node->secondChild(), formErr(Err0150, node->typeInfo->getDisplayNameC(), "s32")}));
                SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U8), context->report({node->secondChild(), formErr(Err0150, node->typeInfo->getDisplayNameC(), "u32")}));
                SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U16), context->report({node->secondChild(), formErr(Err0150, node->typeInfo->getDisplayNameC(), "u32")}));
                SWAG_VERIFY(!node->typeInfo->isBool(), context->report({node->secondChild(), formErr(Err0150, node->typeInfo->getDisplayNameC(), "u32")}));

                node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaArg;
            }

            break;
        }
    }

    return true;
}
