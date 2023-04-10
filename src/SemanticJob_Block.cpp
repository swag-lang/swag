#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "AstOutput.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Report.h"
#include "LanguageSpec.h"
#include "Parser.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    // :ConcreteRef
    node->boolExpression->typeInfo = getConcreteTypeUnRef(node->boolExpression, CONCRETE_ALL);

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not generate backend if 'if' is constant, and has already been evaluated
    if (module->mustOptimizeBC(node) && (node->boolExpression->flags & AST_VALUE_COMPUTED))
    {
        node->boolExpression->flags |= AST_NO_BYTECODE;
        if (node->boolExpression->computedValue->reg.b)
        {
            if (node->elseBlock)
                node->elseBlock->flags |= AST_NO_BYTECODE;
        }
        else
        {
            node->ifBlock->flags |= AST_NO_BYTECODE;
        }

        return true;
    }

    node->byteCodeFct = ByteCodeGenJob::emitIf;
    node->boolExpression->allocateExtension(ExtensionKind::ByteCode);
    node->boolExpression->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterExpr;
    node->ifBlock->allocateExtension(ExtensionKind::ByteCode);
    node->ifBlock->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterIf;

    return true;
}

bool SemanticJob::resolveWhile(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    // :ConcreteRef
    node->boolExpression->typeInfo = getConcreteTypeUnRef(node->boolExpression, CONCRETE_ALL);

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not evaluate while if it's constant and false
    if (module->mustOptimizeBC(node) && (node->boolExpression->flags & AST_VALUE_COMPUTED))
    {
        if (!node->boolExpression->computedValue->reg.b)
        {
            node->boolExpression->flags |= AST_NO_BYTECODE;
            node->block->flags |= AST_NO_BYTECODE;
            return true;
        }
    }

    if (node->boolExpression->flags & AST_VALUE_COMPUTED && node->boolExpression->computedValue->reg.b)
    {
        Diagnostic diag{node->boolExpression, Err(Err0880)};
        auto       note = Diagnostic::help(Hlp(Hlp0030));
        return context->report(diag, note);
    }

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->boolExpression->allocateExtension(ExtensionKind::ByteCode);
    auto extension = node->boolExpression->extByteCode();
    SWAG_ASSERT(!extension->byteCodeBeforeFct || extension->byteCodeBeforeFct == ByteCodeGenJob::emitWhileBeforeExpr);
    extension->byteCodeBeforeFct = ByteCodeGenJob::emitWhileBeforeExpr;

    SWAG_ASSERT(!extension->byteCodeAfterFct || extension->byteCodeAfterFct == ByteCodeGenJob::emitWhileAfterExpr);
    extension->byteCodeAfterFct = ByteCodeGenJob::emitWhileAfterExpr;

    node->block->allocateExtension(ExtensionKind::ByteCode);
    extension = node->block->extByteCode();
    SWAG_ASSERT(!extension->byteCodeAfterFct || extension->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope || extension->byteCodeAfterFct == ByteCodeGenJob::emitLoopAfterBlock);
    extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;

    // :SpecPropagateReturn
    if (node->breakableFlags & BREAKABLE_RETURN_IN_INFINIT_LOOP && node->breakList.empty())
        propagateReturn(node->parent);

    return true;
}

