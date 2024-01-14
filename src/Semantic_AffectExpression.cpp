#include "pch.h"
#include "Ast.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Naming.h"
#include "Report.h"
#include "Semantic.h"
#include "SemanticError.h"
#include "TypeManager.h"

bool Semantic::resolveMove(SemanticContext* context)
{
    auto node  = context->node;
    auto right = node->childs[0];
    SWAG_CHECK(checkIsConcrete(context, right));
    node->inheritOrFlag(right, AST_NO_LEFT_DROP | AST_FORCE_MOVE | AST_NO_RIGHT_DROP);
    node->typeInfo    = right->typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    if (right->resolvedSymbolOverload)
        right->resolvedSymbolOverload->flags |= OVERLOAD_HAS_MAKE_POINTER;

    if (node->flags & AST_FORCE_MOVE)
    {
        if ((right->typeInfo->isStruct() && right->typeInfo->isConst()) || right->typeInfo->isConstPointerRef())
        {
            Diagnostic diag{right, Fmt(Err(Err0559), right->typeInfo->getDisplayNameC())};
            if (right->resolvedSymbolOverload && right->resolvedSymbolOverload->flags & OVERLOAD_VAR_FUNC_PARAM)
            {
                auto note = Diagnostic::note(Nte(Nte0116));
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
bool Semantic::resolveAfterKnownType(SemanticContext* context)
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
    if (node->typeInfo->getConcreteAlias()->isLambda() && mpl->lambda->typeInfo->getConcreteAlias()->isClosure())
    {
        Diagnostic diag{mpl, Err(Err0185)};
        diag.addRange(node, Diagnostic::isType(node->typeInfo));
        auto note = Diagnostic::note(Nte(Nte0103));
        return context->report(diag, note);
    }

    ScopedLock lk(mpl->lambda->mutex);
    mpl->lambda->flags &= ~AST_SPEC_SEMANTIC2;
    launchResolveSubDecl(context, mpl->lambda);
    return true;
}

bool Semantic::checkIsConstAffect(SemanticContext* context, AstNode* left, AstNode* right)
{
    if (left->childs.back()->semFlags & SEMFLAG_IS_CONST_ASSIGN)
    {
        if (!left->typeInfo->isPointerRef() || right->kind == AstNodeKind::KeepRef)
        {
            left->flags |= AST_IS_CONST;
        }
    }

    // Check that left type is mutable
    bool isConst = false;
    if ((left->flags & AST_CONST_EXPR) ||
        (left->flags & AST_IS_CONST) ||
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
                hint = Fmt(Nte(Nte1039), left->typeInfo->getDisplayNameC());
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
            Diagnostic diag{node, node->token, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str())};
            diag.addRange(left, Diagnostic::isType(left->typeInfo));
            auto note1 = Diagnostic::note(Nte(Nte0116));
            return context->report(diag, note, note1);
        }
        else if (left->typeInfo->isConstPointerRef())
        {
            Diagnostic diag{node, node->token, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str())};
            diag.addRange(left, Diagnostic::isType(left->typeInfo));
            return context->report(diag, note);
        }
        else
        {
            Diagnostic diag{node, node->token, Fmt(Err(Err0740), left->resolvedSymbolName->name.c_str())};
            diag.addRange(left, Diagnostic::isType(left->typeInfo));
            return context->report(diag, note);
        }
    }

    if (left->resolvedSymbolOverload &&
        left->resolvedSymbolOverload->flags & OVERLOAD_IS_LET &&
        (!left->resolvedSymbolOverload->typeInfo->isPointerRef() || right->kind == AstNodeKind::KeepRef))
    {
        Diagnostic diag{node, node->token, Err(Err0564)};
        diag.addRange(left, Nte(Nte1050));
        note = Diagnostic::hereIs(left->resolvedSymbolOverload->node);
        return context->report(diag, note);
    }

    if (left->flags & AST_L_VALUE)
    {
        Diagnostic diag{node, node->token, Err(Err0564)};
        if (hint.empty())
            hint = Diagnostic::isType(left);
        diag.addRange(left, hint);
        return context->report(diag, note);
    }

    if (left->resolvedSymbolOverload && left->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE)
    {
        Diagnostic diag{node, node->token, Err(Err0564)};
        if (hint.empty())
            hint = Nte(Nte1018);
        diag.addRange(left, hint);
        return context->report(diag);
    }

    Diagnostic diag{node, node->token, Err(Err0565)};
    if (hint.empty())
        hint = Diagnostic::isType(left);
    diag.addRange(left, hint);
    return context->report(diag);
}

