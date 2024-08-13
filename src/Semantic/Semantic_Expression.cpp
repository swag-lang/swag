#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Semantic/Semantic.h"
#include "Semantic/Symbol/Symbol.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool Semantic::checkIsConstExpr(JobContext* context, bool test, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    if (test)
        return true;

    const auto note = computeNonConstExprNote(expression);

    if (expression->hasSpecialFuncCall())
    {
        Diagnostic err{expression, expression->token, formErr(Err0030, expression->typeInfo->getDisplayNameC())};
        const auto userOp = expression->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
        err.hint          = formNte(Nte0157, userOp->symbol->name.c_str());
        err.addNote(note);
        return context->report(err);
    }

    Utf8 message;
    if (errMsg.length() && errParam.length())
        message = form(errMsg.c_str(), errParam.c_str());
    else if (errMsg.length())
        message = errMsg;
    else
        message = toErr(Err0026);

    Diagnostic err{expression, message};
    err.addNote(note);
    return context->report(err);
}

bool Semantic::checkIsConstExpr(JobContext* context, AstNode* expression, const Utf8& errMsg, const Utf8& errParam)
{
    return checkIsConstExpr(context, expression->hasAstFlag(AST_CONST_EXPR), expression, errMsg, errParam);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveExplicitNoInit(SemanticContext* context)
{
    const auto node = context->node;
    node->parent->addAstFlag(AST_EXPLICITLY_NOT_INITIALIZED);
    node->addAstFlag(AST_CONST_EXPR);
    node->typeInfo = g_TypeMgr->typeInfoVoid;
    return true;
}

bool Semantic::computeExpressionListTupleType(SemanticContext* context, AstNode* node)
{
    for (const auto child : node->children)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        YIELD();
    }

    const auto typeInfo = makeType<TypeInfoList>(TypeInfoKind::TypeListTuple);
    typeInfo->name      = "{";
    typeInfo->sizeOf    = 0;
    typeInfo->declNode  = node;

    node->addAstFlag(AST_CONST_EXPR | AST_R_VALUE);
    for (const auto child : node->children)
    {
        if (!typeInfo->subTypes.empty())
            typeInfo->name += ", ";

        auto typeParam = TypeManager::makeParam();

        // When generating parameters for a closure call, keep the reference if we want one !
        if (child->isNot(AstNodeKind::MakePointer) || !child->hasSpecFlag(AstMakePointer::SPEC_FLAG_TO_REF))
            typeParam->typeInfo = TypeManager::concretePtrRefType(child->typeInfo, CONCRETE_FUNC);
        else
            typeParam->typeInfo = child->typeInfo;

        typeInfo->subTypes.push_back(typeParam);

        // Value has been named
        const auto isNamed = child->extraPointer<AstNode>(ExtraPointerKind::IsNamed);
        if (isNamed)
        {
            typeInfo->name += isNamed->token.text;
            typeInfo->name += ": ";
            typeParam->name = isNamed->token.text;
        }

        typeInfo->name += typeParam->typeInfo->name;
        typeInfo->sizeOf += typeParam->typeInfo->sizeOf;

        if (!child->hasAstFlag(AST_CONST_EXPR))
            node->removeAstFlag(AST_CONST_EXPR);
        if (!child->hasAstFlag(AST_R_VALUE))
            node->removeAstFlag(AST_R_VALUE);
    }

    typeInfo->name += "}";
    node->typeInfo = typeInfo;
    return true;
}

bool Semantic::resolveExpressionListTuple(SemanticContext* context)
{
    const auto node = castAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);
    SWAG_CHECK(computeExpressionListTupleType(context, node));
    YIELD();

    node->setBcNotifyBefore(ByteCodeGen::emitExpressionListBefore);
    node->byteCodeFct = ByteCodeGen::emitExpressionList;

    // If the literal tuple is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the tuple will come from the constant segment.
    if (!node->hasAstFlag(AST_CONST_EXPR) && node->ownerScope && node->ownerFct && node->typeInfo)
    {
        allocateOnStack(node, node->typeInfo);
    }

    return true;
}

