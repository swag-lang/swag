#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"
#include "Semantic.h"
#include "Symbol.h"
#include "TypeManager.h"

bool Semantic::checkIsConstExpr(JobContext* context, bool test, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    if (test)
        return true;

    auto note = computeNonConstExprNote(expression);

    if (expression->hasSpecialFuncCall())
    {
        Diagnostic diag{expression, expression->token, FMT(Err(Err0042), expression->typeInfo->getDisplayNameC())};
        diag.hint = FMT(Nte(Nte0144), expression->extMisc()->resolvedUserOpSymbolOverload->symbol->name.c_str());
        return context->report(diag, note);
    }

    Utf8 message;
    if (errMsg.length() && errParam.length())
        message = FMT(errMsg.c_str(), errParam.c_str());
    else if (errMsg.length())
        message = errMsg;
    else
        message = Err(Err0038);

    Diagnostic diag{expression, message};

    // Just keep the culprit if the culprit is the same as the full expression, and there's no
    // specific requested error message
    if (errMsg.empty() && note->startLocation == diag.startLocation && note->endLocation == diag.endLocation)
    {
        Vector<Utf8> parts;
        Diagnostic::tokenizeError(diag.textMsg, parts);
        diag.textMsg = parts[0];
        diag.textMsg += Diagnostic::ERROR_MESSAGE_SEPARATOR;
        diag.textMsg += note->textMsg;
        note = nullptr;
    }

    return context->report(diag, note);
}

bool Semantic::checkIsConstExpr(JobContext* context, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    return checkIsConstExpr(context, expression->flags & AST_CONST_EXPR, expression, errMsg, errParam);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveExplicitNoInit(SemanticContext* context)
{
    const auto node = context->node;
    node->parent->flags |= AST_EXPLICITLY_NOT_INITIALIZED;
    node->flags |= AST_CONST_EXPR;
    node->typeInfo = g_TypeMgr->typeInfoVoid;
    return true;
}

bool Semantic::computeExpressionListTupleType(SemanticContext* context, AstNode* node)
{
    for (const auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        YIELD();
    }

    const auto typeInfo = makeType<TypeInfoList>(TypeInfoKind::TypeListTuple);
    typeInfo->name      = "{";
    typeInfo->sizeOf    = 0;
    typeInfo->declNode  = node;

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (const auto child : node->childs)
    {
        if (!typeInfo->subTypes.empty())
            typeInfo->name += ", ";

        auto typeParam = TypeManager::makeParam();

        // When generating parameters for a closure call, keep the reference if we want one !
        if (child->kind != AstNodeKind::MakePointer || !(child->specFlags & AstMakePointer::SPECFLAG_TO_REF))
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

bool Semantic::resolveExpressionListTuple(SemanticContext* context)
{
    const auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    SWAG_CHECK(computeExpressionListTupleType(context, node));
    YIELD();

    node->setBcNotifyBefore(ByteCodeGen::emitExpressionListBefore);
    node->byteCodeFct = ByteCodeGen::emitExpressionList;

    // If the literal tuple is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the tuple will come from the constant segment.
    if (!(node->flags & AST_CONST_EXPR) && node->ownerScope && node->ownerFct && node->typeInfo)
    {
        allocateOnStack(node, node->typeInfo);
    }

    return true;
}

bool Semantic::resolveExpressionListArray(SemanticContext* context)
{
    const auto node = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    for (const auto child : node->childs)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        YIELD();
    }

    const auto typeInfo = makeType<TypeInfoList>(TypeInfoKind::TypeListArray);
    SWAG_ASSERT(!node->childs.empty());
    typeInfo->declNode = node;

    node->flags |= AST_CONST_EXPR | AST_R_VALUE;
    for (const auto child : node->childs)
    {
        auto       typeParam = TypeManager::makeParam();
        const auto childType = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
        typeParam->typeInfo  = childType;
        typeInfo->subTypes.push_back(typeParam);
        typeInfo->sizeOf += childType->sizeOf;
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_R_VALUE))
            node->flags &= ~AST_R_VALUE;
    }

    if (node->flags & AST_CONST_EXPR)
        typeInfo->setConst();

    typeInfo->forceComputeName();
    node->setBcNotifyBefore(ByteCodeGen::emitExpressionListBefore);
    node->byteCodeFct = ByteCodeGen::emitExpressionList;
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

bool Semantic::evaluateConstExpression(SemanticContext* context, AstNode* node)
{
    if ((node->flags & AST_CONST_EXPR) &&
        !node->typeInfo->isListArray() &&
        !node->typeInfo->isListTuple() &&
        !node->typeInfo->isSlice())
    {
        SWAG_CHECK(checkIsConcrete(context, node));
        SWAG_CHECK(executeCompilerNode(context, node, true));
        YIELD();
    }

    return true;
}

bool Semantic::evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2)
{
    SWAG_CHECK(evaluateConstExpression(context, node1));
    YIELD();
    SWAG_CHECK(evaluateConstExpression(context, node2));
    YIELD();
    return true;
}

bool Semantic::evaluateConstExpression(SemanticContext* context, AstNode* node1, AstNode* node2, AstNode* node3)
{
    SWAG_CHECK(evaluateConstExpression(context, node1));
    YIELD();
    SWAG_CHECK(evaluateConstExpression(context, node2));
    YIELD();
    SWAG_CHECK(evaluateConstExpression(context, node3));
    YIELD();
    return true;
}