bool SemanticJob::resolveInlineBefore(SemanticContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);

    ErrorCxtStep expNode;
    expNode.node = node->parent;
    expNode.type = ErrCxtStepKind::Inline;
    context->errCxtSteps.push_back(expNode);

    if (node->semFlags & SEMFLAG_RESOLVE_INLINED)
        return true;
    node->semFlags |= SEMFLAG_RESOLVE_INLINED;

    auto func = node->func;

    // :DirectInlineLocalVar
    // For a return by copy, need to reserve room on the stack for the return result
    if (func->returnType && func->returnType->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
    {
        node->flags |= AST_TRANSIENT;
        node->allocateComputedValue();
        node->computedValue->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += func->returnType->typeInfo->sizeOf;
        SemanticJob::setOwnerMaxStackSize(node, node->ownerScope->startStackSize);
    }

    node->scope->startStackSize = node->ownerScope->startStackSize;

    // If we inline a throwable function, be sure the top level function is informed
    if (func->typeInfo->flags & TYPEINFO_CAN_THROW)
        node->ownerFct->specFlags |= AstFuncDecl::SPECFLAG_REG_GET_CONTEXT;

    // Register all function parameters as inline symbols
    if (func->parameters)
    {
        AstIdentifier* identifier = nullptr;
        if (node->parent->kind == AstNodeKind::Identifier)
            identifier = CastAst<AstIdentifier>(node->parent, AstNodeKind::Identifier, AstNodeKind::FuncCall);
        for (size_t i = 0; i < func->parameters->childs.size(); i++)
        {
            auto funcParam = func->parameters->childs[i];

            // If the call parameter of the inlined function is constant, then we register it in a specific
            // constant scope, not in the caller (for mixins)/inline scope.
            // This is a separated scope because mixins do not have their own scope, and we must have a
            // different symbol registration for each constant value
            // :InlineUsingParam
            bool              isConstant   = false;
            AstFuncCallParam* orgCallParam = nullptr;
            if (identifier && identifier->callParameters)
            {
                for (size_t j = 0; j < identifier->callParameters->childs.size(); j++)
                {
                    auto callParam = CastAst<AstFuncCallParam>(identifier->callParameters->childs[j], AstNodeKind::FuncCallParam);
                    if (callParam->indexParam != i)
                        continue;
                    orgCallParam = callParam;
                    if (!(callParam->flags & AST_VALUE_COMPUTED))
                        continue;
                    AddSymbolTypeInfo toAdd;
                    toAdd.node                = callParam;
                    toAdd.typeInfo            = funcParam->typeInfo;
                    toAdd.kind                = SymbolKind::Variable;
                    toAdd.computedValue       = callParam->computedValue;
                    toAdd.flags               = OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN | OVERLOAD_COMPUTED_VALUE;
                    toAdd.storageOffset       = callParam->computedValue->storageOffset;
                    toAdd.storageSegment      = callParam->computedValue->storageSegment;
                    toAdd.aliasName           = &funcParam->token.text;
                    auto overload             = node->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
                    overload->fromInlineParam = orgCallParam;
                    isConstant                = true;
                    break;
                }
            }

            if (!isConstant)
            {
                SWAG_ASSERT(node->parametersScope);
                AddSymbolTypeInfo toAdd;
                toAdd.node                = funcParam;
                toAdd.typeInfo            = funcParam->typeInfo;
                toAdd.kind                = SymbolKind::Variable;
                toAdd.flags               = OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN;
                auto overload             = node->parametersScope->symTable.addSymbolTypeInfo(context, toAdd);
                overload->fromInlineParam = orgCallParam;
            }
        }
    }

    // Sub declarations in an inline block have access to the 'parametersScope', so we must start resolving
    // them only when we have are filling the parameters
    resolveSubDecls(context, node->ownerFct);

    return true;
}

bool SemanticJob::resolveInlineAfter(SemanticContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);
    auto fct  = node->func;

    // No need to check missing return for inline only, because the function has already been
    // checked as a separated function
    if (fct->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
    {
        if (fct->returnType && fct->returnType->typeInfo != g_TypeMgr->typeInfoVoid)
        {
            if (!(node->semFlags & SEMFLAG_SCOPE_HAS_RETURN))
            {
                if (node->semFlags & SEMFLAG_FCT_HAS_RETURN)
                    return context->report({fct, Fmt(Err(Err0748), fct->getDisplayNameC())});
                return context->report({fct, Fmt(Err(Err0606), fct->getDisplayNameC())});
            }
        }
    }

    context->errCxtSteps.pop_back();
    return true;
}

bool SemanticJob::resolveForBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstFor>(context->node, AstNodeKind::For);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveLoopBefore(SemanticContext* context)
{
    auto node                        = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    node->ownerScope->startStackSize = node->ownerScope->parentScope->startStackSize;
    return true;
}

bool SemanticJob::resolveFor(SemanticContext* context)
{
    auto node = CastAst<AstFor>(context->node, AstNodeKind::For);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, node->boolExpression));
    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->allocateExtension(ExtensionKind::ByteCode);
    node->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    node->boolExpression->allocateExtension(ExtensionKind::ByteCode);
    SWAG_ASSERT(!node->boolExpression->extByteCode()->byteCodeBeforeFct);
    SWAG_ASSERT(!node->boolExpression->extByteCode()->byteCodeAfterFct);
    node->boolExpression->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->extByteCode()->byteCodeAfterFct  = ByteCodeGenJob::emitForAfterExpr;

    node->postExpression->allocateExtension(ExtensionKind::ByteCode);
    SWAG_ASSERT(!node->postExpression->extByteCode()->byteCodeBeforeFct);
    node->postExpression->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforePost;

    node->block->allocateExtension(ExtensionKind::ByteCode);
    SWAG_ASSERT(!node->block->extByteCode()->byteCodeAfterFct || node->block->extByteCode()->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope);
    node->block->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;

    // :SpecPropagateReturn
    if (node->breakableFlags & BREAKABLE_RETURN_IN_INFINIT_LOOP && node->breakList.empty())
        propagateReturn(node->parent);

    return true;
}