bool Semantic::resolveExpressionListArray(SemanticContext* context)
{
    const auto node = castAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

    for (const auto child : node->children)
    {
        SWAG_CHECK(checkIsConcreteOrType(context, child));
        YIELD();
    }

    const auto typeInfo = makeType<TypeInfoList>(TypeInfoKind::TypeListArray);
    SWAG_ASSERT(!node->children.empty());
    typeInfo->declNode = node;

    node->addAstFlag(AST_CONST_EXPR | AST_R_VALUE);
    for (const auto child : node->children)
    {
        auto       typeParam = TypeManager::makeParam();
        const auto childType = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
        typeParam->typeInfo  = childType;
        typeInfo->subTypes.push_back(typeParam);
        typeInfo->sizeOf += childType->sizeOf;
        if (!child->hasAstFlag(AST_CONST_EXPR))
            node->removeAstFlag(AST_CONST_EXPR);
        if (!child->hasAstFlag(AST_R_VALUE))
            node->removeAstFlag(AST_R_VALUE);
    }

    if (node->hasAstFlag(AST_CONST_EXPR))
        typeInfo->setConst();

    typeInfo->forceComputeName();
    node->setBcNotifyBefore(ByteCodeGen::emitExpressionListBefore);
    node->byteCodeFct = ByteCodeGen::emitExpressionList;
    node->typeInfo    = typeInfo;

    // If the literal array is not constant, then we need to reserve some space in the
    // stack in order to store it.
    // Otherwise the array will come from the constant segment.
    // :ExprListArrayStorage
    if (!node->hasAstFlag(AST_CONST_EXPR) && node->ownerScope && node->ownerFct)
    {
        allocateOnStack(node, node->typeInfo);
    }

    return true;
}