bool Semantic::resolveConditionalOp(SemanticContext* context)
{
    const auto node = context->node;
    SWAG_ASSERT(node->childs.size() == 3);

    const auto expression = node->childs[0];
    const auto ifTrue     = node->childs[1];
    const auto ifFalse    = node->childs[2];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcreteOrType(context, ifTrue));
    SWAG_CHECK(checkIsConcreteOrType(context, ifFalse));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifTrue, ifFalse));
    YIELD();

    expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_ALL);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expression, CASTFLAG_AUTO_BOOL));

    auto rightT = ifFalse;
    auto leftT  = ifTrue;

    getConcreteTypeUnRef(ifFalse, CONCRETE_ALL);
    getConcreteTypeUnRef(ifTrue, CONCRETE_ALL);

    // We cast the true expression to the false expression.
    // But some times, it's better to do the other way
    if (leftT->typeInfo->isConst() ||
        leftT->typeInfo->isUntypedInteger())
        swap(leftT, rightT);

    // Make the cast
    {
        PushErrCxtStep ec(context, rightT, ErrCxtStepKind::Note, []()
        {
            return Nte(Nte0185);
        });
        SWAG_CHECK(TypeManager::makeCompatibles(context, rightT, leftT, CASTFLAG_COMMUTATIVE | CASTFLAG_STRICT));
    }

    // Determine if we should take the type from the "false" expression, or from the "true"
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
        node->byteCodeFct = ByteCodeGen::emitPassThrough;
        return true;
    }

    expression->setBcNotifyAfter(ByteCodeGen::emitConditionalOpAfterExpr);
    ifTrue->setBcNotifyAfter(ByteCodeGen::emitConditionalOpAfterIfTrue);
    node->byteCodeFct = ByteCodeGen::emitConditionalOp;

    return true;
}

bool Semantic::resolveNullConditionalOp(SemanticContext* context)
{
    const auto node = context->node;
    SWAG_ASSERT(node->childs.size() >= 2);

    const auto expression = node->childs[0];
    const auto ifZero     = node->childs[1];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifZero));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifZero));
    YIELD();

    ifZero->typeInfo    = getConcreteTypeUnRef(ifZero, CONCRETE_ALL);
    const auto typeInfo = getConcreteTypeUnRef(expression, CONCRETE_ALL);

    if (typeInfo->isStruct())
    {
        Diagnostic diag{node->sourceFile, node->token, Err(Err0166)};
        diag.addRange(expression, Diagnostic::isType(typeInfo));
        return context->report(diag);
    }

    if (!typeInfo->isString() &&
        !typeInfo->isPointer() &&
        !typeInfo->isInterface() &&
        !typeInfo->isNativeIntegerOrRune() &&
        !typeInfo->isBool() &&
        !typeInfo->isNativeFloat() &&
        !typeInfo->isLambdaClosure())
    {
        Diagnostic diag{node->sourceFile, node->token, FMT(Err(Err0165), typeInfo->getDisplayNameC())};
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
            default:
                break;
            }
        }

        node->setPassThrough();
        if (notNull)
        {
            node->inheritComputedValue(expression);
            node->typeInfo = expression->typeInfo;
            ifZero->flags |= AST_NO_BYTECODE;
        }
        else
        {
            node->inheritComputedValue(ifZero);
            node->typeInfo = ifZero->typeInfo;
            expression->flags |= AST_NO_BYTECODE;
        }
    }
    else
    {
        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::Note, []()
        {
            return Nte(Nte0023);
        });
        SWAG_CHECK(TypeManager::makeCompatibles(context, expression, ifZero, CASTFLAG_COMMUTATIVE | CASTFLAG_STRICT));

        node->typeInfo    = expression->typeInfo;
        node->byteCodeFct = ByteCodeGen::emitNullConditionalOp;
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveDefer(SemanticContext* context)
{
    const auto node   = CastAst<AstDefer>(context->node, AstNodeKind::Defer);
    node->byteCodeFct = ByteCodeGen::emitDefer;

    SWAG_ASSERT(node->childs.size() == 1);
    const auto expr = node->childs.front();
    expr->flags |= AST_NO_BYTECODE;

    return true;
}

bool Semantic::resolveRange(SemanticContext* context)
{
    const auto node = CastAst<AstRange>(context->node, AstNodeKind::Range);
    SWAG_CHECK(checkIsConcrete(context, node->expressionLow));
    SWAG_CHECK(checkIsConcrete(context, node->expressionUp));

    node->expressionLow->typeInfo = getConcreteTypeUnRef(node->expressionLow, CONCRETE_FUNC | CONCRETE_ALIAS);
    node->expressionUp->typeInfo  = getConcreteTypeUnRef(node->expressionUp, CONCRETE_FUNC | CONCRETE_ALIAS);

    const auto leftTypeInfo = TypeManager::concreteType(node->expressionLow->typeInfo);
    if (!leftTypeInfo->isNativeIntegerOrRune() && !leftTypeInfo->isNativeFloat())
    {
        const Diagnostic diag{node->expressionLow, FMT(Err(Err0364), node->expressionLow->typeInfo->getDisplayNameC())};
        return context->report(diag, Diagnostic::note(Nte(Nte0200)));
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, node->expressionLow, node->expressionUp, CASTFLAG_COMMUTATIVE));

    node->typeInfo = node->expressionLow->typeInfo;
    node->inheritAndFlag1(AST_CONST_EXPR);
    return true;
}
