#include "pch.h"
#include "Ast.h"
#include "SemanticJob.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ErrorIds.h"
#include "Report.h"
#include "ThreadManager.h"
#include "LanguageSpec.h"
#include "Naming.h"

bool SemanticJob::resolveMove(SemanticContext* context)
{
    auto node  = context->node;
    auto right = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, right));
    node->inheritOrFlag(right, AST_NO_LEFT_DROP | AST_FORCE_MOVE | AST_NO_RIGHT_DROP);
    node->typeInfo    = right->typeInfo;
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;
    if (right->resolvedSymbolOverload)
        right->resolvedSymbolOverload->flags |= OVERLOAD_HAS_MAKE_POINTER;

    if (node->flags & AST_FORCE_MOVE)
    {
        if ((right->typeInfo->isStruct() && right->typeInfo->isConst()) || right->typeInfo->isConstPointerRef())
        {
            Diagnostic diag{right, Fmt(Err(Err0559), right->typeInfo->getDisplayNameC())};
            if (right->resolvedSymbolOverload && right->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM)
            {
                auto note = Diagnostic::help(Hlp(Hlp0016));
                return context->report(diag, note);
            }

            return context->report(diag);
        }
    }

    return true;
}

// :DeduceLambdaType
// This function is called when the type has been identified, and there's a lambda waiting for it.
// This will launch the evaluation of the lambda now that we can deduce some missing types (like parameters)
bool SemanticJob::resolveAfterKnownType(SemanticContext* context)
{
    auto node     = context->node;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);
    if (!typeInfo->isLambdaClosure() && !typeInfo->isStruct())
        return true;

    auto findMpl = node->parent->findChild(AstNodeKind::MakePointerLambda);
    if (!findMpl)
        return true;
    auto mpl = CastAst<AstMakePointer>(findMpl, AstNodeKind::MakePointerLambda);
    if (!(mpl->specFlags & AstMakePointer::SPECFLAG_DEP_TYPE))
        return true;

    // Cannot cast from closure to lambda
    if (node->typeInfo->getCA()->isLambda() && mpl->lambda->typeInfo->getCA()->isClosure())
    {
        Diagnostic diag{mpl, Err(Err0185)};
        diag.addRange(node, Diagnostic::isType(node->typeInfo));
        auto note = Diagnostic::help(Hlp(Hlp0003));
        return context->report(diag, note);
    }

    ScopedLock lk(mpl->lambda->mutex);
    mpl->lambda->flags &= ~AST_SPEC_SEMANTIC2;
    launchResolveSubDecl(context, mpl->lambda);
    return true;
}

