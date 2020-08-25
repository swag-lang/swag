#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not generate backend if 'if' is constant, and has already been evaluated
    if (module->mustOptimizeBC(node) > 0 && (node->boolExpression->flags & AST_VALUE_COMPUTED))
    {
        node->boolExpression->flags |= AST_NO_BYTECODE;
        if (node->boolExpression->computedValue.reg.b)
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

    node->byteCodeFct                      = ByteCodeGenJob::emitIf;
    node->boolExpression->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterExpr;
    node->ifBlock->byteCodeAfterFct        = ByteCodeGenJob::emitIfAfterIf;

    return true;
}

bool SemanticJob::resolveWhile(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not evaluate while if it's constant and false
    if (module->mustOptimizeBC(node) > 0 && (node->boolExpression->flags & AST_VALUE_COMPUTED))
    {
        if (!node->boolExpression->computedValue.reg.b)
        {
            node->boolExpression->flags |= AST_NO_BYTECODE;
            node->block->flags |= AST_NO_BYTECODE;
            return true;
        }
    }

    node->byteCodeFct                       = ByteCodeGenJob::emitLoop;
    node->boolExpression->byteCodeBeforeFct = ByteCodeGenJob::emitWhileBeforeExpr;
    node->boolExpression->byteCodeAfterFct  = ByteCodeGenJob::emitWhileAfterExpr;
    node->block->byteCodeAfterFct           = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveInlineBefore(SemanticContext* context)
{
    auto node = CastAst<AstInline>(context->node, AstNodeKind::Inline);

    context->expansionNode.push_back(node->parent);
    if (node->doneFlags & AST_DONE_RESOLVE_INLINED)
        return true;
    node->doneFlags |= AST_DONE_RESOLVE_INLINED;

    node->scope->startStackSize = node->ownerScope->startStackSize;

    // Register all function parameters as inline symbols
    auto func = node->func;
    if (func->parameters)
    {
        for (auto funcParam : func->parameters->childs)
        {
            node->scope->symTable.addSymbolTypeInfo(context, funcParam, funcParam->typeInfo, SymbolKind::Variable, nullptr, OVERLOAD_VAR_INLINE | OVERLOAD_CONST_ASSIGN);
        }
    }

    return true;
}

bool SemanticJob::resolveInlineAfter(SemanticContext* context)
{
    context->expansionNode.pop_back();
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

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression));
    node->byteCodeFct                       = ByteCodeGenJob::emitLoop;
    node->boolExpression->byteCodeBeforeFct = ByteCodeGenJob::emitForBeforeExpr;
    node->boolExpression->byteCodeAfterFct  = ByteCodeGenJob::emitForAfterExpr;
    node->postExpression->byteCodeAfterFct  = ByteCodeGenJob::emitForAfterPost;
    node->block->byteCodeAfterFct           = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveSwitch(SemanticContext* context)
{
    auto node = CastAst<AstSwitch>(context->node, AstNodeKind::Switch);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    node->typeInfo                     = node->expression->typeInfo;
    node->byteCodeFct                  = ByteCodeGenJob::emitSwitch;
    node->expression->byteCodeAfterFct = ByteCodeGenJob::emitSwitchAfterExpr;

    // Deal with complete
    SymbolAttributes attributes;
    SWAG_CHECK(collectAttributes(context, attributes, node->parentAttributes, node, AstNodeKind::Switch, node->attributeFlags));

    auto typeSwitch = TypeManager::concreteType(node->typeInfo);

    // Collect constant expressions, to avoid double definitions
    set<uint64_t> val64;
    set<Utf8>     valText;
    for (auto switchCase : node->cases)
    {
        for (auto expr : switchCase->expressions)
        {
            if (expr->flags & AST_VALUE_COMPUTED)
            {
                if (typeSwitch->isNative(NativeTypeKind::String))
                {
                    if (valText.find(expr->computedValue.text) != valText.end())
                        return context->report({expr, format("switch value '%s' already defined", expr->computedValue.text.c_str())});
                    valText.insert(expr->computedValue.text);
                }
                else
                {
                    if (val64.find(expr->computedValue.reg.u64) != val64.end())
                    {
                        if (typeSwitch->flags & TYPEINFO_INTEGER)
                            return context->report({expr, format("switch value '%d' already defined", expr->computedValue.reg.u64)});
                        return context->report({expr, format("switch value '%f' already defined", expr->computedValue.reg.f64)});
                    }

                    val64.insert(expr->computedValue.reg.u64);
                }
            }
            else if (node->attributeFlags & ATTRIBUTE_COMPLETE)
            {
                return context->report({expr, "expression cannot be evaluated at compile time, and switch is 'swag.complete'"});
            }
        }
    }

    // When a switch is marked as complete, be sure every definitions have been covered
    if (node->attributeFlags & ATTRIBUTE_COMPLETE)
    {
        if (node->typeInfo->kind != TypeInfoKind::Enum)
        {
            return context->report({node, format("'swag.complete' attribute on a switch can only be used for an enum type ('%s' provided)", node->typeInfo->name.c_str())});
        }

        auto typeEnum = CastTypeInfo<TypeInfoEnum>(node->typeInfo, TypeInfoKind::Enum);
        if (typeSwitch->isNative(NativeTypeKind::String))
        {
            if (valText.size() != typeEnum->values.size())
            {
                for (auto one : typeEnum->values)
                {
                    if (valText.find(one->value.text) == valText.end())
                        return context->report({node, node->token, format("switch is incomplete (missing '%s.%s')", typeEnum->name.c_str(), one->namedParam.c_str())});
                }
            }
        }
        else
        {
            if (val64.size() != typeEnum->values.size())
            {
                for (auto one : typeEnum->values)
                {
                    if (val64.find(one->value.reg.u64) == val64.end())
                        return context->report({node, node->token, format("switch is incomplete (missing '%s.%s')", typeEnum->name.c_str(), one->namedParam.c_str())});
                }
            }
        }
    }

    return true;
}