bool SemanticJob::resolveSwitchAfterExpr(SemanticContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node->parent, AstNodeKind::Switch);
    auto typeInfo   = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC);

    // For a switch on an enum, force a 'using' for each case
    if (typeInfo->isEnum())
    {
        // :AutoScope
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        for (auto switchCase : switchNode->cases)
        {
            switchCase->addAlternativeScope(typeEnum->scope);
        }
    }

    // Automatic convert to 'kindOf'
    // This has no sens to do a switch on an 'any'. So instead of raising an error,
    // we implies the usage of '@kindof'. That way we have a switch on the underlying type.
    if (node->typeInfo->isAny())
    {
        switchNode->beforeAutoCastType = node->typeInfo;
        node->byteCodeFct              = ByteCodeGenJob::emitImplicitKindOf;
        auto& typeGen                  = node->sourceFile->module->typeGen;
        SWAG_CHECK(checkIsConcrete(context, node));
        node->allocateComputedValue();
        node->computedValue->storageSegment = getConstantSegFromContext(context->node);
        SWAG_CHECK(typeGen.genExportedTypeInfo(context, node->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, GEN_EXPORTED_TYPE_SHOULD_WAIT, &node->typeInfo));
        if (context->result != ContextResult::Done)
            return true;
    }

    return true;
}

bool SemanticJob::resolveSwitch(SemanticContext* context)
{
    auto node = CastAst<AstSwitch>(context->node, AstNodeKind::Switch);

    // Deal with complete
    SWAG_CHECK(collectAttributes(context, node, nullptr));
    SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_COMPLETE) || node->expression, context->report({node, node->token, Err(Err0607)}));

    node->byteCodeFct = ByteCodeGenJob::emitSwitch;
    if (!node->expression)
    {
        node->allocateExtension(ExtensionKind::ByteCode);
        node->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitBeforeSwitch;
        return true;
    }

    node->expression->allocateExtension(ExtensionKind::ByteCode);
    node->expression->extByteCode()->byteCodeAfterFct = ByteCodeGenJob::emitSwitchAfterExpr;

    SWAG_CHECK(checkIsConcrete(context, node->expression));
    node->typeInfo = node->expression->typeInfo;

    auto typeSwitch = TypeManager::concreteType(node->typeInfo);

    // Verify switch expression type is valid
    SWAG_VERIFY(!typeSwitch->isAny(), context->report({node->expression, Err(Err0608)}));
    switch (typeSwitch->kind)
    {
    case TypeInfoKind::Slice:
    case TypeInfoKind::Array:
    case TypeInfoKind::Interface:
        return context->report({node->expression, Fmt(Err(Err0609), typeSwitch->getDisplayNameC())});
    default:
        break;
    }

    SWAG_VERIFY(!node->cases.empty(), context->report({node, node->token, Err(Err0610)}));

    // Collect constant expressions, to avoid double definitions
    VectorNative<AstNode*> valDef;
    VectorNative<uint64_t> val64;
    Vector<Utf8>           valText;
    for (auto switchCase : node->cases)
    {
        for (auto expr : switchCase->expressions)
        {
            if (expr->flags & AST_VALUE_COMPUTED)
            {
                auto typeExpr = TypeManager::concreteType(expr->typeInfo);
                if (typeExpr->isString())
                {
                    int idx = valText.find(expr->computedValue->text);
                    if (idx != -1)
                    {
                        auto note = Diagnostic::note(valDef[idx], Nte(Nte0014));
                        return context->report({expr, Fmt(Err(Err0611), expr->computedValue->text.c_str())}, note);
                    }

                    valText.push_back(expr->computedValue->text);
                    valDef.push_back(expr);
                }
                else
                {
                    auto value = expr->computedValue->reg.u64;
                    if (expr->flags & AST_VALUE_IS_TYPEINFO)
                        value = expr->computedValue->storageOffset;

                    int idx = val64.find(value);
                    if (idx != -1)
                    {
                        auto note = Diagnostic::note(valDef[idx], Nte(Nte0014));
                        if (expr->flags & AST_VALUE_IS_TYPEINFO)
                            return context->report({expr, Fmt(Err(Err0611), expr->token.ctext())}, note);
                        if (expr->typeInfo->isEnum())
                            return context->report({expr, Fmt(Err(Err0612), expr->token.ctext())}, note);
                        if (typeExpr->isNativeInteger())
                            return context->report({expr, Fmt(Err(Err0613), expr->computedValue->reg.u64)}, note);
                        return context->report({expr, Fmt(Err(Err0614), expr->computedValue->reg.f64)}, note);
                    }

                    val64.push_back(expr->computedValue->reg.u64);
                    valDef.push_back(expr);
                }
            }
            else if (node->attributeFlags & ATTRIBUTE_COMPLETE)
            {
                return checkIsConstExpr(context, expr->flags & AST_VALUE_COMPUTED, expr, Err(Err0615));
            }
        }
    }

    // When a switch is marked as complete, be sure every definitions have been covered
    if (node->attributeFlags & ATTRIBUTE_COMPLETE)
    {
        // No default for a complete switch
        auto back = node->cases.back();
        SWAG_VERIFY(!back->expressions.empty(), context->report({back, back->token, Err(Err0616), Hnt(Hnt0026)}));

        if (node->typeInfo->isEnum() && !node->beforeAutoCastType)
        {
            auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
            if (typeSwitch->isString())
            {
                if (valText.size() != typeEnum->values.size())
                {
                    for (auto one : typeEnum->values)
                    {
                        if (!valText.contains(one->value->text))
                        {
                            Diagnostic diag{node, node->token, Fmt(Err(Err0620), typeEnum->name.c_str(), one->name.c_str())};
                            auto       note = Diagnostic::note(one->declNode, one->declNode->token, Nte(Nte0034));
                            return context->report(diag, note);
                        }
                    }
                }
            }
            else
            {
                if (val64.size() != typeEnum->values.size())
                {
                    for (auto one : typeEnum->values)
                    {
                        if (!val64.contains(one->value->reg.u64))
                        {
                            Diagnostic diag{node, node->token, Fmt(Err(Err0620), typeEnum->name.c_str(), one->name.c_str())};
                            auto       note = Diagnostic::note(one->declNode, one->declNode->token, Nte(Nte0034));
                            return context->report(diag, note);
                        }
                    }
                }
            }
        }

        // Add a safety test in a switch default to catch runtime invalid values
        auto caseNode = Ast::newNode<AstSwitchCase>(nullptr, AstNodeKind::SwitchCase, context->sourceFile, node);
        caseNode->flags |= AST_GENERATED;
        caseNode->specFlags   = AstSwitchCase::SPECFLAG_IS_DEFAULT;
        caseNode->ownerSwitch = node;
        node->cases.push_back(caseNode);
        caseNode->byteCodeFct = ByteCodeGenJob::emitSafetySwitchDefault;
    }

    return true;
}

