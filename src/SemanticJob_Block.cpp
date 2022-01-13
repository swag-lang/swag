#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "AstOutput.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Array.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

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
    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterExpr;
    node->ifBlock->allocateExtension();
    node->ifBlock->extension->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterIf;

    return true;
}

bool SemanticJob::resolveWhile(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

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

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitWhileBeforeExpr;
    node->boolExpression->extension->byteCodeAfterFct  = ByteCodeGenJob::emitWhileAfterExpr;
    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveInlineBefore(SemanticContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);

    JobContext::ErrorContext expNode;
    expNode.node = node->parent;
    expNode.type = JobContext::ErrorContextType::Inline;
    context->errorContextStack.push_back(expNode);

    if (node->doneFlags & AST_DONE_RESOLVE_INLINED)
        return true;
    node->doneFlags |= AST_DONE_RESOLVE_INLINED;

    auto func = node->func;

    // For a return by copy, need to reserve room on the stack for the return result
    if (func->returnType && func->returnType->typeInfo->flags & TYPEINFO_RETURN_BY_COPY)
    {
        node->flags |= AST_TRANSIENT;
        node->allocateComputedValue();
        node->computedValue->storageOffset = node->ownerScope->startStackSize;
        node->ownerScope->startStackSize += func->returnType->typeInfo->sizeOf;
        if (node->ownerFct)
            node->ownerFct->stackSize = max(node->ownerFct->stackSize, node->ownerScope->startStackSize);
    }

    node->scope->startStackSize = node->ownerScope->startStackSize;

    // Register all function parameters as inline symbols
    if (func->parameters)
    {
        AstIdentifier* identifier = nullptr;
        if (node->parent->kind == AstNodeKind::Identifier)
            identifier = CastAst<AstIdentifier>(node->parent, AstNodeKind::Identifier, AstNodeKind::FuncCall);
        auto& symTable = node->parametersScope->symTable;
        for (int i = 0; i < func->parameters->childs.size(); i++)
        {
            auto funcParam = func->parameters->childs[i];

            // If the call parameter of the inlined function is constant, then we register it in a specific
            // constant scope, not in the caller (for mixins)/inline scope.
            // This is a separated scope because mixins do not have their own scope, and we must have a
            // different symbol registration for each constant value
            // :InlineUsingParam
            bool isConstant = false;
            if (identifier && identifier->callParameters)
            {
                for (int j = 0; j < identifier->callParameters->childs.size(); j++)
                {
                    auto callParam = CastAst<AstFuncCallParam>(identifier->callParameters->childs[j], AstNodeKind::FuncCallParam);
                    if (callParam->indexParam != i)
                        continue;
                    if (!(callParam->flags & AST_VALUE_COMPUTED))
                        continue;
                    SWAG_ASSERT(node->parametersScope);
                    symTable.addSymbolTypeInfo(context,
                                               callParam,
                                               funcParam->typeInfo,
                                               SymbolKind::Variable,
                                               callParam->computedValue,
                                               OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN | OVERLOAD_COMPUTED_VALUE,
                                               nullptr,
                                               callParam->computedValue->storageOffset,
                                               callParam->computedValue->storageSegment,
                                               &funcParam->token.text);
                    isConstant = true;
                    break;
                }
            }

            if (!isConstant)
            {
                node->parametersScope->symTable.addSymbolTypeInfo(context,
                                                                  funcParam,
                                                                  funcParam->typeInfo,
                                                                  SymbolKind::Variable,
                                                                  nullptr,
                                                                  OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN);
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
            if (!(node->semFlags & AST_SEM_SCOPE_HAS_RETURN))
            {
                if (node->semFlags & AST_SEM_FCT_HAS_RETURN)
                    return context->report(fct, Fmt(Err(Err0748), fct->getDisplayNameC()));
                return context->report(fct, Fmt(Err(Err0606), fct->getDisplayNameC()));
            }
        }
    }

    context->errorContextStack.pop_back();
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
    node->allocateExtension();
    node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    node->boolExpression->allocateExtension();
    node->boolExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->extension->byteCodeAfterFct  = ByteCodeGenJob::emitForAfterExpr;

    node->postExpression->allocateExtension();
    node->postExpression->extension->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforePost;

    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveSwitchAfterExpr(SemanticContext* context)
{
    auto node       = context->node;
    auto switchNode = CastAst<AstSwitch>(node->parent, AstNodeKind::Switch);

    // For a switch on an enum, force a 'using' for each case
    if (node->typeInfo->kind == TypeInfoKind::Enum)
    {
        // :AutoScope
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        for (auto switchCase : switchNode->cases)
        {
            switchCase->addAlternativeScope(typeEnum->scope);
        }
    }

    // Automatic convert to 'kindOf'
    // This has no sens to do a switch on an 'any'. So instead of raising an error,
    // we implies the usage of '@kindof'. That way we have a switch on the underlying type.
    if (node->typeInfo->isNative(NativeTypeKind::Any))
    {
        switchNode->beforeAutoCastType = node->typeInfo;
        node->byteCodeFct              = ByteCodeGenJob::emitImplicitKindOf;
        auto& typeTable                = node->sourceFile->module->typeTable;
        SWAG_CHECK(checkIsConcrete(context, node));
        node->allocateComputedValue();
        node->computedValue->storageSegment = getConstantSegFromContext(context->node);
        SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, node->typeInfo, node->computedValue->storageSegment, &node->computedValue->storageOffset, MAKE_CONCRETE_SHOULD_WAIT, &node->typeInfo));
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
    SWAG_VERIFY(!(node->attributeFlags & ATTRIBUTE_COMPLETE) || node->expression, context->report(node, Err(Err0607)));

    node->byteCodeFct = ByteCodeGenJob::emitSwitch;
    if (!node->expression)
    {
        node->allocateExtension();
        node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitBeforeSwitch;
        return true;
    }

    node->expression->allocateExtension();
    node->expression->extension->byteCodeAfterFct = ByteCodeGenJob::emitSwitchAfterExpr;

    SWAG_CHECK(checkIsConcrete(context, node->expression));
    node->typeInfo = node->expression->typeInfo;

    auto typeSwitch = TypeManager::concreteType(node->typeInfo);

    // Verify switch expression type is valid
    SWAG_VERIFY(!typeSwitch->isNative(NativeTypeKind::Any), context->report(node->expression, Err(Err0608)));
    switch (typeSwitch->kind)
    {
    case TypeInfoKind::Slice:
    case TypeInfoKind::Array:
    case TypeInfoKind::Interface:
        return context->report(node->expression, Fmt(Err(Err0609), typeSwitch->getDisplayNameC()));
    }

    SWAG_VERIFY(!node->cases.empty(), context->report(node, Err(Err0610)));

    // Collect constant expressions, to avoid double definitions
    Array<AstNode*> valDef;
    Array<uint64_t> val64;
    Array<Utf8>     valText;
    for (auto switchCase : node->cases)
    {
        for (auto expr : switchCase->expressions)
        {
            if (expr->flags & AST_VALUE_COMPUTED)
            {
                auto typeExpr = TypeManager::concreteType(expr->typeInfo);
                if (typeExpr->isNative(NativeTypeKind::String))
                {
                    int idx = valText.find(expr->computedValue->text);
                    if (idx != -1)
                    {
                        Diagnostic note{valDef[idx], Nte(Nte0014), DiagnosticLevel::Note};
                        return context->report({expr, Fmt(Err(Err0611), expr->computedValue->text.c_str())}, &note);
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
                        Diagnostic note{valDef[idx], Nte(Nte0014), DiagnosticLevel::Note};
                        if (expr->flags & AST_VALUE_IS_TYPEINFO)
                            return context->report({expr, Fmt(Err(Err0611), expr->token.ctext())}, &note);
                        if (expr->typeInfo->kind == TypeInfoKind::Enum)
                            return context->report({expr, Fmt(Err(Err0612), expr->token.ctext())}, &note);
                        if (typeExpr->isNativeInteger())
                            return context->report({expr, Fmt(Err(Err0613), expr->computedValue->reg.u64)}, &note);
                        return context->report({expr, Fmt(Err(Err0614), expr->computedValue->reg.f64)}, &note);
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
        SWAG_VERIFY(!back->expressions.empty(), context->report(back, Err(Err0616)));

        if (node->typeInfo->kind != TypeInfoKind::Enum && !node->beforeAutoCastType)
            return context->report(node, Fmt(Err(Err0617), node->typeInfo->getDisplayNameC()));
        if (node->beforeAutoCastType)
            return context->report(node, Fmt(Err(Err0617), node->beforeAutoCastType->getDisplayNameC()));

        if (node->typeInfo->kind == TypeInfoKind::Enum)
        {
            auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
            if (typeSwitch->isNative(NativeTypeKind::String))
            {
                if (valText.size() != typeEnum->values.size())
                {
                    for (auto one : typeEnum->values)
                    {
                        if (!valText.contains(one->value->text))
                        {
                            Diagnostic diag{node, Fmt(Err(Err0620), typeEnum->name.c_str(), one->namedParam.c_str())};
                            Diagnostic note{one->declNode, Nte(Nte0034), DiagnosticLevel::Note};
                            return context->report(diag, &note);
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
                            Diagnostic diag{node, Fmt(Err(Err0620), typeEnum->name.c_str(), one->namedParam.c_str())};
                            Diagnostic note{one->declNode, Nte(Nte0034), DiagnosticLevel::Note};
                            return context->report(diag, &note);
                        }
                    }
                }
            }
        }
        else
        {
            SWAG_ASSERT(false);
        }
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
                if (typeInfo->kind == TypeInfoKind::Struct)
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
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionLow, CASTFLAG_COMPARE));
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, rangeNode->expressionUp, CASTFLAG_COMPARE));
                }
            }
            else
            {
                return context->report(rangeNode, Err(Err0337));
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
                if (typeInfo->kind == TypeInfoKind::Struct)
                {
                    SWAG_CHECK(resolveUserOpCommutative(context, g_LangSpec->name_opEquals, nullptr, nullptr, node->ownerSwitch->expression, oneExpression));
                    if (context->result != ContextResult::Done)
                        return true;
                }
                else
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression, CASTFLAG_COMPARE));
                }
            }

            // switch without an expression : a case is a boolean expressions
            else
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, oneExpression->typeInfo, nullptr, oneExpression, CASTFLAG_COMPARE));
            }
        }
    }

    if (node->ownerSwitch->expression)
        node->typeInfo = node->ownerSwitch->expression->typeInfo;

    if (!node->expressions.empty())
    {
        node->allocateExtension();
        node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeCase;
    }

    node->block->allocateExtension();
    node->block->extension->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->extension->byteCodeAfterFct  = ByteCodeGenJob::emitSwitchCaseAfterBlock;
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
            if (node->expression->typeInfo->kind != TypeInfoKind::Enum)
                SWAG_CHECK(checkIsConcrete(context, node->expression));

            SWAG_CHECK(resolveIntrinsicCountOf(context, node->expression, node->expression));
            if (context->result != ContextResult::Done)
                return true;
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoUInt, node->expression->typeInfo, nullptr, node->expression, CASTFLAG_TRY_COERCE));
            node->typeInfo = node->expression->typeInfo;

            // Do not evaluate loop if it's constant and 0
            if (module->mustOptimizeBC(node) && (node->expression->flags & AST_VALUE_COMPUTED))
            {
                if (!node->expression->computedValue->reg.u64)
                {
                    node->expression->flags |= AST_NO_BYTECODE;
                    node->block->flags |= AST_NO_BYTECODE;
                    return true;
                }
            }
        }

        // Range
        else
        {
            auto rangeNode = CastAst<AstRange>(node->expression, AstNodeKind::Range);
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoInt, rangeNode->expressionLow->typeInfo, nullptr, rangeNode->expressionLow, CASTFLAG_TRY_COERCE));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoInt, rangeNode->expressionUp->typeInfo, nullptr, rangeNode->expressionUp, CASTFLAG_TRY_COERCE));
        }

        node->expression->allocateExtension();
        node->expression->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterExpr;
    }

    node->byteCodeFct = ByteCodeGenJob::emitLoop;
    node->allocateExtension();
    SWAG_ASSERT(!node->extension->byteCodeAfterFct || node->extension->byteCodeAfterFct == ByteCodeGenJob::emitLeaveScope);
    node->extension->byteCodeAfterFct = ByteCodeGenJob::emitLeaveScope;

    node->block->allocateExtension();
    node->block->extension->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    if (!node->expression)
        node->block->extension->byteCodeBeforeFct = ByteCodeGenJob::emitLoopBeforeBlock;

    return true;
}

