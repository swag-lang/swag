#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Format/FormatAst.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Parser/Parser.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

bool Semantic::resolveIf(SemanticContext* context)
{
    const auto module = context->sourceFile->module;
    const auto node   = castAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    // :ConcreteRef
    node->boolExpression->typeInfo = getConcreteTypeUnRef(node->boolExpression, CONCRETE_ALL);

    {
        PushErrCxtStep ec(context, node, ErrCxtStepKind::Note, [] { return toNte(Nte0220); }, true);
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CAST_FLAG_AUTO_BOOL));
    }

    // Do not generate backend if "if" is constant, and has already been evaluated
    if (module->mustOptimizeBytecode(node) && node->boolExpression->hasFlagComputedValue())
    {
        node->boolExpression->addAstFlag(AST_NO_BYTECODE);
        if (node->boolExpression->computedValue()->reg.b)
        {
            if (node->elseBlock)
                node->elseBlock->addAstFlag(AST_NO_BYTECODE);
        }
        else
        {
            node->ifBlock->addAstFlag(AST_NO_BYTECODE);
        }

        return true;
    }

    node->byteCodeFct = ByteCodeGen::emitIf;
    node->boolExpression->setBcNotifyAfter(ByteCodeGen::emitIfAfterExpr);
    node->ifBlock->setBcNotifyAfter(ByteCodeGen::emitIfAfterIf, ByteCodeGen::emitLeaveScope);

    SWAG_CHECK(SemanticError::warnElseDoIf(context, node));

    if (node->hasSpecFlag(AstIf::SPEC_FLAG_ASSIGN))
        SWAG_CHECK(SemanticError::warnUnusedVariables(context, node->ifBlock->ownerScope->parentScope));

    return true;
}

bool Semantic::resolveWhile(SemanticContext* context)
{
    const auto module = context->sourceFile->module;
    const auto node   = castAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    // :ConcreteRef
    node->boolExpression->typeInfo = getConcreteTypeUnRef(node->boolExpression, CONCRETE_ALL);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CAST_FLAG_AUTO_BOOL));

    // Do not evaluate while if it's constant and false
    if (module->mustOptimizeBytecode(node) && node->boolExpression->hasFlagComputedValue())
    {
        if (!node->boolExpression->computedValue()->reg.b)
        {
            node->boolExpression->addAstFlag(AST_NO_BYTECODE);
            node->block->addAstFlag(AST_NO_BYTECODE);
            return true;
        }
    }

    if (node->boolExpression->hasFlagComputedValue() && node->boolExpression->computedValue()->reg.b)
    {
        const Diagnostic err{node->boolExpression, toErr(Err0115)};
        return context->report(err);
    }

    node->byteCodeFct = ByteCodeGen::emitLoop;
    node->boolExpression->setBcNotifyBefore(ByteCodeGen::emitWhileBeforeExpr);
    node->boolExpression->setBcNotifyAfter(ByteCodeGen::emitWhileAfterExpr);
    node->block->setBcNotifyAfter(ByteCodeGen::emitLoopAfterBlock, ByteCodeGen::emitLeaveScope);

    // :SpecPropagateReturn
    if (node->breakableFlags.has(BREAKABLE_RETURN_IN_INFINITE_LOOP) && node->breakList.empty())
        propagateReturn(node->parent);

    return true;
}

