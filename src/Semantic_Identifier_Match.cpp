#include "pch.h"
#include "Ast.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "Diagnostic.h"
#include "LanguageSpec.h"
#include "Module.h"
#include "Naming.h"
#include "Report.h"
#include "SemanticError.h"
#include "SemanticJob.h"
#include "TypeManager.h"

void Semantic::sortParameters(AstNode* allParams)
{
    ScopedLock lk(allParams->mutex);

    if (!allParams || !(allParams->flags & AST_MUST_SORT_CHILDS))
        return;
    if (allParams->childs.size() <= 1)
        return;

    sort(allParams->childs.begin(), allParams->childs.end(), [](AstNode* n1, AstNode* n2)
         {
             AstFuncCallParam* p1 = CastAst<AstFuncCallParam>(n1, AstNodeKind::FuncCallParam);
             AstFuncCallParam* p2 = CastAst<AstFuncCallParam>(n2, AstNodeKind::FuncCallParam);
             return p1->indexParam < p2->indexParam; });

    allParams->flags ^= AST_MUST_SORT_CHILDS;
}

void Semantic::dealWithIntrinsic(SemanticContext* context, AstIdentifier* identifier)
{
    auto module = context->sourceFile->module;

    switch (identifier->tokenId)
    {
    case TokenId::IntrinsicAssert:
    {
        if (module->mustOptimizeBytecode(context->node))
        {
            // Remove assert(true)
            SWAG_ASSERT(identifier->callParameters && !identifier->callParameters->childs.empty());
            auto param = identifier->callParameters->childs.front();
            if (param->hasComputedValue() && param->computedValue->reg.b)
                identifier->flags |= AST_NO_BYTECODE;
        }
        break;
    }
    default:
        break;
    }
}

void Semantic::resolvePendingLambdaTyping(SemanticContext* context, AstNode* funcNode, TypeInfo* resolvedType, int i)
{
    auto funcDecl = CastAst<AstFuncDecl>(funcNode, AstNodeKind::FuncDecl);
    SWAG_ASSERT(!(funcDecl->flags & AST_IS_GENERIC));
    auto typeUndefinedFct = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    auto concreteType     = TypeManager::concreteType(resolvedType);
    auto typeDefinedFct   = CastTypeInfo<TypeInfoFuncAttr>(concreteType, TypeInfoKind::LambdaClosure);

    // Replace every parameters types
    for (size_t paramIdx = 0; paramIdx < typeUndefinedFct->parameters.size(); paramIdx++)
    {
        auto childType = funcDecl->parameters->childs[paramIdx];

        TypeInfo* definedType;
        if (typeDefinedFct->isClosure() && !typeUndefinedFct->isClosure())
            definedType = typeDefinedFct->parameters[paramIdx + 1]->typeInfo;
        else
            definedType = typeDefinedFct->parameters[paramIdx]->typeInfo;

        childType->typeInfo                              = definedType;
        childType->resolvedSymbolOverload->typeInfo      = definedType;
        typeUndefinedFct->parameters[paramIdx]->typeInfo = definedType;
    }

    // Replace generic parameters, if any
    for (size_t paramIdx = 0; paramIdx < typeUndefinedFct->genericParameters.size(); paramIdx++)
    {
        auto undefinedType = typeUndefinedFct->genericParameters[paramIdx];
        auto it            = typeDefinedFct->replaceTypes.find(undefinedType->name);
        if (it != typeDefinedFct->replaceTypes.end())
        {
            undefinedType->name     = it->second.typeInfoReplace->name;
            undefinedType->typeInfo = it->second.typeInfoReplace;
        }
    }

    // Replace every types inside the function
    Ast::visit(funcDecl, [&](AstNode* p)
               {
                   auto it = typeDefinedFct->replaceTypes.find(p->token.text);
                   if (it == typeDefinedFct->replaceTypes.end())
                       return;
                   p->token.text = it->second.typeInfoReplace->name;
                   if (p->resolvedSymbolOverload)
                       p->resolvedSymbolOverload->typeInfo = it->second.typeInfoReplace;
                   p->typeInfo = it->second.typeInfoReplace; });

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
    SWAG_ASSERT(context->node->kind == AstNodeKind::Identifier);

    context->node->semFlags |= SEMFLAG_PENDING_LAMBDA_TYPING;
    funcDecl->semFlags &= ~SEMFLAG_PENDING_LAMBDA_TYPING;

    ScopedLock lk(funcDecl->resolvedSymbolOverload->symbol->mutex);
    if (typeUndefinedFct->returnType->isGeneric())
        funcDecl->resolvedSymbolOverload->flags |= OVERLOAD_INCOMPLETE;
    Semantic::waitSymbolNoLock(context->baseJob, funcDecl->resolvedSymbolOverload->symbol);
    context->baseJob->jobsToAdd.push_back(funcDecl->pendingLambdaJob);
}