bool SemanticJob::resolveCase(SemanticContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (auto oneExpression : node->expressions)
    {
        // Range
        if (oneExpression->kind == AstNodeKind::Range)
        {
            auto rangeNode = CastAst<AstRange>(oneExpression, AstNodeKind::Range);
            if (node->ownerSwitch->expression)
            {
                auto typeInfo = TypeManager::concreteType(node->ownerSwitch->expression->typeInfo);
                if (typeInfo->isStruct())
                {
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, node->ownerSwitch->expression, rangeNode->expressionLow));
                    if (context->result != ContextResult::Done)
                        return true;
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opCmp, nullptr, nullptr, node->ownerSwitch->expression, rangeNode->expressionUp));
                    if (context->result != ContextResult::Done)
                        return true;
                }
                else
                {
                    TypeManager::promoteOne(context, node->ownerSwitch->expression);
                    TypeManager::promoteOne(context, rangeNode->expressionLow);
                    TypeManager::promoteOne(context, rangeNode->expressionUp);
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionLow, CASTFLAG_COMPARE));
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionUp, CASTFLAG_COMPARE));
                }
            }
            else
            {
                return context->report({rangeNode, Err(Err0337)});
            }
        }

        // Single expression
        else
        {
            SWAG_CHECK(checkIsConcreteOrType(context, oneExpression));
            if (context->result != ContextResult::Done)
                return true;

            // switch with an expression : compare case with the switch expression
            if (node->ownerSwitch->expression)
            {
                auto typeInfo = TypeManager::concreteType(node->ownerSwitch->expression->typeInfo);
                if (typeInfo->isStruct())
                {
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, node->ownerSwitch->expression, oneExpression));
                    if (context->result != ContextResult::Done)
                        return true;
                }
                else
                {
                    PushErrCxtStep ec(context, node->ownerSwitch->expression, ErrCxtStepKind::Note, [typeInfo]()
                                      { return Fmt(Nte(Nte0052), typeInfo->getDisplayNameC(), "the switch expression"), Diagnostic::isType(typeInfo); });
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression, CASTFLAG_COMPARE));
                }
            }

            // switch without an expression : a case is a boolean expression
            else
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, oneExpression->typeInfo, nullptr, oneExpression, CASTFLAG_COMPARE | CASTFLAG_AUTO_BOOL));
            }
        }
    }

    if (node->ownerSwitch->expression)
        node->typeInfo = node->ownerSwitch->expression->typeInfo;

    if (!node->expressions.empty())
    {
        node->allocateExtension(ExtensionKind::ByteCode);
        node->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeCase;
    }

    node->block->allocateExtension(ExtensionKind::ByteCode);
    node->block->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->extByteCode()->byteCodeAfterFct  = ByteCodeGenJob::emitSwitchCaseAfterBlock;
    return true;
}

