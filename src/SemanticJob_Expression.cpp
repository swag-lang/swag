#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "TypeManager.h"
#include "LanguageSpec.h"
#include "ErrorIds.h"

bool SemanticJob::checkIsConstExpr(JobContext* context, bool test, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    if (test)
        return true;

    auto note = computeNonConstExprNote(expression);

    if (expression->hasSpecialFuncCall())
    {
        Diagnostic diag{expression, expression->token, Fmt(Err(Err0281), expression->typeInfo->getDisplayNameC())};
        diag.hint = Fmt(Nte(Nte1047), expression->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str());
        return context->report(diag, note);
    }

    Utf8 message;
    if (errMsg.length() && errParam.length())
        message = Fmt(errMsg.c_str(), errParam.c_str());
    else if (errMsg.length())
        message = errMsg;
    else
        message = Err(Err0798);

    Diagnostic diag{expression, message};

    // Just keep the culprit if the culprit is the same as the full expression, and there's no
    // specific requested error message
    if (errMsg.empty() && note->startLocation == diag.startLocation && note->endLocation == diag.endLocation)
    {
        Vector<Utf8> parts;
        Utf8::tokenize(diag.textMsg, '$', parts, true, true);
        diag.textMsg = parts[0];
        diag.textMsg += "$";
        diag.textMsg += note->textMsg;
        note = nullptr;
    }

    return context->report(diag, note);
}

bool SemanticJob::checkIsConstExpr(JobContext* context, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    return checkIsConstExpr(context, expression->flags & AST_CONST_EXPR, expression, errMsg, errParam);
}

bool SemanticJob::resolveExplicitNoInit(SemanticContext* context)
{
    auto node = context->node;
    node->parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
    node->flags |= AST_CONST_EXPR;
    node->typeInfo = g_TypeMgr->typeInfoVoid;
    return true;
}

bool SemanticJob::computeExpressionListTupleType(SemanticContext* context, AstNode* node)
{
    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        if (context->result != ContextResult::Done)
            return true;
    }

    auto typeInfo      = makeType<TypeInfoList>(TypeInfoKind::TypeListTuple);
    typeInfo->name     = "{";
    typeInfo->sizeOf   = 0;
    typeInfo->declNode = node;

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        if (!typeInfo->subTypes.empty())
            typeInfo->name += ", ";

        auto typeParam = g_TypeMgr->makeParam();

        // When generating parameters for a closure call, keep the reference if we want one !
        if (child->kind != AstNodeKind::MakePointer || !(child->specFlags & AstMakePointer::SPECFLAG_TOREF))
            typeParam->typeInfo = TypeManager::concretePtrRef(child->typeInfo);
        else
            typeParam->typeInfo = child->typeInfo;

        typeInfo->subTypes.push_back(typeParam);

        // Value has been named
        if (child->hasExtMisc() && child->extMisc()->isNamed)
        {
            typeInfo->name += child->extMisc()->isNamed->token.text;
            typeInfo->name += ": ";
            typeParam->name = child->extMisc()->isNamed->token.text;
        }

        typeInfo->name += typeParam->typeInfo->name;
        typeInfo->sizeOf += typeParam->typeInfo->sizeOf;

        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    typeInfo->name += "}";
    node->typeInfo = typeInfo;
    return true;
}

bool SemanticJob::resolveExpressionListTuple(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    SWAG_CHECK(computeExpressionListTupleType(context, node));
    if (context->result != ContextResult::Done)
        return true;

    node->setBcNotifBefore(ByteCodeGenJob::emitExpressionListBefore);
    node->byteCodeFct = ByteCodeGenJob::emitExpressionList;

    // If the literal tuple is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the tuple will come from the constant segment.
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct && node->typeInfo)
    {
        allocateOnStack(node, node->typeInfo);
    }

    return true;
}