bool Semantic::setSymbolMatchCallParams(SemanticContext* context, AstIdentifier* identifier, OneMatch& oneMatch)
{
    if (!identifier->callParameters)
        return true;

    auto sourceFile   = context->sourceFile;
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);

    // :ClosureForceFirstParam
    // Add a first dummy parameter in case of closure
    if (typeInfoFunc->isClosure() && !(identifier->specFlags & AstIdentifier::SPECFLAG_CLOSURE_FIRST_PARAM))
    {
        auto fcp = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        Ast::removeFromParent(fcp);
        Ast::addChildFront(identifier->callParameters, fcp);
        fcp->setFlagsValueIsComputed();
        fcp->computedValue->reg.pointer = nullptr;
        fcp->typeInfo                   = g_TypeMgr->typeInfoNull;
        fcp->flags |= AST_GENERATED;
        identifier->addSpecFlags(AstIdentifier::SPECFLAG_CLOSURE_FIRST_PARAM);

        auto node = Ast::newNode<AstLiteral>(nullptr, AstNodeKind::Literal, context->sourceFile, fcp);
        node->setFlagsValueIsComputed();
        node->flags |= AST_GENERATED;
        node->typeInfo = g_TypeMgr->typeInfoNull;
    }

    sortParameters(identifier->callParameters);
    auto maxParams = identifier->callParameters->childs.size();
    for (size_t idx = 0; idx < maxParams; idx++)
    {
        auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[idx], AstNodeKind::FuncCallParam);

        size_t i = idx;
        if (idx < typeInfoFunc->parameters.size() - 1 || !(typeInfoFunc->flags & (TYPEINFO_VARIADIC | TYPEINFO_C_VARIADIC)))
            i = nodeCall->indexParam;

        // This is a lambda that was waiting for a match to have its types, and to continue solving its content
        if (nodeCall->typeInfo->isLambdaClosure() && (nodeCall->typeInfo->declNode->semFlags & SEMFLAG_PENDING_LAMBDA_TYPING))
        {
            resolvePendingLambdaTyping(context, nodeCall->typeInfo->declNode, oneMatch.solvedParameters[i]->typeInfo, (uint32_t) i);
            YIELD();
        }

        uint64_t castFlags = CASTFLAG_AUTO_OPCAST | CASTFLAG_ACCEPT_PENDING | CASTFLAG_PARAMS | CASTFLAG_PTR_REF | CASTFLAG_FOR_AFFECT | CASTFLAG_ACCEPT_MOVE_REF;
        if (i == 0 && oneMatch.ufcs)
            castFlags |= CASTFLAG_UFCS;
        if (oneMatch.oneOverload && !(oneMatch.oneOverload->overload->node->attributeFlags & ATTRIBUTE_OVERLOAD))
            castFlags |= CASTFLAG_TRY_COERCE;

        TypeInfo* toType = nullptr;
        if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
        {
            PushErrCxtStep ec(context, typeInfoFunc->declNode, ErrCxtStepKind::HereIs, nullptr);
            context->castFlagsResult = 0;

            toType = oneMatch.solvedParameters[i]->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, toType, nullptr, nodeCall, castFlags));
            YIELD();

            if (!toType->isPointerRef() && nodeCall->typeInfo->isPointerRef())
            {
                // :ConcreteRef
                setUnRef(nodeCall);
            }
            else if (toType->isConstPointerRef() &&
                     !nodeCall->typeInfo->isPointer() &&
                     !nodeCall->typeInfo->isStruct() &&
                     !nodeCall->typeInfo->isListTuple())
            {
                auto front = nodeCall->childs.front();

                // We have a compile time value (like a literal), and we want a const ref, i.e. a pointer
                // We need to create a temporary variable to store the value, in order to have an address.
                if (front->hasComputedValue() || nodeCall->typeInfo->isListArray())
                {
                    auto varNode = Ast::newVarDecl(sourceFile, Fmt("__7tmp_%d", g_UniqueID.fetch_add(1)), nodeCall);
                    varNode->inheritTokenLocation(nodeCall);
                    varNode->flags |= AST_GENERATED;
                    Ast::removeFromParent(front);
                    Ast::addChildBack(varNode, front);
                    varNode->assignment = front;

                    auto toPtr              = CastTypeInfo<TypeInfoPointer>(toType, TypeInfoKind::Pointer);
                    varNode->type           = Ast::newIdentifierRef(sourceFile, "dummy", varNode);
                    varNode->type->typeInfo = toPtr->pointedType;
                    varNode->type->flags |= AST_NO_SEMANTIC;
                    nodeCall->typeInfo = toPtr->pointedType;

                    auto idNode = Ast::newIdentifierRef(sourceFile, varNode->token.text, nodeCall);
                    idNode->inheritTokenLocation(nodeCall);
                    idNode->flags |= AST_GENERATED;
                    Ast::removeFromParent(idNode);
                    Ast::addChildFront(nodeCall, idNode);

                    context->baseJob->nodes.push_back(idNode);
                    context->baseJob->nodes.push_back(varNode);
                    nodeCall->flags &= ~AST_VALUE_COMPUTED;

                    context->result = ContextResult::NewChilds;
                    return true;
                }
                // We have a value, and we need a reference.
                // Force to keep the address
                else if (front->kind == AstNodeKind::IdentifierRef)
                {
                    front->childs.back()->semFlags |= SEMFLAG_FORCE_TAKE_ADDRESS;
                }
                else
                    return Report::internalError(nodeCall, "cannot deal with value to pointer ref conversion");
            }
            else if (context->castFlagsResult & CASTFLAG_RESULT_FORCE_REF)
            {
                auto front = nodeCall->childs.front();

                // We have a value, and we need a reference.
                // Force to keep the address
                if (front->kind == AstNodeKind::IdentifierRef)
                {
                    front->childs.back()->semFlags |= SEMFLAG_FORCE_TAKE_ADDRESS;
                }
            }
        }
        else if (oneMatch.solvedParameters.size() && oneMatch.solvedParameters.back() && oneMatch.solvedParameters.back()->typeInfo->isTypedVariadic())
        {
            toType = oneMatch.solvedParameters.back()->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters.back()->typeInfo, nullptr, nodeCall));
            YIELD();
        }

        // If passing a closure
        // :FctCallParamClosure
        auto toTypeRef = TypeManager::concreteType(toType, CONCRETE_FORCEALIAS);
        auto makePtrL  = nodeCall->childs.empty() ? nullptr : nodeCall->childs.front();

        if (makePtrL && toTypeRef && toTypeRef->isClosure())
        {
            bool convert = false;
            if (makePtrL->kind == AstNodeKind::MakePointer || makePtrL->kind == AstNodeKind::MakePointerLambda)
                convert = true;
            if (makePtrL->typeInfo && makePtrL->typeInfo->isLambda())
                convert = true;
            if (makePtrL->typeInfo->isPointerNull())
                convert = true;
            if (convert)
            {
                auto varNode = Ast::newVarDecl(sourceFile, Fmt("__ctmp_%d", g_UniqueID.fetch_add(1)), identifier);

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
                    nodeCall->flags &= ~AST_VALUE_COMPUTED;
                    makePtrL->flags |= AST_NO_BYTECODE;
                    Ast::removeFromParent(makePtrL);
                    varNode->allocateExtension(ExtensionKind::Owner);
                    varNode->extOwner()->nodesToFree.push_back(makePtrL);
                }
                else
                {
                    makePtrL->semFlags |= SEMFLAG_ONCE;
                    Ast::removeFromParent(makePtrL);
                    Ast::addChildBack(varNode, makePtrL);
                    varNode->assignment = makePtrL;
                }

                varNode->type           = Ast::newTypeExpression(sourceFile, varNode);
                varNode->type->typeInfo = toType;
                varNode->type->flags |= AST_NO_SEMANTIC;

                auto idRef = Ast::newIdentifierRef(sourceFile, varNode->token.text, nodeCall);
                idRef->allocateExtension(ExtensionKind::Misc);
                idRef->extMisc()->exportNode = makePtrL;

                // Add the 2 nodes to the semantic
                context->baseJob->nodes.push_back(idRef);
                context->baseJob->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, and the new variable, which is a child,
                // will be reevaluated too, so twice because of the push above. So we set a special flag to not reevaluate
                // it twice.
                varNode->semFlags |= SEMFLAG_ONCE;

                context->result = ContextResult::NewChilds;
            }
        }
    }

    // Deal with opAffect automatic conversion
    // :opAffectParam
    for (size_t i = 0; i < maxParams; i++)
    {
        auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
        if (nodeCall->hasExtMisc() && nodeCall->extMisc()->resolvedUserOpSymbolOverload)
        {
            auto overload = nodeCall->extMisc()->resolvedUserOpSymbolOverload;
            if (overload->symbol->name == g_LangSpec->name_opAffect || overload->symbol->name == g_LangSpec->name_opAffectSuffix)
            {
                SWAG_ASSERT(nodeCall->castedTypeInfo);
                nodeCall->extMisc()->resolvedUserOpSymbolOverload = nullptr;
                nodeCall->castedTypeInfo                          = nullptr;

                auto varNode = Ast::newVarDecl(sourceFile, Fmt("__2tmp_%d", g_UniqueID.fetch_add(1)), identifier);
                varNode->inheritTokenLocation(nodeCall);

                // Put child front, because emitCall wants the parameters to be the last
                Ast::removeFromParent(varNode);
                Ast::addChildFront(identifier, varNode);

                auto typeExpr      = Ast::newTypeExpression(sourceFile, varNode);
                typeExpr->typeInfo = nodeCall->typeInfo;
                typeExpr->flags |= AST_NO_SEMANTIC;
                varNode->type = typeExpr;

                auto assign = nodeCall->childs.front();
                if (assign->kind == AstNodeKind::Cast)
                    assign = assign->childs.back();

                CloneContext cloneContext;
                cloneContext.parent      = varNode;
                cloneContext.parentScope = identifier->ownerScope;
                varNode->assignment      = assign->clone(cloneContext);

                Ast::removeFromParent(nodeCall);

                auto newParam        = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
                newParam->indexParam = nodeCall->indexParam;
                Ast::removeFromParent(newParam);
                Ast::insertChild(identifier->callParameters, newParam, (uint32_t) i);

                // If the match is against a 'moveref', then we should have a 'moveref' node and a makepointer.
                if (typeInfoFunc->parameters[nodeCall->indexParam]->typeInfo->isPointerMoveRef())
                {
                    auto moveRefNode = Ast::newNode<AstNode>(nullptr, AstNodeKind::MoveRef, sourceFile, newParam);
                    moveRefNode->flags |= AST_GENERATED;
                    moveRefNode->semanticFct = Semantic::resolveMoveRef;
                    auto mkPtrNode           = Ast::newNode<AstMakePointer>(nullptr, AstNodeKind::MakePointer, sourceFile, moveRefNode);
                    mkPtrNode->flags |= AST_GENERATED;
                    mkPtrNode->semanticFct = Semantic::resolveMakePointer;
                    Ast::newIdentifierRef(sourceFile, varNode->token.text, mkPtrNode);
                }
                else
                {
                    Ast::newIdentifierRef(sourceFile, varNode->token.text, newParam);
                }

                // We want to export the original parameter, not the temporary variable reference
                newParam->allocateExtension(ExtensionKind::Misc);
                newParam->extMisc()->exportNode = nodeCall;
                newParam->allocateExtension(ExtensionKind::Owner);
                newParam->extOwner()->nodesToFree.push_back(nodeCall);

                // Add the 2 nodes to the semantic
                context->baseJob->nodes.push_back(newParam);
                context->baseJob->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, so the new variable, which is a child of
                // that identifier, will be reevaluated too (so twice because of the push above).
                // So we set a special flag to not reevaluate it twice.
                varNode->semFlags |= SEMFLAG_ONCE;

                context->result = ContextResult::NewChilds;
            }
        }
    }

    // Deal with default values for structs and uncomputed values
    // We need to add a temporary variable initialized with the default value, and reference
    // that temporary variable as a new function call parameter
    if (typeInfoFunc->parameters.size() && maxParams < typeInfoFunc->parameters.size())
    {
        AstNode* parameters = nullptr;
        if (typeInfoFunc->declNode->kind == AstNodeKind::FuncDecl)
        {
            auto funcDecl = CastAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl);
            parameters    = funcDecl->parameters;
        }
        else
        {
            auto funcDecl = CastAst<AstTypeLambda>(typeInfoFunc->declNode, AstNodeKind::TypeLambda, AstNodeKind::TypeClosure);
            parameters    = funcDecl->childs.front();
        }

        for (uint32_t i = 0; i < parameters->childs.size(); i++)
        {
            if (parameters->childs[i]->kind != AstNodeKind::FuncDeclParam)
                continue;
            auto funcParam = CastAst<AstVarDecl>(parameters->childs[i], AstNodeKind::FuncDeclParam);
            if (!funcParam->assignment)
                continue;
            switch (funcParam->assignment->tokenId)
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

            auto typeParam = TypeManager::concretePtrRefType(funcParam->typeInfo);
            if (!typeParam->isStruct() &&
                !typeParam->isListTuple() &&
                !typeParam->isListArray() &&
                !typeParam->isAny() &&
                !typeParam->isClosure() &&
                (funcParam->assignment->hasComputedValue()))
                continue;

            bool covered = false;
            for (uint32_t j = 0; j < maxParams; j++)
            {
                auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[j], AstNodeKind::FuncCallParam);
                if (nodeCall->resolvedParameter && nodeCall->resolvedParameter->index == i)
                {
                    covered = true;
                    break;
                }
            }

            if (!covered)
            {
                auto varNode = Ast::newVarDecl(sourceFile, Fmt("__3tmp_%d", g_UniqueID.fetch_add(1)), identifier);

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
                        SWAG_ASSERT(varNode->type->kind == AstNodeKind::TypeExpression);
                        auto typeNode = CastAst<AstTypeExpression>(varNode->type, AstNodeKind::TypeExpression);
                        typeNode->typeFlags &= ~TYPEFLAG_IS_REF;
                    }
                }

                // Need to test sizeof because assignement can be {}. In that case, we just reference
                // the temporary variable
                if (funcParam->assignment->typeInfo->sizeOf && !funcParam->assignment->typeInfo->isPointerNull())
                {
                    varNode->assignment = funcParam->assignment->clone(cloneContext);
                    varNode->assignment->inheritOwners(identifier);
                }

                auto newParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);

                // Make it a named param, in case some other default "normal" parameters are before (because in that case
                // we let the emitCall to deal with those default parameters)
                newParam->allocateExtension(ExtensionKind::Misc);
                newParam->extMisc()->isNamed = funcParam;

                newParam->indexParam = i;
                newParam->flags |= AST_GENERATED;
                Ast::newIdentifierRef(sourceFile, varNode->token.text, newParam);

                // Add the 2 nodes to the semantic
                context->baseJob->nodes.push_back(newParam);
                context->baseJob->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, and the new variable, which is a child,
                // will be reevaluated too, so twice because of the push above. So we set a special flag to not reevaluate
                // it twice.
                varNode->semFlags |= SEMFLAG_ONCE;

                context->result = ContextResult::NewChilds;
            }
        }
    }

    return true;
}