bool Semantic::resolveAffect(SemanticContext* context)
{
    auto node    = CastAst<AstOp>(context->node, AstNodeKind::AffectOp);
    auto left    = node->childs[0];
    auto right   = node->childs[1];
    auto tokenId = node->tokenId;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    YIELD();

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
        auto leftReal = TypeManager::concreteType(leftTypeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        if (leftReal->kind != TypeInfoKind::Struct)
        {
            if (leftTypeInfo->getConcreteAlias()->isEnum() || rightTypeInfo->getConcreteAlias()->isEnum())
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
                if (node->tokenId != TokenId::SymVerticalEqual &&
                    node->tokenId != TokenId::SymAmpersandEqual &&
                    node->tokenId != TokenId::SymCircumflexEqual)
                    return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
                if (!(leftTypeInfo->getConcreteAlias()->flags & TYPEINFO_ENUM_FLAGS) || !(rightTypeInfo->getConcreteAlias()->flags & TYPEINFO_ENUM_FLAGS))
                    return SemanticError::notAllowedError(context, node, leftTypeInfo, "because the enum is not marked with '#[Swag.EnumFlags]'", left);
                forEnumFlags = true;
            }
        }
    }

    rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    SWAG_VERIFY(!rightTypeInfo->isVoid(), context->report({right, Err(Err0569)}));

    // Be sure modifiers are relevant
    if (right->kind == AstNodeKind::NoDrop || right->kind == AstNodeKind::Move)
    {
        PushErrCxtStep ec(context, right, ErrCxtStepKind::Note, [rightTypeInfo]()
                          { return Diagnostic::isType(rightTypeInfo); });

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
        Diagnostic diag{node, node->token, Fmt(Err(Err0005), node->token.ctext(), leftTypeInfo->getDisplayNameC())};
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
    YIELD();

    // Cast from struct to interface : need to wait for all interfaces to be registered
    if (tokenId == TokenId::SymEqual)
    {
        if (leftTypeInfo->isInterface() && rightTypeInfo->isStruct())
        {
            Semantic::waitAllStructInterfaces(context->baseJob, rightTypeInfo);
            YIELD();
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
            return context->report({right, Fmt(Err(Err0572), Naming::aKindName(rightTypeInfo).c_str(), rightTypeInfo->getDisplayNameC())});

        if (forStruct)
        {
            if (arrayNode)
            {
                if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, CASTFLAG_CAST))
                {
                    SWAG_CHECK(waitForStructUserOps(context, left));
                    YIELD();
                }
                else if (forTuple)
                {
                    return context->report({node, node->token, Fmt(Err(Err0168), node->token.ctext())});
                }
                else
                {
                    SymbolName* symbol = nullptr;
                    SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opIndexAffect, left, &symbol));
                    if (!symbol)
                    {
                        YIELD();

                        Diagnostic diag{right, Fmt(Err(Err0225), rightTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC())};
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
                    YIELD();
                }
                else if (rightTypeInfo->isInitializerList())
                {
                    SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_UNCONST | CASTFLAG_ACCEPT_PENDING));
                    YIELD();
                }
                else
                {
                    if (leftTypeInfo->isTuple())
                    {
                        Diagnostic diag{node, node->token, Err(Err0574)};
                        diag.addRange(right, Diagnostic::isType(rightTypeInfo));
                        return context->report(diag);
                    }

                    SWAG_CHECK(resolveUserOpAffect(context, leftTypeInfo, rightTypeInfo, left, right));
                    YIELD();
                }
            }

            break;
        }

        uint32_t castFlags = CASTFLAG_AUTO_BOOL | CASTFLAG_TRY_COERCE | CASTFLAG_FOR_AFFECT | CASTFLAG_ACCEPT_PENDING;
        if (leftTypeInfo->isStruct() ||
            leftTypeInfo->isArray() ||
            leftTypeInfo->isClosure())
            castFlags |= CASTFLAG_UNCONST;

        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right, castFlags));
        YIELD();
        break;
    }

    case TokenId::SymLowerLowerEqual:
    case TokenId::SymGreaterGreaterEqual:
    {
        if (forTuple)
            return context->report({node, node->token, Fmt(Err(Err0168), node->token.ctext())});
        else if (forStruct)
        {
            const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
            break;
        }

        SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, left, right, CASTFLAG_TRY_COERCE));

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return SemanticError::notAllowedError(context, node, leftTypeInfo);
        }

        break;
    }

    case TokenId::SymAmpersandEqual:
    case TokenId::SymVerticalEqual:
    case TokenId::SymCircumflexEqual:
    {
        if (forTuple)
            return context->report({node, node->token, Fmt(Err(Err0168), node->token.ctext())});
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
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString ||
            leftTypeInfo->nativeType == NativeTypeKind::F32 ||
            leftTypeInfo->nativeType == NativeTypeKind::F64)
        {
            return SemanticError::notAllowedError(context, node, leftTypeInfo);
        }
        break;
    }

    case TokenId::SymPlusEqual:
    case TokenId::SymMinusEqual:
    {
        if (forTuple)
            return context->report({node, node->token, Fmt(Err(Err0168), node->token.ctext())});
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
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                auto note = Diagnostic::note(Nte(Nte0146));
                return context->report(diag, note);
            }

            if (leftTypeInfo->isPointerTo(NativeTypeKind::Void))
            {
                Diagnostic diag{node, node->token, Err(Err0111)};
                diag.addRange(left, Diagnostic::isType(leftTypeInfo));
                return context->report(diag);
            }

            rightTypeInfo = TypeManager::concreteType(right->typeInfo);
            SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, Fmt(Err(Err0579), rightTypeInfo->getDisplayNameC())}));
            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, left, right, CASTFLAG_TRY_COERCE));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString)
        {
            return SemanticError::notAllowedError(context, node, leftTypeInfo);
        }
        break;
    }

    case TokenId::SymSlashEqual:
    {
        if (forTuple)
            return context->report({node, node->token, Fmt(Err(Err0168), node->token.ctext())});
        else if (forStruct)
        {
            if (arrayNode)
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, "/=", nullptr, left, arrayNode->structFlatParams));
            else
                SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, "/=", nullptr, left, right));
            break;
        }

        SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString)
        {
            return SemanticError::notAllowedError(context, node, leftTypeInfo);
        }
        break;
    }

    case TokenId::SymPercentEqual:
    case TokenId::SymAsteriskEqual:
    {
        if (forTuple)
            return context->report({node, node->token, Fmt(Err(Err0168), node->token.ctext())});
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
        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CASTFLAG_TRY_COERCE));

        if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
            leftTypeInfo->nativeType == NativeTypeKind::String ||
            leftTypeInfo->nativeType == NativeTypeKind::CString)
        {
            return SemanticError::notAllowedError(context, node, leftTypeInfo);
        }
        break;
    }

    default:
        return Report::internalError(context->node, "resolveAffect, invalid token");
    }

    node->byteCodeFct = ByteCodeGen::emitAffect;
    return true;
}