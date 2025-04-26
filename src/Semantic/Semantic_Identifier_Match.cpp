#include "pch.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Error/SemanticError.h"
#include "Semantic/SemanticJob.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Syntax/Naming.h"
#include "Syntax/Tokenizer/LanguageSpec.h"
#include "Wmf/Module.h"

void Semantic::sortParameters(AstNode* allParams)
{
    ScopedLock lk(allParams->mutex);

    if (!allParams || !allParams->hasAstFlag(AST_MUST_SORT_CHILDREN))
        return;
    if (allParams->childCount() <= 1)
        return;

    std::ranges::sort(allParams->children, [](AstNode* n1, AstNode* n2) {
        const auto p1 = castAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
        const auto p2 = castAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
        return p1->indexParam < p2->indexParam;
    });

    allParams->removeAstFlag(AST_MUST_SORT_CHILDREN);
}

void Semantic::dealWithIntrinsic(const SemanticContext* context, AstIdentifier* identifier)
{
    const auto module = context->sourceFile->module;

    switch (identifier->token.id)
    {
        case TokenId::IntrinsicAssert:
        {
            if (module->mustOptimizeSemantic(context->node))
            {
                SWAG_ASSERT(identifier->callParameters && !identifier->callParameters->children.empty());
                const auto param = identifier->callParameters->firstChild();
                if (param->hasFlagComputedValue() && param->computedValue()->reg.b)
                    identifier->addAstFlag(AST_NO_BYTECODE);
            }
            break;
        }
    }
}

void Semantic::resolvePendingLambdaTyping(const SemanticContext* context, AstNode* funcNode, const TypeInfo* resolvedType)
{
    const auto funcDecl         = castAst<AstFuncDecl>(funcNode, AstNodeKind::FuncDecl);
    const auto typeUndefinedFct = castTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    const auto concreteType     = TypeManager::concreteType(resolvedType);
    const auto typeDefinedFct   = castTypeInfo<TypeInfoFuncAttr>(concreteType, TypeInfoKind::LambdaClosure);

    SWAG_ASSERT(!funcDecl->hasAstFlag(AST_GENERIC));

    // Replace every parameter's types
    for (uint32_t paramIdx = 0; paramIdx < typeUndefinedFct->parameters.size(); paramIdx++)
    {
        const auto childType = funcDecl->parameters->children[paramIdx];

        TypeInfo* definedType;
        if (typeDefinedFct->isClosure() && !typeUndefinedFct->isClosure())
            definedType = typeDefinedFct->parameters[paramIdx + 1]->typeInfo;
        else
            definedType = typeDefinedFct->parameters[paramIdx]->typeInfo;

        childType->typeInfo                              = definedType;
        childType->resolvedSymbolOverload()->typeInfo    = definedType;
        typeUndefinedFct->parameters[paramIdx]->typeInfo = definedType;
    }

    // Replace generic parameters, if any
    for (const auto undefinedType : typeUndefinedFct->genericParameters)
    {
        auto it = typeDefinedFct->replaceTypes.find(undefinedType->name);
        if (it != typeDefinedFct->replaceTypes.end())
        {
            undefinedType->name     = it->second.typeInfoReplace->name;
            undefinedType->typeInfo = it->second.typeInfoReplace;
        }
    }

    // Replace every types inside the function
    Ast::visit(funcDecl, [&](AstNode* p) {
        const auto it = typeDefinedFct->replaceTypes.find(p->token.text);
        if (it == typeDefinedFct->replaceTypes.end())
            return;
        p->token.text             = it->second.typeInfoReplace->name;
        const auto symbolOverload = p->resolvedSymbolOverload();
        if (symbolOverload)
            symbolOverload->typeInfo = it->second.typeInfoReplace;
        p->typeInfo = it->second.typeInfoReplace;
    });

    // Set return type
    if (typeUndefinedFct->returnType->isUndefined())
    {
        typeUndefinedFct->returnType = typeDefinedFct->returnType;
        if (funcDecl->returnType)
            funcDecl->returnType->typeInfo = typeDefinedFct->returnType;
    }

    typeUndefinedFct->forceComputeName();

    // Wake up semantic lambda job
    SWAG_ASSERT(funcDecl->pendingLambdaJob);
    SWAG_ASSERT(context->node->is(AstNodeKind::Identifier));

    context->node->addSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING);
    funcDecl->removeSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING);

    ScopedLock lk(funcDecl->resolvedSymbolOverload()->symbol->mutex);
    if (typeUndefinedFct->returnType->isGeneric())
        funcDecl->resolvedSymbolOverload()->flags.add(OVERLOAD_INCOMPLETE);
    waitSymbolNoLock(context->baseJob, funcDecl->resolvedSymbolOverload()->symbol);
    context->baseJob->jobsToAdd.push_back(funcDecl->pendingLambdaJob);
}