bool SemanticJob::resolveCase(SemanticContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (auto oneExpression : node->expressions)
    {
        SWAG_CHECK(checkIsConcrete(context, oneExpression));
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression, CASTFLAG_COMPARE));
    }

    node->typeInfo                 = node->ownerSwitch->expression->typeInfo;
    node->block->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->byteCodeAfterFct  = ByteCodeGenJob::emitSwitchCaseAfterBlock;
    return true;
}

bool SemanticJob::resolveLoop(SemanticContext* context)
{
    auto module = context->sourceFile->module;
    auto node   = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    auto expression = node->expression;
    SWAG_CHECK(resolveIntrinsicCountOf(context, expression, expression->typeInfo));

    // Do not evaluate loop if it's constant and 0
    if (module->mustOptimizeBC(node) > 0 && (node->expression->flags & AST_VALUE_COMPUTED))
    {
        if (!node->expression->computedValue.reg.u32)
        {
            node->expression->flags |= AST_NO_BYTECODE;
            node->block->flags |= AST_NO_BYTECODE;
            return true;
        }
    }

    node->typeInfo                     = g_TypeMgr.typeInfoU32;
    node->byteCodeFct                  = ByteCodeGenJob::emitLoop;
    node->expression->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterExpr;
    node->block->byteCodeAfterFct      = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}