bool SemanticJob::resolveExpressionListArray(SemanticContext* context)
{
    auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    for (auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        if (context->result == ContextResult::Pending)
            return true;
    }

    auto typeInfo = makeType<TypeInfoList>(TypeInfoKind::TypeListArray);
    SWAG_ASSERT(node->childs.size());
    typeInfo->declNode = node;

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (auto child : node->childs)
    {
        auto typeParam      = g_TypeMgr->makeParam();
        typeParam->typeInfo = child->typeInfo;
        typeInfo->subTypes.push_back(typeParam);
        typeInfo->sizeOf += child->typeInfo->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    if (node->flags & AST_CONST_EXPR)
        typeInfo->setConst();

    typeInfo->forceComputeName();
    node->setBcNotifBefore(ByteCodeGenJob::emitExpressionListBefore);
    node->byteCodeFct = ByteCodeGenJob::emitExpressionList;
    node->typeInfo    = typeInfo;

    // If the literal array is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the array will come from the constant segment.
    // :ExprListArrayStorage
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        allocateOnStack(node, node->typeInfo);
    }

    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node)
{
    if ((node->flags & AST_CONST_EXPR) &&
        !node->typeInfo->isListArray() &&
        !node->typeInfo->isListTuple() &&
        !node->typeInfo->isSlice())
    {
        SWAG_CHECK(checkIsConcrete(context, node));
        SWAG_CHECK(executeCompilerNode(context, node, true));
        if (context->result != ContextResult::Done)
            return true;
    }

    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2)
{
    SWAG_CHECK(evaluateConstExpression(context, node1));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(evaluateConstExpression(context, node2));
    if (context->result == ContextResult::Pending)
        return true;
    return true;
}

bool SemanticJob::evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2, AstNode* node3)
{
    SWAG_CHECK(evaluateConstExpression(context, node1));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(evaluateConstExpression(context, node2));
    if (context->result == ContextResult::Pending)
        return true;
    SWAG_CHECK(evaluateConstExpression(context, node3));
    if (context->result == ContextResult::Pending)
        return true;
    return true;
}

bool SemanticJob::resolveConditionalOp(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 3);

    auto expression = node->childs[0];
    auto ifTrue     = node->childs[1];
    auto ifFalse    = node->childs[2];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcreteOrType(context, ifTrue));
    SWAG_CHECK(checkIsConcreteOrType(context, ifFalse));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifTrue, ifFalse));
    if (context->result == ContextResult::Pending)
        return true;

    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expression, CASTFLAG_AUTO_BOOL));

    auto rightT = ifFalse;
    auto leftT  = ifTrue;

    // We cast the true expression to the false expression.
    // But some times, it's better to do the other way
    if (leftT->typeInfo->isConst() ||
        leftT->typeInfo->isUntypedInteger())
        swap(leftT, rightT);

    // Make the cast
    {
        PushErrCxtStep ec(context, rightT, ErrCxtStepKind::Note, []()
                          { return Nte(Nte0055); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, rightT, leftT, CASTFLAG_COMMUTATIVE | CASTFLAG_STRICT));
    }

    // Determin if we should take the type from the "false" expression, or from the "true"
    if (ifTrue->typeInfo->isPointerNull())
        node->typeInfo = ifFalse->typeInfo;
    else
        node->typeInfo = ifTrue->typeInfo;

    // Constant expression
    if (expression->hasComputedValue())
    {
        node->childs.clear();

        if (expression->computedValue->reg.b)
        {
            node->inheritComputedValue(ifTrue);
            node->childs.push_back(ifTrue);
            ifFalse->release();
        }
        else
        {
            node->inheritComputedValue(ifFalse);
            node->childs.push_back(ifFalse);
            ifTrue->release();
        }

        expression->release();
        node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
        return true;
    }

    expression->setBcNotifAfter(ByteCodeGenJob::emitConditionalOpAfterExpr);
    ifTrue->setBcNotifAfter(ByteCodeGenJob::emitConditionalOpAfterIfTrue);
    node->byteCodeFct = ByteCodeGenJob::emitConditionalOp;

    return true;
}

