#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Tokenizer/LanguageSpec.h"

bool Semantic::resolveMove(SemanticContext* context)
{
    const auto node  = context->node;
    const auto right = node->firstChild();
    SWAG_CHECK(checkIsConcrete(context, right));
    node->inheritAstFlagsOr(right, AST_NO_LEFT_DROP | AST_FORCE_MOVE | AST_NO_RIGHT_DROP);
    node->typeInfo    = right->typeInfo;
    node->byteCodeFct = ByteCodeGen::emitPassThrough;
    if (right->resolvedSymbolOverload())
        right->resolvedSymbolOverload()->flags.add(OVERLOAD_HAS_MAKE_POINTER);

    if (node->hasAstFlag(AST_FORCE_MOVE))
    {
        if ((right->typeInfo->isStruct() && right->typeInfo->isConst()) || right->typeInfo->isConstPointerRef())
        {
            Diagnostic err{right, formErr(Err0213, right->typeInfo->getDisplayNameC())};
            if (right->resolvedSymbolOverload() && right->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
                err.addNote(toNte(Nte0083));
            return context->report(err);
        }
    }

    return true;
}

// @DeduceLambdaType
// This function is called when the type has been identified, and there's a lambda waiting for it.
// This will launch the evaluation of the lambda now that we can deduce some missing types (like parameters)
bool Semantic::resolveAfterKnownType(SemanticContext* context)
{
    const auto node     = context->node;
    const auto typeInfo = TypeManager::concreteType(node->typeInfo);
    if (!typeInfo->isLambdaClosure() && !typeInfo->isStruct())
        return true;

    const auto findMpl = node->parent->findChild(AstNodeKind::MakePointerLambda);
    if (!findMpl)
        return true;
    const auto mpl = castAst<AstMakePointer>(findMpl, AstNodeKind::MakePointerLambda);
    if (!mpl->hasSpecFlag(AstMakePointer::SPEC_FLAG_DEP_TYPE))
        return true;

    // Cannot cast from closure to lambda
    if (node->typeInfo->getConcreteAlias()->isLambda() && mpl->lambda->typeInfo->getConcreteAlias()->isClosure())
    {
        Diagnostic err{mpl, toErr(Err0527)};
        err.addNote(node, Diagnostic::isType(node->typeInfo));
        err.addNote(toNte(Nte0017));
        return context->report(err);
    }

    ScopedLock lk(mpl->lambda->mutex);
    mpl->lambda->removeAstFlag(AST_SPEC_SEMANTIC2);
    launchResolveSubDecl(context, mpl->lambda);
    return true;
}

bool Semantic::checkIsConstAffect(SemanticContext* context, AstNode* left, const AstNode* right)
{
    if (left->lastChild()->hasSemFlag(SEMFLAG_CONST_ASSIGN))
    {
        if (!left->typeInfo->isPointerRef() || right->is(AstNodeKind::KeepRef))
        {
            left->addAstFlag(AST_CONST);
        }
    }

    // Check that left type is mutable
    bool isConst = false;
    if (left->hasAstFlag(AST_CONST_EXPR) ||
        left->hasAstFlag(AST_CONST) ||
        !left->hasAstFlag(AST_L_VALUE) ||
        (left->typeInfo->isConstAlias() && left->typeInfo->isConst()) ||
        (left->typeInfo->isConstPointerRef() && right->isNot(AstNodeKind::KeepRef)))
    {
        isConst = true;
    }

    if (!isConst)
    {
        if (left->resolvedSymbolOverload())
            left->resolvedSymbolOverload()->flags.add(OVERLOAD_HAS_AFFECT);
        return true;
    }

    Utf8              hint;
    const auto        node    = context->node;
    const Diagnostic* note    = nullptr;
    const auto        orgLeft = left;

    if (left->is(AstNodeKind::IdentifierRef))
    {
        // If not, try to find the culprit type
        for (uint32_t i = left->children.count - 1; i != UINT32_MAX; i--)
        {
            const auto child     = left->children[i];
            const auto typeChild = child->typeInfo->getConcreteAlias();
            if (!typeChild)
                continue;

            if (child->is(AstNodeKind::ArrayPointerIndex))
            {
                const auto arr = castAst<AstArrayPointerIndex>(child, AstNodeKind::ArrayPointerIndex);
                if (arr->array->typeInfo->isString())
                {
                    left = arr->array;
                    break;
                }
            }

            if (child->hasAstFlag(AST_FUNC_CALL | AST_FUNC_INLINE_CALL) && (child->typeInfo->isConst() || child->typeInfo->isStruct()))
            {
                left = child;
                hint = formNte(Nte0122, left->typeInfo->getDisplayNameC());
                break;
            }

            if (typeChild->isConst())
            {
                left = child;
                break;
            }
        }

        if (left->is(AstNodeKind::Identifier) && left->hasSpecFlag(AstIdentifier::SPEC_FLAG_FROM_USING | AstIdentifier::SPEC_FLAG_FROM_WITH))
        {
            const auto leftId = castAst<AstIdentifier>(left, AstNodeKind::Identifier);
            hint              = "this is equivalent to [[";
            for (uint32_t ic = 0; ic < orgLeft->childCount(); ic++)
            {
                const auto c = orgLeft->children[ic];
                if (ic)
                    hint += ".";
                hint += c->token.text;
            }

            if (left->hasSpecFlag(AstIdentifier::SPEC_FLAG_FROM_USING))
                hint += "]] because of a [[using]]";
            else
                hint += "]] because of a [[with]]";

            SWAG_ASSERT(left->resolvedSymbolOverload());
            if (left->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_FUNC_PARAM) && left->typeInfo->isConst())
                note = Diagnostic::note(leftId->identifierExtension->fromAlternateVar, toNte(Nte0198));
            else if (!left->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
                note = Diagnostic::note(leftId->identifierExtension->fromAlternateVar, toNte(Nte0198), Diagnostic::isType(left->typeInfo));
        }
    }

    if (left->typeInfo->isConst() && left->resolvedSymbolOverload() && left->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_FUNC_PARAM))
    {
        if (left == left->parent->lastChild())
        {
            Diagnostic err{node, node->token, formErr(Err0091, left->resolvedSymbolName()->name.cstr())};
            err.addNote(left, Diagnostic::isType(left->typeInfo));
            err.addNote(note);
            err.addNote(toNte(Nte0083));
            return context->report(err);
        }

        if (left->typeInfo->isConstPointerRef())
        {
            Diagnostic err{node, node->token, formErr(Err0091, left->resolvedSymbolName()->name.cstr())};
            err.addNote(left, Diagnostic::isType(left->typeInfo));
            err.addNote(note);
            return context->report(err);
        }

        Diagnostic err{node, node->token, formErr(Err0091, left->resolvedSymbolName()->name.cstr())};
        err.addNote(left, Diagnostic::isType(left->typeInfo));
        err.addNote(note);
        return context->report(err);
    }

    if (orgLeft == left &&
        left->resolvedSymbolOverload() &&
        left->resolvedSymbolOverload()->hasFlag(OVERLOAD_VAR_IS_LET) &&
        (!left->resolvedSymbolOverload()->typeInfo->isPointerRef() || right->is(AstNodeKind::KeepRef)))
    {
        Diagnostic err{node, node->token, toErr(Err0093)};
        err.addNote(left, toNte(Nte0013));
        err.addNote(Diagnostic::hereIs(left->resolvedSymbolOverload()->node));
        return context->report(err);
    }

    if (left->hasAstFlag(AST_L_VALUE))
    {
        Diagnostic err{node, node->token, toErr(Err0093)};
        if (hint.empty())
            hint = Diagnostic::isType(left);
        err.addNote(left, hint);
        err.addNote(note);
        return context->report(err);
    }

    if (left->resolvedSymbolOverload() && left->resolvedSymbolOverload()->hasFlag(OVERLOAD_CONST_VALUE))
    {
        Diagnostic err{node, node->token, toErr(Err0093)};
        if (hint.empty())
            hint = toNte(Nte0173);
        err.addNote(left, hint);
        return context->report(err);
    }

    Diagnostic err{node, node->token, toErr(Err0737)};
    if (hint.empty())
        hint = Diagnostic::isType(left);
    err.addNote(left, hint);
    return context->report(err);
}