bool Semantic::resolveInlineBefore(SemanticContext* context)
{
    const auto node = castAst<AstInline>(context->node, AstNodeKind::Inline);

    ErrorCxtStep expNode;
    expNode.node = node->parent;
    expNode.type = ErrCxtStepKind::DuringInline;
    context->errCxtSteps.push_back(expNode);

    if (node->hasSemFlag(SEMFLAG_RESOLVE_INLINED))
        return true;
    node->addSemFlag(SEMFLAG_RESOLVE_INLINED);

    const auto func         = node->func;
    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(func->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

    // :DirectInlineLocalVar
    // For a return by copy, need to reserve room on the stack for the return result
    if (CallConv::returnNeedsStack(typeInfoFunc))
    {
        node->addAstFlag(AST_TRANSIENT);
        allocateOnStack(node, func->returnType->typeInfo);
    }

    node->scope->startStackSize = node->ownerScope->startStackSize;

    // If we inline a throwable function, be sure the top level function is informed
    if (typeInfoFunc->hasFlag(TYPEINFO_CAN_THROW))
        node->ownerFct->addSpecFlag(AstFuncDecl::SPEC_FLAG_REG_GET_CONTEXT);

    // Register all function parameters as inline symbols
    if (func->parameters)
    {
        const AstIdentifier* identifier = nullptr;
        if (node->parent->is(AstNodeKind::Identifier))
            identifier = castAst<AstIdentifier>(node->parent, AstNodeKind::Identifier);
        for (uint32_t i = 0; i < func->parameters->childCount(); i++)
        {
            const auto funcParam = func->parameters->children[i];

            // If the call parameter of the inlined function is constant, then we register it in a specific
            // constant scope, not in the caller (for mixins)/inline scope.
            // This is a separated scope because mixins do not have their own scope, and we must have a
            // different symbol registration for each constant value
            // :InlineUsingParam
            bool              isConstant   = false;
            AstFuncCallParam* orgCallParam = nullptr;
            if (identifier && identifier->callParameters)
            {
                for (const auto& child : identifier->callParameters->children)
                {
                    const auto callParam = castAst<AstFuncCallParam>(child, AstNodeKind::FuncCallParam);
                    if (callParam->indexParam != i)
                        continue;
                    orgCallParam = callParam;
                    if (!callParam->hasFlagComputedValue())
                        continue;
                    AddSymbolTypeInfo toAdd;
                    toAdd.node                = callParam;
                    toAdd.typeInfo            = funcParam->typeInfo;
                    toAdd.kind                = SymbolKind::Variable;
                    toAdd.computedValue       = callParam->computedValue();
                    toAdd.flags               = OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN | OVERLOAD_COMPUTED_VALUE;
                    toAdd.storageOffset       = callParam->computedValue()->storageOffset;
                    toAdd.storageSegment      = callParam->computedValue()->storageSegment;
                    toAdd.aliasName           = funcParam->token.text;
                    const auto overload       = node->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
                    overload->fromInlineParam = orgCallParam;
                    isConstant                = true;
                    break;
                }
            }

            if (!isConstant)
            {
                SWAG_ASSERT(node->parametersScope);
                AddSymbolTypeInfo toAdd;
                toAdd.node                = Ast::clone(funcParam, nullptr);
                toAdd.typeInfo            = funcParam->typeInfo;
                toAdd.kind                = SymbolKind::Variable;
                toAdd.flags               = OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN;
                const auto overload       = node->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
                overload->fromInlineParam = orgCallParam;
            }
        }
    }

    // Sub declarations in an inline block have access to the 'parametersScope', so we must start resolving
    // them only when we have are filling the parameters
    resolveSubDecls(context, node->ownerFct);

    return true;
}

bool Semantic::resolveInlineAfter(SemanticContext* context)
{
    const auto node = castAst<AstInline>(context->node, AstNodeKind::Inline);
    const auto fct  = node->func;

    // No need to check missing return for inline only, because the function has already been
    // checked as a separated function
    if (fct->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
    {
        if (fct->returnType && !fct->returnType->typeInfo->isVoid())
        {
            if (!node->hasSemFlag(SEMFLAG_SCOPE_HAS_RETURN))
            {
                if (node->hasSemFlag(SEMFLAG_FCT_HAS_RETURN))
                    return context->report({fct->returnType, formErr(Err0482, fct->getDisplayNameC())});
                return context->report({fct->returnType, formErr(Err0479, fct->getDisplayNameC(), fct->returnType->typeInfo->getDisplayNameC())});
            }
        }
    }

    context->errCxtSteps.pop_back();
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveForBefore(SemanticContext* context)
{
    const auto node                  = castAst<AstFor>(context->node, AstNodeKind::For);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveLoopBefore(SemanticContext* context)
{
    const auto node                  = castAst<AstLoop>(context->node, AstNodeKind::Loop);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool Semantic::resolveFor(SemanticContext* context)
{
    const auto node = castAst<AstFor>(context->node, AstNodeKind::For);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression));
    node->byteCodeFct = ByteCodeGen::emitLoop;

    node->setBcNotifyAfter(ByteCodeGen::emitLeaveScope);
    node->boolExpression->setBcNotifyBefore(ByteCodeGen::emitForBeforeExpr);
    node->boolExpression->setBcNotifyAfter(ByteCodeGen::emitForAfterExpr);
    node->postExpression->setBcNotifyBefore(ByteCodeGen::emitForBeforePost);
    node->block->setBcNotifyAfter(ByteCodeGen::emitLoopAfterBlock, ByteCodeGen::emitLeaveScope);

    // :SpecPropagateReturn
    if (node->breakableFlags.has(BREAKABLE_RETURN_IN_INFINITE_LOOP) && node->breakList.empty())
        propagateReturn(node->parent);

    return true;
}

bool Semantic::resolveSwitchAfterExpr(SemanticContext* context)
{
    const auto node       = context->node;
    const auto switchNode = castAst<AstSwitch>(node->parent, AstNodeKind::Switch);
    const auto typeInfo   = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC);

    // For a switch on an enum, force a 'using' for each case
    if (typeInfo->isEnum())
    {
        // :AutoScope
        const auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        for (const auto switchCase : switchNode->cases)
        {
            switchCase->addAlternativeScope(typeEnum->scope);
        }
    }

    // Auto convert to the underlying type if necessary
    else
    {
        SWAG_CHECK(makeIntrinsicKindof(context, node));
        YIELD();
    }

    return true;
}

bool Semantic::resolveSwitch(SemanticContext* context)
{
    auto node = castAst<AstSwitch>(context->node, AstNodeKind::Switch);

    // Deal with complete
    SWAG_CHECK(collectAttributes(context, node, nullptr));

    if (node->hasAttribute(ATTRIBUTE_COMPLETE) && !node->expression)
    {
        Diagnostic err{node, node->token, toErr(Err0370)};
        const auto attr = node->findParentAttrUse(g_LangSpec->name_Swag_Complete);
        err.addNote(attr, formNte(Nte0181, "attribute"));
        return context->report(err);
    }

    node->byteCodeFct = ByteCodeGen::emitSwitch;
    if (!node->expression)
    {
        node->setBcNotifyBefore(ByteCodeGen::emitBeforeSwitch);
        return true;
    }

    node->expression->setBcNotifyAfter(ByteCodeGen::emitSwitchAfterExpr);

    SWAG_CHECK(checkIsConcrete(context, node->expression));

    node->expression->typeInfo = getConcreteTypeUnRef(node->expression, CONCRETE_FUNC | CONCRETE_ALIAS);
    node->typeInfo             = node->expression->typeInfo;

    const auto typeSwitch = TypeManager::concreteType(node->typeInfo);

    switch (typeSwitch->kind)
    {
        case TypeInfoKind::Slice:
        case TypeInfoKind::Array:
        case TypeInfoKind::Interface:
            return context->report({node->expression, formErr(Err0575, typeSwitch->getDisplayNameC())});
    }

    SWAG_VERIFY(!node->cases.empty(), context->report({node, node->token, toErr(Err0048)}));

    // Collect constant expressions, to avoid double definitions
    VectorNative<AstNode*> valDef;
    VectorNative<uint64_t> val64;
    Vector<Utf8>           valText;
    for (const auto switchCase : node->cases)
    {
        for (auto expr : switchCase->expressions)
        {
            if (expr->hasFlagComputedValue())
            {
                const auto typeExpr = TypeManager::concreteType(expr->typeInfo);
                if (typeExpr->isString())
                {
                    const int idx = valText.find(expr->computedValue()->text);
                    if (idx != -1)
                    {
                        Diagnostic err{expr, formErr(Err0538, expr->computedValue()->text.cstr())};
                        err.addNote(valDef[idx], toNte(Nte0195));
                        return context->report(err);
                    }

                    valText.push_back(expr->computedValue()->text);
                    valDef.push_back(expr);
                }
                else
                {
                    auto value = expr->computedValue()->reg.u64;
                    if (expr->isConstantGenTypeInfo())
                        value = expr->computedValue()->storageOffset;

                    const int idx = val64.find(value);
                    if (idx != -1)
                    {
                        const auto note = Diagnostic::note(valDef[idx], toNte(Nte0195));
                        if (expr->isConstantGenTypeInfo())
                            return context->report({expr, formErr(Err0537, expr->token.cstr())}, note);
                        if (expr->typeInfo->isEnum())
                            return context->report({expr, formErr(Err0537, expr->token.cstr())}, note);
                        if (typeExpr->isNativeInteger())
                            return context->report({expr, formErr(Err0535, expr->computedValue()->reg.u64)}, note);
                        return context->report({expr, formErr(Err0536, expr->computedValue()->reg.f64)}, note);
                    }

                    val64.push_back(expr->computedValue()->reg.u64);
                    valDef.push_back(expr);
                }
            }
            else if (node->hasAttribute(ATTRIBUTE_COMPLETE))
            {
                return checkIsConstExpr(context, expr->hasFlagComputedValue(), expr, toErr(Err0023));
            }
        }
    }

    if (node->hasAttribute(ATTRIBUTE_COMPLETE))
    {
        // No default for a complete switch
        auto back = node->cases.back();
        if (back->expressions.empty())
        {
            const auto attr = back->findParentAttrUse(g_LangSpec->name_Swag_Complete);
            const auto note = Diagnostic::note(attr, formNte(Nte0181, "attribute"));
            return context->report({back, back->token, toErr(Err0334)}, note);
        }

        // When a switch is marked as complete, be sure every definition have been covered
        if (node->typeInfo->isEnum())
        {
            const auto                  typeEnum0 = castTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
            VectorNative<TypeInfoEnum*> collect;
            typeEnum0->collectEnums(collect);
            for (const auto typeEnum : collect)
            {
                if (typeSwitch->isString())
                {
                    for (const auto one : typeEnum->values)
                    {
                        if (!one->value)
                            continue;
                        if (!valText.contains(one->value->text))
                        {
                            Diagnostic err{node, node->token, formErr(Err0440, typeEnum->name.cstr(), one->name.cstr())};
                            err.addNote(one->declNode, one->declNode->token, toNte(Nte0134));
                            return context->report(err);
                        }
                    }
                }
                else
                {
                    for (const auto one : typeEnum->values)
                    {
                        if (!one->value)
                            continue;
                        if (!val64.contains(one->value->reg.u64))
                        {
                            Diagnostic err{node, node->token, formErr(Err0440, typeEnum->name.cstr(), one->name.cstr())};
                            err.addNote(one->declNode, one->declNode->token, toNte(Nte0134));
                            return context->report(err);
                        }
                    }
                }
            }
        }

        // Add a safety test in a switch default to catch runtime invalid values
        const auto caseNode = Ast::newNode<AstSwitchCase>(AstNodeKind::SwitchCase, nullptr, node);
        caseNode->addAstFlag(AST_GENERATED);
        caseNode->specFlags   = AstSwitchCase::SPEC_FLAG_IS_DEFAULT;
        caseNode->ownerSwitch = node;
        node->cases.push_back(caseNode);
        caseNode->byteCodeFct = ByteCodeGen::emitSafetySwitchDefault;
    }

    return true;
}

bool Semantic::resolveCase(SemanticContext* context)
{
    const auto node = castAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (const auto oneExpression : node->expressions)
    {
        oneExpression->typeInfo = getConcreteTypeUnRef(oneExpression, CONCRETE_FUNC | CONCRETE_ALIAS);

        // Range
        if (oneExpression->is(AstNodeKind::Range))
        {
            auto rangeNode = castAst<AstRange>(oneExpression, AstNodeKind::Range);
            if (node->ownerSwitch->expression)
            {
                const auto typeInfo = TypeManager::concreteType(node->ownerSwitch->expression->typeInfo);
                if (typeInfo->isStruct())
                {
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, node->ownerSwitch->expression, rangeNode->expressionLow));
                    YIELD();
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, node->ownerSwitch->expression, rangeNode->expressionUp));
                    YIELD();
                }
                else
                {
                    SWAG_CHECK(TypeManager::promote32(context, node->ownerSwitch->expression));
                    SWAG_CHECK(TypeManager::promote32(context, rangeNode->expressionLow));
                    SWAG_CHECK(TypeManager::promote32(context, rangeNode->expressionUp));
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionLow, CAST_FLAG_FOR_COMPARE | CAST_FLAG_TRY_COERCE));
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionUp, CAST_FLAG_FOR_COMPARE | CAST_FLAG_TRY_COERCE));
                }
            }
            else
            {
                return context->report({rangeNode, toErr(Err0406)});
            }
        }

        // Single expression
        else
        {
            SWAG_CHECK(checkIsConcreteOrType(context, oneExpression));
            YIELD();

            // switch with an expression : compare case with the switch expression
            if (node->ownerSwitch->expression)
            {
                auto typeInfo = TypeManager::concreteType(node->ownerSwitch->expression->typeInfo);
                if (typeInfo->isStruct())
                {
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, node->ownerSwitch->expression, oneExpression));
                    YIELD();
                }
                else
                {
                    PushErrCxtStep ec(context, node->ownerSwitch->expression, ErrCxtStepKind::Note, [typeInfo] { return formNte(Nte0148, typeInfo->getDisplayNameC(), "the switch expression"); });
                    const auto     typeSwitch = TypeManager::concretePtrRefType(node->ownerSwitch->expression->typeInfo, CONCRETE_FUNC);
                    SWAG_CHECK(TypeManager::makeCompatibles(context, typeSwitch, node->ownerSwitch->expression, oneExpression, CAST_FLAG_FOR_COMPARE));
                }

                // If the switch expression is constant, and the expression is constant too, then we can do the
                // compare right now
                if (node->ownerSwitch->expression->hasFlagComputedValue() && oneExpression->hasFlagComputedValue())
                {
                    SWAG_CHECK(resolveCompOpEqual(context, node->ownerSwitch->expression, oneExpression));
                    if (node->computedValue()->reg.b)
                        node->addSpecFlag(AstSwitchCase::SPEC_FLAG_IS_TRUE);
                    else
                        node->addSpecFlag(AstSwitchCase::SPEC_FLAG_IS_FALSE);
                    node->releaseComputedValue();
                }
            }

            // switch without an expression : a case is a boolean expression
            else
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, oneExpression->typeInfo, nullptr, oneExpression, CAST_FLAG_FOR_COMPARE | CAST_FLAG_AUTO_BOOL));
            }
        }
    }

    if (node->ownerSwitch->expression)
        node->typeInfo = node->ownerSwitch->expression->typeInfo;

    if (!node->expressions.empty())
        node->setBcNotifyBefore(ByteCodeGen::emitSwitchCaseBeforeCase);
    node->block->setBcNotifyBefore(ByteCodeGen::emitSwitchCaseBeforeBlock);
    node->block->setBcNotifyAfter(ByteCodeGen::emitSwitchCaseAfterBlock, ByteCodeGen::emitLeaveScope);

    return true;
}