bool SemanticJob::checkIsConstAffect(SemanticContext* context, AstNode* left, AstNode* right)
{
    bool isConst = false;

    // Check that left type is mutable
    if ((left->flags & AST_IS_CONST) ||
        !(left->flags & AST_L_VALUE) ||
        (left->typeInfo->isFakeAlias() && left->typeInfo->isConst()) ||
        (left->typeInfo->isConstPointerRef() && right->kind != AstNodeKind::KeepRef))
    {
        isConst = true;
    }

    if (!isConst)
    {
        if (left->resolvedSymbolOverload)
            left->resolvedSymbolOverload->flags |= OVERLOAD_HAS_AFFECT;
        return true;
    }

    Utf8        hint;
    auto        node    = context->node;
    Diagnostic* note    = nullptr;
    auto        orgLeft = left;

    if (left->kind == AstNodeKind::IdentifierRef)
    {
        // If not, try to find the culprit type
        for (int i = left->childs.count - 1; i >= 0; i--)
        {
            auto child     = left->childs[i];
            auto typeChild = TypeManager::concreteType(child->typeInfo, CONCRETE_FORCEALIAS);
            if (!typeChild)
                continue;

            if (child->kind == AstNodeKind::ArrayPointerIndex)
            {
                auto arr = CastAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
                if (arr->array->typeInfo->isString())
                {
                    left = arr->array;
                    break;
                }
            }

            if (child->isFunctionCall() && (child->typeInfo->isConst() || child->typeInfo->isStruct()))
            {
                left = child;
                hint = Fmt(Hnt(Hnt0039), left->typeInfo->getDisplayNameC());
                break;
            }

            if (typeChild->isConst())
            {
                left = child;
                break;
            }
        }

        if (left->kind == AstNodeKind::Identifier && left->specFlags & (AstIdentifier::SPECFLAG_FROM_USING | AstIdentifier::SPECFLAG_FROM_WITH))
        {
            auto leftId = CastAst<AstIdentifier>(left, AstNodeKind::Identifier);
            hint        = "this is equivalent to '";
            for (size_t ic = 0; ic < orgLeft->childs.size(); ic++)
            {
                auto c = orgLeft->childs[ic];
                if (ic)
                    hint += ".";
                hint += c->token.text;
            }

            if (left->specFlags & AstIdentifier::SPECFLAG_FROM_USING)
                hint += "' because of a 'using'";
            else
                hint += "' because of a 'with'";

            SWAG_ASSERT(left->resolvedSymbolOverload);
            if (left->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM && left->typeInfo->isConst())
                note = Diagnostic::note(leftId->identifierExtension->fromAlternateVar, Nte(Nte0023));
            else if (!(left->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM))
                note = Diagnostic::note(leftId->identifierExtension->fromAlternateVar, Nte(Nte0023), Diagnostic::isType(left->typeInfo));
        }
    }

    if (left->typeInfo->isConst() && left->resolvedSymbolOverload && left->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        if (left == left->parent->childs.back())
        {
            Diagnostic diag{node, node->token, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str()), Hnt(Hnt0061)};
            if (hint.empty())
                hint = Diagnostic::isType(left->typeInfo);
            diag.addRange(left, hint);
            auto note1 = Diagnostic::help(Hlp(Hlp0016));
            return context->report(diag, note, note1);
        }
        else if (left->typeInfo->isConstPointerRef())
        {
            Diagnostic diag{node, node->token, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str()), Hnt(Hnt0061)};
            if (hint.empty())
                hint = Hnt(Hnt0106);
            diag.addRange(left, hint);
            return context->report(diag, note);
        }
        else
        {
            Diagnostic diag{node, node->token, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str()), Hnt(Hnt0061)};
            if (hint.empty())
                hint = Diagnostic::isType(left);
            diag.addRange(left, hint);
            return context->report(diag, note);
        }
    }

    if (left->resolvedSymbolOverload && left->resolvedSymbolOverload->flags & OVERLOAD_IS_LET)
    {
        Diagnostic diag{node, node->token, Err(Err0564), Hnt(Hnt0061)};
        diag.addRange(left, Hnt(Hnt0050));
        note = Diagnostic::hereIs(left->resolvedSymbolOverload->node);
        return context->report(diag, note);
    }

    if (left->flags & AST_L_VALUE)
    {
        Diagnostic diag{node, node->token, Err(Err0564), Hnt(Hnt0061)};
        if (hint.empty())
            hint = Diagnostic::isType(left);
        diag.addRange(left, hint);
        return context->report(diag, note);
    }

    if (left->resolvedSymbolOverload && left->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE)
    {
        Diagnostic diag{node, node->token, Err(Err0564), Hnt(Hnt0061)};
        if (hint.empty())
            hint = Hnt(Hnt0018);
        diag.addRange(left, hint);
        return context->report(diag);
    }

    Diagnostic diag{node, node->token, Err(Err0565), Hnt(Hnt0061)};
    if (hint.empty())
        hint = Diagnostic::isType(left);
    diag.addRange(left, hint);
    return context->report(diag);
}