bool SemanticJob::resolveLoop(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstLoop>(context->node, AstNodeKind::Loop);

    if (node->expression)
    {
        // No range
        if (node->expression->kind != AstNodeKind::Range)
        {
            auto typeInfo = TypeManager::concretePtrRef(node->expression->typeInfo);
            if (!typeInfo->isEnum())
                SWAG_CHECK(checkIsConcrete(context, node->expression));

            SWAG_CHECK(resolveIntrinsicCountOf(context, node->expression, node->expression));
            if (context->result != ContextResult::Done)
                return true;

            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, node->expression->typeInfo, nullptr, node->expression, CASTFLAG_TRY_COERCE));
            node->typeInfo = node->expression->typeInfo;

            // Do not evaluate loop if it's constant and 0
            if (module->mustOptimizeBC(node) && (node->expression->flags & AST_VALUE_COMPUTED))
            {
                if (!node->expression->computedValue->reg.u64)
                {
                    node->flags |= AST_NO_BYTECODE;
                    node->flags |= AST_NO_BYTECODE_CHILDS;
                    return true;
                }
            }
        }

        // Range
        else
        {
            auto rangeNode = CastAst<AstRange>(node->expression, AstNodeKind::Range);
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoS64, rangeNode->expressionLow->typeInfo, nullptr, rangeNode->expressionLow, CASTFLAG_TRY_COERCE));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoS64, rangeNode->expressionUp->typeInfo, nullptr, rangeNode->expressionUp, CASTFLAG_TRY_COERCE));
        }

        node->expression->allocateExtension(ExtensionKind::ByteCode);
        auto extension = node->expression->extByteCode();
        SWAG_ASSERT(!extension->byteCodeAfterFct || extension->byteCodeAfterFct == ByteCodeGenJob::emitLoopAfterExpr);
        extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterExpr;
    }

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->allocateExtension(ExtensionKind::ByteCode);
    auto extension = node->extByteCode();
    SWAG_ASSERT(!extension->byteCodeAfterFct || extension->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope);
    extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    node->block->allocateExtension(ExtensionKind::ByteCode);
    extension = node->block->extByteCode();
    SWAG_ASSERT(!extension->byteCodeAfterFct || extension->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope || extension->byteCodeAfterFct == ByteCodeGenJob::emitLoopAfterBlock);
    extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;

    if (!node->expression)
    {
        SWAG_ASSERT(!extension->byteCodeBeforeFct || extension->byteCodeBeforeFct == ByteCodeGenJob::emitLoopBeforeBlock);
        extension->byteCodeBeforeFct = ByteCodeGenJob::emitLoopBeforeBlock;
    }

    // :SpecPropagateReturn
    if (node->breakableFlags & BREAKABLE_RETURN_IN_INFINIT_LOOP && node->breakList.empty())
        propagateReturn(node->parent);

    return true;
}

