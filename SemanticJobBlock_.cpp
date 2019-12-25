#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::resolveIf(SemanticContext* context)
{
    auto node = CastAst<AstIf>(context->node, AstNodeKind::If);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));

    // Do not generate backend if 'if' is constant, and has already been evaluated
    if (node->boolExpression->flags & AST_VALUE_COMPUTED)
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
    }
    else
    {
        node->byteCodeFct                      = ByteCodeGenJob::emitIf;
        node->boolExpression->byteCodeAfterFct = ByteCodeGenJob::emitIfAfterExpr;
        node->ifBlock->byteCodeAfterFct        = ByteCodeGenJob::emitIfAfterIf;
    }

    return true;
}

bool SemanticJob::resolveWhile(SemanticContext* context)
{
    auto node = CastAst<AstWhile>(context->node, AstNodeKind::While);
    SWAG_CHECK(checkIsConcrete(context, node->boolExpression));

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr.typeInfoBool, nullptr, node->boolExpression, CASTFLAG_AUTO_BOOL));
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
    return true;
}

bool SemanticJob::resolveCase(SemanticContext* context)
{
    auto node = CastAst<AstSwitchCase>(context->node, AstNodeKind::SwitchCase);
    for (auto oneExpression : node->expressions)
    {
        SWAG_CHECK(checkIsConcrete(context, oneExpression));
        SWAG_CHECK(TypeManager::makeCompatibles(context, node->ownerSwitch->expression, oneExpression));
    }

    node->typeInfo                 = node->ownerSwitch->expression->typeInfo;
    node->block->byteCodeBeforeFct = ByteCodeGenJob::emitSwitchCaseBeforeBlock;
    node->block->byteCodeAfterFct  = ByteCodeGenJob::emitSwitchCaseAfterBlock;
    return true;
}

