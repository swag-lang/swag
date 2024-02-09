#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "AstOutput.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Semantic.h"
#include "TypeManager.h"
#include "Workspace.h"

bool Semantic::resolveIntrinsicTag(SemanticContext* context)
{
    const auto node = context->node;
    switch (node->tokenId)
    {
    case TokenId::IntrinsicSafety:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        YIELD();
        SWAG_CHECK(checkIsConstExpr(context, front->hasComputedValue(), front, Err(Err0033), node->token.text));
        SWAG_VERIFY(front->typeInfo->isString(), context->report({front, FMT(Err(Err0200), node->token.ctext(), front->typeInfo->getDisplayNameC())}));
        node->typeInfo = g_TypeMgr->typeInfoBool;
        node->setFlagsValueIsComputed();

        const auto& w      = front->computedValue->text;
        const auto  module = node->sourceFile->module;
        bool        done   = false;

#define CHECK_SAFETY_NAME(__name, __flag)                                      \
    do                                                                         \
    {                                                                          \
        if (w == g_LangSpec->__name)                                           \
        {                                                                      \
            done                       = true;                                 \
            node->computedValue->reg.b = module->mustEmitSafety(node, __flag); \
        }                                                                      \
    } while(0)

        CHECK_SAFETY_NAME(name_boundcheck, SAFETY_BOUNDCHECK);
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
            return context->report({front, front->token, FMT(Err(Err0373), w.c_str())});
        }

        return true;
    }

    case TokenId::IntrinsicHasTag:
    {
        auto front = node->childs.front();
        SWAG_CHECK(evaluateConstExpression(context, front));
        YIELD();
        SWAG_CHECK(checkIsConstExpr(context, front->hasComputedValue(), front, Err(Err0033), node->token.text));
        SWAG_VERIFY(front->typeInfo->isString(), context->report({front, FMT(Err(Err0200), node->token.ctext(), front->typeInfo->getDisplayNameC())}));
        const auto tag = g_Workspace->hasTag(front->computedValue->text);
        node->typeInfo = g_TypeMgr->typeInfoBool;
        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = tag ? true : false;
        return true;
    }

    case TokenId::IntrinsicGetTag:
    {
        auto       nameNode   = node->childs[0];
        const auto typeNode   = node->childs[1];
        auto       defaultVal = node->childs[2];
        SWAG_CHECK(evaluateConstExpression(context, nameNode));
        YIELD();
        SWAG_CHECK(evaluateConstExpression(context, defaultVal));
        YIELD();

        SWAG_CHECK(checkIsConstExpr(context, nameNode->hasComputedValue(), nameNode, Err(Err0033), node->token.text));
        SWAG_VERIFY(nameNode->typeInfo->isString(), context->report({nameNode, FMT(Err(Err0200), node->token.ctext(), nameNode->typeInfo->getDisplayNameC())}));
        SWAG_VERIFY(defaultVal->computedValue, context->report({defaultVal, FMT(Err(Err0201), typeNode->typeInfo->getDisplayNameC())}));
        SWAG_CHECK(TypeManager::makeCompatibles(context, typeNode->typeInfo, defaultVal->typeInfo, nullptr, defaultVal, CASTFLAG_DEFAULT));

        node->setFlagsValueIsComputed();
        node->typeInfo = typeNode->typeInfo;

        const auto tag = g_Workspace->hasTag(nameNode->computedValue->text);
        if (tag)
        {
            if (!TypeManager::makeCompatibles(context, typeNode->typeInfo, tag->type, nullptr, typeNode, CASTFLAG_JUST_CHECK))
            {
                const Diagnostic diag{typeNode, FMT(Err(Err0652), typeNode->typeInfo->getDisplayNameC(), tag->type->getDisplayNameC(), tag->name.c_str())};
                const auto       note = Diagnostic::note(typeNode, FMT(Nte(Nte0022), tag->cmdLine.c_str()));
                note->sourceFile      = nullptr;
                note->showSourceCode  = false;
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

bool Semantic::resolveIntrinsicMakeCallback(SemanticContext* context, AstNode* node)
{
    auto       first     = node->childs.front();
    const auto typeFirst = TypeManager::concreteType(first->typeInfo);

    // Check first parameter
    if (!typeFirst->isLambdaClosure())
        return context->report({first, FMT(Err(Err0204), typeFirst->getDisplayNameC())});

    const auto typeFunc = castTypeInfo<TypeInfoFuncAttr>(typeFirst, TypeInfoKind::LambdaClosure);
    if (typeFunc->parameters.size() > SWAG_LIMIT_CB_MAX_PARAMS)
    {
        const Diagnostic diag{first, FMT(Err(Err0738), SWAG_LIMIT_CB_MAX_PARAMS, typeFunc->parameters.size())};
        return context->report(diag, Diagnostic::hereIs(typeFunc->declNode));
    }

    if (typeFunc->numReturnRegisters() > 1)
    {
        const Diagnostic diag{first, FMT(Err(Err0737), typeFunc->returnType->getDisplayNameC())};
        return context->report(diag, Diagnostic::hereIs(typeFunc->declNode));
    }

    node->typeInfo    = first->typeInfo;
    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeCallback;
    return true;
}

bool Semantic::resolveIntrinsicMakeSlice(SemanticContext* context, AstNode* node, TypeInfo* typeInfo, const char* name)
{
    auto       first  = node->childs.front();
    const auto second = node->childs.back();

    // Must start with a pointer of the same type as the slice
    if (!first->typeInfo->isPointer())
        return context->report({first, FMT(Err(Err0196), name, first->typeInfo->getDisplayNameC())});
    if (!first->typeInfo->isPointerArithmetic() && !first->typeInfo->isCString())
        return context->report({first, FMT(Err(Err0195), name, first->typeInfo->getDisplayNameC())});

    const auto ptrPointer = castTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    SWAG_ASSERT(ptrPointer->pointedType);

    // Slice count
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, second->typeInfo, nullptr, second, CASTFLAG_TRY_COERCE));

    // Create slice type
    const auto ptrSlice   = makeType<TypeInfoSlice>();
    ptrSlice->pointedType = ptrPointer->pointedType;
    if (ptrPointer->isConst())
        ptrSlice->flags |= TYPEINFO_CONST;
    ptrSlice->computeName();
    node->typeInfo = ptrSlice;

    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeSlice;
    return true;
}

bool Semantic::resolveIntrinsicMakeAny(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    auto first  = node->childs.front();
    auto second = node->childs.back();

    // Check first parameter
    if (!first->typeInfo->isPointer())
        return context->report({first, FMT(Err(Err0196), node->token.ctext(), first->typeInfo->getDisplayNameC())});

    const auto ptrPointer = castTypeInfo<TypeInfoPointer>(first->typeInfo, TypeInfoKind::Pointer);
    if (!ptrPointer->pointedType)
        return context->report({first, Err(Err0202)});

    // Check second parameter
    SWAG_CHECK(checkIsConcreteOrType(context, second));
    if (second->isConstantGenTypeInfo())
    {
        const auto genType  = second->getConstantGenTypeInfo();
        const auto realType = context->sourceFile->module->typeGen.getRealType(second->computedValue->storageSegment, genType);

        if (!TypeManager::makeCompatibles(context, ptrPointer->pointedType, realType, nullptr, second, CASTFLAG_JUST_CHECK))
        {
            Diagnostic diag{first, FMT(Err(Err0006), first->typeInfo->getDisplayNameC(), realType->getDisplayNameC())};
            diag.addRange(second->token, Diagnostic::isType(realType));
            return context->report(diag);
        }
    }

    YIELD();
    if (!second->typeInfo->isPointerToTypeInfo())
        return context->report({second, FMT(Err(Err0203), second->typeInfo->getDisplayNameC())});

    node->typeInfo    = g_TypeMgr->typeInfoAny;
    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeAny;
    return true;
}

bool Semantic::resolveIntrinsicMakeInterface(SemanticContext* context)
{
    const auto node   = context->node;
    const auto params = node->childs.front();

    auto       first      = params->childs[0];
    auto       second     = params->childs[1];
    auto       third      = params->childs[2];
    const auto sourceFile = context->sourceFile;
    const auto module     = sourceFile->module;
    auto&      typeGen    = module->typeGen;

    SWAG_CHECK(checkIsConcrete(context, first));
    SWAG_CHECK(checkIsConcreteOrType(context, second));
    YIELD();

    third->allocateComputedValue();
    third->computedValue->storageSegment = getConstantSegFromContext(third);
    SWAG_CHECK(typeGen.genExportedTypeInfo(context, third->typeInfo, third->computedValue->storageSegment, &third->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT));
    YIELD();

    const auto firstTypeInfo = first->typeInfo->getConcreteAlias();
    SWAG_VERIFY(firstTypeInfo->isPointer() || firstTypeInfo->isStruct(), context->report({first, FMT(Err(Err0205), firstTypeInfo->getDisplayNameC())}));
    SWAG_VERIFY(second->typeInfo->isPointerToTypeInfo(), context->report({second, FMT(Err(Err0206), second->typeInfo->getDisplayNameC())}));
    const auto thirdTypeInfo = third->typeInfo->getConcreteAlias();
    SWAG_VERIFY(thirdTypeInfo->isInterface(), context->report({third, FMT(Err(Err0207), thirdTypeInfo->getDisplayNameC())}));

    node->typeInfo = third->typeInfo;
    third->addFlag(AST_NO_BYTECODE);

    node->byteCodeFct = ByteCodeGen::emitIntrinsicMakeInterface;
    return true;
}

bool Semantic::resolveIntrinsicCountOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = TypeManager::concretePtrRef(expression->typeInfo);
    typeInfo      = typeInfo->getConcreteAlias();

    if (expression->resolvedSymbolName && expression->resolvedSymbolName->kind == SymbolKind::EnumValue)
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);
    else if (expression->resolvedSymbolName && expression->resolvedSymbolName->kind == SymbolKind::Variable)
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ENUM);

    if (typeInfo->isEnum())
    {
        const auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
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
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);
        node->typeInfo       = g_TypeMgr->typeInfoU64;
        if (expression->hasComputedValue())
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
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCountOf;
        }
    }
    else if (typeInfo->isArray())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);
        node->setFlagsValueIsComputed();
        const auto typeArray         = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        node->computedValue->reg.u64 = typeArray->count;
        if (node->computedValue->reg.u64 > UINT32_MAX)
            node->typeInfo = g_TypeMgr->typeInfoU64;
        else
            node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
    }
    else if (typeInfo->isSlice())
    {
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        // :SliceLiteral
        // Slice literal. This can happen for enum values
        if (expression->hasComputedValue())
        {
            const auto slice = (SwagSlice*) node->computedValue->getStorageAddr();
            if (slice->count > UINT32_MAX)
                node->typeInfo = g_TypeMgr->typeInfoU64;
            else
                node->typeInfo = g_TypeMgr->typeInfoUntypedInt;
            node->computedValue->reg.u64 = slice->count;
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
        node->computedValue->reg.u64 = typeList->subTypes.size();
        if (node->computedValue->reg.u64 > UINT32_MAX)
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
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({expression, Err(Err0742)}));
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

        SWAG_VERIFY(typeInfo->isNativeInteger(), context->report({expression, FMT(Err(Err0152), typeInfo->getDisplayNameC())}));
        if (expression->hasComputedValue())
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (expression->computedValue->reg.s8 < 0)
                        return context->report({expression, FMT(Err(Err0151), node->computedValue->reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (expression->computedValue->reg.s16 < 0)
                        return context->report({expression, FMT(Err(Err0151), node->computedValue->reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (expression->computedValue->reg.s32 < 0)
                        return context->report({expression, FMT(Err(Err0151), node->computedValue->reg.s32)});
                    break;
                case NativeTypeKind::S64:
                    if (expression->computedValue->reg.s64 < 0)
                        return context->report({expression, FMT(Err(Err0150), node->computedValue->reg.s64)});
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

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, typeInfo, nullptr, node, CASTFLAG_TRY_COERCE));
    }

    return true;
}

bool Semantic::resolveIntrinsicDataOf(SemanticContext* context, AstNode* node, AstNode* expression)
{
    auto typeInfo = TypeManager::concretePtrRefType(expression->typeInfo);
    
    if (typeInfo->isListArray())
    {
        const auto typeList  = castTypeInfo<TypeInfoList>(typeInfo, TypeInfoKind::TypeListArray);
        const auto typeArray = TypeManager::convertTypeListToArray(context, typeList, expression->flags & AST_CONST_EXPR);
        SWAG_CHECK(TypeManager::makeCompatibles(context, typeArray, nullptr, expression, CASTFLAG_DEFAULT));
        typeInfo = typeArray;
    }

    if (typeInfo->isString())
    {
        // :ConcreteRef
        expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_FUNC | CONCRETE_ALIAS);

        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoU8, TYPEINFO_CONST | TYPEINFO_POINTER_ARITHMETIC);
        if (expression->hasComputedValue())
        {
            node->setFlagsValueIsComputed();
            if (expression->computedValue->text.buffer == nullptr)
            {
                node->typeInfo                      = g_TypeMgr->typeInfoNull;
                node->computedValue->storageSegment = nullptr;
                node->computedValue->storageOffset  = UINT32_MAX;
            }
            else
            {
                node->computedValue->storageSegment = getConstantSegFromContext(node);
                node->computedValue->storageOffset  = node->computedValue->storageSegment->addString(expression->computedValue->text);
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

        const auto ptrSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto       ptrFlags = TYPEINFO_POINTER_ARITHMETIC;
        if (ptrSlice->isConst())
            ptrFlags |= TYPEINFO_CONST;
        node->typeInfo = g_TypeMgr->makePointerTo(ptrSlice->pointedType, ptrFlags);

        if (expression->hasComputedValue())
        {
            node->inheritComputedValue(expression);
            const auto slice = (SwagSlice*) node->computedValue->getStorageAddr();
            if (!slice->buffer)
            {
                node->typeInfo                      = g_TypeMgr->typeInfoNull;
                node->computedValue->storageSegment = nullptr;
                node->computedValue->storageOffset  = UINT32_MAX;
            }
            else
            {
                node->computedValue->storageOffset = node->computedValue->storageSegment->offset((uint8_t*) slice->buffer);
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

        const auto ptrArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto       ptrFlags = TYPEINFO_POINTER_ARITHMETIC;
        if (ptrArray->isConst())
            ptrFlags |= TYPEINFO_CONST;
        node->typeInfo = g_TypeMgr->makePointerTo(ptrArray->pointedType, ptrFlags);

        if (expression->hasComputedValue())
        {
            node->inheritComputedValue(expression);
            if (!expression->computedValue->storageSegment)
            {
                node->typeInfo                      = g_TypeMgr->typeInfoNull;
                node->computedValue->storageSegment = nullptr;
                node->computedValue->storageOffset  = UINT32_MAX;
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
        if (expression->hasComputedValue())
        {
            node->inheritComputedValue(expression);
            SWAG_ASSERT(expression->computedValue);

            if (!expression->computedValue->storageSegment)
            {
                node->typeInfo                      = g_TypeMgr->typeInfoNull;
                node->computedValue->storageSegment = nullptr;
                node->computedValue->storageOffset  = UINT32_MAX;
            }
            else
            {
                const auto any = (SwagAny*) expression->computedValue->getStorageAddr();
                if (!any->value)
                {
                    node->typeInfo                      = g_TypeMgr->typeInfoNull;
                    node->computedValue->storageSegment = nullptr;
                    node->computedValue->storageOffset  = UINT32_MAX;
                }
                else
                {
                    node->computedValue->storageOffset = node->computedValue->storageSegment->offset((uint8_t*) any->value);
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
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({expression, Err(Err0744)}));
        node->typeInfo = typeInfo;
        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opData, nullptr, nullptr, node, nullptr));
        YIELD();
        node->typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
        if (!node->byteCodeFct)
            node->byteCodeFct = ByteCodeGen::emitIntrinsicDataOf;
    }
    else
    {
        return context->report({expression, FMT(Err(Err0743), typeInfo->getDisplayNameC())});
    }

    return true;
}

bool Semantic::resolveIntrinsicStringOf(SemanticContext* context)
{
    const auto node     = context->node;
    auto       expr     = node->childs.front();
    const auto typeInfo = expr->typeInfo;

    node->setFlagsValueIsComputed();
    SWAG_CHECK(checkIsConcreteOrType(context, expr, true));
    YIELD();

    if (expr->computedValue)
    {
        if (expr->isConstantGenTypeInfo())
            node->computedValue->text = Utf8{expr->getConstantGenTypeInfo()->fullName};
        else if (typeInfo->isString())
            node->computedValue->text = expr->computedValue->text;
        else if (typeInfo->isNative())
            node->computedValue->text = Ast::literalToString(typeInfo, *expr->computedValue);
        else if (typeInfo->isEnum())
            node->computedValue->text = Ast::enumToString(typeInfo, expr->computedValue->text, expr->computedValue->reg);
        else
            return context->report({expr, Err(Err0734)});
    }
    else if (expr->typeInfo->isCode())
    {
        Concat concat;
        concat.init(4 * 1024);

        AstOutput::OutputContext outputContext;
        const auto               typeCode = castTypeInfo<TypeInfoCode>(expr->typeInfo, TypeInfoKind::Code);
        AstOutput::outputNode(outputContext, concat, typeCode->content);
        for (auto b = concat.firstBucket; b; b = b->nextBucket)
            node->computedValue->text.append((const char*) b->data, concat.bucketCount(b));
    }
    else if (expr->resolvedSymbolName)
        node->computedValue->text = expr->resolvedSymbolName->getFullName();
    else if (expr->resolvedSymbolOverload)
        node->computedValue->text = expr->resolvedSymbolOverload->symbol->getFullName();
    else
        return context->report({expr, Err(Err0734)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveIntrinsicNameOf(SemanticContext* context)
{
    const auto node = context->node;
    auto       expr = node->childs.front();

    node->setFlagsValueIsComputed();
    SWAG_CHECK(checkIsConcreteOrType(context, expr, true));
    YIELD();

    if (expr->isConstantGenTypeInfo())
        node->computedValue->text = Utf8{expr->getConstantGenTypeInfo()->name};
    else if (expr->resolvedSymbolName)
        node->computedValue->text = expr->resolvedSymbolName->name;
    else if (expr->resolvedSymbolOverload)
        node->computedValue->text = expr->resolvedSymbolOverload->symbol->name;
    else
        return context->report({expr, Err(Err0733)});

    node->typeInfo = g_TypeMgr->typeInfoString;
    return true;
}

bool Semantic::resolveIntrinsicRunes(SemanticContext* context)
{
    const auto node     = context->node;
    auto       expr     = node->childs.front();
    const auto typeInfo = expr->typeInfo;

    SWAG_CHECK(checkIsConstExpr(context, expr->hasComputedValue(), expr));
    SWAG_VERIFY(typeInfo->isString(), context->report({expr, FMT(Err(Err0209), typeInfo->getDisplayNameC())}));
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
    const auto storageSegment           = getConstantSegFromContext(context->node);
    node->computedValue->storageSegment = storageSegment;

    SwagSlice* slice;
    node->computedValue->storageOffset = storageSegment->reserve(sizeof(SwagSlice), (uint8_t**) &slice);
    slice->count                       = (uint64_t) runes.size();

    uint8_t* addrDst;
    storageSegment->reserve((uint32_t) runes.size() * sizeof(uint32_t), &addrDst);
    slice->buffer = addrDst;

    // Setup array
    memcpy(addrDst, runes.data(), runes.size() * sizeof(uint32_t));

    node->typeInfo = g_TypeMgr->typeInfoSliceRunes;
    return true;
}

bool Semantic::resolveIntrinsicSpread(SemanticContext* context)
{
    auto       node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->childs.front();
    const auto typeInfo = TypeManager::concreteType(expr->typeInfo);
    node->byteCodeFct   = ByteCodeGen::emitIntrinsicSpread;

    SWAG_VERIFY(node->parent && node->parent->parent && node->parent->parent->kind == AstNodeKind::FuncCallParam, context->report({node, node->token, Err(Err0453)}));

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
        typeArr->count       = (uint32_t) typeList->subTypes.size();
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
        return context->report({expr, FMT(Err(Err0387), typeInfo->getDisplayNameC())});
    }

    const auto typeVar = makeType<TypeInfoVariadic>(TypeInfoKind::TypedVariadic);
    typeVar->rawType   = node->typeInfo;
    typeVar->computeName();
    node->typeInfo = typeVar;
    node->typeInfo->flags |= TYPEINFO_SPREAD;

    return true;
}

bool Semantic::resolveIntrinsicKindOf(SemanticContext* context)
{
    const auto node       = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    const auto expr       = node->childs.front();
    const auto sourceFile = context->sourceFile;
    auto&      typeGen    = sourceFile->module->typeGen;

    // Will be runtime for an 'any' type
    if (expr->typeInfo->isAny())
    {
        SWAG_CHECK(checkIsConcrete(context, expr));

        if (expr->hasComputedValue())
        {
            const auto any                     = (SwagAny*) expr->computedValue->getStorageAddr();
            expr->computedValue->storageOffset = expr->computedValue->storageSegment->offset((uint8_t*) any->type);
            node->inheritComputedValue(expr);
            node->addFlag(AST_VALUE_IS_GEN_TYPEINFO);
            node->typeInfo = g_TypeMgr->typeInfoTypeType;
            SWAG_CHECK(setupIdentifierRef(context, node));
        }
        else
        {
            node->allocateComputedValue();
            node->computedValue->storageSegment = getConstantSegFromContext(node);
            SWAG_CHECK(
                typeGen.genExportedTypeInfo(context, expr->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node
                    ->typeInfo));
            YIELD();
            node->byteCodeFct = ByteCodeGen::emitIntrinsicKindOf;
            node->addFlag(AST_R_VALUE);
            SWAG_CHECK(setupIdentifierRef(context, node));
        }

        return true;
    }

    // Will be runtime for an 'interface' type
    if (expr->typeInfo->isInterface())
    {
        SWAG_CHECK(checkIsConcrete(context, expr));
        node->allocateComputedValue();
        node->computedValue->storageSegment = getConstantSegFromContext(node);
        SWAG_CHECK(
            typeGen.genExportedTypeInfo(context, expr->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node->
                typeInfo));
        YIELD();
        node->byteCodeFct = ByteCodeGen::emitIntrinsicKindOf;
        node->addFlag(AST_R_VALUE);
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
    auto       expr     = node->childs.front();
    auto       typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, Err(Err0396)}));

    if (expr->flags & AST_CONST_EXPR)
    {
        SWAG_CHECK(executeCompilerNode(context, expr, true));
        YIELD();
    }

    expr->addFlag(AST_NO_BYTECODE);

    // @mktype on a typeinfo will give back the original compiler type
    if (typeInfo->isPointerToTypeInfo() &&
        expr->computedValue &&
        expr->computedValue->storageOffset != UINT32_MAX &&
        expr->computedValue->storageSegment != nullptr)
    {
        const auto addr        = expr->computedValue->getStorageAddr();
        const auto newTypeInfo = context->sourceFile->module->typeGen.getRealType(expr->computedValue->storageSegment, (ExportedTypeInfo*) addr);
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

bool Semantic::resolveIntrinsicTypeOf(SemanticContext* context)
{
    const auto node     = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);
    auto       expr     = node->childs.front();
    const auto typeInfo = expr->typeInfo;

    SWAG_VERIFY(!typeInfo->isKindGeneric(), context->report({expr, Err(Err0396)}));

    expr->addFlag(AST_NO_BYTECODE);

    SWAG_CHECK(resolveTypeAsExpression(context, expr, &node->typeInfo));
    YIELD();
    node->inheritComputedValue(expr);
    SWAG_CHECK(setupIdentifierRef(context, node));
    return true;
}

bool Semantic::resolveIntrinsicProperty(SemanticContext* context)
{
    auto node = castAst<AstIntrinsicProp>(context->node, AstNodeKind::IntrinsicProp);

    switch (node->tokenId)
    {
    case TokenId::IntrinsicSpread:
        SWAG_CHECK(resolveIntrinsicSpread(context));
        return true;

    case TokenId::IntrinsicIsConstExpr:
    {
        const auto expr = node->childs.front();
        node->typeInfo  = g_TypeMgr->typeInfoBool;
        expr->addFlag(AST_NO_BYTECODE);

        // Special case for a function parameter in a validif block, should be done at runtime
        if (expr->isValidIfParam(expr->resolvedSymbolOverload))
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicIsConstExprSI;
            break;
        }

        node->setFlagsValueIsComputed();
        node->computedValue->reg.b = expr->hasComputedValue();
        break;
    }

    case TokenId::IntrinsicSizeOf:
    {
        auto expr = node->childs.front();
        SWAG_VERIFY(!expr->typeInfo->isGeneric(), context->report({expr, Err(Err0154)}));
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
        SWAG_VERIFY(!expr->typeInfo->isGeneric(), context->report({expr, Err(Err0149)}));
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
        const auto expr = node->childs.front();
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
        const auto expr     = node->childs.front();
        const auto typeInfo = expr->typeInfo->getConcreteAlias();
        if (!typeInfo->isEnum() && !typeInfo->isArray())
            SWAG_CHECK(checkIsConcrete(context, expr));
        node->inheritComputedValue(expr);
        SWAG_CHECK(resolveIntrinsicCountOf(context, node, expr));
        break;
    }

    case TokenId::IntrinsicDataOf:
    {
        const auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicDataOf(context, node, expr));
        break;
    }

    case TokenId::IntrinsicMakeAny:
    {
        const auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeAny(context, node, expr->typeInfo));
        break;
    }

    case TokenId::IntrinsicMakeSlice:
    {
        const auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkslice"));
        break;
    }

    case TokenId::IntrinsicMakeString:
    {
        auto expr = node->childs.front();
        SWAG_CHECK(checkIsConcrete(context, expr));
        if (!expr->typeInfo->isPointerTo(NativeTypeKind::U8))
            return context->report({expr, FMT(Err(Err0208), expr->typeInfo->getDisplayNameC())});
        SWAG_CHECK(resolveIntrinsicMakeSlice(context, node, expr->typeInfo, "@mkstring"));
        node->typeInfo = g_TypeMgr->typeInfoString;
        break;
    }

    case TokenId::IntrinsicMakeCallback:
    {
        const auto expr = node->childs.front();
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
        const auto typeInfo = node->childs[0]->typeInfo;
        typeInfo->computeScopedName();
        SWAG_VERIFY(typeInfo->scopedName == "*Swag.CVaList", context->report({node, FMT(Err(Err0650), typeInfo->getDisplayNameC())}));

        if (node->tokenId == TokenId::IntrinsicCVaStart)
        {
            SWAG_VERIFY(node->ownerFct && node->ownerFct->parameters && !node->ownerFct->parameters->childs.empty(), context->report({node, node->token, Err(Err0452)}));
            const auto typeParam = node->ownerFct->parameters->childs.back()->typeInfo;
            SWAG_VERIFY(typeParam->isCVariadic(), context->report({node, node->token, Err(Err0452)}));
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaStart;
        }
        else if (node->tokenId == TokenId::IntrinsicCVaEnd)
        {
            node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaEnd;
        }
        else
        {
            node->typeInfo = node->childs[1]->typeInfo;

            SWAG_VERIFY(node->typeInfo->numRegisters() == 1, context->report({node->childs[1], FMT(Err(Err0394), node->typeInfo->getDisplayNameC())}));

            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::F32), context->report({node->childs[1], FMT(Err(Err0153), node->typeInfo->getDisplayNameC(), "f64")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S8), context->report({node->childs[1], FMT(Err(Err0153), node->typeInfo->getDisplayNameC(), "s32")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::S16), context->report({node->childs[1], FMT(Err(Err0153), node->typeInfo->getDisplayNameC(), "s32")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U8), context->report({node->childs[1], FMT(Err(Err0153), node->typeInfo->getDisplayNameC(), "u32")}));
            SWAG_VERIFY(!node->typeInfo->isNative(NativeTypeKind::U16), context->report({node->childs[1], FMT(Err(Err0153), node->typeInfo->getDisplayNameC(), "u32")}));
            SWAG_VERIFY(!node->typeInfo->isBool(), context->report({node->childs[1], FMT(Err(Err0153), node->typeInfo->getDisplayNameC(), "u32")}));

            node->byteCodeFct = ByteCodeGen::emitIntrinsicCVaArg;
        }

        break;
    }
    default:
        break;
    }

    return true;
}