static bool isStatementIdentifier(AstIdentifier* identifier)
{
    if (identifier->isSilentCall())
        return false;

    auto checkParent = identifier->identifierRef()->parent;
    if (checkParent->kind == AstNodeKind::Try ||
        checkParent->kind == AstNodeKind::Catch ||
        checkParent->kind == AstNodeKind::TryCatch ||
        checkParent->kind == AstNodeKind::Assume)
    {
        checkParent = checkParent->parent;
    }

    if (checkParent->kind == AstNodeKind::Statement ||
        checkParent->kind == AstNodeKind::StatementNoScope ||
        checkParent->kind == AstNodeKind::Defer ||
        checkParent->kind == AstNodeKind::SwitchCaseBlock)
    {
        // If this is the last identifier
        if (identifier == identifier->identifierRef()->childs.back())
            return true;

        // If this is not the last identifier, and it's not a function call
        auto back = identifier->identifierRef()->childs.back();
        if (back->kind == AstNodeKind::Identifier && !((AstIdentifier*) back)->callParameters)
            return true;
    }

    return false;
}

bool Semantic::setSymbolMatch(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* identifier, OneMatch& oneMatch)
{
    auto symbol       = oneMatch.symbolOverload->symbol;
    auto overload     = oneMatch.symbolOverload;
    auto dependentVar = oneMatch.dependentVar;
    auto sourceFile   = context->sourceFile;

    // Mark as used
    if (symbol)
        symbol->flags |= SYMBOL_USED;
    if (dependentVar && dependentVar->resolvedSymbolName)
        dependentVar->resolvedSymbolName->flags |= SYMBOL_USED;
    if (dependentVar && dependentVar->resolvedSymbolOverload)
        dependentVar->resolvedSymbolOverload->symbol->flags |= SYMBOL_USED;

    auto prevNode = identifierRef->previousResolvedNode;

    // Test x.toto with x not a struct (like a native type for example), but toto is known, so
    // no error was raised before
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        !overload->typeInfo->isLambdaClosure() &&
        !identifierRef->startScope &&
        !identifier->isSilentCall() &&
        prevNode &&
        !prevNode->typeInfo->isPointerTo(TypeInfoKind::Struct) &&
        !prevNode->typeInfo->isStruct())
    {
        Diagnostic diag{prevNode, Fmt(Err(Err0257), prevNode->token.ctext(), prevNode->typeInfo->getDisplayNameC())};
        return context->report(diag);
    }

    // If a variable on the left has only been used for scoping, and not evaluated as an ufcs source, then this is an
    // error too, cause it's too strange.
    // x.toto() with toto taking no argument for example, but toto is 'in' x scope.
    if (symbol &&
        symbol->kind == SymbolKind::Function &&
        identifierRef->startScope &&
        prevNode &&
        prevNode->resolvedSymbolName &&
        (prevNode->resolvedSymbolName->kind == SymbolKind::Variable || prevNode->resolvedSymbolName->kind == SymbolKind::Function) &&
        !(prevNode->flags & AST_FROM_UFCS))
    {
        if (prevNode->kind == AstNodeKind::Identifier && prevNode->specFlags & AstIdentifier::SPECFLAG_FROM_WITH)
        {
            Diagnostic                diag{prevNode, Fmt(Err(Err0586), prevNode->token.ctext(), symbol->name.c_str())};
            Vector<const Diagnostic*> notes;
            auto                      prevIdentifier = CastAst<AstIdentifier>(prevNode, AstNodeKind::Identifier);
            auto                      widthNode      = prevIdentifier->identifierExtension->fromAlternateVar;
            notes.push_back(Diagnostic::note(oneMatch.oneOverload->overload->node, oneMatch.oneOverload->overload->node->getTokenName(), Fmt(Nte(Nte0154), prevNode->typeInfo->getDisplayNameC())));
            notes.push_back(Diagnostic::hereIs(widthNode));
            notes.push_back(Diagnostic::note(Fmt(Nte(Nte0031), identifierRef->startScope->name.c_str())));
            return context->report(diag, notes);
        }

        if (oneMatch.oneOverload->scope == identifierRef->startScope)
        {
            Diagnostic diag{prevNode, Fmt(Err(Err0585), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), prevNode->token.ctext(), symbol->name.c_str())};
            diag.addRange(identifier->token, Fmt(Nte(Nte0154), prevNode->typeInfo->getDisplayNameC()));
            Vector<const Diagnostic*> notes;
            notes.push_back(Diagnostic::note(Fmt(Nte(Nte0109), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), prevNode->token.ctext(), symbol->name.c_str())));
            notes.push_back(Diagnostic::hereIs(oneMatch.oneOverload->overload));
            notes.push_back(Diagnostic::note(Fmt(Nte(Nte0035), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), identifierRef->startScope->name.c_str())));
            return context->report(diag, notes);
        }

        Diagnostic diag{prevNode, Fmt(Err(Err0585), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), prevNode->token.ctext(), symbol->name.c_str())};
        diag.addRange(identifier->token, Fmt(Nte(Nte0154), prevNode->typeInfo->getDisplayNameC()));
        return context->report(diag, Diagnostic::hereIs(oneMatch.oneOverload->overload));
    }

    // A.X and A is an array : missing index
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        identifier->typeInfo->isArray() &&
        identifier->parent->kind != AstNodeKind::ArrayPointerIndex &&
        identifier->parent == identifierRef &&
        identifierRef->childs.back() != identifier)
    {
        return context->report({identifier, Fmt(Err(Err0548), symbol->name.c_str(), identifier->typeInfo->getDisplayNameC())});
    }

    // A.X and A is a slice : missing index
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        identifier->typeInfo->isSlice() &&
        identifier->parent->kind != AstNodeKind::ArrayPointerIndex &&
        identifier->parent == identifierRef &&
        identifierRef->childs.back() != identifier)
    {
        return context->report({identifier, Fmt(Err(Err0549), symbol->name.c_str(), identifier->typeInfo->getDisplayNameC())});
    }

    // Reapply back the values of the match to the call parameter node
    for (auto& pp : oneMatch.paramParameters)
    {
        pp.param->indexParam        = pp.indexParam;
        pp.param->resolvedParameter = pp.resolvedParameter;
    }

    if (prevNode && symbol->kind == SymbolKind::Variable)
    {
        identifier->flags |= AST_L_VALUE;

        // Direct reference to a constexpr typeinfo
        if (prevNode->hasComputedValue())
        {
            if (prevNode->typeInfo->isStruct() || prevNode->typeInfo->isPointer())
            {
                auto ptr = (uint8_t*) prevNode->computedValue->getStorageAddr();
                if (derefConstant(context, ptr, overload, prevNode->computedValue->storageSegment))
                {
                    prevNode->flags |= AST_NO_BYTECODE;
                    identifierRef->previousResolvedNode = context->node;
                    identifier->resolvedSymbolName      = overload->symbol;
                    identifier->resolvedSymbolOverload  = overload;
                    return true;
                }
            }
        }

        // Direct reference of a struct field inside a const array
        if (prevNode->kind == AstNodeKind::ArrayPointerIndex && prevNode->typeInfo->isStruct())
        {
            auto arrayNode = CastAst<AstArrayPointerIndex>(prevNode, AstNodeKind::ArrayPointerIndex);
            auto arrayOver = arrayNode->array->resolvedSymbolOverload;
            if (arrayOver && (arrayOver->flags & OVERLOAD_COMPUTED_VALUE) && arrayNode->access->hasComputedValue())
            {
                auto typePtr = CastTypeInfo<TypeInfoArray>(arrayNode->array->typeInfo, TypeInfoKind::Array);
                auto ptr     = (uint8_t*) arrayOver->computedValue.getStorageAddr();
                ptr += arrayNode->access->computedValue->reg.u64 * typePtr->finalType->sizeOf;
                if (derefConstant(context, ptr, overload, arrayOver->computedValue.storageSegment))
                {
                    prevNode->flags |= AST_NO_BYTECODE;
                    identifierRef->previousResolvedNode = context->node;
                    identifier->resolvedSymbolName      = overload->symbol;
                    identifier->resolvedSymbolOverload  = overload;
                    return true;
                }
            }
        }
    }

    // If this a L or R value
    if (overload->flags & OVERLOAD_VAR_STRUCT)
    {
        if (symbol->kind != SymbolKind::GenericType)
        {
            if (identifierRef->previousResolvedNode)
            {
                if (identifierRef->previousResolvedNode->flags & AST_R_VALUE)
                    identifier->flags |= AST_L_VALUE | AST_R_VALUE;
                else
                    identifier->flags |= (identifierRef->previousResolvedNode->flags & AST_L_VALUE);
            }
        }
    }
    else if (symbol->kind == SymbolKind::Variable)
    {
        identifier->flags |= AST_L_VALUE | AST_R_VALUE;
    }

    // Do not register a sub impl scope, for ufcs to use the real variable
    if (!(overload->flags & OVERLOAD_IMPL_IN_STRUCT))
    {
        identifierRef->resolvedSymbolName     = symbol;
        identifierRef->resolvedSymbolOverload = overload;
    }

    identifier->resolvedSymbolName     = symbol;
    identifier->resolvedSymbolOverload = overload;

    if (identifier->typeInfo->isGeneric())
        identifier->flags |= AST_IS_GENERIC;
    else if (overload->flags & OVERLOAD_GENERIC && !(identifier->flags & AST_FROM_GENERIC))
        identifier->flags |= AST_IS_GENERIC;

    // Symbol is linked to a using var : insert the variable name before the symbol
    // Except if symbol is a constant !
    if (!(overload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        if (dependentVar && identifierRef->kind == AstNodeKind::IdentifierRef && symbol->kind != SymbolKind::Function)
        {
            auto idRef = CastAst<AstIdentifierRef>(identifierRef, AstNodeKind::IdentifierRef);
            if (dependentVar->kind == AstNodeKind::IdentifierRef)
            {
                for (int i = (int) dependentVar->childs.size() - 1; i >= 0; i--)
                {
                    auto child  = dependentVar->childs[i];
                    auto idNode = Ast::newIdentifier(dependentVar->sourceFile, child->token.text, idRef, nullptr);
                    idNode->inheritOrFlag(idRef, AST_IN_MIXIN);
                    idNode->inheritTokenLocation(idRef);
                    idNode->allocateIdentifierExtension();
                    idNode->identifierExtension->fromAlternateVar = child;
                    Ast::addChildFront(idRef, idNode);
                    context->baseJob->nodes.push_back(idNode);
                    if (i == 0 && identifier->identifierExtension)
                    {
                        idNode->identifierExtension->scopeUpMode  = identifier->identifierExtension->scopeUpMode;
                        idNode->identifierExtension->scopeUpValue = identifier->identifierExtension->scopeUpValue;
                    }

                    idNode->addSpecFlags(AstIdentifier::SPECFLAG_FROM_USING);
                }
            }
            else
            {
                auto idNode = Ast::newIdentifier(dependentVar->sourceFile, dependentVar->token.text, idRef, nullptr);
                idNode->inheritOrFlag(idRef, AST_IN_MIXIN);
                idNode->inheritTokenLocation(identifier);

                // We need to insert at the right place, but the identifier 'childParentIdx' can be the wrong one
                // if it's not a direct child of 'idRef'. So we need to find the direct child of 'idRef', which is
                // also a parent if 'identifier', in order to get the right child index, and insert the 'using'
                // just before.
                AstNode* newParent = identifier;
                while (newParent->parent != idRef)
                    newParent = newParent->parent;

                idNode->addSpecFlags(AstIdentifier::SPECFLAG_FROM_USING);

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
            context->result              = ContextResult::NewChilds;
            return true;
        }
    }

    // If this is a typealias, find the right thing
    auto typeAlias  = identifier->typeInfo;
    auto symbolKind = symbol->kind;
    if (symbol->kind == SymbolKind::TypeAlias)
    {
        typeAlias = TypeManager::concreteType(identifier->typeInfo, CONCRETE_FORCEALIAS);
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
            Diagnostic diag{identifier, Err(Err0625)};
            return context->report(diag);
        }
    }

    auto idRef = identifier->identifierRef();
    switch (symbolKind)
    {
    case SymbolKind::GenericType:
        SWAG_CHECK(setupIdentifierRef(context, identifier));
        break;

    case SymbolKind::Namespace:
        identifierRef->startScope = CastTypeInfo<TypeInfoNamespace>(identifier->typeInfo, identifier->typeInfo->kind)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::Enum:
        SWAG_CHECK(setupIdentifierRef(context, identifier));
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::EnumValue:

        if (idRef &&
            idRef->previousResolvedNode &&
            idRef->previousResolvedNode->resolvedSymbolName->kind == SymbolKind::Variable)
        {
            Diagnostic diag{idRef->previousResolvedNode, Fmt(Err(Err0260), idRef->previousResolvedNode->typeInfo->getDisplayNameC())};
            return context->report(diag);
        }

        SWAG_CHECK(setupIdentifierRef(context, identifier));
        identifier->setFlagsValueIsComputed();
        identifier->flags |= AST_R_VALUE;
        *identifier->computedValue = identifier->resolvedSymbolOverload->computedValue;
        break;

    case SymbolKind::Struct:
    case SymbolKind::Interface:
    {
        if (!(overload->flags & OVERLOAD_IMPL_IN_STRUCT))
            SWAG_CHECK(setupIdentifierRef(context, identifier));
        identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(typeAlias, typeAlias->kind)->scope;

        if (!identifier->callParameters)
            identifier->flags |= AST_CONST_EXPR;

        // Be sure it's the NAME{} syntax
        if (identifier->callParameters &&
            !(identifier->flags & AST_GENERATED) &&
            !(identifier->callParameters->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
        {
            Diagnostic diag{identifier, identifier->token, Err(Err0377)};
            return context->report(diag);
        }

        // A struct with parameters is in fact the creation of a temporary variable
        bool canConvertStructParams = false;
        bool canOptimAffect         = false;
        if (identifier->callParameters &&
            !(identifier->flags & AST_GENERATED) &&
            !(identifier->flags & AST_IN_TYPE_VAR_DECLARATION) &&
            !(identifier->flags & AST_IN_FUNC_DECL_PARAMS))
        {
            canConvertStructParams = true;
            if (identifier->parent->parent->kind == AstNodeKind::VarDecl || identifier->parent->parent->kind == AstNodeKind::ConstDecl)
            {
                auto varNode = CastAst<AstVarDecl>(identifier->parent->parent, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
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
            auto maxParams = identifier->callParameters->childs.size();
            for (size_t i = 0; i < maxParams; i++)
            {
                auto   nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                size_t idx      = nodeCall->indexParam;
                if (idx < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[idx])
                {
                    uint64_t castFlags = CASTFLAG_TRY_COERCE | CASTFLAG_FORCE_UNCONST | CASTFLAG_PTR_REF;
                    if (canOptimAffect)
                        castFlags |= CASTFLAG_NO_TUPLE_TO_STRUCT;
                    SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[idx]->typeInfo, nullptr, nodeCall, castFlags));
                }
            }
        }

        // Optim if we have var = Struct{}
        // In that case, no need to generate a temporary variable. We just consider Struct{} as the type definition
        // of 'var'
        if (canOptimAffect)
        {
            auto varNode        = CastAst<AstVarDecl>(identifier->parent->parent, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
            auto typeNode       = Ast::newTypeExpression(sourceFile, varNode);
            varNode->type       = typeNode;
            varNode->assignment = nullptr;
            typeNode->addSpecFlags(AstType::SPECFLAG_HAS_STRUCT_PARAMETERS);
            typeNode->addSpecFlags(AstTypeExpression::SPECFLAG_DONE_GEN);
            identifier->semFlags |= SEMFLAG_ONCE;
            Ast::removeFromParent(identifier->parent);
            Ast::addChildBack(typeNode, identifier->parent);
            typeNode->identifier = identifier->parent;
            context->baseJob->nodes.pop_back();
            context->baseJob->nodes.pop_back();
            context->baseJob->nodes.push_back(typeNode);
            context->result = ContextResult::NewChilds;
            return true;
        }

        // A struct with parameters is in fact the creation of a temporary variable
        if (canConvertStructParams)
        {
            if (identifier->parent->parent->kind != AstNodeKind::TypeExpression ||
                !(identifier->parent->parent->specFlags & AstTypeExpression::SPECFLAG_DONE_GEN))
            {
                SWAG_CHECK(Ast::convertStructParamsToTmpVar(context, identifier));
                return true;
            }
        }

        break;
    }

    case SymbolKind::Variable:
    {
        // If this is a struct variable, and it's referenced (identifierRef has a startScope), then we
        // wait for the struct container to be solved. We do not want the semantic to continue with
        // an unsolved struct, because that means that storageOffset has not been computed yet
        // (and in some cases we can go to the bytecode generation with the struct not solved).
        if (overload->flags & OVERLOAD_VAR_STRUCT && identifier->identifierRef()->startScope)
        {
            auto parentStructNode = identifier->identifierRef()->startScope->owner;
            if (parentStructNode->resolvedSymbolOverload)
            {
                Semantic::waitOverloadCompleted(context->baseJob, parentStructNode->resolvedSymbolOverload);
                YIELD();
            }
        }

        overload->flags |= OVERLOAD_USED;

        // Be sure usage is valid
        auto ownerFct = identifier->ownerFct;
        if (ownerFct)
        {
            auto fctAttributes = ownerFct->attributeFlags;
            if (!(fctAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER) && !(ownerFct->flags & AST_IN_RUN_BLOCK))
            {
                Diagnostic diag{identifier, Fmt(Err(Err0175), Naming::kindName(overload->node).c_str(), overload->node->token.ctext(), ownerFct->token.ctext())};
                auto       note = Diagnostic::note(overload->node, overload->node->token, Fmt(Nte(Nte0147), Naming::kindName(overload->node).c_str()));
                return context->report(diag, note);
            }
        }

        // Transform the variable to a constant node
        if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        {
            if (overload->node->isConstantGenTypeInfo())
                identifier->flags |= AST_VALUE_IS_GEN_TYPEINFO;
            identifier->setFlagsValueIsComputed();
            *identifier->computedValue = overload->computedValue;

            // If constant is inside an expression, everything before should be constant too.
            // Otherwise that means that we reference a constant threw a variable
            //
            // var x = y.Constant where y is a variable
            //
            auto checkParent = identifier->parent;
            auto child       = (AstNode*) identifier;
            while (checkParent->kind == AstNodeKind::ArrayPointerIndex ||
                   checkParent->kind == AstNodeKind::ArrayPointerSlicing)
            {
                child       = checkParent;
                checkParent = checkParent->parent;
            }

            // In that case, we should not generate bytecode for everything before the constant,
            // as we have the scope, and this is enough.
            SWAG_ASSERT(checkParent->kind == AstNodeKind::IdentifierRef);
            auto childIdx = child->childParentIdx();
            for (uint32_t i = 0; i < childIdx; i++)
            {
                auto brother = checkParent->childs[i];
                if (!(brother->hasComputedValue()) &&
                    brother->resolvedSymbolOverload &&
                    brother->resolvedSymbolOverload->symbol->kind == SymbolKind::Variable)
                {
                    brother->flags |= AST_NO_BYTECODE;
                }
            }
        }

        // Setup parent if necessary
        SWAG_CHECK(setupIdentifierRef(context, identifier));

        auto typeInfo = TypeManager::concretePtrRefType(identifier->typeInfo);

        // If this is a 'code' variable, when passing code from one macro to another, then do not generate bytecode
        // for it, as this is not really a variable
        if (typeInfo->isCode())
            identifier->flags |= AST_NO_BYTECODE;

        // Lambda call
        if (typeInfo->isLambdaClosure() && identifier->callParameters)
        {
            auto typeInfoRet = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::LambdaClosure)->returnType;
            typeInfoRet      = TypeManager::concreteType(typeInfoRet, CONCRETE_FORCEALIAS);

            // Check return value
            if (!typeInfoRet->isVoid())
            {
                if (isStatementIdentifier(identifier))
                {
                    if (!(overload->node->attributeFlags & ATTRIBUTE_DISCARDABLE) && !(identifier->flags & AST_DISCARD))
                    {
                        Diagnostic diag(identifier, identifier->token, Fmt(Err(Err0749), overload->node->token.ctext()));
                        return context->report(diag, Diagnostic::hereIs(overload));
                    }
                    else
                    {
                        identifier->flags |= AST_DISCARD;
                    }
                }
            }
            else if (typeInfoRet->isVoid() && (identifier->flags & AST_DISCARD))
            {
                Diagnostic diag{identifier, identifier->token, Err(Err0158)};
                return context->report(diag, Diagnostic::hereIs(overload));
            }

            // From now this is considered as a function, not a lambda
            auto funcType           = (TypeInfoFuncAttr*) typeInfo->clone();
            funcType->kind          = TypeInfoKind::FuncAttr;
            identifier->typeInfo    = funcType;
            identifier->byteCodeFct = ByteCodeGen::emitLambdaCall;

            // Try/Assume
            if (identifier->hasExtOwner() &&
                identifier->extOwner()->ownerTryCatchAssume &&
                (identifier->typeInfo->flags & TYPEINFO_CAN_THROW))
            {
                switch (identifier->extOwner()->ownerTryCatchAssume->kind)
                {
                case AstNodeKind::Try:
                    identifier->setBcNotifAfter(ByteCodeGen::emitTry);
                    break;
                case AstNodeKind::TryCatch:
                    identifier->setBcNotifAfter(ByteCodeGen::emitTryCatch);
                    break;
                case AstNodeKind::Catch:
                    identifier->setBcNotifAfter(ByteCodeGen::emitCatch);
                    break;
                case AstNodeKind::Assume:
                    identifier->setBcNotifAfter(ByteCodeGen::emitAssume);
                    break;
                default:
                    break;
                }
            }

            // Need to make all types compatible, in case a cast is necessary
            SWAG_CHECK(setSymbolMatchCallParams(context, identifier, oneMatch));

            // For a return by copy, need to reserve room on the stack for the return result
            if (CallConv::returnNeedsStack(funcType))
            {
                identifier->flags |= AST_TRANSIENT;
                allocateOnStack(identifier, funcType->concreteReturnType());
            }
        }
        else
        {
            if (isStatementIdentifier(identifier))
            {
                Diagnostic diag{idRef, Err(Err0625)};
                return context->report(diag);
            }
        }

        break;
    }

    case SymbolKind::Function:
    {
        auto funcDecl = CastAst<AstFuncDecl>(overload->node, AstNodeKind::FuncDecl);

        // Be sure that we didn't use a variable as a 'scope'
        auto childIdx = identifier->childParentIdx();
        if (childIdx)
        {
            auto prev = identifier->identifierRef()->childs[childIdx - 1];
            if (prev->resolvedSymbolName && prev->resolvedSymbolName->kind == SymbolKind::Variable && !(prev->flags & AST_FROM_UFCS))
            {
                Diagnostic diag{prev, Fmt(Err(Err0585), Naming::kindName(prev->resolvedSymbolOverload->node).c_str(), prev->token.ctext(), identifier->token.ctext())};
                diag.addRange(identifier->token, Fmt(Nte(Nte0154), prev->typeInfo->getDisplayNameC()));
                return context->report(diag, Diagnostic::hereIs(funcDecl));
            }
        }

        identifier->flags |= AST_SIDE_EFFECTS;

        // Be sure it's () and not {}
        if (identifier->callParameters && (identifier->callParameters->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
            return context->report({identifier->callParameters, Fmt(Err(Err0291), identifier->token.ctext())});

        // Capture syntax
        if (identifier->callParameters && !identifier->callParameters->aliasNames.empty())
        {
            if (!(overload->node->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN)))
            {
                auto       cp = identifier->callParameters;
                Diagnostic diag{cp->sourceFile, cp->aliasNames.front().startLocation, cp->aliasNames.back().endLocation, Fmt(Err(Err0675), identifier->token.ctext())};
                diag.addRange(identifier->token, Nte(Nte0155));
                return context->report(diag);
            }
        }

        // Now we need to be sure that the function is now complete
        // If not, we need to wait for it
        Semantic::waitOverloadCompleted(context->baseJob, overload);
        YIELD();

        if (identifier->token.text == g_LangSpec->name_opInit)
            return context->report({identifier, identifier->token, Err(Err0109)});
        if (identifier->token.text == g_LangSpec->name_opDrop)
            return context->report({identifier, identifier->token, Err(Err0108)});
        if (identifier->token.text == g_LangSpec->name_opPostCopy)
            return context->report({identifier, identifier->token, Err(Err0110)});
        if (identifier->token.text == g_LangSpec->name_opPostMove)
            return context->report({identifier, identifier->token, Err(Err0111)});

        // Be sure this is not a 'forward' decl
        if (funcDecl->isEmptyFct() && !funcDecl->isForeign() && identifier->token.text[0] != '@')
        {
            Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0292), identifier->token.ctext())};
            return context->report(diag, Diagnostic::hereIs(overload));
        }

        if (identifier->callParameters)
            identifier->flags |= AST_L_VALUE | AST_R_VALUE;
        else if (identifier->parent->parent->kind == AstNodeKind::MakePointerLambda)
            identifier->flags |= AST_L_VALUE | AST_R_VALUE;
        else if (identifier->parent->parent->kind == AstNodeKind::MakePointer)
            identifier->flags |= AST_L_VALUE | AST_R_VALUE;

        // Need to make all types compatible, in case a cast is necessary
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            SWAG_CHECK(setSymbolMatchCallParams(context, identifier, oneMatch));
            YIELD();
        }

        // Be sure the call is valid
        if ((identifier->token.text[0] != '@') && !funcDecl->isForeign())
        {
            auto ownerFct = identifier->ownerFct;
            if (ownerFct)
            {
                auto fctAttributes = ownerFct->attributeFlags;

                if (!(fctAttributes & ATTRIBUTE_COMPILER) && (funcDecl->attributeFlags & ATTRIBUTE_COMPILER) && !(identifier->flags & AST_IN_RUN_BLOCK))
                {
                    Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0176), funcDecl->token.ctext(), ownerFct->token.ctext())};
                    auto       note = Diagnostic::note(overload->node, overload->node->token, Nte(Nte0156));
                    return context->report(diag, note);
                }
            }
        }

        if (identifier->forceTakeAddress())
        {
            // This is for a lambda
            if (identifier->parent->parent->kind == AstNodeKind::MakePointer || identifier->parent->parent->kind == AstNodeKind::MakePointerLambda)
            {
                if (!identifier->callParameters)
                {
                    // The makePointer will deal with the real make lambda thing
                    identifier->flags |= AST_NO_BYTECODE;
                    break;
                }
            }
        }

        // The function call is constexpr if the function is, and all parameters are
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
        if (identifier->resolvedSymbolOverload->node->flags & AST_CONST_EXPR)
        {
            if (identifier->callParameters)
                identifier->inheritAndFlag1(identifier->callParameters, AST_CONST_EXPR);
            else
                identifier->flags |= AST_CONST_EXPR;

            // Be sure that the return type is compatible with a compile time execution.
            // Otherwise, we do not want the AST_CONST_EXPR_FLAG
            if (identifier->flags & AST_CONST_EXPR)
            {
                auto returnType = typeFunc->concreteReturnType();

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
                    !(identifier->semFlags & SEMFLAG_EXEC_RET_STACK))
                {
                    identifier->flags &= ~AST_CONST_EXPR;
                }
            }
        }

        auto returnType = TypeManager::concreteType(identifier->typeInfo);

        // Check return value
        if (!returnType->isVoid())
        {
            if (isStatementIdentifier(identifier))
            {
                if (!(funcDecl->attributeFlags & ATTRIBUTE_DISCARDABLE) && !(identifier->flags & AST_DISCARD))
                {
                    Diagnostic diag(identifier, identifier->token, Fmt(Err(Err0747), overload->node->token.ctext()));
                    return context->report(diag, Diagnostic::hereIs(overload));
                }
                else
                {
                    identifier->flags |= AST_DISCARD;
                }
            }
        }
        else if (returnType->isVoid() && (identifier->flags & AST_DISCARD))
        {
            Diagnostic diag{identifier, identifier->token, Err(Err0158)};
            return context->report(diag, Diagnostic::hereIs(overload));
        }

        if (funcDecl->mustInline() && !isFunctionButNotACall(context, identifier, overload->symbol))
        {
            // Mixin and macros must be inlined here, because no call is possible
            bool forceInline = false;
            if (funcDecl->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
                forceInline = true;

            if (!(identifier->specFlags & AstIdentifier::SPECFLAG_NO_INLINE) || forceInline)
            {
                // Expand inline function. Do not expand an inline call inside a function marked as inline.
                // The expansion will be done at the lowest level possible
                if (!identifier->ownerFct || !identifier->ownerFct->mustInline() || forceInline)
                {
                    // Need to wait for function full semantic resolve
                    Semantic::waitFuncDeclFullResolve(context->baseJob, funcDecl);
                    YIELD();

                    // First pass, we inline the function.
                    // The identifier for the function call will be reresolved later when the content
                    // of the inline os done.
                    if (!(identifier->flags & AST_INLINED))
                    {
                        identifier->flags |= AST_INLINED;

                        // In case of an inline call inside an inline function, the identifier kind has been changed to
                        // AstNodeKind::FuncCall in the original function. So we restore it to be a simple identifier.
                        identifier->kind = AstNodeKind::Identifier;

                        SWAG_CHECK(makeInline(context, funcDecl, identifier));
                    }
                    else
                    {
                        SWAG_CHECK(setupIdentifierRef(context, identifier));
                    }

                    identifier->byteCodeFct = ByteCodeGen::emitPassThrough;
                    return true;
                }
            }
        }

        identifier->kind = AstNodeKind::FuncCall;

        // @print behaves like a normal function, so we want an emitCall in that case
        if (identifier->token.text[0] == '@' && identifier->tokenId != TokenId::IntrinsicPrint)
        {
            dealWithIntrinsic(context, identifier);
            identifier->byteCodeFct = ByteCodeGen::emitIntrinsic;
        }
        else if (funcDecl->isForeign())
            identifier->byteCodeFct = ByteCodeGen::emitForeignCall;
        else
            identifier->byteCodeFct = ByteCodeGen::emitCall;

        // Try/Assume
        if (identifier->hasExtOwner() &&
            identifier->extOwner()->ownerTryCatchAssume &&
            (identifier->typeInfo->flags & TYPEINFO_CAN_THROW))
        {
            switch (identifier->extOwner()->ownerTryCatchAssume->kind)
            {
            case AstNodeKind::Try:
                identifier->setBcNotifAfter(ByteCodeGen::emitTry);
                break;
            case AstNodeKind::TryCatch:
                identifier->setBcNotifAfter(ByteCodeGen::emitTryCatch);
                break;
            case AstNodeKind::Catch:
                identifier->setBcNotifAfter(ByteCodeGen::emitCatch);
                break;
            case AstNodeKind::Assume:
                identifier->setBcNotifAfter(ByteCodeGen::emitAssume);
                break;
            default:
                break;
            }
        }

        // Setup parent if necessary
        if (returnType->isStruct())
        {
            identifier->semFlags |= SEMFLAG_IS_CONST_ASSIGN_INHERIT;
            identifier->semFlags |= SEMFLAG_IS_CONST_ASSIGN;
        }

        SWAG_CHECK(setupIdentifierRef(context, identifier));

        // For a return by copy, need to reserve room on the stack for the return result
        // Order is important, because otherwhise this could call isPlainOldData, which could be not resolved
        if (CallConv::returnNeedsStack(typeFunc))
        {
            identifier->flags |= AST_TRANSIENT;
            allocateOnStack(identifier, returnType);
        }

        break;
    }
    default:
        break;
    }

    return true;
}