bool Semantic::resolveLoop(SemanticContext* context)
{
    const auto module = context->sourceFile->module;
    const auto node   = castAst<AstLoop>(context->node, AstNodeKind::Loop);

    if (node->expression)
    {
        // No range
        if (node->expression->isNot(AstNodeKind::Range))
        {
            const auto typeInfo = TypeManager::concretePtrRef(node->expression->typeInfo);
            if (!typeInfo->isEnum())
                SWAG_CHECK(checkIsConcrete(context, node->expression));

            {
                PushErrCxtStep ec(context, node, ErrCxtStepKind::Note, [] { return toNte(Nte0021); }, true);
                SWAG_CHECK(resolveIntrinsicCountOf(context, node->expression, node->expression));
                YIELD();
            }

            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, node->expression->typeInfo, nullptr, node->expression, CAST_FLAG_TRY_COERCE));
            node->typeInfo = node->expression->typeInfo;

            // Do not evaluate loop if it's constant and 0
            if (module->mustOptimizeBytecode(node) && node->expression->hasFlagComputedValue())
            {
                if (!node->expression->computedValue()->reg.u64)
                {
                    node->addAstFlag(AST_NO_BYTECODE);
                    node->addAstFlag(AST_NO_BYTECODE_CHILDREN);
                    return true;
                }
            }
        }

        // Range
        else
        {
            const auto rangeNode = castAst<AstRange>(node->expression, AstNodeKind::Range);
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoS64, rangeNode->expressionLow->typeInfo, nullptr, rangeNode->expressionLow, CAST_FLAG_TRY_COERCE));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoS64, rangeNode->expressionUp->typeInfo, nullptr, rangeNode->expressionUp, CAST_FLAG_TRY_COERCE));
        }

        node->expression->allocateExtension(ExtensionKind::ByteCode);
        node->expression->setBcNotifyAfter(ByteCodeGen::emitLoopAfterExpr);
    }

    node->byteCodeFct = ByteCodeGen::emitLoop;
    node->setBcNotifyAfter(ByteCodeGen::emitLeaveScope);
    node->block->setBcNotifyAfter(ByteCodeGen::emitLoopAfterBlock, ByteCodeGen::emitLeaveScope);
    if (!node->expression)
        node->block->setBcNotifyBefore(ByteCodeGen::emitLoopBeforeBlock);

    // :SpecPropagateReturn
    if (node->breakableFlags.has(BREAKABLE_RETURN_IN_INFINITE_LOOP) && node->breakList.empty())
        propagateReturn(node->parent);

    SWAG_CHECK(SemanticError::warnWhereDoIf(context));

    return true;
}