bool SemanticJob::resolveNullConditionalOp(SemanticContext* context)
{
    auto node = context->node;
    SWAG_ASSERT(node->childs.size() >= 2);

    auto expression = node->childs[0];
    auto ifZero     = node->childs[1];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifZero));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifZero));
    if (context->result != ContextResult::Done)
        return true;

    auto typeInfo = TypeManager::concreteType(expression->typeInfo);

    if (typeInfo->isStruct())
    {
        Diagnostic diag{node->sourceFile, node->token, Err(Err0342)};
        diag.addRange(expression, Diagnostic::isType(typeInfo));
        return context->report(diag);
    }
    else if (!typeInfo->isString() &&
             !typeInfo->isPointer() &&
             !typeInfo->isInterface() &&
             !typeInfo->isNativeIntegerOrRune() &&
             !typeInfo->isNativeFloat() &&
             !typeInfo->isLambdaClosure())
    {
        Diagnostic diag{node->sourceFile, node->token, Fmt(Err(Err0332), typeInfo->getDisplayNameC())};
        diag.addRange(expression, Diagnostic::isType(typeInfo));
        return context->report(diag);
    }

    if (expression->hasComputedValue())
    {
        bool notNull = true;
        if (typeInfo->isString())
        {
            notNull = expression->computedValue->text.buffer != nullptr;
        }
        else if (typeInfo->isInterface())
        {
            notNull = expression->computedValue->storageSegment != nullptr;
        }
        else
        {
            switch (typeInfo->sizeOf)
            {
            case 1:
                notNull = expression->computedValue->reg.u8 != 0;
                break;
            case 2:
                notNull = expression->computedValue->reg.u16 != 0;
                break;
            case 4:
                notNull = expression->computedValue->reg.u32 != 0;
                break;
            case 8:
                notNull = expression->computedValue->reg.u64 != 0;
                break;
            }
        }

        if (notNull)
        {
            node->inheritComputedValue(expression);
            node->typeInfo = expression->typeInfo;
        }
        else
        {
            node->inheritComputedValue(ifZero);
            node->typeInfo = ifZero->typeInfo;
        }
    }
    else
    {
        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::Note, []()
                           { return Nte(Nte0132); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, expression, ifZero, CASTFLAG_COMMUTATIVE | CASTFLAG_STRICT));

        node->typeInfo    = expression->typeInfo;
        node->byteCodeFct = ByteCodeGenJob::emitNullConditionalOp;
    }

    return true;
}

bool SemanticJob::resolveDefer(SemanticContext* context)
{
    auto node         = CastAst<AstDefer>(context->node, AstNodeKind::Defer);
    node->byteCodeFct = ByteCodeGenJob::emitDefer;

    SWAG_ASSERT(node->childs.size() == 1);
    auto expr = node->childs.front();
    expr->flags |= AST_NO_BYTECODE;

    return true;
}

bool SemanticJob::resolveRange(SemanticContext* context)
{
    auto node = CastAst<AstRange>(context->node, AstNodeKind::Range);
    SWAG_CHECK(checkIsConcrete(context, node->expressionLow));
    SWAG_CHECK(checkIsConcrete(context, node->expressionUp));

    auto typeInfo = TypeManager::concreteType(node->expressionLow->typeInfo);
    if (!typeInfo->isNativeIntegerOrRune() && !typeInfo->isNativeFloat())
        return context->report({node->expressionLow, Fmt(Err(Err0002), node->expressionLow->typeInfo->getDisplayNameC())});

    SWAG_CHECK(TypeManager::makeCompatibles(context, node->expressionLow, node->expressionUp, CASTFLAG_COMMUTATIVE));

    node->typeInfo = node->expressionLow->typeInfo;
    node->inheritAndFlag1(AST_CONST_EXPR);
    return true;
}