bool Semantic::setSymbolMatchCallParams(SemanticContext* context, const OneMatch& oneMatch, AstIdentifier* identifier)
{
    if (!identifier->callParameters)
        return true;

    const auto typeInfoFunc = castTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);

    // :ClosureForceFirstParam
    // Add a first dummy parameter in case of closure
    if (typeInfoFunc->isClosure() && !identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_CLOSURE_FIRST_PARAM))
    {
        const auto fcp = Ast::newFuncCallParam(nullptr, identifier->callParameters);
        Ast::removeFromParent(fcp);
        Ast::addChildFront(identifier->callParameters, fcp);
        fcp->setFlagsValueIsComputed();
        fcp->computedValue()->reg.pointer = nullptr;
        fcp->typeInfo                     = g_TypeMgr->typeInfoNull;
        fcp->addAstFlag(AST_GENERATED);
        identifier->addSpecFlag(AstIdentifier::SPEC_FLAG_CLOSURE_FIRST_PARAM);

        const auto node = Ast::newNode<AstLiteral>(AstNodeKind::Literal, nullptr, fcp);
        node->setFlagsValueIsComputed();
        node->addAstFlag(AST_GENERATED);
        node->typeInfo = g_TypeMgr->typeInfoNull;
    }

    sortParameters(identifier->callParameters);
    const auto maxParams = identifier->callParameters->childCount();
    for (uint32_t idx = 0; idx < maxParams; idx++)
    {
        const auto nodeCall = castAst<AstFuncCallParam>(identifier->callParameters->children[idx], AstNodeKind::FuncCallParam);

        size_t i = idx;
        if (idx < typeInfoFunc->parameters.size() - 1 || !typeInfoFunc->hasFlag(TYPEINFO_VARIADIC | TYPEINFO_C_VARIADIC))
            i = nodeCall->indexParam;

        // This is a lambda that was waiting for a match to have its types, and to continue solving its content
        if (nodeCall->typeInfo->isLambdaClosure() && nodeCall->typeInfo->declNode->hasSemFlag(SEMFLAG_PENDING_LAMBDA_TYPING))
        {
            resolvePendingLambdaTyping(context, nodeCall->typeInfo->declNode, oneMatch.solvedParameters[i]->typeInfo);
            YIELD();
        }

        CastFlags castFlags = CAST_FLAG_AUTO_OP_CAST | CAST_FLAG_ACCEPT_PENDING | CAST_FLAG_PARAMS | CAST_FLAG_PTR_REF | CAST_FLAG_FOR_AFFECT | CAST_FLAG_ACCEPT_MOVE_REF;
        if (i == 0 && oneMatch.ufcs)
            castFlags.add(CAST_FLAG_UFCS);
        if (oneMatch.oneOverload && !oneMatch.oneOverload->overload->node->hasAttribute(ATTRIBUTE_OVERLOAD))
            castFlags.add(CAST_FLAG_TRY_COERCE);

        TypeInfo* toType = nullptr;
        if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
        {
            {
                const auto     declParam = oneMatch.solvedParameters[i]->declNode;
                PushErrCxtStep ec(context, declParam, ErrCxtStepKind::Note, [declParam] { return formNte(Nte0182, declParam->token.cstr()); });
                context->castFlagsResult = 0;

                toType = oneMatch.solvedParameters[i]->typeInfo;
                SWAG_CHECK(TypeManager::makeCompatibles(context, toType, nullptr, nodeCall, castFlags));
                YIELD();
            }

            // The UFCS parameter is a value, and we want a reference.
            // Mark the symbol with OVERLOAD_HAS_MAKE_POINTER because it will avoid warning of non usage.
            if (context->castFlagsResult.has(CAST_RESULT_FORCE_REF))
            {
                const auto idRef    = identifier->identifierRef();
                const auto id       = idRef->children[idRef->childCount() - 2];
                const auto overload = id->resolvedSymbolOverload();
                if (overload)
                {
                    const auto     declParam = oneMatch.solvedParameters[i]->declNode;
                    PushErrCxtStep ec0(context, declParam, ErrCxtStepKind::Note, [id, declParam] { return formNte(Nte0096, id->token.cstr(), declParam->typeInfo->getDisplayNameC()); });
                    SWAG_CHECK(checkCanTakeAddress(context, id));
                    overload->flags.add(OVERLOAD_HAS_MAKE_POINTER);
                }
            }

            const auto typeCall = TypeManager::concreteType(nodeCall->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
            if (!toType->isPointerRef() && typeCall->isPointerRef())
            {
                setUnRef(nodeCall);
            }
            else if (oneMatch.solvedCastFlags[i].has(CAST_RESULT_FROM_REF))
            {
                setUnRef(nodeCall);
                if (nodeCall->typeInfoCast)
                {
                    const auto fromCast = TypeManager::concreteType(nodeCall->typeInfoCast, CONCRETE_FUNC | CONCRETE_ALIAS);
                    if (fromCast->isPointerRef())
                        nodeCall->typeInfoCast = fromCast->getFinalType();
                }
            }
            else if (toType->isConstPointerRef() &&
                     !nodeCall->typeInfo->isPointer() &&
                     !nodeCall->typeInfo->isStruct() &&
                     !nodeCall->typeInfo->isListTuple())
            {
                const auto front = nodeCall->firstChild();

                // We have a compile-time value (like a literal), and we want a const ref, i.e. a pointer
                // We need to create a temporary variable to store the value, in order to have an address.
                if (front->hasFlagComputedValue() || nodeCall->typeInfo->isListArray())
                {
                    const auto varNode = Ast::newVarDecl(form(R"(__7tmp_%d)", g_UniqueID.fetch_add(1)), nullptr, nodeCall);
                    varNode->inheritTokenLocation(nodeCall->token);
                    varNode->addAstFlag(AST_GENERATED);
                    Ast::removeFromParent(front);
                    Ast::addChildBack(varNode, front);
                    varNode->assignment = front;

                    const auto toPtr        = castTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
                    varNode->type           = Ast::newIdentifierRef("dummy", nullptr, varNode);
                    varNode->type->typeInfo = toPtr->pointedType;
                    varNode->type->addAstFlag(AST_NO_SEMANTIC);
                    nodeCall->typeInfo = toPtr->pointedType;

                    const auto idNode = Ast::newIdentifierRef(varNode->token.text, nullptr, nodeCall);
                    idNode->inheritTokenLocation(nodeCall->token);
                    idNode->addAstFlag(AST_GENERATED);
                    Ast::removeFromParent(idNode);
                    Ast::addChildFront(nodeCall, idNode);

                    context->baseJob->nodes.push_back(idNode);
                    context->baseJob->nodes.push_back(varNode);
                    nodeCall->removeAstFlag(AST_COMPUTED_VALUE);

                    context->result = ContextResult::NewChildren;
                    return true;
                }

                // We have a value, and we need a reference.
                // Force to keep the address
                if (front->is(AstNodeKind::IdentifierRef))
                {
                    front->lastChild()->addSemFlag(SEMFLAG_FORCE_TAKE_ADDRESS);
                }
                else
                    return Report::internalError(nodeCall, "cannot deal with value to pointer ref conversion");
            }
            else if (context->castFlagsResult.has(CAST_RESULT_FORCE_REF))
            {
                const auto front = nodeCall->firstChild();

                // We have a value, and we need a reference.
                // Force to keep the address
                if (front->is(AstNodeKind::IdentifierRef))
                {
                    front->lastChild()->addSemFlag(SEMFLAG_FORCE_TAKE_ADDRESS);
                }
            }
        }
        else if (!oneMatch.solvedParameters.empty() && oneMatch.solvedParameters.back() && oneMatch.solvedParameters.back()->typeInfo->isTypedVariadic())
        {
            toType = oneMatch.solvedParameters.back()->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters.back()->typeInfo, nullptr, nodeCall));
            YIELD();
        }

        // If passing a closure
        // :FctCallParamClosure
        const auto toTypeRef = TypeManager::concreteType(toType, CONCRETE_FORCE_ALIAS);
        auto       makePtrL  = nodeCall->children.empty() ? nullptr : nodeCall->firstChild();

        if (makePtrL && toTypeRef && toTypeRef->isClosure())
        {
            bool convert = false;
            if (makePtrL->is(AstNodeKind::MakePointer) || makePtrL->is(AstNodeKind::MakePointerLambda))
                convert = true;
            if (makePtrL->typeInfo && makePtrL->typeInfo->isLambda())
                convert = true;
            if (makePtrL->typeInfo->isPointerNull())
                convert = true;
            if (convert)
            {
                const auto varNode = Ast::newVarDecl(form(R"(__ctmp_%d)", g_UniqueID.fetch_add(1)), nullptr, identifier);
                varNode->addAstFlag(AST_GENERATED);

                // Put child front, because emitCall wants the parameters to be the last
                Ast::removeFromParent(varNode);
                Ast::addChildFront(identifier, varNode);

                if (makePtrL->typeInfo->isLambda())
                {
                    varNode->assignment = Ast::clone(makePtrL, varNode);
                    Ast::removeFromParent(makePtrL);
                    varNode->allocateExtension(ExtensionKind::Owner);
                    varNode->extOwner()->nodesToFree.push_back(makePtrL);
                }
                else if (makePtrL->typeInfo->isPointerNull())
                {
                    nodeCall->removeAstFlag(AST_COMPUTED_VALUE);
                    makePtrL->addAstFlag(AST_NO_BYTECODE);
                    Ast::removeFromParent(makePtrL);
                    varNode->allocateExtension(ExtensionKind::Owner);
                    varNode->extOwner()->nodesToFree.push_back(makePtrL);
                }
                else
                {
                    makePtrL->addSemFlag(SEMFLAG_ONCE);
                    Ast::removeFromParent(makePtrL);
                    Ast::addChildBack(varNode, makePtrL);
                    varNode->assignment = makePtrL;
                }

                varNode->type           = Ast::newTypeExpression(nullptr, varNode);
                varNode->type->typeInfo = toType;
                varNode->type->addAstFlag(AST_NO_SEMANTIC);

                const auto idRef = Ast::newIdentifierRef(varNode->token.text, nullptr, nodeCall);
                idRef->addExtraPointer(ExtraPointerKind::ExportNode, makePtrL);

                // Add the 2 nodes to the semantic
                context->baseJob->nodes.push_back(idRef);
                context->baseJob->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, and the new variable, which is a child,
                // will be reevaluated too, so twice because of the push above. So we set a special flag to not reevaluate
                // it twice.
                varNode->addSemFlag(SEMFLAG_ONCE);

                context->result = ContextResult::NewChildren;
            }
        }
    }

    // Deal with opAffect automatic conversion
    // :opAffectParam
    for (uint32_t i = 0; i < maxParams; i++)
    {
        const auto nodeCall = castAst<AstFuncCallParam>(identifier->callParameters->children[i], AstNodeKind::FuncCallParam);
        const auto userOp   = nodeCall->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp);
        if (userOp)
        {
            if (userOp->symbol->name == g_LangSpec->name_opAffect || userOp->symbol->name == g_LangSpec->name_opAffectLiteral)
            {
                SWAG_ASSERT(nodeCall->typeInfoCast);
                nodeCall->addExtraPointer(ExtraPointerKind::UserOp, nullptr);
                nodeCall->typeInfoCast = nullptr;

                const auto varNode = Ast::newVarDecl(form(R"(__10tmp_%d)", g_UniqueID.fetch_add(1)), nullptr, identifier);
                varNode->inheritTokenLocation(nodeCall->token);
                varNode->addAstFlag(AST_GENERATED);

                // Put child front, because emitCall wants the parameters to be the last
                Ast::removeFromParent(varNode);
                Ast::addChildFront(identifier, varNode);

                const auto typeExpr = Ast::newTypeExpression(nullptr, varNode);
                typeExpr->typeInfo  = nodeCall->typeInfo;
                typeExpr->addAstFlag(AST_NO_SEMANTIC);
                varNode->type = typeExpr;

                auto assign = nodeCall->firstChild();
                if (assign->is(AstNodeKind::Cast))
                    assign = assign->lastChild();

                CloneContext cloneContext;
                cloneContext.parent      = varNode;
                cloneContext.parentScope = identifier->ownerScope;
                varNode->assignment      = assign->clone(cloneContext);

                Ast::removeFromParent(nodeCall);

                const auto newParam  = Ast::newFuncCallParam(nullptr, identifier->callParameters);
                newParam->indexParam = nodeCall->indexParam;
                Ast::removeFromParent(newParam);
                Ast::insertChild(identifier->callParameters, newParam, i);

                // If the match is against a 'moveref', then we should have a 'moveref' node and a make pointer.
                if (typeInfoFunc->parameters[nodeCall->indexParam]->typeInfo->isPointerMoveRef())
                {
                    const auto moveRefNode = Ast::newNode<AstNode>(AstNodeKind::MoveRef, nullptr, newParam);
                    moveRefNode->addAstFlag(AST_GENERATED);
                    moveRefNode->semanticFct = resolveMoveRef;
                    const auto mkPtrNode     = Ast::newNode<AstMakePointer>(AstNodeKind::MakePointer, nullptr, moveRefNode);
                    mkPtrNode->addAstFlag(AST_GENERATED);
                    mkPtrNode->semanticFct = resolveMakePointer;
                    (void) Ast::newIdentifierRef(varNode->token.text, nullptr, mkPtrNode);
                }
                else
                {
                    (void) Ast::newIdentifierRef(varNode->token.text, nullptr, newParam);
                }

                // We want to export the original parameter, not the temporary variable reference
                newParam->addExtraPointer(ExtraPointerKind::ExportNode, nodeCall);
                newParam->allocateExtension(ExtensionKind::Owner);
                newParam->extOwner()->nodesToFree.push_back(nodeCall);

                // Add the 2 nodes to the semantic
                context->baseJob->nodes.push_back(newParam);
                context->baseJob->nodes.push_back(varNode);

                // If call is inline, then the identifier will be reevaluated, so the new variable, which is a child of
                // that identifier, will be reevaluated too (so twice because of the push above).
                // So we set a special flag to not reevaluate it twice.
                varNode->addSemFlag(SEMFLAG_ONCE);

                context->result = ContextResult::NewChildren;
            }
        }
    }

    // Deal with default values for structs and uncomputed values
    // We need to add a temporary variable initialized with the default value, and reference
    // that temporary variable as a new function call parameter
    if (!typeInfoFunc->parameters.empty() && maxParams < typeInfoFunc->parameters.size())
    {
        AstNode* parameters = nullptr;
        if (typeInfoFunc->declNode->is(AstNodeKind::FuncDecl))
        {
            const auto funcDecl = castAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl);
            parameters          = funcDecl->parameters;
        }
        else
        {
            const auto funcDecl = castAst<AstTypeExpression>(typeInfoFunc->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
            parameters          = funcDecl->firstChild();
        }

        for (uint32_t i = 0; i < parameters->childCount(); i++)
        {
            if (parameters->children[i]->isNot(AstNodeKind::FuncDeclParam))
                continue;
            const auto funcParam = castAst<AstVarDecl>(parameters->children[i], AstNodeKind::FuncDeclParam);
            if (!funcParam->assignment)
                continue;
            switch (funcParam->assignment->token.id)
            {
                case TokenId::CompilerCallerLocation:
                case TokenId::CompilerCallerFunction:
                case TokenId::CompilerBuildCfg:
                case TokenId::CompilerOs:
                case TokenId::CompilerArch:
                case TokenId::CompilerCpu:
                case TokenId::CompilerSwagOs:
                case TokenId::CompilerBackend:
                    continue;
                default:
                    break;
            }

            const auto typeParam = TypeManager::concretePtrRefType(funcParam->typeInfo);
            if (!typeParam->isStruct() &&
                !typeParam->isListTuple() &&
                !typeParam->isListArray() &&
                !typeParam->isAny() &&
                !typeParam->isClosure() &&
                funcParam->assignment->hasFlagComputedValue())
                continue;

            bool covered = false;
            for (uint32_t j = 0; j < maxParams; j++)
            {
                const auto nodeCall = castAst<AstFuncCallParam>(identifier->callParameters->children[j], AstNodeKind::FuncCallParam);
                if (nodeCall->resolvedParameter && nodeCall->resolvedParameter->index == i)
                {
                    covered = true;
                    break;
                }
            }

            if (!covered)
            {
                const auto varNode = Ast::newVarDecl(form(R"(__3tmp_%d)", g_UniqueID.fetch_add(1)), nullptr, identifier);
                varNode->addAstFlag(AST_GENERATED);
                varNode->addSpecFlag(AstVarDecl::SPEC_FLAG_POST_STACK);

                // Put child front, because emitCall wants the parameters to be the last
                Ast::removeFromParent(varNode);
                Ast::addChildFront(identifier, varNode);

                CloneContext cloneContext;
                cloneContext.parent        = varNode;
                cloneContext.forceLocation = &identifier->token;
                cloneContext.parentScope   = identifier->ownerScope;
                cloneContext.removeFlags   = AST_IN_FUNC_DECL_PARAMS;

                if (funcParam->type)
                {
                    varNode->type = funcParam->type->clone(cloneContext);

                    // Need to have the real type for the variable
                    if (funcParam->type->typeInfo->isPointerRef())
                    {
                        SWAG_ASSERT(varNode->type->is(AstNodeKind::TypeExpression));
                        const auto typeNode = castAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
                        typeNode->typeFlags.remove(TYPE_FLAG_IS_REF);
                    }
                }

                // Need to test sizeof because assignment can be {}. In that case, we just reference
                // the temporary variable
                if (funcParam->assignment->typeInfo->sizeOf && !funcParam->assignment->typeInfo->isPointerNull())
                {
                    varNode->assignment = funcParam->assignment->clone(cloneContext);
                    varNode->assignment->inheritOwners(identifier);
                }

                const auto newParam = Ast::newFuncCallParam(nullptr, identifier->callParameters);

                // Make it a named param, in case some other default "normal" parameters are before (because in that case
                // we let the emitCall to deal with those default parameters)
                newParam->addExtraPointer(ExtraPointerKind::IsNamed, funcParam);

                newParam->indexParam = i;
                newParam->addAstFlag(AST_GENERATED);
                (void) Ast::newIdentifierRef(varNode->token.text, nullptr, newParam);

                // Add the 2 nodes to the semantic
                context->baseJob->nodes.push_back(newParam);
                context->baseJob->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, and the new variable, which is a child,
                // will be reevaluated too, so twice because of the push above. So we set a special flag to not reevaluate
                // it twice.
                varNode->addSemFlag(SEMFLAG_ONCE);

                context->result = ContextResult::NewChildren;
            }
        }
    }

    // :VariadicAllocStackForAny
    if (typeInfoFunc->hasFlag(TYPEINFO_VARIADIC))
    {
        const auto numFuncParams = typeInfoFunc->parameters.size();
        for (int i = static_cast<int>(maxParams - 1); i >= static_cast<int>(numFuncParams - 1); i--)
        {
            const auto child     = identifier->callParameters->children[i];
            const auto typeParam = TypeManager::concreteType(child->typeInfo, CONCRETE_FUNC);
            if (!typeParam->isStruct() &&
                !typeParam->isArray() &&
                !typeParam->isListArray() &&
                !typeParam->isListTuple())
            {
                child->allocateExtension(ExtensionKind::Misc);
                child->extMisc()->stackOffset = child->ownerScope->startStackSize;
                child->ownerScope->startStackSize += typeParam->numRegisters() * sizeof(Register);
                setOwnerMaxStackSize(child, child->ownerScope->startStackSize);
            }
        }
    }

    return true;
}