bool Semantic::resolveVisit(SemanticContext* context)
{
    auto job  = context->baseJob;
    auto node = castAst<AstVisit>(context->node, AstNodeKind::Visit);

    auto typeInfo = TypeManager::concretePtrRefType(node->expression->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);

    if (!typeInfo->isEnum())
        SWAG_CHECK(checkIsConcrete(context, node->expression));
    if (typeInfo->isListArray())
        typeInfo = TypeManager::convertTypeListToArray(context, castTypeInfo<TypeInfoList>(typeInfo), node->expression->hasAstFlag(AST_CONST_EXPR));

    // Be sure that aliases are not defined elsewhere
    for (const auto& c : node->aliasNames)
    {
        auto id   = createTmpId(context, node, c.text);
        id->token = c;
        SWAG_CHECK(resolveIdentifier(context, id, RI_FOR_ZERO_GHOSTING));
        YIELD();
    }

    // Struct type : convert to a opVisit call
    AstNode* newExpression = nullptr;
    if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!typeInfo->isTuple(), context->report({node->expression, toErr(Err0279)}));

        AstIdentifierRef* identifierRef = nullptr;
        AstIdentifier*    callVisit     = nullptr;
        AstVarDecl*       varNode       = nullptr;

        if (node->expression->isNot(AstNodeKind::IdentifierRef))
        {
            varNode = Ast::newVarDecl(form(R"(__9tmp_%d)", g_UniqueID.fetch_add(1)), nullptr, node);
            varNode->addAstFlag(AST_GENERATED);
            newExpression       = Ast::clone(node->expression, varNode);
            varNode->assignment = newExpression;

            Ast::removeFromParent(node->expression);
            identifierRef = Ast::newIdentifierRef(varNode->token.text, nullptr, node);
            newExpression = identifierRef;
        }
        else
        {
            identifierRef = castAst<AstIdentifierRef>(Ast::cloneRaw(node->expression, node));
            newExpression = identifierRef;
        }

        callVisit = Ast::newIdentifier(identifierRef, form(R"(opVisit%s)", node->extraNameToken.cstr()), nullptr, identifierRef);
        callVisit->allocateIdentifierExtension();
        callVisit->identifierExtension->aliasNames = node->aliasNames;
        callVisit->inheritTokenLocation(node->token);

        // Generic parameters
        callVisit->genericParameters = Ast::newFuncCallGenParams(nullptr, callVisit);
        callVisit->genericParameters->addAstFlag(AST_NO_BYTECODE);

        auto child0      = Ast::newFuncCallParam(nullptr, callVisit->genericParameters);
        child0->typeInfo = g_TypeMgr->typeInfoBool;
        child0->setFlagsValueIsComputed();
        child0->computedValue()->reg.b = node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER);
        child0->addAstFlag(AST_NO_SEMANTIC);

        auto child1      = Ast::newFuncCallParam(nullptr, callVisit->genericParameters);
        child1->typeInfo = g_TypeMgr->typeInfoBool;
        child1->setFlagsValueIsComputed();
        child1->computedValue()->reg.b = node->hasSpecFlag(AstVisit::SPEC_FLAG_BACK);
        child1->addAstFlag(AST_NO_SEMANTIC);

        // Call with arguments
        callVisit->callParameters = Ast::newFuncCallParams(nullptr, callVisit);

        Ast::removeFromParent(node->block);
        Ast::addChildBack(node, node->block);
        node->expression->addAstFlag(AST_NO_BYTECODE);

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node->block);
        if (varNode)
            job->nodes.push_back(varNode);
        job->nodes.push_back(node);
        return true;
    }

    if (!node->extraNameToken.text.empty())
    {
        Diagnostic err{node, node->extraNameToken, formErr(Err0701, Naming::aKindName(typeInfo).cstr())};
        err.addNote(node->expression, Diagnostic::isType(typeInfo));
        return context->report(err);
    }

    if (node->aliasNames.size() > 2)
    {
        Diagnostic err{node, node->aliasNames[2], formErr(Err0684, node->aliasNames.size())};
        return context->report(err);
    }

    Utf8 alias0Name = node->aliasNames.empty() ? Utf8("#alias0") : node->aliasNames[0].text;
    Utf8 alias1Name = node->aliasNames.size() <= 1 ? Utf8("#alias1") : node->aliasNames[1].text;
    Utf8 content;

    // Get back the expression string
    FormatAst     fmtAst{context->tmpConcat};
    FormatContext fmtCxt;
    context->tmpConcat.init(1024);
    SWAG_CHECK(fmtAst.outputNode(fmtCxt, node->expression));
    Utf8 result = fmtAst.getUtf8();

    AstNode* newVar        = nullptr;
    size_t   firstAliasVar = 0;
    uint32_t id            = g_UniqueID.fetch_add(1);

    Utf8 visitBack;
    if (node->hasSpecFlag(AstVisit::SPEC_FLAG_BACK))
    {
        visitBack += " ";
        visitBack += g_LangSpec->name_back;
    }

    // Multi dimensional array
    if (typeInfo->isArray() && castTypeInfo<TypeInfoArray>(typeInfo)->pointedType->isArray())
    {
        auto typeArray   = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto pointedType = typeArray->finalType;

        auto varDecl = Ast::newVarDecl(form("__tmp%u", id), nullptr, node);
        varDecl->addSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN | AstVarDecl::SPEC_FLAG_LET);
        varDecl->assignment = Ast::newIntrinsicProp(TokenId::IntrinsicDataOf, nullptr, varDecl);
        (void) Ast::clone(node->expression, varDecl->assignment);
        varDecl->assignment->firstChild()->addAstFlag(AST_NO_SEMANTIC);
        newVar = varDecl;

        firstAliasVar = 2;
        content += "{ ";
        content += form(R"(let __addr%u = cast(%s ^%s) __tmp%u; )", id, typeArray->isConst() ? "const" : "", typeArray->finalType->name.cstr(), id);
        content += form(R"(loop%s %u { )", visitBack.cstr(), typeArray->totalCount);
        if (node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u + #index; )", id);
        }
        else if (pointedType->isStruct())
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = #ref &__addr%u[#index]; )", id);
        }
        else
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u[#index]; )", id);
        }

        content += R"(let )";
        content += alias1Name;
        content += R"( = #index;}})";
    }

    // One dimensional array
    else if (typeInfo->isArray())
    {
        auto typeArray   = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto pointedType = typeArray->pointedType;

        auto varDecl        = Ast::newVarDecl(form("__addr%u", id), nullptr, node);
        varDecl->assignment = Ast::newIntrinsicProp(TokenId::IntrinsicDataOf, nullptr, varDecl);
        varDecl->addSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN | AstVarDecl::SPEC_FLAG_LET);
        (void) Ast::clone(node->expression, varDecl->assignment);
        varDecl->assignment->firstChild()->addAstFlag(AST_NO_SEMANTIC);
        newVar = varDecl;

        firstAliasVar = 1;
        content += "{ ";
        content += form(R"(loop%s %u { )", visitBack.cstr(), typeArray->totalCount);
        if (node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u + #index; )", id);
        }
        else if (pointedType->isStruct())
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = #ref &__addr%u[#index]; )", id);
        }
        else
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u[#index]; )", id);
        }

        content += R"(let )";
        content += alias1Name;
        content += R"( = #index;}})";
    }

    // Slice
    else if (typeInfo->isSlice())
    {
        auto typeSlice   = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto pointedType = typeSlice->pointedType;

        auto varDecl = Ast::newVarDecl(form("__tmp%u", id), nullptr, node);
        varDecl->addSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN | AstVarDecl::SPEC_FLAG_LET);
        varDecl->assignment = Ast::clone(node->expression, varDecl);
        newVar              = varDecl;

        firstAliasVar = 1;
        content += "{ ";
        content += form(R"(let __addr%u = @dataof(__tmp%u); )", id, id);
        content += form(R"(loop%s __tmp%u { )", visitBack.cstr(), id);
        if (node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        {
            content += "let ";
            content += alias0Name;
            content += form(R"( = __addr%u + #index; )", id);
        }
        else if (pointedType->isStruct())
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = #ref &__addr%u[#index]; )", id);
        }
        else
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u[#index]; )", id);
        }

        content += R"(let )";
        content += alias1Name;
        content += R"( = #index;}})";
    }

    // String
    else if (typeInfo->isString())
    {
        auto varDecl = Ast::newVarDecl(form("__tmp%u", id), nullptr, node);
        varDecl->addSpecFlag(AstVarDecl::SPEC_FLAG_CONST_ASSIGN | AstVarDecl::SPEC_FLAG_LET);
        varDecl->assignment = Ast::clone(node->expression, varDecl);
        newVar              = varDecl;

        firstAliasVar = 1;
        content += "{ ";
        content += form(R"(let __addr%u = @dataof(__tmp%u); )", id, id);
        content += form(R"(loop%s __tmp%u { )", visitBack.cstr(), id);
        if (node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u + #index; )", id);
        }
        else
        {
            content += R"(let )";
            content += alias0Name;
            content += form(R"( = __addr%u[#index]; )", id);
        }

        content += R"(let )";
        content += alias1Name;
        content += R"( = #index;}})";
    }

    // Variadic
    else if (typeInfo->isVariadic() || typeInfo->isTypedVariadic())
    {
        if (node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        {
            Diagnostic err{node, node->wantPointerToken, toErr(Err0275)};
            err.addNote(node->expression, Diagnostic::isType(node->expression->typeInfo));
            return context->report(err);
        }

        content += form(R"({ loop%s %s { )", visitBack.cstr(), result.cstr());
        firstAliasVar = 0;
        content += R"(let )";
        content += alias0Name;
        content += form(R"( = %s[#index]; )", result.cstr());

        content += R"(let )";
        content += alias1Name;
        content += R"( = #index;}})";
    }

    // Enum
    else if (typeInfo->isEnum())
    {
        if (node->hasSpecFlag(AstVisit::SPEC_FLAG_WANT_POINTER))
        {
            Diagnostic err{node, node->wantPointerToken, toErr(Err0276)};
            err.addNote(node->expression, Diagnostic::isType(node->expression->typeInfo));
            return context->report(err);
        }

        auto typeEnum = castTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        content += form(R"({ let __addr%u = @typeof(%s); )", id, result.cstr());
        content += form(R"(loop%s %d { )", visitBack.cstr(), typeEnum->values.size());
        firstAliasVar = 1;
        content += R"(let )";
        content += alias0Name;
        content += R"( = dref cast(const* )";
        content += typeInfo->name;
        content += form(R"() __addr%u.values[#index].value; )", id);

        content += R"(let )";
        content += alias1Name;
        content += R"( = #index;}})";
    }
    else if (typeInfo->isPointer())
    {
        return context->report({node->expression, formErr(Err0278, typeInfo->getDisplayNameC())});
    }
    else
    {
        return context->report({node->expression, formErr(Err0280, typeInfo->getDisplayNameC())});
    }

    node->expression->addAstFlag(AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

    Parser parser;
    parser.setup(context, context->sourceFile->module, context->sourceFile, PARSER_DEFAULT);
    SWAG_CHECK(parser.constructEmbeddedAst(content, node, node, CompilerAstKind::EmbeddedInstruction, false));

    newExpression = node->lastChild();
    if (newVar)
    {
        Ast::removeFromParent(newVar);
        Ast::addChildFront(newExpression, newVar);
        newVar->inheritOwners(newExpression);
    }

    // In case of error (like an already defined identifier), we need to set the correct location of declared
    // variables
    size_t countVar   = 0;
    size_t countAlias = 0;
    Ast::visit(newExpression, [&](AstNode* x) {
        if (countAlias >= node->aliasNames.size())
            return;
        if (x->is(AstNodeKind::VarDecl))
        {
            if (countVar == firstAliasVar)
            {
                x->token.startLocation = node->aliasNames[countAlias].startLocation;
                x->token.endLocation   = node->aliasNames[countAlias].endLocation;
                countAlias++;
                firstAliasVar++;
            }

            countVar++;
        }
    });

    // First child is the let in the statement, and first child of this is the loop node
    auto loopNode = castAst<AstLoop>(node->lastChild()->lastChild(), AstNodeKind::Loop);
    loopNode->setOwnerBreakable(node->safeOwnerBreakable());
    Ast::removeFromParent(node->block);
    Ast::addChildBack(loopNode->block, node->block);
    SWAG_ASSERT(node->block);

    Ast::visit(context, node->block, [&](ErrorContext*, AstNode* x) {
        if (!x->hasOwnerBreakable())
            x->setOwnerBreakable(loopNode);
        else if (x->ownerBreakable()->isParentOf(loopNode))
        {
            if (x->token.is(TokenId::KwdBreak))
                x->ownerBreakable()->breakList.erase_unordered_by_val(castAst<AstBreakContinue>(x));
            else if (x->token.is(TokenId::KwdContinue))
                x->ownerBreakable()->continueList.erase_unordered_by_val(castAst<AstBreakContinue>(x));
            x->setOwnerBreakable(loopNode);
        }
        if (x->is(AstNodeKind::Visit))
            return Ast::VisitResult::Stop;
        return Ast::VisitResult::Continue;
    });
    node->block->removeAstFlag(AST_NO_SEMANTIC);
    loopNode->block->token.endLocation = node->block->token.endLocation;

    // Re-root the parent scope of the user block so that it points to the scope of the loop block
    auto ownerScope = node->block->ownerScope;
    ownerScope->parentScope->removeChildNoLock(ownerScope);
    ownerScope->parentScope = loopNode->block->firstChild()->ownerScope;
    ownerScope->parentScope->addChildNoLock(ownerScope);

    job->nodes.pop_back();
    job->nodes.push_back(newExpression);
    job->nodes.push_back(node);

    SWAG_CHECK(SemanticError::warnWhereDoIf(context));

    return true;
}

bool Semantic::resolveIndex(SemanticContext* context)
{
    auto node = context->node;

    auto ownerBreakable = node->safeOwnerBreakable();
    while (ownerBreakable && !ownerBreakable->breakableFlags.has(BREAKABLE_CAN_HAVE_INDEX))
        ownerBreakable = ownerBreakable->safeOwnerBreakable();
    SWAG_VERIFY(ownerBreakable, context->report({node, toErr(Err0312)}));

    ownerBreakable->breakableFlags.add(BREAKABLE_NEED_INDEX);

    // Take the type from the expression
    if (ownerBreakable->is(AstNodeKind::Loop))
    {
        const auto loopNode = castAst<AstLoop>(ownerBreakable, AstNodeKind::Loop);
        if (loopNode->expression && loopNode->expression->typeInfo->isNativeInteger())
            node->typeInfo = loopNode->expression->typeInfo;
    }

    if (!node->typeInfo)
        node->typeInfo = g_TypeMgr->typeInfoU64;
    node->byteCodeFct = ByteCodeGen::emitIndex;
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::preResolveSubstBreakContinue(SemanticContext* context)
{
    const auto node = castAst<AstSubstBreakContinue>(context->node, AstNodeKind::SubstBreakContinue);

    if (node->safeOwnerBreakable() == node->altSubstBreakable)
        node->defaultSubst->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);
    else
        node->altSubst->addAstFlag(AST_NO_SEMANTIC | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDREN);

    return true;
}

bool Semantic::resolveBreak(SemanticContext* context)
{
    auto node = castAst<AstBreakContinue>(context->node, AstNodeKind::Break);

    // Label has been defined : search the corresponding ScopeBreakable node
    if (!node->label.text.empty())
    {
        auto breakable = node->safeOwnerBreakable();
        while (breakable && (breakable->isNot(AstNodeKind::ScopeBreakable) || breakable->token.text != node->label.text))
            breakable = breakable->safeOwnerBreakable();
        SWAG_VERIFY(breakable, context->report({node->token.sourceFile, node->label, formErr(Err0722, node->label.text.cstr())}));
        node->setOwnerBreakable(breakable);
    }

    SWAG_VERIFY(node->hasOwnerBreakable(), context->report({node, toErr(Err0329)}));
    node->ownerBreakable()->breakList.push_back(node);

    if (node->parent->is(AstNodeKind::SwitchCaseBlock) && node->parent->firstChild() != node)
        SWAG_CHECK(context->report({node, toErr(Wrn0007), DiagnosticLevel::Warning}));

    SWAG_CHECK(SemanticError::warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGen::emitBreak;
    return true;
}

bool Semantic::resolveUnreachable(SemanticContext* context)
{
    const auto node = context->node;
    SWAG_CHECK(SemanticError::warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGen::emitUnreachable;
    return true;
}

bool Semantic::resolveFallThrough(SemanticContext* context)
{
    auto node = castAst<AstBreakContinue>(context->node, AstNodeKind::FallThrough);
    SWAG_VERIFY(node->hasOwnerBreakable() && node->ownerBreakable()->is(AstNodeKind::Switch), context->report({node, toErr(Err0337)}));
    node->ownerBreakable()->fallThroughList.push_back(node);

    // Be sure we are in a case
    auto parent = node->parent;
    while (parent && parent->isNot(AstNodeKind::SwitchCase) && parent != node->safeOwnerBreakable())
        parent = parent->parent;
    SWAG_VERIFY(parent && parent->is(AstNodeKind::SwitchCase), context->report({node, toErr(Err0336)}));
    node->switchCase = castAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);
    SWAG_VERIFY(node->switchCase->caseIndex != UINT32_MAX, context->report({node, toErr(Err0336)}));

    // 'fallthrough' cannot be used on the last case, this has no sens
    const auto switchBlock = castAst<AstSwitch>(node->ownerBreakable(), AstNodeKind::Switch);
    SWAG_VERIFY(node->switchCase->caseIndex < switchBlock->cases.size() - 1, context->report({node, toErr(Err0335)}));

    SWAG_CHECK(SemanticError::warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGen::emitFallThrough;

    return true;
}

bool Semantic::resolveContinue(SemanticContext* context)
{
    auto node = castAst<AstBreakContinue>(context->node, AstNodeKind::Continue);
    SWAG_VERIFY(node->hasOwnerBreakable(), context->report({node, toErr(Err0333)}));

    auto checkBreakable = node->safeOwnerBreakable();
    while (checkBreakable && !checkBreakable->breakableFlags.has(BREAKABLE_CAN_HAVE_CONTINUE))
        checkBreakable = checkBreakable->safeOwnerBreakable();
    SWAG_VERIFY(checkBreakable, context->report({node, toErr(Err0333)}));
    checkBreakable->continueList.push_back(node);
    node->setOwnerBreakable(checkBreakable);

    SWAG_CHECK(SemanticError::warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGen::emitContinue;
    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveScopeBreakable(SemanticContext* context)
{
    const auto node = castAst<AstScopeBreakable>(context->node, AstNodeKind::ScopeBreakable);
    node->block->setBcNotifyBefore(ByteCodeGen::emitLabelBeforeBlock);
    node->block->setBcNotifyAfter(ByteCodeGen::emitLoopAfterBlock, ByteCodeGen::emitLeaveScope);
    return true;
}