bool SemanticJob::resolveVisit(SemanticContext* context)
{
    auto job        = context->job;
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstVisit>(context->node, AstNodeKind::Visit);

    auto typeInfo = TypeManager::concretePtrRefType(node->expression->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
    if (!typeInfo->isEnum())
        SWAG_CHECK(checkIsConcrete(context, node->expression));

    // Be sure that aliases are not defined elsewhere
    for (auto c : node->aliasNames)
    {
        auto id   = createTmpId(context, node, c.text);
        id->token = c;
        SWAG_CHECK(resolveIdentifier(context, id, RI_FOR_ZERO_GHOSTING));
        if (context->result != ContextResult::Done)
            return true;
    }

    // Struct type : convert to a opVisit call
    AstNode* newExpression = nullptr;
    if (typeInfo->isStruct())
    {
        SWAG_VERIFY(!(typeInfo->isTuple()), context->report({node->expression, Err(Err0624), Diagnostic::isType(typeInfo)}));
        SWAG_VERIFY(node->expression->kind == AstNodeKind::IdentifierRef, Report::internalError(node->expression, "resolveVisit expression, should be an identifier"));

        auto identifierRef = (AstIdentifierRef*) Ast::clone(node->expression, node);
        auto callVisit     = Ast::newIdentifier(sourceFile, Fmt("opVisit%s", node->extraNameToken.ctext()), identifierRef, identifierRef);
        callVisit->allocateIdentifierExtension();
        callVisit->identifierExtension->aliasNames = node->aliasNames;
        callVisit->inheritTokenLocation(node);

        // Generic parameters
        callVisit->genericParameters = Ast::newFuncCallGenParams(sourceFile, callVisit);
        callVisit->genericParameters->flags |= AST_NO_BYTECODE;
        auto child      = Ast::newFuncCallParam(sourceFile, callVisit->genericParameters);
        child->typeInfo = g_TypeMgr->typeInfoBool;
        child->setFlagsValueIsComputed();
        child->computedValue->reg.b = node->specFlags & AstVisit::SPECFLAG_WANT_POINTER;
        child->flags |= AST_NO_SEMANTIC;

        // Call with arguments
        callVisit->callParameters = Ast::newFuncCallParams(sourceFile, callVisit);
        newExpression             = identifierRef;

        Ast::removeFromParent(node->block);
        Ast::addChildBack(node, node->block);
        node->expression->flags |= AST_NO_BYTECODE;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node->block);
        job->nodes.push_back(node);
        return true;
    }

    if (!node->extraNameToken.text.empty())
    {
        Diagnostic diag{node, node->extraNameToken, Fmt(Err(Err0625), typeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0026);
        return context->report(diag);
    }

    if (node->aliasNames.size() > 2)
    {
        Diagnostic diag{node, node->aliasNames[2], Fmt(Err(Err0626), node->aliasNames.size())};
        diag.hint = Hnt(Hnt0026);
        return context->report(diag);
    }

    Utf8 alias0Name = node->aliasNames.empty() ? Utf8("@alias0") : node->aliasNames[0].text;
    Utf8 alias1Name = node->aliasNames.size() <= 1 ? Utf8("@alias1") : node->aliasNames[1].text;
    Utf8 content;

    // Get back the expression string
    auto& concat = context->job->tmpConcat;
    concat.init(1024);
    AstOutput::OutputContext outputContext;
    SWAG_CHECK(AstOutput::outputNode(outputContext, concat, node->expression));
    concat.addU8(0);
    SWAG_ASSERT(concat.firstBucket->nextBucket == nullptr);

    AstNode* newVar        = nullptr;
    int      firstAliasVar = 0;
    int      id            = g_UniqueID.fetch_add(1);

    // Multi dimensional array
    if (typeInfo->isArray() && ((TypeInfoArray*) typeInfo)->pointedType->isArray())
    {
        auto typeArray   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto pointedType = typeArray->finalType;

        auto varDecl        = Ast::newVarDecl(sourceFile, Utf8::format("__tmp%u", id), node);
        varDecl->assignment = Ast::newIntrinsicProp(sourceFile, TokenId::IntrinsicDataOf, varDecl);
        Ast::clone(node->expression, varDecl->assignment);
        newVar = varDecl;

        firstAliasVar = 2;
        content += "{ ";
        content += Fmt("var __addr%u = cast(%s ^%s) __tmp%u; ", id, typeArray->isConst() ? "const" : "", typeArray->finalType->name.c_str(), id);
        content += Fmt("loop %u { ", typeArray->totalCount);
        if (node->specFlags & AstVisit::SPECFLAG_WANT_POINTER)
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u + @index; ", id);
        }
        else if (pointedType->isStruct())
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = ref &__addr%u[@index]; ", id);
        }
        else
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u[@index]; ", id);
        }

        content += "var ";
        content += alias1Name;
        content += " = @index;}}";
    }

    // One dimensional array
    else if (typeInfo->isArray())
    {
        auto typeArray   = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto pointedType = typeArray->pointedType;

        auto varDecl        = Ast::newVarDecl(sourceFile, Utf8::format("__addr%u", id), node);
        varDecl->assignment = Ast::newIntrinsicProp(sourceFile, TokenId::IntrinsicDataOf, varDecl);
        Ast::clone(node->expression, varDecl->assignment);
        newVar = varDecl;

        firstAliasVar = 1;
        content += "{ ";
        content += Fmt("loop %u { ", typeArray->totalCount);
        if (node->specFlags & AstVisit::SPECFLAG_WANT_POINTER)
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u + @index; ", id);
        }
        else if (pointedType->isStruct())
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = ref &__addr%u[@index]; ", id);
        }
        else
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u[@index]; ", id);
        }

        content += "var ";
        content += alias1Name;
        content += " = @index;}}";
    }

    // Slice
    else if (typeInfo->isSlice())
    {
        auto typeSlice   = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto pointedType = typeSlice->pointedType;

        auto varDecl        = Ast::newVarDecl(sourceFile, Utf8::format("__tmp%u", id), node);
        varDecl->assignment = Ast::clone(node->expression, varDecl);
        newVar              = varDecl;

        firstAliasVar = 1;
        content += "{ ";
        content += Fmt("var __addr%u = @dataof(__tmp%u); ", id, id);
        content += Fmt("loop __tmp%u { ", id);
        if (node->specFlags & AstVisit::SPECFLAG_WANT_POINTER)
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u + @index; ", id);
        }
        else if (pointedType->isStruct())
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = ref &__addr%u[@index]; ", id);
        }
        else
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u[@index]; ", id);
        }

        content += "var ";
        content += alias1Name;
        content += " = @index;}}";
    }

    // String
    else if (typeInfo->isString())
    {
        auto varDecl        = Ast::newVarDecl(sourceFile, Utf8::format("__tmp%u", id), node);
        varDecl->assignment = Ast::clone(node->expression, varDecl);
        newVar              = varDecl;

        firstAliasVar = 1;
        content += "{ ";
        content += Fmt("var __addr%u = @dataof(__tmp%u); ", id, id);
        content += Fmt("loop __tmp%u { ", id);
        if (node->specFlags & AstVisit::SPECFLAG_WANT_POINTER)
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u + @index; ", id);
        }
        else
        {
            content += "var ";
            content += alias0Name;
            content += Fmt(" = __addr%u[@index]; ", id);
        }
        content += "var ";
        content += alias1Name;
        content += " = @index;}}";
    }

    // Variadic
    else if (typeInfo->isVariadic() || typeInfo->isTypedVariadic())
    {
        SWAG_VERIFY(!(node->specFlags & AstVisit::SPECFLAG_WANT_POINTER), context->report({node, Err(Err0627)}));
        content += Fmt("{ loop %s { ", (const char*) concat.firstBucket->datas);
        firstAliasVar = 0;
        content += "var ";
        content += alias0Name;
        content += Fmt(" = %s[@index]; ", (const char*) concat.firstBucket->datas);
        content += "var ";
        content += alias1Name;
        content += " = @index;}}";
    }

    // Enum
    else if (typeInfo->isEnum())
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        SWAG_VERIFY(!(node->specFlags & AstVisit::SPECFLAG_WANT_POINTER), context->report({node, Err(Err0636)}));
        content += Fmt("{ var __addr%u = @typeof(%s); ", id, (const char*) concat.firstBucket->datas);
        content += Fmt("loop %d { ", typeEnum->values.size());
        firstAliasVar = 1;
        content += "var ";
        content += alias0Name;
        content += " = dref cast(const* ";
        content += typeInfo->name;
        content += Fmt(") __addr%u.values[@index].value; ", id);
        content += "var ";
        content += alias1Name;
        content += " = @index;}}";
    }

    else
    {
        // Special error in case of a pointer
        if (typeInfo->isPointer())
        {
            auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            if (typePtr->pointedType->isEnum() ||
                typePtr->pointedType->isVariadic() ||
                typePtr->pointedType->isTypedVariadic() ||
                typePtr->pointedType->isSlice() ||
                typePtr->pointedType->isArray() ||
                typePtr->pointedType->isStruct() ||
                typePtr->pointedType->isString())
            {
                return context->report({node->expression, Fmt(Err(Err0628), typeInfo->getDisplayNameC()), Hnt(Hnt0037)});
            }
            else
            {
                return context->report({node->expression, Fmt(Err(Err0628), typeInfo->getDisplayNameC()), Hnt(Hnt0036)});
            }
        }

        return context->report({node->expression, Fmt(Err(Err0629), typeInfo->getDisplayNameC()), Hnt(Hnt0006)});
    }

    node->expression->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    Parser parser;
    parser.setup(context, context->sourceFile->module, context->sourceFile);
    SWAG_CHECK(parser.constructEmbeddedAst(content, node, node, CompilerAstKind::EmbeddedInstruction, false));

    newExpression = node->childs.back();
    if (newVar)
    {
        Ast::removeFromParent(newVar);
        Ast::addChildFront(newExpression, newVar);
        newVar->inheritOwners(newExpression);
    }

    // In case of error (like an already defined identifier), we need to set the correct location of declared
    // variables
    int countVar   = 0;
    int countAlias = 0;
    Ast::visit(newExpression, [&](AstNode* x)
               {
                   if (countAlias >= node->aliasNames.size())
                       return;
                   if (x->kind == AstNodeKind::VarDecl)
                   {
                       if (countVar == firstAliasVar)
                       {
                           x->token.startLocation = node->aliasNames[countAlias].startLocation;
                           x->token.endLocation   = node->aliasNames[countAlias].endLocation;
                           countAlias++;
                           firstAliasVar++;
                       }

                       countVar++;
                   } });

    // First child is the let in the statement, and first child of this is the loop node
    auto loopNode            = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
    loopNode->ownerBreakable = node->ownerBreakable;
    Ast::removeFromParent(node->block);
    Ast::addChildBack(loopNode->block, node->block);
    SWAG_ASSERT(node->block);
    Ast::visit(context, node->block, [&](ErrorContext* context, AstNode* x)
               {
            if (!x->ownerBreakable)
                x->ownerBreakable = loopNode;
            if (x->kind == AstNodeKind::Visit)
                return false;
            return true; });
    node->block->flags &= ~AST_NO_SEMANTIC;
    loopNode->block->token.endLocation = node->block->token.endLocation;

    // Re-root the parent scope of the user block so that it points to the scope of the loop block
    auto ownerScope = node->block->ownerScope;
    ownerScope->parentScope->removeChildNoLock(ownerScope);
    ownerScope->parentScope = loopNode->block->childs.front()->ownerScope;
    ownerScope->parentScope->addChildNoLock(ownerScope);

    job->nodes.pop_back();
    job->nodes.push_back(newExpression);
    job->nodes.push_back(node);

    return true;
}