namespace
{
    bool isStatementIdentifier(const AstIdentifier* identifier)
    {
        if (identifier->isSilentCall())
            return false;

        auto checkParent = identifier->identifierRef()->parent;

        if (checkParent->is(AstNodeKind::FuncCallParam) &&
            checkParent->parent->is(AstNodeKind::CompilerInject))
            checkParent = checkParent->parent->parent;

        if (checkParent->is(AstNodeKind::Try) ||
            checkParent->is(AstNodeKind::Catch) ||
            checkParent->is(AstNodeKind::TryCatch) ||
            checkParent->is(AstNodeKind::Assume))
        {
            checkParent = checkParent->parent;
        }

        if (checkParent->is(AstNodeKind::Statement) ||
            checkParent->is(AstNodeKind::StatementNoScope) ||
            checkParent->is(AstNodeKind::Defer) ||
            checkParent->is(AstNodeKind::SwitchCaseBlock))
        {
            // If this is the last identifier
            if (identifier == identifier->identifierRef()->lastChild())
                return true;

            // If this is not the last identifier, and it's not a function call
            const auto back = identifier->identifierRef()->lastChild();
            if (back->is(AstNodeKind::Identifier) && !castAst<AstIdentifier>(back)->callParameters)
                return true;
        }

        return false;
    }
}

bool Semantic::setSymbolMatchVar(SemanticContext* context, const OneMatch& oneMatch, AstIdentifierRef* idRef, AstIdentifier* identifier, SymbolOverload* overload)
{
    // If this is a struct variable, and it's referenced (identifierRef has a startScope), then we
    // wait for the struct container to be solved. We do not want the semantic to continue with
    // an unsolved struct, because that means that storageOffset has not been computed yet
    // (and in some cases we can go to the bytecode generation with the struct not solved).
    if (overload->hasFlag(OVERLOAD_VAR_STRUCT) && identifier->identifierRef()->startScope)
    {
        const auto parentStructNode = identifier->identifierRef()->startScope->owner;
        waitStructOverloadDefined(context->baseJob, parentStructNode->typeInfo);
        YIELD();
        waitOverloadCompleted(context->baseJob, parentStructNode->resolvedSymbolOverload());
        YIELD();
    }

    overload->flags.add(OVERLOAD_USED);

    // Be sure usage is valid
    const auto ownerFct = identifier->ownerFct;
    if (ownerFct)
    {
        if (!ownerFct->hasAttribute(ATTRIBUTE_COMPILER) && overload->node->hasAttribute(ATTRIBUTE_COMPILER) && !ownerFct->hasAstFlag(AST_IN_RUN_BLOCK))
        {
            Diagnostic err{identifier, formErr(Err0142, Naming::kindName(overload->node).cstr(), overload->node->token.cstr(), ownerFct->getDisplayNameC())};
            err.addNote(overload->node, overload->node->token, formNte(Nte0159, Naming::kindName(overload->node).cstr()));
            return context->report(err);
        }
    }

    // Transform the variable to a constant node
    if (overload->hasFlag(OVERLOAD_CONST_VALUE))
    {
        if (overload->node->isConstantGenTypeInfo())
            identifier->addAstFlag(AST_VALUE_GEN_TYPEINFO);
        identifier->setFlagsValueIsComputed();
        *identifier->computedValue() = overload->computedValue;

        // If constant is inside an expression, everything before should be constant too.
        // Otherwise, that means that we reference a constant threw a variable
        //
        // var x = y.Constant where y is a variable
        //
        auto checkParent = identifier->parent;
        auto child       = castAst<AstNode>(identifier);
        while (checkParent->is(AstNodeKind::ArrayPointerIndex) ||
               checkParent->is(AstNodeKind::ArrayPointerSlicing))
        {
            child       = checkParent;
            checkParent = checkParent->parent;
        }

        // In that case, we cannot generate bytecode for everything before the constant,
        // as we have the scope, and this is enough.
        SWAG_ASSERT(checkParent->is(AstNodeKind::IdentifierRef));
        const auto childIdx = child->childParentIdx();
        for (uint32_t i = 0; i < childIdx; i++)
        {
            const auto brother = checkParent->children[i];
            if (!brother->hasFlagComputedValue() &&
                brother->resolvedSymbolOverload() &&
                brother->resolvedSymbolOverload()->symbol->is(SymbolKind::Variable))
            {
                brother->addAstFlag(AST_NO_BYTECODE);
            }
        }
    }

    // Setup parent if necessary
    SWAG_CHECK(Semantic::setupIdentifierRef(context, identifier));

    const auto typeInfo = TypeManager::concretePtrRefType(identifier->typeInfo);

    // If this is a 'code' variable, when passing code from one macro to another, then do not generate bytecode
    // for it, as this is not really a variable
    if (typeInfo->isCode())
        identifier->addAstFlag(AST_NO_BYTECODE);

    // Lambda call
    if (typeInfo->isLambdaClosure() && identifier->callParameters)
    {
        auto typeInfoRet = castTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::LambdaClosure)->returnType;
        typeInfoRet      = TypeManager::concreteType(typeInfoRet, CONCRETE_FORCE_ALIAS);

        // Check return value
        if (!typeInfoRet->isVoid())
        {
            if (isStatementIdentifier(identifier))
            {
                if (!overload->node->hasAttribute(ATTRIBUTE_DISCARDABLE) && !identifier->hasAstFlag(AST_DISCARD))
                {
                    const Diagnostic err(identifier, identifier->token, formErr(Err0713, overload->node->token.cstr()));
                    return context->report(err, Diagnostic::hereIs(overload));
                }

                identifier->addAstFlag(AST_DISCARD);
            }
        }
        else if (typeInfoRet->isVoid() && identifier->hasAstFlag(AST_DISCARD))
        {
            const Diagnostic err{identifier, identifier->token, toErr(Err0128)};
            return context->report(err, Diagnostic::hereIs(overload));
        }

        // From now this is considered as a function, not a lambda
        const auto funcType     = castTypeInfo<TypeInfoFuncAttr>(typeInfo->clone());
        funcType->kind          = TypeInfoKind::FuncAttr;
        identifier->typeInfo    = funcType;
        identifier->byteCodeFct = ByteCodeGen::emitLambdaCall;

        // Try/Assume
        setEmitTryCatchAssume(identifier, identifier->typeInfo);

        // Need to make all types compatible, in case a cast is necessary
        SWAG_CHECK(Semantic::setSymbolMatchCallParams(context, oneMatch, identifier));

        // For a return by copy, need to reserve room on the stack for the return result
        if (funcType->returnNeedsStack())
        {
            identifier->addAstFlag(AST_TRANSIENT);
            allocateOnStack(identifier, funcType->concreteReturnType());
        }
    }
    else
    {
        if (isStatementIdentifier(identifier))
        {
            const Diagnostic err{idRef, toErr(Err0516)};
            return context->report(err);
        }
    }

    return true;
}