bool Semantic::resolveAffect(SemanticContext* context)
{
    auto       node    = castAst<AstOp>(context->node, AstNodeKind::AffectOp);
    const auto left    = node->firstChild();
    auto       right   = node->secondChild();
    const auto tokenId = node->token.id;

    SWAG_CHECK(checkIsConcrete(context, left));
    SWAG_CHECK(checkIsConcreteOrType(context, right));
    YIELD();

    SWAG_CHECK(checkIsConstAffect(context, left, right));

    // @ConcreteRef
    TypeInfo* leftTypeInfo  = TypeManager::concreteType(left->typeInfo, CONCRETE_ALIAS);
    TypeInfo* rightTypeInfo = getConcreteTypeUnRef(right, CONCRETE_FUNC | CONCRETE_ALIAS);
    if (right->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = getConcreteTypeUnRef(left, CONCRETE_ALIAS);
    else if (leftTypeInfo->isPointerRef() && !rightTypeInfo->isPointerRef())
        leftTypeInfo = getConcreteTypeUnRef(left, CONCRETE_ALIAS);
    SWAG_ASSERT(leftTypeInfo);
    SWAG_ASSERT(rightTypeInfo);

    // Special case for enum: nothing is possible, except for flags
    bool forEnumFlags = false;
    if (node->token.isNot(TokenId::SymEqual))
    {
        const auto leftReal = TypeManager::concreteType(leftTypeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
        if (leftReal->kind != TypeInfoKind::Struct)
        {
            if (leftTypeInfo->getConcreteAlias()->isEnum() || rightTypeInfo->getConcreteAlias()->isEnum())
            {
                SWAG_CHECK(TypeManager::makeCompatibles(context, left, right));
                if (node->token.isNot(TokenId::SymVerticalEqual) &&
                    node->token.isNot(TokenId::SymAmpersandEqual) &&
                    node->token.isNot(TokenId::SymCircumflexEqual))
                    return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
                if (!leftTypeInfo->getConcreteAlias()->hasFlag(TYPEINFO_ENUM_FLAGS) || !rightTypeInfo->getConcreteAlias()->hasFlag(TYPEINFO_ENUM_FLAGS))
                    return SemanticError::notAllowedError(context, node, leftTypeInfo, "because the enum is not marked with [[#[Swag.EnumFlags]]]", left);
                forEnumFlags = true;
            }
        }
    }

    rightTypeInfo = TypeManager::concretePtrRefType(right->typeInfo);

    // Be sure modifiers are relevant
    if (right->is(AstNodeKind::NoDrop) || right->is(AstNodeKind::Move))
    {
        PushErrCxtStep ec(context, right, ErrCxtStepKind::Note, [rightTypeInfo] { return Diagnostic::isType(rightTypeInfo); });

        const auto leftConcrete = TypeManager::concreteType(leftTypeInfo);
        if (right->hasAstFlag(AST_NO_LEFT_DROP))
            SWAG_VERIFY(leftConcrete->isSame(rightTypeInfo, CAST_FLAG_CAST),
                    context->report({node, node->token, formErr(Err0535, leftConcrete->getDisplayNameC(), g_LangSpec->name_sharp_nodrop.cstr(), rightTypeInfo->getDisplayNameC())}));
        if (right->hasAstFlag(AST_NO_RIGHT_DROP))
            SWAG_VERIFY(leftConcrete->isSame(rightTypeInfo, CAST_FLAG_CAST),
                    context->report({node, node->token, formErr(Err0535, leftConcrete->getDisplayNameC(), g_LangSpec->name_sharp_moveraw.cstr(), rightTypeInfo->getDisplayNameC())}));
        if (right->hasAstFlag(AST_FORCE_MOVE))
            SWAG_VERIFY(leftConcrete->isSame(rightTypeInfo, CAST_FLAG_CAST),
                    context->report({node, node->token, formErr(Err0535, leftConcrete->getDisplayNameC(), g_LangSpec->name_sharp_move.cstr(), rightTypeInfo->getDisplayNameC())}));
    }

    // No direct operations on any, except affect any to any
    if (leftTypeInfo->isAny() && node->token.isNot(TokenId::SymEqual))
    {
        Diagnostic err{node, node->token, formErr(Err0596, node->token.cstr(), leftTypeInfo->getDisplayNameC())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    // Is this an array like affectation?
    AstArrayPointerIndex* arrayNode = nullptr;
    if (left->is(AstNodeKind::IdentifierRef) && left->lastChild()->is(AstNodeKind::ArrayPointerIndex))
    {
        arrayNode            = castAst<AstArrayPointerIndex>(left->lastChild(), AstNodeKind::ArrayPointerIndex);
        const auto arrayType = TypeManager::concretePtrRefType(arrayNode->array->typeInfo);
        if (!arrayType->isStruct())
            arrayNode = nullptr;

        // Add self and value in list of parameters
        if (arrayNode)
        {
            if (!node->hasSemFlag(SEMFLAG_FLAT_PARAMS))
            {
                auto leftNode = arrayNode;
                while (leftNode->array->is(AstNodeKind::ArrayPointerIndex))
                    leftNode = castAst<AstArrayPointerIndex>(leftNode->array, AstNodeKind::ArrayPointerIndex);
                arrayNode->structFlatParams.push_back(right);
                arrayNode->structFlatParams.push_front(leftNode->array);
                node->addSemFlag(SEMFLAG_FLAT_PARAMS);
            }
        }
    }

    node->typeInfo = g_TypeMgr->typeInfoBool;

    const bool forStruct = leftTypeInfo->isStruct();
    const bool forTuple  = leftTypeInfo->isTuple();

    SWAG_CHECK(evaluateConstExpression(context, right));
    YIELD();

    // Cast from struct to interface: need to wait for all interfaces to be registered
    if (tokenId == TokenId::SymEqual)
    {
        if (leftTypeInfo->isInterface() && rightTypeInfo->isStruct())
        {
            waitAllStructInterfaces(context->baseJob, rightTypeInfo);
            YIELD();
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CAST_FLAG_UN_CONST));
        }
    }

    // For tuples, we can only affect
    else if (forTuple)
    {
        Diagnostic err{node, node->token, formErr(Err0599, node->token.cstr())};
        err.addNote(left, Diagnostic::isType(leftTypeInfo));
        return context->report(err);
    }

    switch (tokenId)
    {
        case TokenId::SymEqual:
        {
            if (forStruct)
            {
                if (arrayNode)
                {
                    if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, CAST_FLAG_CAST))
                    {
                        SWAG_CHECK(waitForStructUserOps(context, left));
                        YIELD();
                    }
                    else if (forTuple)
                    {
                        return context->report({node, node->token, formErr(Err0599, node->token.cstr())});
                    }
                    else
                    {
                        SymbolName* symbol = nullptr;
                        SWAG_CHECK(hasUserOp(context, g_LangSpec->name_opIndexAffect, left, &symbol));
                        if (!symbol)
                        {
                            YIELD();

                            Diagnostic err{right, formErr(Err0239, rightTypeInfo->getDisplayNameC(), leftTypeInfo->getDisplayNameC())};
                            err.hint = Diagnostic::isType(rightTypeInfo);
                            err.addNote(left, Diagnostic::isType(leftTypeInfo));
                            err.addNote(node, node->token, formNte(Nte0154, "opIndexAffect", rightTypeInfo->getDisplayNameC()));
                            return context->report(err);
                        }

                        SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAffect, nullptr, nullptr, left, arrayNode->structFlatParams));
                    }
                }
                else
                {
                    if (leftTypeInfo->kind == rightTypeInfo->kind && rightTypeInfo->isSame(leftTypeInfo, CAST_FLAG_CAST | CAST_FLAG_FOR_AFFECT))
                    {
                        SWAG_CHECK(waitForStructUserOps(context, left));
                        YIELD();
                    }
                    else if (rightTypeInfo->isInitializerList())
                    {
                        SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CAST_FLAG_UN_CONST | CAST_FLAG_ACCEPT_PENDING));
                        YIELD();
                    }
                    else
                    {
                        if (leftTypeInfo->isTuple())
                        {
                            Diagnostic err{node, node->token, toErr(Err0569)};
                            err.addNote(left, Diagnostic::isType(leftTypeInfo));
                            err.addNote(right, Diagnostic::isType(rightTypeInfo));
                            return context->report(err);
                        }

                        SWAG_CHECK(resolveUserOpAffect(context, leftTypeInfo, rightTypeInfo, left, right));
                        YIELD();
                    }
                }

                break;
            }

            CastFlags castFlags = CAST_FLAG_AUTO_BOOL | CAST_FLAG_TRY_COERCE | CAST_FLAG_FOR_AFFECT | CAST_FLAG_ACCEPT_PENDING;
            if (leftTypeInfo->isStruct() ||
                leftTypeInfo->isArray() ||
                leftTypeInfo->isClosure())
                castFlags.add(CAST_FLAG_UN_CONST);

            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, nullptr, right, castFlags));
            YIELD();
            break;
        }

        case TokenId::SymLowerLowerEqual:
        case TokenId::SymGreaterGreaterEqual:
        {
            if (forStruct)
            {
                const char* op = tokenId == TokenId::SymLowerLowerEqual ? "<<=" : ">>=";
                if (arrayNode)
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
                else
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
                break;
            }

            SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU32, left, right, CAST_FLAG_TRY_COERCE));

            SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
            if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
                leftTypeInfo->nativeType == NativeTypeKind::String ||
                leftTypeInfo->nativeType == NativeTypeKind::CString ||
                leftTypeInfo->nativeType == NativeTypeKind::F32 ||
                leftTypeInfo->nativeType == NativeTypeKind::F64)
            {
                return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
            }

            break;
        }

        case TokenId::SymAmpersandEqual:
        case TokenId::SymVerticalEqual:
        case TokenId::SymCircumflexEqual:
        {
            if (forStruct)
            {
                auto op = "&=";
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
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CAST_FLAG_TRY_COERCE));

            if (leftTypeInfo->nativeType == NativeTypeKind::String ||
                leftTypeInfo->nativeType == NativeTypeKind::CString ||
                leftTypeInfo->nativeType == NativeTypeKind::F32 ||
                leftTypeInfo->nativeType == NativeTypeKind::F64)
            {
                return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
            }
            break;
        }

        case TokenId::SymPlusEqual:
        case TokenId::SymMinusEqual:
        {
            if (forStruct)
            {
                const char* op = tokenId == TokenId::SymPlusEqual ? "+=" : "-=";
                if (arrayNode)
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
                else
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
                break;
            }

            // @PointerArithmetic
            if (leftTypeInfo->isPointer())
            {
                if (!leftTypeInfo->isPointerArithmetic())
                {
                    Diagnostic err{node, node->token, toErr(Err0225)};
                    err.addNote(left, Diagnostic::isType(leftTypeInfo));
                    err.addNote(toNte(Nte0101));
                    return context->report(err);
                }

                if (leftTypeInfo->isPointerTo(NativeTypeKind::Void))
                {
                    Diagnostic err{node, node->token, toErr(Err0226)};
                    err.addNote(left, Diagnostic::isType(leftTypeInfo));
                    return context->report(err);
                }

                rightTypeInfo = TypeManager::concreteType(right->typeInfo);
                SWAG_VERIFY(rightTypeInfo->isNativeInteger(), context->report({right, formErr(Err0227, rightTypeInfo->getDisplayNameC())}));
                SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoU64, left, right, CAST_FLAG_TRY_COERCE));
                break;
            }

            SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CAST_FLAG_TRY_COERCE));

            if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
                leftTypeInfo->nativeType == NativeTypeKind::String ||
                leftTypeInfo->nativeType == NativeTypeKind::CString)
            {
                return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
            }
            break;
        }

        case TokenId::SymSlashEqual:
        {
            if (forStruct)
            {
                if (arrayNode)
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, "/=", nullptr, left, arrayNode->structFlatParams));
                else
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, "/=", nullptr, left, right));
                break;
            }

            SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CAST_FLAG_TRY_COERCE));

            if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
                leftTypeInfo->nativeType == NativeTypeKind::String ||
                leftTypeInfo->nativeType == NativeTypeKind::CString)
            {
                return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
            }
            break;
        }

        case TokenId::SymPercentEqual:
        case TokenId::SymAsteriskEqual:
        {
            if (forStruct)
            {
                const char* op = tokenId == TokenId::SymPercentEqual ? "%=" : "*=";
                if (arrayNode)
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opIndexAssign, op, nullptr, left, arrayNode->structFlatParams));
                else
                    SWAG_CHECK(resolveUserOp(context, g_LangSpec->name_opAssign, op, nullptr, left, right));
                break;
            }

            SWAG_CHECK(checkTypeIsNative(context, leftTypeInfo, rightTypeInfo, left, right));
            SWAG_CHECK(TypeManager::makeCompatibles(context, leftTypeInfo, left, right, CAST_FLAG_TRY_COERCE));

            if (leftTypeInfo->nativeType == NativeTypeKind::Bool ||
                leftTypeInfo->nativeType == NativeTypeKind::String ||
                leftTypeInfo->nativeType == NativeTypeKind::CString)
            {
                return SemanticError::notAllowedError(context, node, leftTypeInfo, nullptr, left);
            }
            break;
        }

        default:
            return Report::internalError(context->node, "resolveAffect, invalid token");
    }

    node->byteCodeFct = ByteCodeGen::emitAffect;
    return true;
}