bool SemanticJob::resolveVisit(SemanticContext* context)
{
    auto job        = context->job;
    auto sourceFile = context->sourceFile;
    auto node       = CastAst<AstVisit>(context->node, AstNodeKind::Visit);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    // Struct type : convert to a opVisit call
    auto     typeInfo      = node->expression->typeInfo;
    AstNode* newExpression = nullptr;
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        SWAG_VERIFY(node->expression->kind == AstNodeKind::IdentifierRef, internalError(context, "cannot resolve visit, expression is not an identifier"));
        auto identifierRef     = Ast::clone(node->expression, node);
        auto identifier        = Ast::newIdentifier(sourceFile, format("opVisit%s", node->extraNameToken.text.c_str()), (AstIdentifierRef*) identifierRef, identifierRef);
        identifier->aliasNames = node->aliasNames;
        identifier->token      = node->token;

        // Generic parameters
        identifier->genericParameters = Ast::newFuncCallParams(sourceFile, identifier);
        identifier->genericParameters->flags |= AST_NO_BYTECODE;
        auto child                 = Ast::newFuncCallParam(sourceFile, identifier->genericParameters);
        child->typeInfo            = g_TypeMgr.typeInfoBool;
        child->computedValue.reg.b = node->wantPointer;
        child->flags |= AST_VALUE_COMPUTED | AST_NO_SEMANTIC;

        // Call with arguments
        identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
        newExpression              = identifierRef;

        Ast::removeFromParent(node->block);
        Ast::addChildBack(node, node->block);
        node->expression->flags |= AST_NO_BYTECODE;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node->block);
        job->nodes.push_back(node);
        return true;
    }

    SWAG_VERIFY(node->extraNameToken.text.empty(), context->report({node, node->extraNameToken, format("'visit' extra name is only valid for a struct type, and type is '%s'", typeInfo->name.c_str())}));

    Utf8 alias0Name = node->aliasNames.empty() ? Utf8("@alias0") : node->aliasNames[0];
    Utf8 alias1Name = node->aliasNames.size() <= 1 ? Utf8("@alias1") : node->aliasNames[1];
    Utf8 content;

    // Get back the expression string
    auto& concat = context->job->tmpConcat;
    concat.init(1024);
    Ast::output(concat, node->expression);
    concat.addU8(0);
    SWAG_ASSERT(concat.firstBucket->nextBucket == nullptr);

    // Multi dimensional array
    if (typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) typeInfo)->pointedType->kind == TypeInfoKind::Array)
    {
        auto typeArray = (TypeInfoArray*) typeInfo;
        content += format("{ let __addr = cast(*%s) @dataof(%s); ", typeArray->finalType->name.c_str(), (const char*) concat.firstBucket->datas);
        content += format("const __count = @sizeof(%s) / %d; ", (const char*) concat.firstBucket->datas, typeArray->finalType->sizeOf);
        content += format("loop __count { ", (const char*) concat.firstBucket->datas);
        if (node->wantConstPointer)
            content += format("let %s: const @typeof(__addr) = __addr + @index; ", alias0Name.c_str());
        else if (node->wantPointer)
            content += format("let %s = __addr + @index; ", alias0Name.c_str());
        else
            content += format("let %s = __addr[@index]; ", alias0Name.c_str());
        content += format("let %s = @index; ", alias1Name.c_str());
        content += "}} ";
        SyntaxJob syntaxJob;
        syntaxJob.constructEmbedded(content, node, node, CompilerAstKind::EmbeddedInstruction);

        newExpression = node->childs.back();

        // First child is the let in the statement, and first child of this is the loop node
        auto loopNode = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
        Ast::removeFromParent(node->block);
        Ast::addChildBack(loopNode->block, node->block);
        Ast::visit(node->block, [&](AstNode* x) { if (!x->ownerBreakable) x->ownerBreakable = loopNode; });
        node->block->flags &= ~AST_NO_SEMANTIC;

        // Re-root the parent scope of the user block so that it points to the scope of the loop block
        node->block->ownerScope->parentScope = loopNode->block->childs.front()->ownerScope;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node);
    }

    // One dimensional array
    else if (typeInfo->isNative(NativeTypeKind::String) || typeInfo->kind == TypeInfoKind::Slice || typeInfo->kind == TypeInfoKind::Array)
    {
        content += format("{ let __addr = @dataof(%s); ", (const char*) concat.firstBucket->datas);
        content += format("loop %s { ", (const char*) concat.firstBucket->datas);
        if (node->wantConstPointer)
            content += format("let %s: const @typeof(__addr) = __addr + @index; ", alias0Name.c_str());
        else if (node->wantPointer)
            content += format("let %s = __addr + @index; ", alias0Name.c_str());
        else
            content += format("let %s = __addr[@index]; ", alias0Name.c_str());
        content += format("let %s = @index; ", alias1Name.c_str());
        content += "}} ";
        SyntaxJob syntaxJob;
        syntaxJob.constructEmbedded(content, node, node, CompilerAstKind::EmbeddedInstruction);

        newExpression = node->childs.back();

        // First child is the let in the statement, and first child of this is the loop node
        auto loopNode = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
        Ast::removeFromParent(node->block);
        Ast::addChildBack(loopNode->block, node->block);
        SWAG_ASSERT(node->block);
        Ast::visit(node->block, [&](AstNode* x) { if(!x->ownerBreakable) x->ownerBreakable = loopNode; });
        node->block->flags &= ~AST_NO_SEMANTIC;

        // Re-root the parent scope of the user block so that it points to the scope of the loop block
        node->block->ownerScope->parentScope = loopNode->block->childs.front()->ownerScope;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node);
    }
    else
    {
        return context->report({node->expression, format("invalid type '%s' for visit", typeInfo->name.c_str())});
    }

    return true;
}