bool Semantic::setSymbolMatchFunc(SemanticContext* context, const OneMatch& oneMatch, AstIdentifier* identifier, const SymbolOverload* overload)
{
    const auto funcDecl = castAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

    // Be sure that we didn't use a variable as a 'scope'
    const auto childIdx = identifier->childParentIdx();
    if (childIdx)
    {
        const auto prev       = identifier->identifierRef()->children[childIdx - 1];
        const auto symbolName = prev->resolvedSymbolName();
        if (symbolName && symbolName->is(SymbolKind::Variable) && !prev->hasAstFlag(AST_FROM_UFCS))
        {
            Diagnostic err{prev, formErr(Err0480, Naming::kindName(prev->resolvedSymbolOverload()->node).cstr(), prev->token.cstr(), identifier->token.cstr())};
            err.addNote(identifier->token, formNte(Nte0169, prev->typeInfo->getDisplayNameC()));
            return context->report(err, Diagnostic::hereIs(funcDecl));
        }
    }

    identifier->addAstFlag(AST_SIDE_EFFECTS);

    // Be sure it's () and not {}
    if (identifier->callParameters && identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
        return context->report({identifier->callParameters, formErr(Err0187, identifier->token.cstr())});

    // Capture syntax
    if (identifier->callParameters && !identifier->callParameters->aliasNames.empty())
    {
        if (!overload->node->hasAttribute(ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN))
        {
            const auto cp = identifier->callParameters;
            Diagnostic err{cp->token.sourceFile, cp->aliasNames.front().startLocation, cp->aliasNames.back().endLocation, formErr(Err0621, identifier->token.cstr())};
            err.addNote(identifier->token, toNte(Nte0170));
            return context->report(err);
        }
    }

    // Now we need to be sure that the function is now complete
    // If not, we need to wait for it
    waitOverloadCompleted(context->baseJob, overload);
    YIELD();

    if (identifier->token.is(g_LangSpec->name_opDrop))
        return context->report({identifier, identifier->token, toErr(Err0087)});
    if (identifier->token.is(g_LangSpec->name_opPostCopy))
        return context->report({identifier, identifier->token, toErr(Err0088)});
    if (identifier->token.is(g_LangSpec->name_opPostMove))
        return context->report({identifier, identifier->token, toErr(Err0089)});

    // Be sure this is not a 'forward' decl
    if (funcDecl->isEmptyFct() && !funcDecl->isForeign() && !identifier->hasIntrinsicName())
    {
        const Diagnostic err{identifier, identifier->token, formErr(Err0186, identifier->token.cstr())};
        return context->report(err, Diagnostic::hereIs(overload));
    }

    const auto pr2 = identifier->getParent(2);
    if (identifier->callParameters)
        identifier->addAstFlag(AST_L_VALUE | AST_R_VALUE);
    else if (pr2->is(AstNodeKind::MakePointerLambda))
        identifier->addAstFlag(AST_L_VALUE | AST_R_VALUE);
    else if (pr2->is(AstNodeKind::MakePointer))
        identifier->addAstFlag(AST_L_VALUE | AST_R_VALUE);

    // Need to make all types compatible, in case a cast is necessary
    if (!identifier->ownerFct || !identifier->ownerFct->hasAstFlag(AST_GENERIC))
    {
        SWAG_CHECK(Semantic::setSymbolMatchCallParams(context, oneMatch, identifier));
        YIELD();
    }

    // Be sure the call is valid
    if (identifier->token.text[0] != '@' && !funcDecl->isForeign())
    {
        const auto ownerFct = identifier->ownerFct;
        if (ownerFct)
        {
            if (!ownerFct->hasAttribute(ATTRIBUTE_COMPILER) && funcDecl->hasAttribute(ATTRIBUTE_COMPILER) && !identifier->hasAstFlag(AST_IN_RUN_BLOCK))
            {
                Diagnostic err{identifier, identifier->getTokenName(), formErr(Err0143, funcDecl->getDisplayNameC(), ownerFct->getDisplayNameC())};
                err.addNote(overload->node, overload->node->getTokenName(), toNte(Nte0171));
                return context->report(err);
            }
        }
    }

    if (identifier->isForceTakeAddress())
    {
        // This is for a lambda
        if (pr2->is(AstNodeKind::MakePointer) || pr2->is(AstNodeKind::MakePointerLambda))
        {
            if (!identifier->callParameters)
            {
                // The makePointer will deal with the real make lambda thing
                identifier->addAstFlag(AST_NO_BYTECODE);
                return true;
            }
        }
    }

    // The function call is constexpr if the function is, and all parameters are
    const auto typeFunc   = castTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
    const auto returnType = typeFunc->concreteReturnType();

    // Be sure that the return type is compatible with a compile-time execution.
    // Otherwise, we do not want the AST_CONST_EXPR_FLAG
    if (overload->node->hasAstFlag(AST_CONST_EXPR))
    {
        if (identifier->callParameters)
            identifier->inheritAstFlagsAnd(identifier->callParameters, AST_CONST_EXPR);
        else
            identifier->addAstFlag(AST_CONST_EXPR);
        if (identifier->hasAstFlag(AST_CONST_EXPR))
        {
            // :CheckConstExprFuncReturnType
            if (returnType &&
                !returnType->isString() &&
                !returnType->isNativeIntegerOrRune() &&
                !returnType->isNativeFloat() &&
                !returnType->isBool() &&
                !returnType->isPointerToTypeInfo() &&
                !returnType->isStruct() &&  // Treated later (as errors)
                !returnType->isArray() &&   // Treated later (as errors)
                !returnType->isClosure() && // Treated later (as errors)
                !identifier->hasSemFlag(SEMFLAG_EXEC_RET_STACK))
            {
                identifier->removeAstFlag(AST_CONST_EXPR);
            }
        }
    }

    // Check return value discard
    if (!returnType->isVoid())
    {
        if (isStatementIdentifier(identifier))
        {
            if (!funcDecl->hasAttribute(ATTRIBUTE_DISCARDABLE) && !identifier->hasAstFlag(AST_DISCARD))
            {
                const Diagnostic err(identifier, identifier->token, formErr(Err0711, overload->node->token.cstr()));
                return context->report(err, Diagnostic::hereIs(overload));
            }

            identifier->addAstFlag(AST_DISCARD);
        }
    }
    else
    {
        if (identifier->hasAstFlag(AST_DISCARD))
        {
            const Diagnostic err{identifier, identifier->token, toErr(Err0128)};
            return context->report(err, Diagnostic::hereIs(overload));
        }
    }

    bool canInline = true;
    if (!mustInline(funcDecl) || isFunctionButNotACall(context, identifier, overload->symbol))
        canInline = false;
    if (identifier->hasSpecFlag(AstIdentifier::SPEC_FLAG_NO_INLINE) && !funcDecl->hasAttribute(ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
        canInline = false;
    // Do not expand an inline call inside a function marked as inline.
    // The expansion will be done at the lowest level possible
    if (canInline && mustInline(identifier->ownerFct))
        canInline = false;

    if (canInline)
    {
        SWAG_CHECK(makeInline(context, identifier));
        YIELD();
        return true;
    }

    if (funcDecl->hasAttribute(ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
    {
        identifier->byteCodeFct = nullptr;
        return true;
    }

    identifier->addAstFlag(AST_FUNC_CALL);

    // @print behaves like a normal function, so we want an emitCall in that case
    if (identifier->hasIntrinsicName() && identifier->token.isNot(TokenId::IntrinsicPrint))
    {
        dealWithIntrinsic(context, identifier);
        identifier->byteCodeFct = ByteCodeGen::emitIntrinsic;
    }
    else if (funcDecl->isForeign())
        identifier->byteCodeFct = ByteCodeGen::emitForeignCall;
    else
        identifier->byteCodeFct = ByteCodeGen::emitCall;

    // Try/Assume
    setEmitTryCatchAssume(identifier, identifier->typeInfo);

    // Setup parent if necessary
    if (returnType->isStruct())
        identifier->addSemFlag(SEMFLAG_CONST_ASSIGN_INHERIT | SEMFLAG_CONST_ASSIGN);

    SWAG_CHECK(Semantic::setupIdentifierRef(context, identifier));

    // For a return by copy, need to reserve room on the stack for the return result
    // Order is important, because otherwise this could call isPlainOldData, which could be not resolved
    if (typeFunc->returnNeedsStack())
    {
        identifier->addAstFlag(AST_TRANSIENT);
        allocateOnStack(identifier, returnType);
    }

    return true;
}

bool Semantic::setSymbolMatchStruct(SemanticContext* context, OneMatch& oneMatch, AstIdentifierRef* identifierRef, AstIdentifier* identifier, const SymbolOverload* overload, TypeInfo* typeAlias)
{
    if (!overload->hasFlag(OVERLOAD_IMPL_IN_STRUCT))
        SWAG_CHECK(Semantic::setupIdentifierRef(context, identifier));
    identifierRef->startScope = castTypeInfo<TypeInfoStruct>(typeAlias)->scope;

    if (!identifier->callParameters)
        identifier->addAstFlag(AST_CONST_EXPR);

    // Be sure it's the NAME{} syntax
    if (identifier->callParameters &&
        !identifier->hasAstFlag(AST_GENERATED) &&
        !identifier->callParameters->hasSpecFlag(AstFuncCallParams::SPEC_FLAG_CALL_FOR_STRUCT))
    {
        const Diagnostic err{identifier, identifier->token, toErr(Err0237)};
        return context->report(err);
    }

    // A struct with parameters is in fact the creation of a temporary variable
    bool canConvertStructParams = false;
    bool canOptimAffect         = false;
    if (identifier->callParameters &&
        !identifier->hasAstFlag(AST_GENERATED) &&
        !identifier->hasAstFlag(AST_IN_TYPE_VAR_DECLARATION) &&
        !identifier->hasAstFlag(AST_IN_FUNC_DECL_PARAMS))
    {
        canConvertStructParams = true;
        const auto pr2         = identifier->getParent(2);
        if (pr2->is(AstNodeKind::VarDecl) || pr2->is(AstNodeKind::ConstDecl))
        {
            const auto varNode = castAst<AstVarDecl>(pr2, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
            if (varNode->assignment == identifier->parent && !varNode->type)
            {
                // Optim if we have var = Struct{}
                // In that case, no need to generate a temporary variable. We just consider Struct{} as the type definition
                // of 'var'
                canOptimAffect = true;
            }
        }
    }

    // Need to make all types compatible, in case a cast is necessary
    if (identifier->callParameters)
    {
        sortParameters(identifier->callParameters);
        const auto maxParams = identifier->callParameters->childCount();
        for (uint32_t i = 0; i < maxParams; i++)
        {
            const auto   nodeCall = castAst<AstFuncCallParam>(identifier->callParameters->children[i], AstNodeKind::FuncCallParam);
            const size_t idx      = nodeCall->indexParam;
            if (idx < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[idx])
            {
                CastFlags castFlags = CAST_FLAG_TRY_COERCE | CAST_FLAG_FORCE_UN_CONST | CAST_FLAG_PTR_REF;
                if (canOptimAffect)
                    castFlags.add(CAST_FLAG_NO_TUPLE_TO_STRUCT);
                SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[idx]->typeInfo, nullptr, nodeCall, castFlags));

                const auto typeCall = TypeManager::concreteType(nodeCall->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
                if (!oneMatch.solvedParameters[idx]->typeInfo->isPointerRef() && typeCall->isPointerRef())
                    setUnRef(nodeCall);
            }
        }
    }

    // Optim if we have var = Struct{}
    // In that case, no need to generate a temporary variable. We just consider Struct{} as the type definition
    // of 'var'
    if (canOptimAffect)
    {
        const auto varNode  = castAst<AstVarDecl>(identifier->getParent(2), AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
        const auto typeNode = Ast::newTypeExpression(nullptr, varNode);
        varNode->type       = typeNode;
        varNode->assignment = nullptr;
        typeNode->addSpecFlag(AstType::SPEC_FLAG_HAS_STRUCT_PARAMETERS);
        typeNode->addSpecFlag(AstTypeExpression::SPEC_FLAG_DONE_GEN);
        identifier->addSemFlag(SEMFLAG_ONCE);
        Ast::removeFromParent(identifier->parent);
        Ast::addChildBack(typeNode, identifier->parent);
        typeNode->identifier = identifier->parent;
        context->baseJob->nodes.pop_back();
        context->baseJob->nodes.pop_back();
        context->baseJob->nodes.push_back(typeNode);
        context->result = ContextResult::NewChildren;
        return true;
    }

    // A struct with parameters is in fact the creation of a temporary variable
    if (canConvertStructParams)
    {
        const auto pr2 = identifier->getParent(2);
        if (pr2->isNot(AstNodeKind::TypeExpression) || !pr2->hasSpecFlag(AstTypeExpression::SPEC_FLAG_DONE_GEN))
        {
            SWAG_CHECK(Ast::convertStructParamsToTmpVar(context, identifier));
            return true;
        }
    }

    return true;
}

bool Semantic::setSymbolMatchUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, const AstIdentifier* identifier, AstNode* dependentVar)
{
    const auto idRef = castAst<AstIdentifierRef>(identifierRef, AstNodeKind::IdentifierRef);
    if (dependentVar->is(AstNodeKind::IdentifierRef))
    {
        for (uint32_t i = dependentVar->childCount() - 1; i != UINT32_MAX; i--)
        {
            const auto child         = dependentVar->children[i];
            const auto idNode        = Ast::newIdentifier(idRef, child->token.text, nullptr, nullptr);
            idNode->token.sourceFile = dependentVar->token.sourceFile;
            idNode->inheritAstFlagsOr(idRef, AST_IN_MIXIN);
            idNode->inheritTokenLocation(idRef->token);
            idNode->allocateIdentifierExtension();
            idNode->identifierExtension->fromAlternateVar = child;
            Ast::addChildFront(idRef, idNode);
            context->baseJob->nodes.push_back(idNode);
            if (i == 0 && identifier->identifierExtension)
            {
                idNode->identifierExtension->scopeUpMode  = identifier->identifierExtension->scopeUpMode;
                idNode->identifierExtension->scopeUpValue = identifier->identifierExtension->scopeUpValue;
            }

            idNode->addSpecFlag(AstIdentifier::SPEC_FLAG_FROM_USING);
        }
    }
    else
    {
        const auto idNode        = Ast::newIdentifier(idRef, dependentVar->token.text, nullptr, nullptr);
        idNode->token.sourceFile = dependentVar->token.sourceFile;
        idNode->inheritAstFlagsOr(idRef, AST_IN_MIXIN);
        idNode->inheritTokenLocation(identifier->token);

        // We need to insert at the right place, but the identifier 'childParentIdx' can be the wrong one
        // if it's not a direct child of 'idRef'. So we need to find the direct child of 'idRef', which is
        // also a parent if 'identifier', in order to get the right child index, and insert the 'using'
        // just before.
        const AstNode* newParent = identifier;
        while (newParent->parent != idRef)
            newParent = newParent->parent;

        idNode->addSpecFlag(AstIdentifier::SPEC_FLAG_FROM_USING);

        if (identifier->identifierExtension || dependentVar)
            idNode->allocateIdentifierExtension();

        if (identifier->identifierExtension)
        {
            idNode->identifierExtension->scopeUpMode  = identifier->identifierExtension->scopeUpMode;
            idNode->identifierExtension->scopeUpValue = identifier->identifierExtension->scopeUpValue;
        }

        if (idNode->identifierExtension)
            idNode->identifierExtension->fromAlternateVar = dependentVar;

        Ast::insertChild(idRef, idNode, newParent->childParentIdx());
        context->baseJob->nodes.push_back(idNode);
    }

    context->node->semanticState = AstNodeResolveState::Enter;
    context->result              = ContextResult::NewChildren;
    return true;
}

bool Semantic::setMatchResultAndType(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* identifier, OneMatch& oneMatch)
{
    if (identifier->isSilentCall())
        identifier->typeInfo = identifierRef->typeInfo;
    else if (oneMatch.typeWasForced)
        identifier->typeInfo = oneMatch.typeWasForced;
    else
        identifier->typeInfo = oneMatch.symbolOverload->typeInfo;

    SWAG_CHECK(setMatchResult(context, identifierRef, identifier, oneMatch));
    return true;
}

bool Semantic::checkMatchResult(SemanticContext*        context,
                                const AstIdentifierRef* identifierRef,
                                AstIdentifier*          identifier,
                                const OneMatch&         oneMatch,
                                const SymbolName*       symbol,
                                const SymbolOverload*   overload,
                                AstNode*                prevNode)
{
    // Test x.toto with x not a struct (like a native type for example), but toto is known, so
    // no error was raised before
    if (symbol &&
        symbol->is(SymbolKind::Variable) &&
        !overload->typeInfo->isLambdaClosure() &&
        !identifierRef->startScope &&
        !identifier->isSilentCall() &&
        prevNode &&
        !prevNode->typeInfo->isPointerTo(TypeInfoKind::Struct) &&
        !prevNode->typeInfo->isStruct())
    {
        const Diagnostic err{prevNode, formErr(Err0176, prevNode->token.cstr(), prevNode->typeInfo->getDisplayNameC())};
        return context->report(err);
    }

    // If a variable on the left has only been used for scoping, and not evaluated as an UFCS source, then this is an
    // error too, because it's too strange.
    // x.toto() with toto taking no argument for example, but toto is 'in' x scope.
    if (symbol &&
        symbol->is(SymbolKind::Function) &&
        identifierRef->startScope &&
        prevNode &&
        prevNode->resolvedSymbolName() &&
        (prevNode->resolvedSymbolName()->is(SymbolKind::Variable) || prevNode->resolvedSymbolName()->is(SymbolKind::Function)) &&
        !prevNode->hasAstFlag(AST_FROM_UFCS))
    {
        if (prevNode->is(AstNodeKind::Identifier) && prevNode->hasSpecFlag(AstIdentifier::SPEC_FLAG_FROM_WITH))
        {
            Diagnostic err{prevNode, formErr(Err0481, prevNode->token.cstr(), symbol->name.cstr())};
            const auto prevIdentifier = castAst<AstIdentifier>(prevNode, AstNodeKind::Identifier);
            const auto widthNode      = prevIdentifier->identifierExtension->fromAlternateVar;
            err.addNote(oneMatch.oneOverload->overload->node, oneMatch.oneOverload->overload->node->getTokenName(), formNte(Nte0169, prevNode->typeInfo->getDisplayNameC()));
            err.addNote(Diagnostic::hereIs(widthNode));
            err.addNote(formNte(Nte0034, identifierRef->startScope->name.cstr()));
            return context->report(err);
        }

        if (oneMatch.oneOverload->scope == identifierRef->startScope)
        {
            Diagnostic err{prevNode, formErr(Err0480, Naming::kindName(prevNode->resolvedSymbolName()->kind).cstr(), prevNode->token.cstr(), symbol->name.cstr())};
            err.addNote(identifier->token, formNte(Nte0169, prevNode->typeInfo->getDisplayNameC()));
            err.addNote(formNte(Nte0106, Naming::kindName(prevNode->resolvedSymbolName()->kind).cstr(), prevNode->token.cstr(), symbol->name.cstr()));
            err.addNote(Diagnostic::hereIs(oneMatch.oneOverload->overload));
            err.addNote(formNte(Nte0042, Naming::kindName(prevNode->resolvedSymbolName()->kind).cstr(), identifierRef->startScope->name.cstr()));
            return context->report(err);
        }

        Diagnostic err{prevNode, formErr(Err0480, Naming::kindName(prevNode->resolvedSymbolName()->kind).cstr(), prevNode->token.cstr(), symbol->name.cstr())};
        err.addNote(identifier->token, formNte(Nte0169, prevNode->typeInfo->getDisplayNameC()));
        err.addNote(Diagnostic::hereIs(oneMatch.oneOverload->overload));
        return context->report(err);
    }

    // A.X and A is an array : missing index
    if (symbol &&
        symbol->is(SymbolKind::Variable) &&
        identifier->typeInfo->isArray() &&
        identifier->parent->isNot(AstNodeKind::ArrayPointerIndex) &&
        identifier->parent == identifierRef &&
        identifierRef->lastChild() != identifier)
    {
        const Diagnostic err{identifier, formErr(Err0445, symbol->name.cstr(), identifier->typeInfo->getDisplayNameC())};
        return context->report(err);
    }

    // A.X and A is a slice : missing index
    if (symbol &&
        symbol->is(SymbolKind::Variable) &&
        identifier->typeInfo->isSlice() &&
        identifier->parent->isNot(AstNodeKind::ArrayPointerIndex) &&
        identifier->parent == identifierRef &&
        identifierRef->lastChild() != identifier)
    {
        const Diagnostic err{identifier, formErr(Err0445, symbol->name.cstr(), identifier->typeInfo->getDisplayNameC())};
        return context->report(err);
    }

    return true;
}

bool Semantic::setMatchResult(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* identifier, OneMatch& oneMatch)
{
    const auto symbol       = oneMatch.symbolOverload->symbol;
    const auto overload     = oneMatch.symbolOverload;
    const auto dependentVar = oneMatch.dependentVar;

    // Mark as used
    if (symbol)
        symbol->flags.add(SYMBOL_USED);
    if (dependentVar && dependentVar->resolvedSymbolName())
        dependentVar->resolvedSymbolName()->flags.add(SYMBOL_USED);
    if (dependentVar && dependentVar->resolvedSymbolOverload())
        dependentVar->resolvedSymbolOverload()->symbol->flags.add(SYMBOL_USED);

    // Verify that this match is possible
    SWAG_CHECK(checkMatchResult(context, identifierRef, identifier, oneMatch, symbol, overload, identifierRef->previousResolvedNode));

    // Reapply back the values of the match to the call parameter node
    for (const auto& pp : oneMatch.paramParameters)
    {
        pp.param->indexParam        = pp.indexParam;
        pp.param->resolvedParameter = pp.resolvedParameter;
    }

    const auto prevNode = identifierRef->previousResolvedNode;
    if (prevNode && symbol->is(SymbolKind::Variable))
    {
        identifier->addAstFlag(AST_L_VALUE);

        // Direct reference to a constexpr typeinfo
        if (prevNode->hasFlagComputedValue())
        {
            if (prevNode->typeInfo->isStruct() || prevNode->typeInfo->isPointer())
            {
                if (!prevNode->computedValue()->storageSegment)
                {
                    const Diagnostic err{prevNode, toErr(Err0218)};
                    return context->report(err);
                }

                const auto ptr = static_cast<uint8_t*>(prevNode->computedValue()->getStorageAddr());
                if (derefConstant(context, ptr, overload, prevNode->computedValue()->storageSegment))
                {
                    prevNode->addAstFlag(AST_NO_BYTECODE);
                    identifierRef->previousResolvedNode = context->node;
                    identifier->setResolvedSymbol(overload->symbol, overload);
                    return true;
                }
            }
        }

        // Direct reference of a struct field inside a const array
        if (prevNode->is(AstNodeKind::ArrayPointerIndex) && prevNode->typeInfo->isStruct())
        {
            const auto arrayNode = castAst<AstArrayPointerIndex>(prevNode, AstNodeKind::ArrayPointerIndex);
            const auto arrayOver = arrayNode->array->resolvedSymbolOverload();
            if (arrayOver && arrayOver->hasFlag(OVERLOAD_CONST_VALUE) && arrayNode->access->hasFlagComputedValue())
            {
                const auto typePtr = castTypeInfo<TypeInfoArray>(arrayNode->array->typeInfo, TypeInfoKind::Array);
                auto       ptr     = static_cast<uint8_t*>(arrayOver->computedValue.getStorageAddr());
                ptr += arrayNode->access->computedValue()->reg.u64 * typePtr->finalType->sizeOf;
                if (derefConstant(context, ptr, overload, arrayOver->computedValue.storageSegment))
                {
                    prevNode->addAstFlag(AST_NO_BYTECODE);
                    identifierRef->previousResolvedNode = context->node;
                    identifier->setResolvedSymbol(overload->symbol, overload);
                    return true;
                }
            }
        }
    }

    // If this is an L or R value
    if (overload->hasFlag(OVERLOAD_VAR_STRUCT))
    {
        if (symbol->isNot(SymbolKind::GenericType))
        {
            if (identifierRef->previousResolvedNode)
            {
                if (identifierRef->previousResolvedNode->hasAstFlag(AST_R_VALUE))
                    identifier->addAstFlag(AST_L_VALUE | AST_R_VALUE);
                else
                    identifier->addAstFlag(identifierRef->previousResolvedNode->flags.mask(AST_L_VALUE));
            }
        }
    }
    else if (symbol->is(SymbolKind::Variable))
    {
        identifier->addAstFlag(AST_L_VALUE | AST_R_VALUE);
    }

    // Do not register a sub impl scope, for UFCS to use the real variable
    if (!overload->hasFlag(OVERLOAD_IMPL_IN_STRUCT))
    {
        identifierRef->setResolvedSymbol(symbol, overload);
    }

    identifier->setResolvedSymbol(symbol, overload);

    if (identifier->typeInfo->isGeneric())
        identifier->addAstFlag(AST_GENERIC);
    else if (overload->hasFlag(OVERLOAD_GENERIC) && !identifier->hasAstFlag(AST_FROM_GENERIC))
        identifier->addAstFlag(AST_GENERIC);

    // Symbol is linked to a using var : insert the variable name before the symbol
    // Except if symbol is a constant !
    if (!overload->hasFlag(OVERLOAD_CONST_VALUE))
    {
        if (dependentVar && identifierRef->is(AstNodeKind::IdentifierRef) && symbol->isNot(SymbolKind::Function))
        {
            SWAG_CHECK(setSymbolMatchUsingVar(context, identifierRef, identifier, dependentVar));
            return true;
        }
    }

    // If this is a typealias, find the right thing
    auto typeAlias  = identifier->typeInfo;
    auto symbolKind = symbol->kind;
    if (symbol->is(SymbolKind::TypeAlias))
    {
        typeAlias = TypeManager::concreteType(identifier->typeInfo, CONCRETE_FORCE_ALIAS);
        if (typeAlias->isStruct())
            symbolKind = SymbolKind::Struct;
        else if (typeAlias->isInterface())
            symbolKind = SymbolKind::Interface;
        else if (typeAlias->isEnum())
            symbolKind = SymbolKind::Enum;
    }

    SWAG_CHECK(SemanticError::warnDeprecated(context, identifier));

    if (symbolKind != SymbolKind::Variable && symbolKind != SymbolKind::Function)
    {
        if (isStatementIdentifier(identifier))
        {
            const Diagnostic err{identifier, toErr(Err0516)};
            return context->report(err);
        }
    }

    const auto idRef = identifier->identifierRef();
    switch (symbolKind)
    {
        case SymbolKind::GenericType:
            SWAG_CHECK(setupIdentifierRef(context, identifier));
            break;

        case SymbolKind::Namespace:
            identifierRef->startScope = castTypeInfo<TypeInfoNamespace>(identifier->typeInfo, identifier->typeInfo->kind)->scope;
            identifier->addAstFlag(AST_CONST_EXPR);
            break;

        case SymbolKind::Enum:
            SWAG_CHECK(setupIdentifierRef(context, identifier));
            identifier->addAstFlag(AST_CONST_EXPR);
            break;

        case SymbolKind::EnumValue:
            if (idRef && idRef->previousResolvedNode && idRef->previousResolvedNode->resolvedSymbolName()->is(SymbolKind::Variable))
            {
                const Diagnostic err{idRef->previousResolvedNode, formErr(Err0178, idRef->previousResolvedNode->typeInfo->getDisplayNameC())};
                return context->report(err);
            }

            SWAG_CHECK(setupIdentifierRef(context, identifier));
            identifier->setFlagsValueIsComputed();
            identifier->addAstFlag(AST_R_VALUE);
            *identifier->computedValue() = identifier->resolvedSymbolOverload()->computedValue;
            break;

        case SymbolKind::Struct:
        case SymbolKind::Interface:
            SWAG_CHECK(setSymbolMatchStruct(context, oneMatch, identifierRef, identifier, overload, typeAlias));
            YIELD();
            break;

        case SymbolKind::Variable:
            SWAG_CHECK(setSymbolMatchVar(context, oneMatch, idRef, identifier, overload));
            YIELD();
            break;

        case SymbolKind::Function:
            SWAG_CHECK(setSymbolMatchFunc(context, oneMatch, identifier, overload));
            YIELD();
            break;
    }

    return true;
}

bool Semantic::registerMatch(SemanticContext*            context,
                             AstNode*                    node,
                             VectorNative<OneTryMatch*>& tryMatches,
                             VectorNative<OneMatch*>&    matches,
                             VectorNative<OneMatch*>&    genericMatches,
                             VectorNative<OneMatch*>&    genericMatchesSI,
                             OneTryMatch&                oneOverload,
                             TypeInfo*                   typeWasForced)
{
    const auto genericParameters = oneOverload.genericParameters;
    const auto dependentVar      = oneOverload.dependentVar;
    const auto overload          = oneOverload.overload;
    const auto symbol            = overload->symbol;

    if (overload->hasFlag(OVERLOAD_GENERIC))
    {
        // Be sure that we would like to instantiate in case we do not have user generic parameters.
        bool asMatch = false;
        if (!oneOverload.genericParameters)
        {
            const auto pr2 = context->node->getParent(2);
            if (pr2->isNot(AstNodeKind::Invalid))
            {
                bool isLast = false;
                if (node && node->is(AstNodeKind::Identifier))
                {
                    const auto id = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
                    if (id == id->identifierRef()->lastChild())
                        isLast = true;
                }

                if (pr2->is(AstNodeKind::IntrinsicProp) && pr2->token.is(TokenId::CompilerIntrinsicTypeOf))
                    asMatch = true;
                else if (pr2->is(AstNodeKind::IntrinsicProp) && pr2->token.is(TokenId::IntrinsicKindOf))
                    asMatch = true;
                else if (pr2->is(AstNodeKind::IntrinsicProp) && pr2->token.is(TokenId::CompilerIntrinsicNameOf))
                    asMatch = true;
                else if (isLast && pr2->is(AstNodeKind::BinaryOp) && pr2->token.is(TokenId::SymEqualEqual) && overload->symbol->is(SymbolKind::Struct))
                    asMatch = true;
                else if (isLast && pr2->is(AstNodeKind::BinaryOp) && pr2->token.is(TokenId::SymExclamEqual) && overload->symbol->is(SymbolKind::Struct))
                    asMatch = true;
                else if (pr2->is(AstNodeKind::CompilerIntrinsicDefined))
                    asMatch = true;
            }
        }

        const auto match = context->getOneMatch();
        if (asMatch)
        {
            match->symbolOverload   = overload;
            match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
            match->solvedCastFlags  = std::move(oneOverload.symMatchContext.solvedCastFlags);
            match->dependentVar     = dependentVar;
            match->ufcs             = oneOverload.ufcs;
            match->altFlags         = oneOverload.altFlags;
            match->oneOverload      = &oneOverload;
            match->matchFlags       = oneOverload.symMatchContext.matchFlags;
            match->castFlagsResult  = oneOverload.symMatchContext.castFlagsResult;
            matches.push_back(match);
            tryMatches.clear();
        }
        else
        {
            match->matchFlags                  = oneOverload.symMatchContext.matchFlags;
            match->castFlagsResult             = oneOverload.symMatchContext.castFlagsResult;
            match->symbolName                  = symbol;
            match->symbolOverload              = overload;
            match->genericParametersCallTypes  = std::move(oneOverload.symMatchContext.genericParametersCallTypes);
            match->genericParametersCallValues = std::move(oneOverload.symMatchContext.genericParametersCallValues);
            match->genericReplaceTypes         = std::move(oneOverload.symMatchContext.genericReplaceTypes);
            match->genericReplaceValues        = std::move(oneOverload.symMatchContext.genericReplaceValues);
            match->parameters                  = std::move(oneOverload.symMatchContext.parameters);
            match->solvedParameters            = std::move(oneOverload.symMatchContext.solvedParameters);
            match->solvedCastFlags             = std::move(oneOverload.symMatchContext.solvedCastFlags);
            match->genericParameters           = genericParameters;
            match->numOverloadsWhenChecked     = oneOverload.cptOverloads;
            match->numOverloadsInitWhenChecked = oneOverload.cptOverloadsInit;
            match->ufcs                        = oneOverload.ufcs;
            match->altFlags                    = oneOverload.altFlags;
            if (overload->node->hasAstFlag(AST_HAS_CONSTRAINTS) && overload->node->is(AstNodeKind::FuncDecl))
                genericMatchesSI.push_back(match);
            else
                genericMatches.push_back(match);
        }
    }
    else
    {
        bool canRegisterInstance = true;

        // Be sure that we are not an identifier with generic parameters.
        // In that case we do not want to have instances.
        // Weird.
        if (node && node->is(AstNodeKind::Identifier))
        {
            const auto id = castAst<AstIdentifier>(node, AstNodeKind::Identifier);
            if (id->genericParameters)
            {
                for (const auto p : id->genericParameters->children)
                {
                    if (p->hasAstFlag(AST_GENERIC))
                    {
                        canRegisterInstance = false;
                        break;
                    }
                }
            }
        }

        if (canRegisterInstance)
        {
            const auto match        = context->getOneMatch();
            match->symbolOverload   = overload;
            match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
            match->solvedCastFlags  = std::move(oneOverload.symMatchContext.solvedCastFlags);
            match->dependentVar     = dependentVar;
            match->ufcs             = oneOverload.ufcs;
            match->altFlags         = oneOverload.altFlags;
            match->oneOverload      = &oneOverload;
            match->typeWasForced    = typeWasForced;
            match->matchFlags       = oneOverload.symMatchContext.matchFlags;
            match->castFlagsResult  = oneOverload.symMatchContext.castFlagsResult;

            // As indexParam and resolvedParameter are directly stored in the node, we need to save them
            // with the corresponding match, as they can be overwritten by another match attempt
            for (const auto p : oneOverload.symMatchContext.parameters)
            {
                if (p->isNot(AstNodeKind::FuncCallParam))
                    continue;

                OneMatch::ParamParameter pp;
                const auto               param = castAst<AstFuncCallParam>(p, AstNodeKind::FuncCallParam);

                pp.param             = param;
                pp.indexParam        = param->indexParam;
                pp.resolvedParameter = param->resolvedParameter;
                match->paramParameters.push_back(pp);
            }

            matches.push_back(match);
        }
    }

    return true;
}

bool Semantic::doSimpleMatch(SemanticContext* context, VectorNative<OneMatch*>& matches, OneTryMatch* const& oneMatch, OneTryMatch& oneOverload)
{
    if (oneOverload.symMatchContext.parameters.empty() && oneOverload.symMatchContext.genericParameters.empty())
    {
        const auto symbolKind = oneOverload.overload->symbol->kind;

        if (symbolKind != SymbolKind::Attribute &&
            symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            !oneOverload.overload->typeInfo->isLambdaClosure())
        {
            const auto match        = context->getOneMatch();
            match->symbolOverload   = oneOverload.overload;
            match->scope            = oneMatch->scope;
            match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
            match->solvedCastFlags  = std::move(oneOverload.symMatchContext.solvedCastFlags);
            match->dependentVar     = oneOverload.dependentVar;
            match->ufcs             = oneOverload.ufcs;
            match->altFlags         = oneOverload.altFlags;
            matches.push_back(match);
            return true;
        }
    }
    return false;
}

bool Semantic::needToWaitForSymbol(Job* job, SymbolName* symbol)
{
    bool needLock = false;

    {
        SharedLock ls(symbol->mutex);
        if ((symbol->isNot(SymbolKind::Function) || symbol->cptOverloadsInit != symbol->overloads.size()) && symbol->cptOverloads)
            needLock = true;
    }

    if (needLock)
    {
        ScopedLock ls(symbol->mutex);
        if ((symbol->isNot(SymbolKind::Function) || symbol->cptOverloadsInit != symbol->overloads.size()) && symbol->cptOverloads)
        {
            waitSymbolNoLock(job, symbol);
            return true;
        }
    }

    return true;
}

bool Semantic::tryOneMatch(SemanticContext* context, OneTryMatch& oneOverload, const SymbolOverload* overload, TypeInfo* rawTypeInfo, MatchIdParamsFlags flags)
{
    // This way, a special cast can be done for the first parameter of a function
    if (oneOverload.ufcs)
        oneOverload.symMatchContext.matchFlags.add(SymbolMatchContext::MATCH_UFCS);

    // We collect type replacements depending on where the identifier is
    Generic::setContextualGenericTypeReplacement(context, oneOverload, overload, flags);

    oneOverload.symMatchContext.semContext = context;
    context->castFlagsResult               = 0;

    if (rawTypeInfo->isStruct())
    {
        const auto typeInfo = castTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
        Match::match(typeInfo, oneOverload.symMatchContext);
    }
    else if (rawTypeInfo->isInterface())
    {
        const auto typeInfo = castTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Interface);
        Match::match(typeInfo, oneOverload.symMatchContext);
    }
    else if (rawTypeInfo->isFuncAttr())
    {
        const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr);
        Match::match(typeInfo, oneOverload.symMatchContext);
    }
    else if (rawTypeInfo->isLambdaClosure())
    {
        const auto typeInfo = castTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::LambdaClosure);
        Match::match(typeInfo, oneOverload.symMatchContext);
    }
    else if (rawTypeInfo->isKindGeneric())
    {
        oneOverload.symMatchContext.result = MatchResult::Ok;
    }
    else if (rawTypeInfo->isArray())
    {
        const auto typeArr   = castTypeInfo<TypeInfoArray>(rawTypeInfo, TypeInfoKind::Array);
        const auto typeFinal = TypeManager::concreteType(typeArr->finalType);
        const auto typeInfo  = castTypeInfo<TypeInfoFuncAttr>(typeFinal, TypeInfoKind::LambdaClosure);
        Match::match(typeInfo, oneOverload.symMatchContext);
    }
    else
    {
        SWAG_ASSERT(false);
    }

    return true;
}

bool Semantic::dealWithMatchResults(SemanticContext*            context,
                                    VectorNative<OneTryMatch*>& tryMatches,
                                    AstNode*&                   node,
                                    MatchIdParamsFlags          flags,
                                    VectorNative<OneMatch*>&    matches,
                                    VectorNative<OneMatch*>&    genericMatches,
                                    VectorNative<OneMatch*>&    genericMatchesSI,
                                    bool                        forStruct,
                                    uint32_t                    prevMatchesCount)
{
    const bool justCheck = flags.has(MIP_JUST_CHECK);

    // If to match an instance, we always need an automatic opCast, then we only keep generic matches in order
    // to create an instance with the exact type.
    // We only test the first match here, because the filtering of matches would have removed it if some other instances
    // without autoOpCast are present.
    if (!matches.empty() && matches[0]->castFlagsResult.has(CAST_RESULT_GEN_AUTO_OP_CAST) && (!genericMatches.empty() || !genericMatchesSI.empty()))
    {
        prevMatchesCount = 0;
        matches.clear();
    }

    // All choices were removed because of where
    /////////////////////////////////////////////////////////////////////
    if (genericMatches.empty() && !genericMatchesSI.empty() && matches.empty() && prevMatchesCount)
    {
        if (justCheck)
            return false;
        return SemanticError::cannotMatchIdentifierError(context, tryMatches, node);
    }

    // Multi instantiation in case of where
    /////////////////////////////////////////////////////////////////////
    if (!genericMatchesSI.empty() && matches.empty() && !prevMatchesCount)
    {
        if (justCheck)
            return true;

        Set<SymbolName*> symbols;
        for (const auto& g : genericMatchesSI)
            symbols.insert(g->symbolName);
        for (auto& g : symbols)
            g->mutex.lock();

        for (const auto& g : genericMatchesSI)
        {
            Generic::checkCanInstantiateGenericSymbol(context, *g);
            if (context->result != ContextResult::Done)
                break;
        }

        if (context->result == ContextResult::Done)
        {
            for (const auto& g : genericMatchesSI)
            {
                SWAG_CHECK(Generic::instantiateFunction(context, g->genericParameters, *g, true));
            }
        }

        for (auto& g : symbols)
            g->mutex.unlock();

        return true;
    }

    // This is a generic match. We need to instantiate.
    /////////////////////////////////////////////////////////////////////
    if (genericMatches.size() == 1 && matches.empty())
    {
        if (justCheck && !flags.has(MIP_SECOND_GENERIC_TRY))
            return true;
        SWAG_CHECK(Generic::instantiateGenericSymbol(context, *genericMatches[0], forStruct));
        return true;
    }

    // One match, but want none !
    /////////////////////////////////////////////////////////////////////
    if (matches.size() == 1 && flags.has(MIP_FOR_ZERO_GHOSTING))
    {
        if (justCheck)
            return false;

        if (!node)
            node = context->node;

        const auto symbol   = tryMatches[0]->overload->symbol;
        const auto overload = matches[0]->symbolOverload;
        return SemanticError::duplicatedSymbolError(context, node->token.sourceFile, node->token, symbol->kind, symbol->name, overload->symbol->kind, overload->node);
    }

    // Success !!!
    /////////////////////////////////////////////////////////////////////
    if (matches.size() == 1)
    {
        return true;
    }

    // Ambiguity with generics
    /////////////////////////////////////////////////////////////////////
    if (genericMatches.size() > 1)
    {
        if (justCheck)
            return false;
        return SemanticError::ambiguousGenericError(context, node, tryMatches, genericMatches);
    }

    // We remove all generated nodes, because if they exist, they do not participate in the error
    for (uint32_t i = 0; i < tryMatches.size() && tryMatches.size() != 1; i++)
    {
        if (tryMatches[i]->overload->node->hasAstFlag(AST_FROM_GENERIC))
        {
            tryMatches[i] = tryMatches.back();
            tryMatches.pop_back();
            i--;
        }
    }

    // There's no match at all
    /////////////////////////////////////////////////////////////////////
    if (matches.empty())
    {
        if (!flags.has(MIP_SECOND_GENERIC_TRY))
        {
            VectorNative<OneTryMatch*> cpyOverloads;
            for (auto& oneMatch : tryMatches)
            {
                if (oneMatch->symMatchContext.result == MatchResult::NotEnoughGenericArguments &&
                    oneMatch->symMatchContext.genericParameters.empty())
                {
                    cpyOverloads.push_back(oneMatch);
                }
            }

            if (!cpyOverloads.empty())
            {
                const auto result = matchIdentifierParameters(context, cpyOverloads, node, flags | MIP_JUST_CHECK | MIP_SECOND_GENERIC_TRY);
                if (result)
                    return true;
            }
        }

        if (justCheck)
            return false;

        return SemanticError::cannotMatchIdentifierError(context, tryMatches, node);
    }

    // There is more than one possible match
    /////////////////////////////////////////////////////////////////////
    if (matches.size() > 1)
    {
        // This is an identifier for a name alias. We are fine.
        if (node && node->is(AstNodeKind::Identifier) && node->hasSpecFlag(AstIdentifier::SPEC_FLAG_NAME_ALIAS))
            return true;

        if (justCheck)
            return false;

        return SemanticError::ambiguousOverloadError(context, node, matches, flags);
    }

    return true;
}

bool Semantic::matchIdentifierParameters(SemanticContext* context, VectorNative<OneTryMatch*>& tryMatches, AstNode* node, MatchIdParamsFlags flags)
{
    const auto job              = context->baseJob;
    auto&      matches          = context->cacheMatches;
    auto&      genericMatches   = context->cacheGenericMatches;
    auto&      genericMatchesSI = context->cacheGenericMatchesSI;

    context->clearMatch();
    context->clearGenericMatch();
    context->bestSignatureInfos.clear();

    bool forStruct = false;
    for (const auto& oneMatch : tryMatches)
    {
        auto& oneOverload = *oneMatch;

        // For everything except functions/attributes/structs (which have overloads), this is a match
        if (doSimpleMatch(context, matches, oneMatch, oneOverload))
            continue;

        // Major contention offender.
        SWAG_CHECK(needToWaitForSymbol(job, oneOverload.overload->symbol));
        YIELD();

        const auto overload    = oneOverload.overload;
        const auto symbol      = overload->symbol;
        auto       rawTypeInfo = overload->typeInfo;

        // :DupGen
        TypeInfo* typeWasForced = nullptr;
        if (node && node->parent && node->parent->inSimpleReturn())
        {
            rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_FORCE_ALIAS);
            if (rawTypeInfo->isStruct())
            {
                const auto fctTypeInfo = castTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                if (fctTypeInfo->returnType && fctTypeInfo->returnType->isStruct())
                {
                    auto       rawTypeStruct    = castTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                    const auto returnStructType = castTypeInfo<TypeInfoStruct>(fctTypeInfo->returnType, TypeInfoKind::Struct);
                    if (returnStructType->genericParameters.size() == rawTypeStruct->genericParameters.size() && !rawTypeStruct->genericParameters.empty())
                    {
                        rawTypeStruct = castTypeInfo<TypeInfoStruct>(rawTypeInfo->clone());
                        rawTypeInfo   = rawTypeStruct;
                        typeWasForced = rawTypeInfo;
                        for (uint32_t i = 0; i < returnStructType->genericParameters.size(); i++)
                        {
                            rawTypeStruct->genericParameters[i]->name = returnStructType->genericParameters[i]->name;
                        }
                    }
                }
            }
        }

        // If this is a type alias that already has a generic instance, accept to not have generic
        // parameters on the source symbol
        if (rawTypeInfo->isAlias())
        {
            rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_FORCE_ALIAS);
            if (rawTypeInfo->isStruct())
            {
                const auto typeInfo = castTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                if (!typeInfo->isGeneric())
                    oneOverload.symMatchContext.matchFlags.add(SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC);
            }
        }

        forStruct = rawTypeInfo->isStruct() || rawTypeInfo->isInterface();

        // The main deal ! Try the match
        SWAG_CHECK(tryOneMatch(context, oneOverload, overload, rawTypeInfo, flags));
        YIELD();

        if (oneOverload.symMatchContext.result == MatchResult::NotEnoughGenericArguments)
        {
            SWAG_CHECK(tryOneMatch(context, oneOverload, overload, rawTypeInfo, flags | MIP_SECOND_GENERIC_DEDUCE));
            YIELD();
        }

        // For a function, sometime, we do not want call parameters
        bool forcedFine = false;

        // Be sure this is not because of a generic error
        if (oneOverload.symMatchContext.result != MatchResult::NotEnoughGenericArguments &&
            oneOverload.symMatchContext.result != MatchResult::TooManyGenericArguments &&
            oneOverload.symMatchContext.result != MatchResult::BadGenericSignature)
        {
            if (isFunctionButNotACall(context, node, symbol))
            {
                oneOverload.symMatchContext.result = MatchResult::Ok;
                forcedFine                         = true;
            }
        }

        // We need () for a function call !
        const bool emptyParams = oneOverload.symMatchContext.parameters.empty() && !oneOverload.callParameters;
        if (!forcedFine && emptyParams && oneOverload.symMatchContext.result == MatchResult::Ok && symbol->is(SymbolKind::Function))
        {
            oneOverload.symMatchContext.result = MatchResult::MissingArguments;
        }

        // Function type without call parameters
        if (emptyParams && oneOverload.symMatchContext.result == MatchResult::NotEnoughArguments)
        {
            if (symbol->is(SymbolKind::Variable))
                oneOverload.symMatchContext.result = MatchResult::Ok;
        }

        // In case of errors, remember the 'best' signature in order to generate the best possible
        // accurate error. We find the longest match (the one that failed on the right most parameter)
        if (oneOverload.symMatchContext.result != MatchResult::Ok)
        {
            if (context->bestSignatureInfos.badSignatureParameterIdx == UINT32_MAX ||
                oneOverload.symMatchContext.badSignatureInfos.badSignatureParameterIdx > context->bestSignatureInfos.badSignatureParameterIdx)
            {
                context->bestMatchResult    = oneOverload.symMatchContext.result;
                context->bestSignatureInfos = oneOverload.symMatchContext.badSignatureInfos;
                context->bestOverload       = overload;
            }
        }

        if (oneOverload.symMatchContext.result != MatchResult::Ok)
            continue;

        SWAG_CHECK(registerMatch(context, node, tryMatches, matches, genericMatches, genericMatchesSI, oneOverload, typeWasForced));
        if (tryMatches.empty())
            break;
    }

    // Filter depending on various priorities
    const auto prevMatchesCount = matches.size();
    SWAG_CHECK(filterMatches(context, matches, genericMatches, genericMatchesSI));
    YIELD();

    // Process the results
    return dealWithMatchResults(context, tryMatches, node, flags, matches, genericMatches, genericMatchesSI, forStruct, prevMatchesCount);
}