bool SemanticJob::resolveVisit(SemanticContext* context)
{
    auto job        = context->job;
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstVisit>(context->node, AstNodeKind::Visit);

    auto typeInfo = TypeManager::concreteReference(node->expression->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Enum)
        SWAG_CHECK(checkIsConcrete(context, node->expression));

    // Struct type : convert to a opVisit call
    AstNode* newExpression = nullptr;
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_VERIFY(!(typeInfo->flags & TYPEINFO_STRUCT_IS_TUPLE), context->report(Hint::isType(typeInfo), {node->expression, Err(Err0624)}));
        SWAG_VERIFY(node->expression->kind == AstNodeKind::IdentifierRef, sourceFile->internalError(node->expression, "resolveVisit expression, should be an identifier"));

        auto identifierRef    = (AstIdentifierRef*) Ast::clone(node->expression, node);
        auto callVisit        = Ast::newIdentifier(sourceFile, Fmt("opVisit%s", node->extraNameToken.ctext()), identifierRef, identifierRef);
        callVisit->aliasNames = node->aliasNames;
        callVisit->inheritTokenLocation(node);

        // Generic parameters
        callVisit->genericParameters = Ast::newFuncCallParams(sourceFile, callVisit);
        callVisit->genericParameters->flags |= AST_NO_BYTECODE;
        auto child      = Ast::newFuncCallParam(sourceFile, callVisit->genericParameters);
        child->typeInfo = g_TypeMgr->typeInfoBool;
        child->setFlagsValueIsComputed();
        child->computedValue->reg.b = node->specFlags & AST_SPEC_VISIT_WANTPOINTER;
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

    SWAG_VERIFY(node->extraNameToken.text.empty(), context->report({node, node->extraNameToken, Fmt(Err(Err0625), typeInfo->getDisplayNameC())}));
    SWAG_VERIFY(node->aliasNames.size() <= 2, context->report({node, node->aliasNames[2], Fmt(Err(Err0626), node->aliasNames.size())}));

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
    node->expression->flags |= AST_NO_BYTECODE | AST_NO_BYTECODE_CHILDS;

    int firstAliasVar = 0;
    int id            = g_UniqueID.fetch_add(1);

    // Multi dimensional array
    if (typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) typeInfo)->pointedType->kind == TypeInfoKind::Array)
    {
        auto typeArray   = (TypeInfoArray*) typeInfo;
        auto pointedType = typeArray->finalType;

        firstAliasVar = 1;
        content += Fmt("{ var __addr%u = cast(*%s) @dataof(%s); ", id, typeArray->finalType->name.c_str(), (const char*) concat.firstBucket->datas);
        content += Fmt("const __count%u = @sizeof(%s) / %u; ", id, (const char*) concat.firstBucket->datas, typeArray->finalType->sizeOf);
        content += Fmt("loop __count%u { ", id);
        if (node->specFlags & AST_SPEC_VISIT_WANTPOINTER)
            content += Fmt("var %s = __addr%u + @index; ", alias0Name.c_str(), id);
        else if (pointedType->kind == TypeInfoKind::Struct)
        {
            pointedType->computeScopedName();
            content += Fmt("var %s = cast(const *%s) __addr%u[@index]; ", alias0Name.c_str(), pointedType->scopedName.c_str(), id);
        }
        else
            content += Fmt("var %s = __addr%u[@index]; ", alias0Name.c_str(), id);
        content += Fmt("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // One dimensional array
    else if (typeInfo->kind == TypeInfoKind::Slice || typeInfo->kind == TypeInfoKind::Array)
    {
        TypeInfo* pointedType;
        if (typeInfo->kind == TypeInfoKind::Slice)
            pointedType = ((TypeInfoSlice*) typeInfo)->pointedType;
        else
            pointedType = ((TypeInfoArray*) typeInfo)->pointedType;

        firstAliasVar = 1;
        content += Fmt("{ var __addr%u = @dataof(%s); ", id, (const char*) concat.firstBucket->datas);
        content += Fmt("loop %s { ", (const char*) concat.firstBucket->datas);
        if (node->specFlags & AST_SPEC_VISIT_WANTPOINTER)
            content += Fmt("var %s = __addr%u + @index; ", alias0Name.c_str(), id);
        else if (pointedType->kind == TypeInfoKind::Struct)
        {
            pointedType->computeScopedName();
            content += Fmt("var %s = cast(const *%s) __addr%u[@index]; ", alias0Name.c_str(), pointedType->scopedName.c_str(), id);
        }
        else
            content += Fmt("var %s = __addr%u[@index]; ", alias0Name.c_str(), id);
        content += Fmt("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // String
    else if (typeInfo->isNative(NativeTypeKind::String))
    {
        content += Fmt("{ var __addr%u = @dataof(%s); ", id, (const char*) concat.firstBucket->datas);
        content += Fmt("loop %s { ", (const char*) concat.firstBucket->datas);
        firstAliasVar = 1;
        if (node->specFlags & AST_SPEC_VISIT_WANTPOINTER)
            content += Fmt("var %s = __addr%u + @index; ", alias0Name.c_str(), id);
        else
            content += Fmt("var %s = __addr%u[@index]; ", alias0Name.c_str(), id);
        content += Fmt("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // Variadic
    else if (typeInfo->kind == TypeInfoKind::Variadic || typeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        SWAG_VERIFY(!(node->specFlags & AST_SPEC_VISIT_WANTPOINTER), context->report(node, Err(Err0627)));
        content += Fmt("{ loop %s { ", (const char*) concat.firstBucket->datas);
        firstAliasVar = 0;
        content += Fmt("var %s = %s[@index]; ", alias0Name.c_str(), (const char*) concat.firstBucket->datas);
        content += Fmt("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    // Enum
    else if (typeInfo->kind == TypeInfoKind::Enum)
    {
        auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        SWAG_VERIFY(!(node->specFlags & AST_SPEC_VISIT_WANTPOINTER), context->report(node, Err(Err0636)));
        content += Fmt("{ var __addr%u = @typeof(%s); ", id, (const char*) concat.firstBucket->datas);
        content += Fmt("loop %d { ", typeEnum->values.size());
        firstAliasVar = 1;
        content += Fmt("var %s = dref cast(const *%s) __addr%u.values[@index].value; ", alias0Name.c_str(), typeInfo->name.c_str(), id);
        content += Fmt("var %s = @index; ", alias1Name.c_str());
        content += "}} ";
    }

    else
    {
        // Special error in case of a pointer
        if (typeInfo->kind == TypeInfoKind::Pointer)
        {
            auto typePtr = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            if (typePtr->pointedType->kind == TypeInfoKind::Enum ||
                typePtr->pointedType->kind == TypeInfoKind::Variadic ||
                typePtr->pointedType->kind == TypeInfoKind::TypedVariadic ||
                typePtr->pointedType->kind == TypeInfoKind::Slice ||
                typePtr->pointedType->kind == TypeInfoKind::Array ||
                typePtr->pointedType->kind == TypeInfoKind::Struct ||
                typePtr->pointedType->isNative(NativeTypeKind::String))
            {
                PushErrHint errh(Hnt(Hnt0037));
                return context->report(node->expression, Fmt(Err(Err0628), typeInfo->getDisplayNameC()));
            }
            else
            {
                PushErrHint errh(Hnt(Hnt0036));
                return context->report(node->expression, Fmt(Err(Err0628), typeInfo->getDisplayNameC()));
            }
        }

        PushErrHint errh(Hnt(Hnt0006));
        return context->report(node->expression, Fmt(Err(Err0629), typeInfo->getDisplayNameC()));
    }

    SyntaxJob syntaxJob;
    syntaxJob.constructEmbedded(content, node, node, CompilerAstKind::EmbeddedInstruction, false);
    newExpression = node->childs.back();

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
                   }
               });

    // First child is the let in the statement, and first child of this is the loop node
    auto loopNode = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
    Ast::removeFromParent(node->block);
    Ast::addChildBack(loopNode->block, node->block);
    SWAG_ASSERT(node->block);
    Ast::visit(node->block, [&](AstNode* x)
               {
                   if (!x->ownerBreakable)
                       x->ownerBreakable = loopNode;
               });
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
    SWAG_VERIFY(ownerBreakable, context->report(node, Err(Err0630)));

    ownerBreakable->breakableFlags |= BREAKABLE_NEED_INDEX;

    // Take the type from the expression
    if (ownerBreakable->kind == AstNodeKind::Loop)
    {
        auto loopNode = CastAst<AstLoop>(ownerBreakable, AstNodeKind::Loop);
        if (loopNode->expression && loopNode->expression->typeInfo->isNativeInteger())
            node->typeInfo = loopNode->expression->typeInfo;
    }

    if (!node->typeInfo)
        node->typeInfo = g_TypeMgr->typeInfoUInt;
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
    if (!node->label.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::ScopeBreakable || breakable->token.text != node->label))
            breakable = breakable->ownerBreakable;
        SWAG_VERIFY(breakable, context->report(node, Fmt(Err(Err0631), node->label.c_str())));
        node->ownerBreakable = breakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report(node, Err(Err0632)));
    node->ownerBreakable->breakList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitBreak;
    return true;
}

bool SemanticJob::resolveFallThrough(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::FallThrough);
    SWAG_VERIFY(node->ownerBreakable && node->ownerBreakable->kind == AstNodeKind::Switch, context->report(node, Err(Err0633)));
    node->ownerBreakable->fallThroughList.push_back(node);

    // Be sure we are in a case
    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::SwitchCase && parent != node->ownerBreakable)
        parent = parent->parent;
    SWAG_VERIFY(parent && parent->kind == AstNodeKind::SwitchCase, context->report(node, Err(Err0634)));
    node->switchCase = CastAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);

    // 'fallthrough' cannot be used on the last case, this has no sens
    auto switchBlock = CastAst<AstSwitch>(node->ownerBreakable, AstNodeKind::Switch);
    SWAG_VERIFY(node->switchCase->caseIndex < switchBlock->cases.size() - 1, context->report(node, Err(Err0635)));

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitFallThrough;

    return true;
}