bool SemanticJob::resolveIndex(SemanticContext* context)
{
    auto node = context->node;
    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'@index' can only be used inside a breakable loop"}));
    SWAG_VERIFY(node->ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_INDEX, context->report({node, node->token, "'@index' can only be used inside a breakable loop"}));

    node->ownerBreakable->breakableFlags |= BREAKABLE_NEED_INDEX;
    node->typeInfo    = g_TypeMgr.typeInfoU32;
    node->byteCodeFct = ByteCodeGenJob::emitIndex;
    return true;
}

bool SemanticJob::resolveBreak(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Break);

    // Label has been defined : search the corresponding LabelBreakable node
    if (!node->label.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::LabelBreakable || breakable->name != node->label))
            breakable = breakable->ownerBreakable;
        SWAG_VERIFY(breakable, context->report({node, format("unknown label '%s'", node->label.c_str())}));
        node->ownerBreakable = breakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'break' can only be used inside a breakable block"}));
    node->ownerBreakable->breakList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitBreak;
    return true;
}

bool SemanticJob::resolveFallThrough(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::FallThrough);
    SWAG_VERIFY(node->ownerBreakable && node->ownerBreakable->kind == AstNodeKind::Switch, context->report({node, node->token, "'fallthrough' can only be used inside a 'switch' block"}));
    node->ownerBreakable->fallThroughList.push_back(node);

    // Be sure we are in a case
    auto parent = node->parent;
    while (parent && parent->kind != AstNodeKind::SwitchCase && parent != node->ownerBreakable)
        parent = parent->parent;
    SWAG_VERIFY(parent && parent->kind == AstNodeKind::SwitchCase, context->report({node, node->token, "'fallthrough' can only be used inside a 'case' block"}));
    node->switchCase = CastAst<AstSwitchCase>(parent, AstNodeKind::SwitchCase);

    // 'fallthrough' cannot be used on the last case, this has no sens
    auto switchBlock = CastAst<AstSwitch>(node->ownerBreakable, AstNodeKind::Switch);
    SWAG_VERIFY(node->switchCase->caseIndex < switchBlock->cases.size() - 1, context->report({node, node->token, "'fallthrough' cannot be used in the last 'case' of the swtich"}));

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitFallThrough;

    return true;
}

bool SemanticJob::resolveContinue(SemanticContext* context)
{
    auto node = CastAst<AstBreakContinue>(context->node, AstNodeKind::Continue);

    // Label has been defined : search the corresponding LabelBreakable node
    AstBreakable* lastBreakable = nullptr;
    if (!node->label.empty())
    {
        auto breakable = node->ownerBreakable;
        while (breakable && (breakable->kind != AstNodeKind::LabelBreakable || breakable->name != node->label))
        {
            if (breakable->kind != AstNodeKind::LabelBreakable)
                lastBreakable = breakable;
            breakable = breakable->ownerBreakable;
        }

        SWAG_VERIFY(breakable, context->report({node, format("unknown label '%s'", node->label.c_str())}));
        node->ownerBreakable = lastBreakable;
    }

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'continue' can only be used inside a breakable loop"}));
    SWAG_VERIFY(node->ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_CONTINUE, context->report({node, node->token, "'continue' can only be used inside a breakable loop"}));
    node->ownerBreakable->continueList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitContinue;
    return true;
}

bool SemanticJob::resolveLabel(SemanticContext* context)
{
    // Be sure we don't have ghosting (the same label in a parent)
    auto node  = CastAst<AstLabelBreakable>(context->node, AstNodeKind::LabelBreakable);
    auto check = node->parent;
    while (check)
    {
        if (check->kind == AstNodeKind::LabelBreakable)
        {
            if (check->name == node->name)
            {
                Diagnostic diag(node, node->token, format("label name '%s' already defined in the hierarchy", node->name.c_str()));
                Diagnostic note(check, check->token, "this is the other definition", DiagnosticLevel::Note);
                context->report(diag, &note);
            }
        }

        check = check->parent;
    }

    node->block->byteCodeAfterFct = ByteCodeGenJob::emitLoopAfterBlock;
    return true;
}