bool SemanticJob::resolveAffect(SemanticContext* context)
{
    auto node    = CastAst<AstOp>(context->node, AstNodeKind::AffectOp);
    auto left    = node->childs[0];
    auto right   = node->childs[1];
    auto tokenId = node->tokenId;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    if (context->result != ContextResult::Done)
        return true;

    SWAG_CHECK(checkIsConstAffect(context, left, right));

    // :ConcreteRef
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);
    TypeInfo* rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_ALIAS);
    if (right->semFlags & SEMFLAG_FROM_REF)
        leftTypeInfo = getConcreteTypeUnRef(left, CONCRETE_ALIAS);
    else if (leftTypeInfo->isPointerRef() && !rightTypeInfo->isPointerRef())
        leftTypeInfo = getConcreteTypeUnRef(left, CONCRETE_ALIAS);
    SWAG_ASSERT(leftTypeInfo);
    SWAG_ASSERT(rightTypeInfo);

    // Special case for enum : nothing is possible, except for flags
    bool forEnumFlags = false;
    if (node->tokenId != TokenId::SymEqual)
    {
        if (leftTypeInfo->getCA()->isEnum() || rightTypeInfo->getCA()->isEnum())
        {
            SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
            if (node->tokenId != TokenId::SymVerticalEqual &&
                node->tokenId != TokenId::SymAmpersandEqual &&
                node->tokenId != TokenId::SymCircumflexEqual)
                return notAllowed(context, node, leftTypeInfo, nullptr, left);
            if (!(leftTypeInfo->getCA()->flags & TYPEINFO_ENUM_FLAGS) || !(rightTypeInfo->getCA()->flags & TYPEINFO_ENUM_FLAGS))
                return notAllowed(context, node, leftTypeInfo, "because the enum is not marked with 'Swag.EnumFlags'", left);
            forEnumFlags = true;
        }
    }

    rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    SWAG_VERIFY(!rightTypeInfo->isVoid(), context->report({right, Err(Err0569)}));

    // Be sure modifiers are relevant
    if (right->kind == AstNodeKind::NoDrop || right->kind == AstNodeKind::Move)
    {
        auto leftConcrete = TypeManager::concreteType(leftTypeInfo);
        if (right->flags & AST_NO_LEFT_DROP)
            SWAG_VERIFY(leftConcrete->isSame(rightTypeInfo, CASTFLAG_CAST), context->report({node, node->token, Fmt(Err(Err0568), g_LangSpec->name_nodrop.c_str(), leftConcrete->getDisplayNameC(), rightTypeInfo->getDisplayNameC())}));
        if (right->flags & AST_NO_RIGHT_DROP)
            SWAG_VERIFY(leftConcrete->isSame(rightTypeInfo, CASTFLAG_CAST), context->report({node, node->token, Fmt(Err(Err0568), g_LangSpec->name_moveraw.c_str(), leftConcrete->getDisplayNameC(), rightTypeInfo->getDisplayNameC())}));
        if (right->flags & AST_FORCE_MOVE)
            SWAG_VERIFY(leftConcrete->isSame(rightTypeInfo, CASTFLAG_CAST), context->report({node, node->token, Fmt(Err(Err0568), g_LangSpec->name_move.c_str(), leftConcrete->getDisplayNameC(), rightTypeInfo->getDisplayNameC())}));
    }

    // No direct operations on any, except affect any to any
    if (leftTypeInfo->isAny() && node->tokenId != TokenId::SymEqual)
    {
        Diagnostic diag{node, node->token, Fmt(Err(Err0570), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
        diag.hint = Hnt(Hnt0061);
        diag.addRange(left, Diagnostic::isType(leftTypeInfo));
        return context->report(diag);
    }

    // Is this an array like affectation ?
    AstArrayPointerIndex* arrayNode = nullptr;
    if (left->kind == AstNodeKind::IdentifierRef && left->childs.back()->kind == AstNodeKind::ArrayPointerIndex)
    {
        arrayNode      = CastAst<AstArrayPointerIndex>(left->childs.back(), AstNodeKind::ArrayPointerIndex);
        auto arrayType = TypeManager::concretePtrRefType(arrayNode->array->typeInfo);
        if (!arrayType->isStruct())
            arrayNode = nullptr;

        // Add self and value in list of parameters
        if (arrayNode)
        {
            if (!(node->semFlags & SEMFLAG_FLAT_PARAMS))
            {
                auto leftNode = arrayNode;
                while (leftNode->array->kind == AstNodeKind::ArrayPointerIndex)
                    leftNode = CastAst<AstArrayPointerIndex>(leftNode->array, AstNodeKind::ArrayPointerIndex);
                arrayNode->structFlatParams.push_back(right);
                arrayNode->structFlatParams.push_front(leftNode->array);
                node->semFlags |= SEMFLAG_FLAT_PARAMS;
            }
        }
    }

    node->typeInfo = g_TypeMgr->typeInfoBool;

    bool forStruct = leftTypeInfo->isStruct();
    bool forTuple  = leftTypeInfo->isTuple();

    SWAG_CHECK(evaluateConstExpression(context, right));
    if (context->result == ContextResult::Pending)
        return true;

    // Cast from struct to interface : need to wait for all interfaces to be registered
    if (tokenId == TokenId::SymEqual)
    {
        if (leftTypeInfo->isInterface() && rightTypeInfo->isStruct())
        {
            context->job->waitAllStructInterfaces(rightTypeInfo);
            if (context->result == ContextResult::Pending)
                return true;
            PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [leftTypeInfo]()
                              { return Diagnostic::isType(leftTypeInfo); });
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST));
        }
    }

    switch (tokenId)
    {
    case TokenId::SymEqual:
    {
        if (!leftTypeInfo->isNative() &&
            !leftTypeInfo->isPointer() &&
            !leftTypeInfo->isSlice() &&
            !leftTypeInfo->isLambdaClosure() &&
            !leftTypeInfo->isListTuple() &&
            !leftTypeInfo->isListArray() &&
            !leftTypeInfo->isStruct() &&
            !leftTypeInfo->isInterface() &&
            !leftTypeInfo->isArray() &&
            !leftTypeInfo->isAlias() &&
            !leftTypeInfo->isEnum())
            return context->report({left, Fmt(Err(Err0571), Naming::kindName(leftTypeInfo).c_str(), leftTypeInfo->getDisplayNameC())});
        if (!rightTypeInfo->isNative() &&
            !rightTypeInfo->isPointer() &&
            !rightTypeInfo->isSlice() &&
            !rightTypeInfo->isLambdaClosure() &&
            !rightTypeInfo->isListTuple() &&
            !rightTypeInfo->isListArray() &&
            !rightTypeInfo->isStruct() &&
            !rightTypeInfo->isInterface() &&
            !rightTypeInfo->isArray() &&
            !rightTypeInfo->isAlias())
            return context->report({right, Fmt(Err(Err0572), rightTypeInfo->getDisplayNameC(), Naming::aKindName(rightTypeInfo).c_str())});

        if (forStruct)
        {
            if (arrayNode)
            {
                if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, CASTFLAG_CAST))
                {
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else if (forTuple)
                {
                    return context->report({node, Err(Err0573)});
                }
                else
                {
                    SymbolName* symbol = nullptr;
                    SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opIndexAffect, left, &symbol));
                    if (!symbol)
                    {
                        if (context->result != ContextResult::Done)
                            return true;

                        Diagnostic diag{right, Fmt(Err(Err0225), rightTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC())};
                        diag.hint = Diagnostic::isType(rightTypeInfo);
                        diag.addRange(left, Diagnostic::isType(leftTypeInfo));

                        auto note = Diagnostic::note(node, node->token, Fmt(Nte(Nte0051), "opIndexAffect", rightTypeInfo->getDisplayNameC()));
                        return context->report(diag, note);
                    }

                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAffect, nullptr, nullptr, left, arrayNode->structFlatParams));
                }
            }
            else
            {
                if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, CASTFLAG_CAST | CASTFLAG_FOR_AFFECT))
                {
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else if (rightTypeInfo->isInitializerList())
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST | CASTFLAG_ACCEPT_PENDING));
                    if (context->result == ContextResult::Pending)
                        return true;
                }
                else
                {
                    if (leftTypeInfo->isTuple())
                        return context->report({node, Err(Err0574)});
                    SWAG_CHECK(resolveUserOpAffect(context, leftTypeInfo, rightTypeInfo, left, right));
                    if (context->result != ContextResult::Done)
                        return true;
                }
            }

            break;
        }

        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::MsgPrio, [rightTypeInfo, leftTypeInfo]()
                           { return Fmt(Err(Err0196), "affect", rightTypeInfo->getDisplayNameC(), "to", leftTypeInfo->getDisplayNameC()); });
        PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [left]()
                          { return Diagnostic::isType(left); });

        uint32_t castFlags = CASTFLAG_AUTO_BOOL | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT | CASTFLAG_ACCEPT_PENDING;
        if (leftTypeInfo->isStruct() ||
            leftTypeInfo->isArray() ||
            leftTypeInfo->isClosure())
            castFlags |= CASTFLAG_UNCONST;

        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right, castFlags));
        if (context->result == ContextResult::Pending)
            return true;
        break;
    }

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
    {
        if (forTuple)
            return context->report({node, Err(Err0573)});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        auto           mmsg = tokenId == TokenId::SymLowerLowerEqual ? "make a left shift with" : "make a right shift with";
        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::MsgPrio, [mmsg, leftTypeInfo, rightTypeInfo]()
                           { return Fmt(Err(Err0196), mmsg, leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()); });
        PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [left]()
                          { return Diagnostic::isType(left); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, left, right, CASTFLAG_TRY_COERCE));

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return notAllowed(context, node, leftTypeInfo);
        }

        break;
    }

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
    {
        if (forTuple)
            return context->report({node, Err(Err0573)});
        else if (forStruct)
        {
            const char* op = "&=";
            if (tokenId == TokenId::SymVerticalEqual)
                op = "|=";
            else if (tokenId == TokenId::SymCircumflexEqual)
                op = "^=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        SWAG_CHECK(forEnumFlags || checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::MsgPrio, [leftTypeInfo, rightTypeInfo]()
                           { return Fmt(Err(Err0196), "make a bit operation with", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()); });
        PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [left]()
                          { return Diagnostic::isType(left); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;
    }

    case TokenId::SymPlusEqual:
    case TokenId::SymMinusEqual:
    {
        if (forTuple)
            return context->report({node, Err(Err0573)});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPlusEqual ? "+=" : "-=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        // :PointerArithmetic
        if (leftTypeInfo->isPointer())
        {
            if (!leftTypeInfo->isPointerArithmetic())
            {
                Diagnostic diag{node, node->token, Err(Err0192)};
                diag.hint = Hnt(Hnt0061);
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                auto note = Diagnostic::help(Hlp(Hlp0046));
                return context->report(diag, note);
            }

            if (leftTypeInfo->isPointerTo(NativeTypeKind::Void))
            {
                Diagnostic diag{node, node->token, Err(Err0111)};
                diag.hint = Hnt(Hnt0061);
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                return context->report(diag);
            }

            rightTypeInfo = TypeManager::concreteType(right->typeInfo);
            SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, Fmt(Err(Err0579), rightTypeInfo->getDisplayNameC())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, left, right, CASTFLAG_TRY_COERCE));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

        auto           mmsg = tokenId == TokenId::SymPlusEqual ? "add" : "substract";
        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::MsgPrio, [mmsg, leftTypeInfo, rightTypeInfo]()
                           { return Fmt(Err(Err0196), mmsg, leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()); });
        PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [left]()
                          { return Diagnostic::isType(left); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;
    }

    case TokenId::SymSlashEqual:
    {
        if (forTuple)
            return context->report({node, Err(Err0573)});
        else if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, "/=", nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, "/=", nullptr, left, right));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::MsgPrio, [leftTypeInfo, rightTypeInfo]()
                           { return Fmt(Err(Err0196), "divide", leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()); });
        PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [left]()
                          { return Diagnostic::isType(left); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;
    }

    case TokenId::SymPercentEqual:
    case TokenId::SymAsteriskEqual:
    {
        if (forTuple)
            return context->report({node, Err(Err0573)});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymPercentEqual ? "%=" : "*=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));

        auto           mmsg = tokenId == TokenId::SymPercentEqual ? "make a modulo with" : "multiply";
        PushErrCxtStep ec1(context, nullptr, ErrCxtStepKind::MsgPrio, [mmsg, leftTypeInfo, rightTypeInfo]()
                           { return Fmt(Err(Err0196), mmsg, leftTypeInfo->getDisplayNameC(), "and", rightTypeInfo->getDisplayNameC()); });
        PushErrCxtStep ec(context, left, ErrCxtStepKind::Hint2, [left]()
                          { return Diagnostic::isType(left); });
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString)
        {
            return notAllowed(context, node, leftTypeInfo);
        }
        break;
    }

    default:
        return Report::internalError(context->node, "resolveAffect, invalid token");
    }

    node->byteCodeFct = ByteCodeGenJob::emitAffect;
    return true;
}