bool SemanticJob::resolveLoop(SemanticContext* context)
{
    auto node = CastAst<AstLoop>(context->node, AstNodeKind::Loop);
    SWAG_CHECK(checkIsConcrete(context, node->expression));

    auto expression = node->expression;

    if (!resolveCountProperty(context, expression, expression->typeInfo))
    {
        auto typeInfo = expression->typeInfo;
        SWAG_VERIFY(typeInfo->flags & TYPEINFO_INTEGER, context->report({expression, format("expression should be of type integer, but is '%s'", typeInfo->name.c_str())}));
        SWAG_VERIFY(typeInfo->sizeOf <= 4, context->report({expression, format("expression should be a 32 bit integer, but is '%s'", typeInfo->name.c_str())}));
        if (expression->flags & AST_VALUE_COMPUTED)
        {
            if (!(typeInfo->flags & TYPEINFO_UNSIGNED))
            {
                switch (typeInfo->nativeType)
                {
                case NativeTypeKind::S8:
                    if (expression->computedValue.reg.s8 < 0)
                        return context->report({expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s8)});
                    break;
                case NativeTypeKind::S16:
                    if (expression->computedValue.reg.s16 < 0)
                        return context->report({expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s16)});
                    break;
                case NativeTypeKind::S32:
                    if (expression->computedValue.reg.s32 < 0)
                        return context->report({expression, format("constant value should be unsigned, but is '%d'", expression->computedValue.reg.s32)});
                    break;
                }
            }
        }
        else
        {
            SWAG_VERIFY(typeInfo->flags & TYPEINFO_UNSIGNED, context->report({expression, format("expression should be of type unsigned integer, but is '%s'", typeInfo->name.c_str())}));
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
        auto identifierRef         = Ast::clone(node->expression, node);
        auto identifier            = Ast::newIdentifier(sourceFile, format("opVisit%s", node->extraNameToken.text.c_str()), (AstIdentifierRef*) identifierRef, identifierRef);
        identifier->aliasNames     = node->aliasNames;
        identifier->token          = node->token;
        identifier->callParameters = Ast::newFuncCallParams(sourceFile, identifier);
        newExpression              = identifierRef;

        SWAG_ASSERT(!node->block->parent);
        Ast::addChildBack(node, node->block);
        node->expression->flags |= AST_NO_BYTECODE;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node->block);
        job->nodes.push_back(node);
        return true;
    }

    SWAG_VERIFY(node->extraNameToken.text.empty(), context->report({node, node->extraNameToken, format("'visit' extra name is only valid for a struct type, and type is '%s'", typeInfo->name.c_str())}));

    Utf8 alias0Name = node->aliasNames.empty() ? "@alias0" : node->aliasNames[0];
    Utf8 alias1Name = node->aliasNames.size() <= 1 ? "@alias1" : node->aliasNames[1];
    Utf8 content;

    // Get back the expression string
    auto& concat = context->job->tmpConcat;
    concat.clear();
    Ast::output(concat, node->expression);
    concat.addU8(0);
    SWAG_ASSERT(concat.firstBucket->nextBucket == nullptr);

    if (typeInfo->flags & TYPEINFO_INTEGER)
    {
        SWAG_VERIFY(!node->wantPointer, context->report({node, node->wantPointerToken, "cannot visit by pointer for integers"}));
        content = format("loop %s { ", (const char*) concat.firstBucket->datas, alias0Name.c_str());
        content += format("let %s = @index; ", alias0Name.c_str());
        content += "} ";
        SyntaxJob syntaxJob;
        syntaxJob.constructEmbedded(content, node);

        newExpression = node->childs.back();
        auto loopNode = CastAst<AstLoop>(newExpression, AstNodeKind::Loop);
        Ast::addChildBack(loopNode->block, node->block);
        node->block->flags &= ~AST_NO_SEMANTIC;

        // Re-root the parent scope of the user block so that it points to the scope of the loop block
        node->block->ownerScope->parentScope = loopNode->block->childs.front()->ownerScope;

        job->nodes.pop_back();
        job->nodes.push_back(newExpression);
        job->nodes.push_back(node);
    }

    // Multi dimensional array : special case, as we want to visit everything
    else if (typeInfo->kind == TypeInfoKind::Array && ((TypeInfoArray*) typeInfo)->pointedType->kind == TypeInfoKind::Array)
    {
        auto typeArray = (TypeInfoArray*) typeInfo;
        content += format("{ let __addr = cast(*%s) @dataof(%s); ", typeArray->finalType->name.c_str(), (const char*) concat.firstBucket->datas);
        content += format("const __count = @sizeof(%s) / %d; ", (const char*) concat.firstBucket->datas, typeArray->finalType->sizeOf);
        content += format("loop __count { ", (const char*) concat.firstBucket->datas);
        if (node->wantPointer)
            content += format("let %s = __addr + @index; ", alias0Name.c_str());
        else
            content += format("let %s = __addr[@index]; ", alias0Name.c_str());
        content += format("let %s = @index; ", alias1Name.c_str());
        content += "}} ";
        SyntaxJob syntaxJob;
        syntaxJob.constructEmbedded(content, node);

        newExpression = node->childs.back();

        // First child is the let in the statement, and first child of this is the loop node
        auto loopNode = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
        Ast::addChildBack(loopNode->block, node->block);
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
        if (node->wantPointer)
            content += format("let %s = __addr + @index; ", alias0Name.c_str());
        else
            content += format("let %s = __addr[@index]; ", alias0Name.c_str());
        content += format("let %s = @index; ", alias1Name.c_str());
        content += "}} ";
        SyntaxJob syntaxJob;
        syntaxJob.constructEmbedded(content, node);

        newExpression = node->childs.back();

        // First child is the let in the statement, and first child of this is the loop node
        auto loopNode = CastAst<AstLoop>(node->childs.back()->childs.back(), AstNodeKind::Loop);
        Ast::addChildBack(loopNode->block, node->block);
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
    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'index' can only be used inside a breakable loop"}));
    SWAG_VERIFY(node->ownerBreakable->breakableFlags & BREAKABLE_CAN_HAVE_INDEX, context->report({node, node->token, "'index' can only be used inside a breakable loop"}));

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

    SWAG_VERIFY(node->ownerBreakable, context->report({node, node->token, "'break' can only be used inside a breakable scope"}));
    node->ownerBreakable->breakList.push_back(node);

    SWAG_CHECK(checkUnreachableCode(context));
    node->byteCodeFct = ByteCodeGenJob::emitBreak;
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