bool SemanticJob::resolveContinue(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Continue);

    // Label has been defined : search the corresponding ScopeBreakable node
    AstBreakable* lastBreakable = nullptr;
    if (!node->label.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::ScopeBreakable || breakable->token.text != node->label))
        {
            if (breakable->kind != AstNodeKind::ScopeBreakable)
                lastBreakable = breakable;
            breakable = breakable->ownerBreakable;
        }

        SWAG_VERIFY(breakable, context->report(node, Fmt(Err(Err0631), node->label.c_str())));
        node->ownerBreakable = lastBreakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report(node, Err(Err0637)));

    auto checkBreakable = node->ownerBreakable;
    while (checkBreakable && !(checkBreakable->breakableFlags & BREAKABLE_CAN_HAVE_CONTINUE))
        checkBreakable = checkBreakable->ownerBreakable;
    SWAG_VERIFY(checkBreakable, context->report(node, Err(Err0637)));
    checkBreakable->continueList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitContinue;
    return true;
}

bool SemanticJob::resolveScopeBreakable(SemanticContext* context)
{
    auto node = CastAst<AstScopeBreakable>(context->node, AstNodeKind::ScopeBreakable);
    node->block->allocateExtension();
    node->block->extension->byteCodeBeforeFct = ByteCodeGenJob::emitLabelBeforeBlock;
    node->block->extension->byteCodeAfterFct  = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}