bool SemanticJob::resolveGetErr(SemanticContext* context)
{
    auto node         = context->node;
    node->typeInfo    = g_TypeMgr->typeInfoString;
    node->byteCodeFct = ByteCodeGenJob::emitGetErr;
    return true;
}

bool SemanticJob::resolveIndex(SemanticContext* context)
{
    auto node = context->node;

    auto ownerBreakable = node->ownerBreakable;
    while (ownerBreakable && !(ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_INDEX))
        ownerBreakable = ownerBreakable->ownerBreakable;
    SWAG_VERIFY(ownerBreakable, context->report({node, Err(Err0630)}));

    ownerBreakable->breakableFlags |= BREAKABLE_NEED_INDEX;

    // Take the type from the expression
    if (ownerBreakable->kind == AstNodeKind::Loop)
    {
        auto loopNode = CastAst<AstLoop>(ownerBreakable, AstNodeKind::Loop);
        if (loopNode->expression && loopNode->expression->typeInfo->isNativeInteger())
            node->typeInfo = loopNode->expression->typeInfo;
    }

    if (!node->typeInfo)
        node->typeInfo = g_TypeMgr->typeInfoU64;
    node->byteCodeFct = ByteCodeGenJob::emitIndex;
    return true;
}

bool SemanticJob::preResolveSubstBreakContinue(SemanticContext* context)
{
    auto node = CastAst<AstSubstBreakContinue>(context->node, AstNodeKind::SubstBreakContinue);

    if (node->ownerBreakable == node->altSubstBreakable)
        node->defaultSubst->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;
    else
        node->altSubst->flags |= AST_NO_SEMANTIC | AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    return true;
}