bool Semantic::evaluateConstExpression(SemanticContext* context, AstNode* node)
{
    if (node->hasAstFlag(AST_CONST_EXPR) &&
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
    SWAG_ASSERT(node->childCount() == 3);

    const auto expression = node->firstChild();
    const auto ifTrue     = node->secondChild();
    const auto ifFalse    = node->children[2];
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcreteOrType(context, ifTrue));
    SWAG_CHECK(checkIsConcreteOrType(context, ifFalse));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifTrue, ifFalse));
    YIELD();

    expression->typeInfo = getConcreteTypeUnRef(expression, CONCRETE_ALL);
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoBool, nullptr, expression, CAST_FLAG_AUTO_BOOL));

    auto rightT = ifFalse;
    auto leftT  = ifTrue;

    getConcreteTypeUnRef(ifFalse, CONCRETE_ALL);
    getConcreteTypeUnRef(ifTrue, CONCRETE_ALL);

    // We cast the true expression to the false expression.
    // But some times, it's better to do the other way
    if (leftT->typeInfo->isConst() ||
        leftT->typeInfo->isUntypedInteger())
        std::swap(leftT, rightT);

    // Make the cast
    {
        PushErrCxtStep ec(context, rightT, ErrCxtStepKind::Note, [] { return toNte(Nte0201); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, rightT, leftT, CAST_FLAG_COMMUTATIVE | CAST_FLAG_STRICT));
    }

    // Determine if we should take the type from the "false" expression, or from the "true"
    if (ifTrue->typeInfo->isPointerNull())
        node->typeInfo = ifFalse->typeInfo;
    else
        node->typeInfo = ifTrue->typeInfo;

    // Constant expression
    if (expression->hasFlagComputedValue())
    {
        node->children.clear();

        if (expression->computedValue()->reg.b)
        {
            node->inheritComputedValue(ifTrue);
            node->children.push_back(ifTrue);
            ifFalse->release();
        }
        else
        {
            node->inheritComputedValue(ifFalse);
            node->children.push_back(ifFalse);
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
    SWAG_ASSERT(node->childCount() >= 2);

    const auto expression = node->firstChild();
    const auto ifZero     = node->secondChild();
    SWAG_CHECK(checkIsConcrete(context, expression));
    SWAG_CHECK(checkIsConcrete(context, ifZero));

    SWAG_CHECK(evaluateConstExpression(context, expression, ifZero));
    YIELD();

    ifZero->typeInfo    = getConcreteTypeUnRef(ifZero, CONCRETE_ALL);
    const auto typeInfo = getConcreteTypeUnRef(expression, CONCRETE_ALL);

    if (typeInfo->isStruct())
    {
        Diagnostic err{node->token.sourceFile, node->token, toErr(Err0561)};
        err.addNote(expression, Diagnostic::isType(typeInfo));
        return context->report(err);
    }

    if (!typeInfo->isString() &&
        !typeInfo->isPointer() &&
        !typeInfo->isInterface() &&
        !typeInfo->isNativeIntegerOrRune() &&
        !typeInfo->isBool() &&
        !typeInfo->isNativeFloat() &&
        !typeInfo->isLambdaClosure())
    {
        Diagnostic err{node->token.sourceFile, node->token, formErr(Err0562, typeInfo->getDisplayNameC())};
        err.addNote(expression, Diagnostic::isType(typeInfo));
        return context->report(err);
    }

    if (expression->hasFlagComputedValue())
    {
        bool notNull = true;
        if (typeInfo->isString())
        {
            notNull = expression->computedValue()->text.buffer != nullptr;
        }
        else if (typeInfo->isInterface())
        {
            notNull = expression->computedValue()->storageSegment != nullptr;
        }
        else
        {
            switch (typeInfo->sizeOf)
            {
                case 1:
                    notNull = expression->computedValue()->reg.u8 != 0;
                    break;
                case 2:
                    notNull = expression->computedValue()->reg.u16 != 0;
                    break;
                case 4:
                    notNull = expression->computedValue()->reg.u32 != 0;
                    break;
                case 8:
                    notNull = expression->computedValue()->reg.u64 != 0;
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
            ifZero->addAstFlag(AST_NO_BYTECODE);
        }
        else
        {
            node->inheritComputedValue(ifZero);
            node->typeInfo = ifZero->typeInfo;
            expression->addAstFlag(AST_NO_BYTECODE);
        }
    }
    else
    {
        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::Note, [] { return toNte(Nte0021); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, expression, ifZero, CAST_FLAG_COMMUTATIVE | CAST_FLAG_STRICT));

        node->typeInfo    = expression->typeInfo;
        node->byteCodeFct = ByteCodeGen::emitNullConditionalOp;
    }

    return true;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
bool Semantic::resolveDefer(SemanticContext* context)
{
    const auto node   = castAst<AstDefer>(context->node, AstNodeKind::Defer);
    node->byteCodeFct = ByteCodeGen::emitDefer;

    SWAG_ASSERT(node->childCount() == 1);
    const auto expr = node->firstChild();
    expr->addAstFlag(AST_NO_BYTECODE);

    return true;
}

bool Semantic::resolveRange(SemanticContext* context)
{
    const auto node = castAst<AstRange>(context->node, AstNodeKind::Range);
    SWAG_CHECK(checkIsConcrete(context, node->expressionLow));
    SWAG_CHECK(checkIsConcrete(context, node->expressionUp));

    node->expressionLow->typeInfo = getConcreteTypeUnRef(node->expressionLow, CONCRETE_FUNC | CONCRETE_ALIAS);
    node->expressionUp->typeInfo  = getConcreteTypeUnRef(node->expressionUp, CONCRETE_FUNC | CONCRETE_ALIAS);

    const auto leftTypeInfo = TypeManager::concreteType(node->expressionLow->typeInfo);
    if (!leftTypeInfo->isNativeIntegerOrRune() && !leftTypeInfo->isNativeFloat())
    {
        Diagnostic err{node->expressionLow, formErr(Err0213, node->expressionLow->typeInfo->getDisplayNameC())};
        err.addNote(toNte(Nte0062));
        return context->report(err);
    }

    const bool downSigned   = node->expressionLow->typeInfo->isNativeIntegerSigned() && !node->expressionLow->typeInfo->isUntypedInteger();
    const bool upSigned     = node->expressionUp->typeInfo->isNativeIntegerSigned() && !node->expressionUp->typeInfo->isUntypedInteger();
    const bool downUnSigned = node->expressionLow->typeInfo->isNativeIntegerUnsigned();
    const bool upUnSigned   = node->expressionUp->typeInfo->isNativeIntegerUnsigned();
    if ((downSigned && upUnSigned) || (downUnSigned && upSigned))
    {
        Diagnostic err{node, node->token, toErr(Err0552)};
        err.addNote(node->expressionLow, Diagnostic::isType(node->expressionLow));
        err.addNote(node->expressionUp, Diagnostic::isType(node->expressionUp));
        return context->report(err);
    }

    SWAG_CHECK(TypeManager::makeCompatibles(context, node->expressionLow, node->expressionUp, CAST_FLAG_COMMUTATIVE));

    node->typeInfo = node->expressionLow->typeInfo;
    node->inheritAstFlagsAnd(AST_CONST_EXPR);
    return true;
}