bool Semantic::computeMatch(SemanticContext* context, AstIdentifier* identifier, ResolveIdFlags riFlags, VectorNative<OneSymbolMatch>& symbolsMatch, AstIdentifierRef* identifierRef)
{
    const auto orgResolvedSymbolOverload = identifierRef->resolvedSymbolOverload();
    const auto orgResolvedSymbolName     = identifierRef->resolvedSymbolName();
    const auto orgPreviousResolvedNode   = identifierRef->previousResolvedNode;
    const auto isForcedUFCS              = identifier->isForcedUFCS();
    auto&      toSolveOverload           = context->cacheToSolveOverload;
    auto&      listTryMatch              = context->cacheListTryMatch;

    while (true)
    {
        context->cacheMatches.clear();
        toSolveOverload.clear();

        // Collect all overloads to solve. We collect also the number of overloads when the collect is done, in
        // case that number changes (other thread) during the resolution. Because if the number of overloads differs
        // at one point in the process (for a given symbol), then this will invalidate the resolution
        // (number of overloads can change when instantiating a generic)
        for (const auto& p : symbolsMatch)
        {
            const auto symbol = p.symbol;
            SharedLock lk(symbol->mutex);
            for (const auto over : symbol->overloads)
            {
                OneOverload t;
                t.overload         = over;
                t.scope            = p.scope;
                t.altFlags         = p.altFlags;
                t.cptOverloads     = symbol->overloads.size();
                t.cptOverloadsInit = symbol->cptOverloadsInit;
                toSolveOverload.push_back(t);
            }
        }

        // Can happen if a symbol is inside a disabled #if for example
        if (toSolveOverload.empty())
        {
            if (identifierRef->hasAstFlag(AST_SILENT_CHECK))
                return true;
            return context->report({identifier, formErr(Err0706, identifier->token.cstr())});
        }

        context->clearTryMatch();
        for (const auto& oneOver : toSolveOverload)
        {
            const auto symbolOverload = oneOver.overload;

            identifierRef->setResolvedSymbol(orgResolvedSymbolName, orgResolvedSymbolOverload);
            identifierRef->previousResolvedNode = orgPreviousResolvedNode;

            // Is there a using variable associated with the symbol to solve ?
            AstNode* dependentVar     = nullptr;
            AstNode* dependentVarLeaf = nullptr;
            SWAG_CHECK(Semantic::getUsingVar(context, identifierRef, identifier, symbolOverload, &dependentVar, &dependentVarLeaf));
            YIELD();

            // Get the UFCS first parameter if we can
            AstNode* ufcsFirstParam = nullptr;

            // The UFCS parameter has already been set in we are evaluating an identifier for the second time
            // (when we inline a function call)
            if (!identifier->callParameters || identifier->callParameters->children.empty() || !identifier->callParameters->firstChild()->hasAstFlag(AST_TO_UFCS))
            {
                SWAG_CHECK(Semantic::getUFCS(context, identifierRef, identifier, symbolOverload, &ufcsFirstParam));
                YIELD();
                if (identifier->hasSemFlag(SEMFLAG_FORCE_UFCS) && !ufcsFirstParam)
                    continue;
            }

            // If the last parameter of a function is of type 'code', and the last call parameter is not,
            // then we take the next statement, after the function, and put it as the last parameter
            SWAG_CHECK(appendLastCodeStatement(context, identifier, symbolOverload));

            // This if for a lambda
            bool forLambda = false;
            if (identifier->isForceTakeAddress() && isFunctionButNotACall(context, identifier, symbolOverload->symbol))
                forLambda = true;

            // Will try with UFCS, and will try without
            for (int tryUFCS = 0; tryUFCS < 2; tryUFCS++)
            {
                auto  tryMatch        = context->getTryMatch();
                auto& symMatchContext = tryMatch->symMatchContext;

                tryMatch->symMatchContext.matchFlags.add(forLambda ? SymbolMatchContext::MATCH_FOR_LAMBDA : 0);

                tryMatch->genericParameters = identifier->genericParameters;
                tryMatch->callParameters    = identifier->callParameters;
                tryMatch->dependentVar      = dependentVar;
                tryMatch->dependentVarLeaf  = dependentVarLeaf;
                tryMatch->overload          = symbolOverload;
                tryMatch->scope             = oneOver.scope;
                tryMatch->altFlags          = oneOver.altFlags;
                tryMatch->ufcs              = ufcsFirstParam || isForcedUFCS;
                tryMatch->cptOverloads      = oneOver.cptOverloads;
                tryMatch->cptOverloadsInit  = oneOver.cptOverloadsInit;

                SWAG_CHECK(Semantic::fillMatchContextCallParameters(context, symMatchContext, identifier, symbolOverload, ufcsFirstParam));
                YIELD();
                SWAG_CHECK(Semantic::fillMatchContextGenericParameters(context, symMatchContext, identifier, symbolOverload));

                listTryMatch.push_back(tryMatch);

                if (!ufcsFirstParam)
                    break;
                if (identifier->hasSemFlag(SEMFLAG_FORCE_UFCS))
                    break;
                ufcsFirstParam = nullptr;
            }
        }

        if (listTryMatch.empty())
            break;

        MatchIdParamsFlags mipFlags = 0;
        if (riFlags.has(RI_FOR_GHOSTING))
            mipFlags.add(MIP_FOR_GHOSTING);
        if (riFlags.has(RI_FOR_ZERO_GHOSTING))
            mipFlags.add(MIP_FOR_ZERO_GHOSTING);
        SWAG_CHECK(Semantic::matchIdentifierParameters(context, listTryMatch, identifier, mipFlags));

        switch (context->result)
        {
            case ContextResult::Pending:
                identifierRef->setResolvedSymbol(orgResolvedSymbolName, orgResolvedSymbolOverload);
                identifierRef->previousResolvedNode = orgPreviousResolvedNode;
                return true;

            case ContextResult::NewChildren1:
                identifierRef->setResolvedSymbol(orgResolvedSymbolName, orgResolvedSymbolOverload);
                identifierRef->previousResolvedNode = orgPreviousResolvedNode;
                context->result                     = ContextResult::NewChildren;
                return true;

            // The number of overloads for a given symbol has been changed by another thread, which
            // means that we need to restart everything...
            case ContextResult::NewChildren:
                context->result = ContextResult::Done;
                break;

            default:
                return true;
        }
    }

    return true;
}