bool SemanticJob::resolveBreak(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Break);

    // Label has been defined : search the corresponding ScopeBreakable node
    if (!node->label.text.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::ScopeBreakable || breakable->token.text != node->label.text))
            breakable = breakable->ownerBreakable;
        SWAG_VERIFY(breakable, context->report({node->sourceFile, node->label, Fmt(Err(Err0631), node->label.text.c_str())}));
        node->ownerBreakable = breakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, Err(Err0632)}));
    node->ownerBreakable->breakList.push_back(node);

    SWAG_CHECK(warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitBreak;
    return true;
}

bool SemanticJob::resolveFallThrough(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::FallThrough);
    SWAG_VERIFY(node->ownerBreakable && node->ownerBreakable->kind == AstNodeKind::Switch, context->report({node, Err(Err0633)}));
    node->ownerBreakable->fallThroughList.push_back(node);

    // Be sure we are in a case
    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::SwitchCase && parent != node->ownerBreakable)
        parent = parent->parent;
    SWAG_VERIFY(parent && parent->kind == AstNodeKind::SwitchCase, context->report({node, Err(Err0634)}));
    node->switchCase = CastAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);

    // 'fallthrough' cannot be used on the last case, this has no sens
    auto switchBlock = CastAst<AstSwitch>(node->ownerBreakable, AstNodeKind::Switch);
    SWAG_VERIFY(node->switchCase->caseIndex < switchBlock->cases.size() - 1, context->report({node, Err(Err0635)}));

    SWAG_CHECK(warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitFallThrough;

    return true;
}

bool SemanticJob::resolveContinue(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Continue);
    SWAG_VERIFY(node->ownerBreakable, context->report({node, Err(Err0637)}));

    auto checkBreakable = node->ownerBreakable;
    while (checkBreakable && !(checkBreakable->breakableFlags & BREAKABLE_CAN_HAVE_CONTINUE))
        checkBreakable = checkBreakable->ownerBreakable;
    SWAG_VERIFY(checkBreakable, context->report({node, Err(Err0637)}));
    checkBreakable->continueList.push_back(node);

    SWAG_CHECK(warnUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitContinue;
    return true;
}

bool SemanticJob::resolveScopeBreakable(SemanticContext* context)
{
    auto node = CastAst<AstScopeBreakable>(context->node, AstNodeKind::ScopeBreakable);
    node->block->allocateExtension(ExtensionKind::ByteCode);
    SWAG_ASSERT(!node->block->extByteCode()->byteCodeBeforeFct);
    SWAG_ASSERT(!node->block->extByteCode()->byteCodeAfterFct || node->block->extByteCode()->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope);
    node->block->extByteCode()->byteCodeBeforeFct = ByteCodeGenJob::emitLabelBeforeBlock;
    node->block->extByteCode()->byteCodeAfterFct  = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}