bool Semantic::matchIdentifierParameters(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node, uint32_t flags)
{
    bool  justCheck        = flags & MIP_JUST_CHECK;
    auto  job              = context->baseJob;
    auto& matches          = context->cacheMatches;
    auto& genericMatches   = context->cacheGenericMatches;
    auto& genericMatchesSI = context->cacheGenericMatchesSI;

    context->clearMatch();
    context->clearGenericMatch();
    context->bestSignatureInfos.clear();

    bool forStruct = false;
    for (auto oneMatch : overloads)
    {
        auto& oneOverload       = *oneMatch;
        auto  genericParameters = oneOverload.genericParameters;
        auto  callParameters    = oneOverload.callParameters;
        auto  dependentVar      = oneOverload.dependentVar;
        auto  overload          = oneOverload.overload;
        auto  symbol            = overload->symbol;

        if (oneOverload.symMatchContext.parameters.empty() && oneOverload.symMatchContext.genericParameters.empty())
        {
            auto symbolKind = symbol->kind;

            // For everything except functions/attributes/structs (which have overloads), this is a match
            if (symbolKind != SymbolKind::Attribute &&
                symbolKind != SymbolKind::Function &&
                symbolKind != SymbolKind::Struct &&
                symbolKind != SymbolKind::Interface &&
                !overload->typeInfo->isLambdaClosure())
            {
                auto match              = context->getOneMatch();
                match->symbolOverload   = overload;
                match->scope            = oneMatch->scope;
                match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
                match->dependentVar     = dependentVar;
                match->ufcs             = oneOverload.ufcs;
                matches.push_back(match);
                continue;
            }
        }

        // Major contention offender.
        // Do it in two passes, as most of the time if will fail
        {
            bool needLock = false;

            {
                SharedLock ls(symbol->mutex);
                if ((symbol->kind != SymbolKind::Function || symbol->cptOverloadsInit != symbol->overloads.size()) && symbol->cptOverloads)
                    needLock = true;
            }

            if (needLock)
            {
                ScopedLock ls(symbol->mutex);
                if ((symbol->kind != SymbolKind::Function || symbol->cptOverloadsInit != symbol->overloads.size()) && symbol->cptOverloads)
                {
                    Semantic::waitSymbolNoLock(job, symbol);
                    return true;
                }
            }
        }

        auto rawTypeInfo = overload->typeInfo;

        // :DupGen
        TypeInfo* typeWasForced = nullptr;
        if (node && node->parent && node->parent->inSimpleReturn())
        {
            rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_FORCEALIAS);
            if (rawTypeInfo->isStruct())
            {
                auto fctTypeInfo = CastTypeInfo<TypeInfoFuncAttr>(node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
                if (fctTypeInfo->returnType && fctTypeInfo->returnType->isStruct())
                {
                    auto rawTypeStruct    = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                    auto returnStructType = CastTypeInfo<TypeInfoStruct>(fctTypeInfo->returnType, TypeInfoKind::Struct);
                    if (returnStructType->genericParameters.size() == rawTypeStruct->genericParameters.size() && rawTypeStruct->genericParameters.size())
                    {
                        rawTypeStruct = (TypeInfoStruct*) rawTypeInfo->clone();
                        rawTypeInfo   = rawTypeStruct;
                        typeWasForced = rawTypeInfo;
                        for (size_t i = 0; i < returnStructType->genericParameters.size(); i++)
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
            rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_FORCEALIAS);
            if (rawTypeInfo->isStruct())
            {
                auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                if (!typeInfo->isGeneric())
                    oneOverload.symMatchContext.matchFlags |= SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC;
            }
        }

        // This way, a special cast can be done for the first parameter of a function
        if (oneOverload.ufcs)
            oneOverload.symMatchContext.matchFlags |= SymbolMatchContext::MATCH_UFCS;

        // We collect type replacements depending on where the identifier is
        Generic::setContextualGenericTypeReplacement(context, oneOverload, overload, flags);

        oneOverload.symMatchContext.semContext = context;
        context->castFlagsResult               = 0;

        if (rawTypeInfo->isStruct())
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
            Match::match(typeInfo, oneOverload.symMatchContext);
            YIELD();
        }
        else if (rawTypeInfo->isInterface())
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Interface);
            Match::match(typeInfo, oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->isFuncAttr())
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr);
            Match::match(typeInfo, oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->isLambdaClosure())
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::LambdaClosure);
            Match::match(typeInfo, oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->isKindGeneric())
        {
            oneOverload.symMatchContext.result = MatchResult::Ok;
        }
        else if (rawTypeInfo->isArray())
        {
            auto typeArr   = CastTypeInfo<TypeInfoArray>(rawTypeInfo, TypeInfoKind::Array);
            auto typeFinal = TypeManager::concreteType(typeArr->finalType);
            auto typeInfo  = CastTypeInfo<TypeInfoFuncAttr>(typeFinal, TypeInfoKind::LambdaClosure);
            Match::match(typeInfo, oneOverload.symMatchContext);
        }
        else
        {
            SWAG_ASSERT(false);
        }

        YIELD();

        // For a function, sometime, we do not want call parameters
        bool forcedFine = false;

        // Be sure this is not because of a generic error
        if (oneOverload.symMatchContext.result != MatchResult::NotEnoughGenericParameters &&
            oneOverload.symMatchContext.result != MatchResult::TooManyGenericParameters &&
            oneOverload.symMatchContext.result != MatchResult::BadGenericSignature)
        {
            if (isFunctionButNotACall(context, node, symbol))
            {
                oneOverload.symMatchContext.result = MatchResult::Ok;
                forcedFine                         = true;
            }
        }

        // We need () for a function call !
        bool emptyParams = oneOverload.symMatchContext.parameters.empty() && !callParameters;
        if (!forcedFine && emptyParams && oneOverload.symMatchContext.result == MatchResult::Ok && symbol->kind == SymbolKind::Function)
        {
            oneOverload.symMatchContext.result = MatchResult::MissingParameters;
        }

        // Function type without call parameters
        if (emptyParams && oneOverload.symMatchContext.result == MatchResult::NotEnoughParameters)
        {
            if (symbol->kind == SymbolKind::Variable)
                oneOverload.symMatchContext.result = MatchResult::Ok;
        }

        // In case of errors, remember the 'best' signature in order to generate the best possible
        // accurate error. We find the longest match (the one that failed on the right most parameter)
        if (oneOverload.symMatchContext.result != MatchResult::Ok)
        {
            if (context->bestSignatureInfos.badSignatureParameterIdx == -1 ||
                (oneOverload.symMatchContext.badSignatureInfos.badSignatureParameterIdx > context->bestSignatureInfos.badSignatureParameterIdx))
            {
                context->bestMatchResult    = oneOverload.symMatchContext.result;
                context->bestSignatureInfos = oneOverload.symMatchContext.badSignatureInfos;
                context->bestOverload       = overload;
            }
        }

        if (oneOverload.symMatchContext.result == MatchResult::Ok)
        {
            if (overload->flags & OVERLOAD_GENERIC)
            {
                // Be sure that we would like to instantiate in case we do not have user generic parameters.
                bool asMatch = false;
                if (!oneOverload.genericParameters && context->node->parent && context->node->parent->parent)
                {
                    auto grandParent = context->node->parent->parent;

                    bool isLast = false;
                    if (node && node->kind == AstNodeKind::Identifier)
                    {
                        auto id = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
                        if (id == id->identifierRef()->childs.back())
                            isLast = true;
                    }

                    if (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicTypeOf)
                        asMatch = true;
                    else if (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicKindOf)
                        asMatch = true;
                    else if (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicNameOf)
                        asMatch = true;
                    else if (isLast && grandParent->kind == AstNodeKind::BinaryOp && grandParent->tokenId == TokenId::SymEqualEqual && overload->symbol->kind == SymbolKind::Struct)
                        asMatch = true;
                    else if (isLast && grandParent->kind == AstNodeKind::BinaryOp && grandParent->tokenId == TokenId::SymExclamEqual && overload->symbol->kind == SymbolKind::Struct)
                        asMatch = true;
                    else if (grandParent->kind == AstNodeKind::IntrinsicDefined)
                        asMatch = true;
                }

                if (asMatch)
                {
                    auto match              = context->getOneMatch();
                    match->symbolOverload   = overload;
                    match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
                    match->dependentVar     = dependentVar;
                    match->ufcs             = oneOverload.ufcs;
                    match->oneOverload      = &oneOverload;
                    match->matchFlags       = oneOverload.symMatchContext.matchFlags;
                    match->castFlagsResult  = oneOverload.symMatchContext.castFlagsResult;
                    matches.push_back(match);
                    break;
                }

                auto* match                        = context->getOneGenericMatch();
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
                match->genericParameters           = genericParameters;
                match->numOverloadsWhenChecked     = oneOverload.cptOverloads;
                match->numOverloadsInitWhenChecked = oneOverload.cptOverloadsInit;
                match->ufcs                        = oneOverload.ufcs;
                if (overload->node->flags & AST_HAS_SELECT_IF && overload->node->kind == AstNodeKind::FuncDecl)
                    genericMatchesSI.push_back(match);
                else
                    genericMatches.push_back(match);
            }
            else
            {
                bool canRegisterInstance = true;

                // Be sure that we are not an identifier with generic parameters.
                // In that case we do not want to have instances.
                // Weird.
                if (node && node->kind == AstNodeKind::Identifier)
                {
                    auto id = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
                    if (id->genericParameters)
                    {
                        for (auto p : id->genericParameters->childs)
                        {
                            if (p->flags & AST_IS_GENERIC)
                            {
                                canRegisterInstance = false;
                                break;
                            }
                        }
                    }
                }

                if (canRegisterInstance)
                {
                    auto match              = context->getOneMatch();
                    match->symbolOverload   = overload;
                    match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
                    match->dependentVar     = dependentVar;
                    match->ufcs             = oneOverload.ufcs;
                    match->autoOpCast       = oneOverload.symMatchContext.autoOpCast;
                    match->oneOverload      = &oneOverload;
                    match->typeWasForced    = typeWasForced;
                    match->matchFlags       = oneOverload.symMatchContext.matchFlags;
                    match->castFlagsResult  = oneOverload.symMatchContext.castFlagsResult;

                    // As indexParam and resolvedParameter are directly stored in the node, we need to save them
                    // with the corresponding match, as they can be overwritten by another match attempt
                    for (auto p : oneOverload.symMatchContext.parameters)
                    {
                        if (p->kind != AstNodeKind::FuncCallParam)
                            continue;

                        OneMatch::ParamParameter pp;
                        auto                     param = CastAst<AstFuncCallParam>(p, AstNodeKind::FuncCallParam);

                        pp.param             = param;
                        pp.indexParam        = param->indexParam;
                        pp.resolvedParameter = param->resolvedParameter;
                        match->paramParameters.push_back(pp);
                    }

                    matches.push_back(match);
                }
            }
        }
    }

    auto prevMatchesCount = matches.size();
    SWAG_CHECK(filterMatches(context, matches));
    YIELD();
    SWAG_CHECK(filterMatchesInContext(context, matches));
    YIELD();
    SWAG_CHECK(filterGenericMatches(context, matches, genericMatches));
    YIELD();
    SWAG_CHECK(filterGenericMatches(context, matches, genericMatchesSI));
    YIELD();

    // If to match an instance, we always need an automatic opCast, then we only keep generic matches in order
    // to create an instance with the exact type.
    // We only test the first match here, because the filtering of matches would have remove it if some other instances
    // without autoOpCast are present.
    if (matches.size() > 0 && matches[0]->autoOpCast && (genericMatches.size() > 0 || genericMatchesSI.size() > 0))
    {
        prevMatchesCount = 0;
        matches.clear();
    }

    // All choices were removed because of #validif
    if (!genericMatches.size() && genericMatchesSI.size() && matches.empty() && prevMatchesCount)
    {
        if (justCheck)
            return false;
        return SemanticError::cannotMatchIdentifierError(context, overloads, node);
    }

    // Multi instantiation in case of #validif
    if (genericMatchesSI.size() && matches.empty() && !prevMatchesCount)
    {
        if (justCheck)
            return true;

        Set<SymbolName*> symbols;
        for (auto& g : genericMatchesSI)
            symbols.insert(g->symbolName);
        for (auto& g : symbols)
            g->mutex.lock();

        for (auto& g : genericMatchesSI)
        {
            Generic::checkCanInstantiateGenericSymbol(context, *g);
            if (context->result != ContextResult::Done)
                break;
        }

        if (context->result == ContextResult::Done)
        {
            for (auto& g : genericMatchesSI)
            {
                SWAG_CHECK(Generic::instantiateFunction(context, g->genericParameters, *g, true));
            }
        }

        for (auto& g : symbols)
            g->mutex.unlock();
        return true;
    }

    // This is a generic
    if (genericMatches.size() == 1 && matches.empty())
    {
        if (justCheck && !(flags & MIP_SECOND_GENERIC_TRY))
            return true;
        SWAG_CHECK(Generic::instantiateGenericSymbol(context, *genericMatches[0], forStruct));
        return true;
    }

    // Done !!!
    if (matches.size() == 1)
    {
        // We should have no symbol
        if (flags & MIP_FOR_ZERO_GHOSTING)
        {
            if (justCheck)
                return false;
            if (!node)
                node = context->node;

            auto symbol = overloads[0]->overload->symbol;
            auto match  = matches[0];
            return SemanticError::duplicatedSymbolError(context, node->sourceFile, node->token, symbol->kind, symbol->name, match->symbolOverload->symbol->kind, match->symbolOverload->node);
        }

        return true;
    }

    // Ambiguity with generics
    if (genericMatches.size() > 1)
    {
        if (justCheck)
            return false;
        return SemanticError::ambiguousGenericError(context, node, overloads, genericMatches);
    }

    // We remove all generated nodes, because if they exist, they do not participate to the
    // error
    auto oneTry = overloads[0];
    for (size_t i = 0; i < overloads.size(); i++)
    {
        if (overloads[i]->overload->node->flags & AST_FROM_GENERIC)
        {
            overloads[i] = overloads.back();
            overloads.pop_back();
            i--;
        }
    }

    // Be sure to have something. This should raise in case of internal error only, because
    // we must have at least the generic symbol
    if (overloads.empty())
        overloads.push_back(oneTry);

    // There's no match at all
    if (matches.size() == 0)
    {
        if (!(flags & MIP_SECOND_GENERIC_TRY))
        {
            VectorNative<OneTryMatch*> cpyOverloads;
            for (auto& oneMatch : overloads)
            {
                if (oneMatch->symMatchContext.result == MatchResult::NotEnoughGenericParameters &&
                    oneMatch->symMatchContext.genericParameters.empty())
                {
                    cpyOverloads.push_back(oneMatch);
                }
            }

            if (!cpyOverloads.empty())
            {
                auto result = matchIdentifierParameters(context, cpyOverloads, node, flags | MIP_JUST_CHECK | MIP_SECOND_GENERIC_TRY);
                if (result)
                    return true;
            }
        }

        if (justCheck)
            return false;

        return SemanticError::cannotMatchIdentifierError(context, overloads, node);
    }

    // There is more than one possible match, and this is an identifier for a name alias.
    // We are fine
    if (matches.size() > 1 &&
        node &&
        node->kind == AstNodeKind::Identifier &&
        (node->specFlags & AstIdentifier::SPECFLAG_NAME_ALIAS))
    {
        return true;
    }

    // There is more than one possible match
    if (matches.size() > 1)
    {
        if (justCheck)
            return false;
        return SemanticError::ambiguousOverloadError(context, node, overloads, matches, flags);
    }

    return true;
}