bool Semantic::matchRetval(SemanticContext* context, VectorNative<OneSymbolMatch>& symbolsMatch, const AstIdentifier* identifier)
{
    // Be sure this is correct
    SWAG_CHECK(resolveRetVal(context));
    const auto fctDecl = identifier->hasOwnerInline() ? identifier->ownerInline()->func : identifier->ownerFct;
    SWAG_ASSERT(fctDecl);
    const auto typeFct = castTypeInfo<TypeInfoFuncAttr>(fctDecl->typeInfo, TypeInfoKind::FuncAttr);
    SWAG_ASSERT(typeFct->returnType->isStruct());
    addSymbolMatch(symbolsMatch, typeFct->returnType->declNode->resolvedSymbolName(), nullptr, 0);
    return true;
}

bool Semantic::matchSharpSelf(SemanticContext* context, VectorNative<OneSymbolMatch>& symbolsMatch, AstIdentifierRef* identifierRef, AstIdentifier* identifier)
{
    SWAG_VERIFY(identifier->ownerFct, context->report({identifier, toErr(Err0310)}));
    AstNode* parent = identifier;
    while (parent->ownerFct->hasAttribute(ATTRIBUTE_SHARP_FUNC) && parent->ownerFct->parent->ownerFct)
        parent = parent->ownerFct->parent;
    SWAG_VERIFY(parent, context->report({parent, toErr(Err0310)}));

    if (parent->ownerScope->is(ScopeKind::Struct) || parent->ownerScope->is(ScopeKind::Enum))
    {
        parent = parent->ownerScope->owner;
        identifier->addAstFlag(AST_CAN_MATCH_INCOMPLETE);
    }
    else
    {
        SWAG_VERIFY(parent->ownerFct, context->report({parent, toErr(Err0310)}));
        parent = parent->ownerFct;

        // Force scope
        if (!identifier->callParameters && identifier != identifierRef->lastChild())
        {
            identifier->addSemFlag(SEMFLAG_FORCE_SCOPE);
            identifier->typeInfo                = g_TypeMgr->typeInfoVoid;
            identifierRef->previousResolvedNode = identifier;
            if (identifier->hasOwnerInline())
                identifierRef->startScope = identifier->ownerInline()->parametersScope;
            else
                identifierRef->startScope = identifier->ownerFct->scope;
            identifier->addAstFlag(AST_NO_BYTECODE);
            return true;
        }
    }

    addSymbolMatch(symbolsMatch, parent->resolvedSymbolName(), nullptr, 0);
    return true;
}
