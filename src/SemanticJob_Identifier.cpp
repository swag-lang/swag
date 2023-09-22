#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Workspace.h"
#include "Generic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Module.h"
#include "Report.h"
#include "LanguageSpec.h"
#include "Naming.h"

bool SemanticJob::resolveNameAlias(SemanticContext* context)
{
    auto node = CastAst<AstAlias>(context->node, AstNodeKind::NameAlias);
    auto back = node->childs.back();

    SWAG_ASSERT(node->resolvedSymbolName);
    SWAG_ASSERT(back->resolvedSymbolName);

    SWAG_CHECK(collectAttributes(context, node, nullptr));
    node->flags |= AST_NO_BYTECODE;

    // Constraints with alias on a variable
    if (back->resolvedSymbolName->kind == SymbolKind::Variable)
    {
        // alias x = struct.x is not possible
        if (back->kind == AstNodeKind::IdentifierRef)
        {
            int cptVar = 0;
            for (auto& c : back->childs)
            {
                if (c->resolvedSymbolName && c->resolvedSymbolName->kind == SymbolKind::Variable)
                {
                    SWAG_VERIFY(cptVar == 0, context->report({back, Err(Err0029), Nte(Nte1061)}));
                    cptVar++;
                }
            }
        }
    }

    if (back->resolvedSymbolName->kind != SymbolKind::Namespace &&
        back->resolvedSymbolName->kind != SymbolKind::Function &&
        back->resolvedSymbolName->kind != SymbolKind::Variable)
    {
        Diagnostic diag{back, Fmt(Err(Err0030), Naming::aKindName(back->resolvedSymbolName->kind).c_str())};
        diag.hint = Nte(Nte1075);

        Diagnostic* note = nullptr;
        if (back->resolvedSymbolName->kind == SymbolKind::Enum ||
            back->resolvedSymbolName->kind == SymbolKind::Interface ||
            back->resolvedSymbolName->kind == SymbolKind::TypeAlias ||
            back->resolvedSymbolName->kind == SymbolKind::Struct)
        {
            note = Diagnostic::note(node, node->kwdLoc, Fmt(Nte(Nte0125), Naming::aKindName(back->resolvedSymbolName->kind).c_str()));
        }

        return context->report(diag, note);
    }

    SWAG_CHECK(node->ownerScope->symTable.registerNameAlias(context, node, node->resolvedSymbolName, back->resolvedSymbolName, back->resolvedSymbolOverload));
    if (node->attributeFlags & ATTRIBUTE_PUBLIC)
        node->ownerScope->addPublicNode(node);
    return true;
}

bool SemanticJob::preResolveIdentifierRef(SemanticContext* context)
{
    auto node = CastAst<AstIdentifierRef>(context->node, AstNodeKind::IdentifierRef);

    // When duplicating an identifier ref, and solve it again, we need to be sure that all that
    // stuff is reset
    if (!(node->semFlags & SEMFLAG_TYPE_SOLVED))
    {
        node->typeInfo             = nullptr;
        node->previousResolvedNode = nullptr;
        node->startScope           = nullptr;
    }

    return true;
}

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto node      = static_cast<AstIdentifierRef*>(context->node);
    auto childBack = node->childs.back();

    // Keep resolution
    if (!node->typeInfo || !(node->semFlags & SEMFLAG_TYPE_SOLVED))
    {
        node->resolvedSymbolName     = childBack->resolvedSymbolName;
        node->resolvedSymbolOverload = childBack->resolvedSymbolOverload;
        node->typeInfo               = childBack->typeInfo;
    }

    node->token.text  = childBack->token.text;
    node->byteCodeFct = ByteCodeGenJob::emitIdentifierRef;

    // Flag inheritance
    node->flags |= AST_CONST_EXPR | AST_FROM_GENERIC_REPLACE;
    for (auto child : node->childs)
    {
        if (!(child->flags & AST_CONST_EXPR))
            node->flags &= ~AST_CONST_EXPR;
        if (!(child->flags & AST_FROM_GENERIC_REPLACE))
            node->flags &= ~AST_FROM_GENERIC_REPLACE;
        if (child->flags & AST_IS_GENERIC)
            node->flags |= AST_IS_GENERIC;
        if (child->flags & AST_IS_CONST)
            node->flags |= AST_IS_CONST;
    }

    if (childBack->hasComputedValue())
        node->inheritComputedValue(childBack);
    node->inheritOrFlag(childBack, AST_L_VALUE | AST_R_VALUE | AST_TRANSIENT | AST_VALUE_IS_GENTYPEINFO | AST_SIDE_EFFECTS);

    if (childBack->semFlags & SEMFLAG_IS_CONST_ASSIGN)
    {
        if (!childBack->typeInfo || !childBack->typeInfo->isPointerRef())
        {
            node->flags |= AST_IS_CONST;
        }
    }

    // Symbol is in fact a constant value : no need for bytecode
    if (node->resolvedSymbolOverload &&
        (node->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        if (!node->hasComputedValue())
        {
            node->setFlagsValueIsComputed();
            *node->computedValue = node->resolvedSymbolOverload->computedValue;
        }

        node->flags |= AST_NO_BYTECODE_CHILDS;

        // If literal is stored in a data segment, then it's still a left value (we can take the address for example)
        if (!node->computedValue->storageSegment || node->computedValue->storageOffset == 0xFFFFFFFF)
            node->flags &= ~AST_L_VALUE;
    }

    return true;
}

bool SemanticJob::setupIdentifierRef(SemanticContext* context, AstNode* node)
{
    // If type of previous one was const, then we force this node to be const (cannot change it)
    if (node->parent->typeInfo && node->parent->typeInfo->isConst())
        node->flags |= AST_IS_CONST;
    auto overload = node->resolvedSymbolOverload;
    if (overload && overload->flags & OVERLOAD_CONST_ASSIGN)
        node->semFlags |= SEMFLAG_IS_CONST_ASSIGN;

    if (node->parent->kind != AstNodeKind::IdentifierRef)
        return true;

    auto identifierRef = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);

    // If we cannot assign previous, and this was AST_IS_CONST_ASSIGN_INHERIT, then we cannot assign
    // this one either
    if (identifierRef->previousResolvedNode && (identifierRef->previousResolvedNode->semFlags & SEMFLAG_IS_CONST_ASSIGN_INHERIT))
    {
        node->semFlags |= SEMFLAG_IS_CONST_ASSIGN;
        node->semFlags |= SEMFLAG_IS_CONST_ASSIGN_INHERIT;
    }

    identifierRef->previousResolvedNode = node;
    identifierRef->startScope           = nullptr;

    auto typeInfo  = TypeManager::concreteType(node->typeInfo, CONCRETE_FUNC | CONCRETE_ALIAS);
    auto scopeType = TypeManager::concreteType(typeInfo, CONCRETE_FORCEALIAS);
    if (scopeType->isLambdaClosure())
    {
        auto funcType = CastTypeInfo<TypeInfoFuncAttr>(scopeType, TypeInfoKind::LambdaClosure);
        scopeType     = TypeManager::concreteType(funcType->returnType, CONCRETE_FORCEALIAS);
    }

    if (!(identifierRef->semFlags & SEMFLAG_TYPE_SOLVED))
        identifierRef->typeInfo = typeInfo;

    switch (scopeType->kind)
    {
    case TypeInfoKind::Enum:
    {
        identifierRef->startScope = CastTypeInfo<TypeInfoEnum>(scopeType, TypeInfoKind::Enum)->scope;
        node->typeInfo            = typeInfo;
        break;
    }
    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(scopeType, TypeInfoKind::Pointer);
        auto subType     = TypeManager::concreteType(typePointer->pointedType);
        if (subType->isStruct() || subType->isInterface())
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::TypeListArray:
    case TypeInfoKind::TypeListTuple:
        identifierRef->startScope = CastTypeInfo<TypeInfoList>(scopeType, scopeType->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Interface:
    case TypeInfoKind::Struct:
        identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(scopeType, scopeType->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(scopeType, TypeInfoKind::Array);
        auto subType   = TypeManager::concreteType(typeArray->finalType);
        if (subType->isStruct())
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(scopeType, TypeInfoKind::Slice);
        auto subType   = TypeManager::concreteType(typeSlice->pointedType);
        if (subType->isStruct())
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    default:
        break;
    }

    return true;
}

void SemanticJob::sortParameters(AstNode* allParams)
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

void SemanticJob::dealWithIntrinsic(SemanticContext* context, AstIdentifier* identifier)
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

void SemanticJob::resolvePendingLambdaTyping(AstFuncCallParam* nodeCall, OneMatch* oneMatch, int i)
{
    auto funcDecl = CastAst<AstFuncDecl>(nodeCall->typeInfo->declNode, AstNodeKind::FuncDecl);
    SWAG_ASSERT(!(funcDecl->flags & AST_IS_GENERIC));
    auto typeUndefinedFct = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    auto concreteType     = TypeManager::concreteType(oneMatch->solvedParameters[i]->typeInfo);
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
            undefinedType->name     = it->second->name;
            undefinedType->typeInfo = it->second;
        }
    }

    // Replace every types inside the function
    Ast::visit(funcDecl, [&](AstNode* p)
               {
                   auto it = typeDefinedFct->replaceTypes.find(p->token.text);
                   if (it == typeDefinedFct->replaceTypes.end())
                       return;
                   p->token.text = it->second->name;
                   if (p->resolvedSymbolOverload)
                       p->resolvedSymbolOverload->typeInfo = it->second;
                   p->typeInfo = it->second; });

    // Set return type
    if (typeUndefinedFct->returnType->isNative(NativeTypeKind::Undefined))
    {
        typeUndefinedFct->returnType = typeDefinedFct->returnType;
        if (funcDecl->returnType)
            funcDecl->returnType->typeInfo = typeDefinedFct->returnType;
    }

    typeUndefinedFct->computeName();

    // Wake up semantic lambda job
    SWAG_ASSERT(funcDecl->pendingLambdaJob);
    funcDecl->semFlags &= ~SEMFLAG_PENDING_LAMBDA_TYPING;
    g_ThreadMgr.addJob(funcDecl->pendingLambdaJob);
}

bool SemanticJob::setSymbolMatchCallParams(SemanticContext* context, AstIdentifier* identifier, OneMatch& oneMatch)
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
        identifier->specFlags |= AstIdentifier::SPECFLAG_CLOSURE_FIRST_PARAM;

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
            resolvePendingLambdaTyping(nodeCall, &oneMatch, (uint32_t) i);

        uint64_t castFlags = CASTFLAG_AUTO_OPCAST | CASTFLAG_ACCEPT_PENDING | CASTFLAG_PARAMS | CASTFLAG_PTR_REF | CASTFLAG_FOR_AFFECT | CASTFLAG_ACCEPT_MOVE_REF;
        if (i == 0 && oneMatch.ufcs)
            castFlags |= CASTFLAG_UFCS;

        TypeInfo* toType = nullptr;
        if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
        {
            PushErrCxtStep ec(context, typeInfoFunc->declNode, ErrCxtStepKind::HereIs, nullptr);
            context->castFlagsResult = 0;

            toType = oneMatch.solvedParameters[i]->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, toType, nullptr, nodeCall, castFlags));
            if (context->result != ContextResult::Done)
                return true;

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
                if (front->hasComputedValue())
                {
                    auto varNode = Ast::newVarDecl(sourceFile, Fmt("__7tmp_%d", g_UniqueID.fetch_add(1)), nodeCall);
                    varNode->inheritTokenLocation(nodeCall);
                    varNode->flags |= AST_GENERATED;
                    Ast::removeFromParent(front);
                    Ast::addChildBack(varNode, front);
                    varNode->assignment = front;

                    auto idNode = Ast::newIdentifierRef(sourceFile, varNode->token.text, nodeCall);
                    idNode->inheritTokenLocation(nodeCall);
                    idNode->flags |= AST_GENERATED;
                    Ast::removeFromParent(idNode);
                    Ast::addChildFront(nodeCall, idNode);

                    context->job->nodes.push_back(idNode);
                    context->job->nodes.push_back(varNode);
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
            if (context->result != ContextResult::Done)
                return true;
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
                context->job->nodes.push_back(idRef);
                context->job->nodes.push_back(varNode);

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
                    moveRefNode->semanticFct = SemanticJob::resolveMoveRef;
                    auto mkPtrNode           = Ast::newNode<AstMakePointer>(nullptr, AstNodeKind::MakePointer, sourceFile, moveRefNode);
                    mkPtrNode->flags |= AST_GENERATED;
                    mkPtrNode->semanticFct = SemanticJob::resolveMakePointer;
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
                context->job->nodes.push_back(newParam);
                context->job->nodes.push_back(varNode);

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
                context->job->nodes.push_back(newParam);
                context->job->nodes.push_back(varNode);

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

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* identifier, OneMatch& oneMatch)
{
    auto symbol       = oneMatch.symbolOverload->symbol;
    auto overload     = oneMatch.symbolOverload;
    auto dependentVar = oneMatch.dependentVar;
    auto sourceFile   = context->sourceFile;

    setIdentifierAccess(identifier, overload);

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
        Diagnostic diag{prevNode, Fmt(Err(Err0085), prevNode->token.ctext(), prevNode->typeInfo->getDisplayNameC())};
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
            Diagnostic                diag{prevNode, Fmt(Err(Err0310), prevNode->token.ctext(), symbol->name.c_str())};
            Vector<const Diagnostic*> notes;
            auto                      prevIdentifier = CastAst<AstIdentifier>(prevNode, AstNodeKind::Identifier);
            auto                      widthNode      = prevIdentifier->identifierExtension->fromAlternateVar;
            notes.push_back(Diagnostic::note(oneMatch.oneOverload->overload->node, Fmt(Nte(Nte1073), prevNode->typeInfo->getDisplayNameC())));
            notes.push_back(Diagnostic::hereIs(widthNode, false, true));
            notes.push_back(Diagnostic::note(Fmt(Nte(Nte1130), identifierRef->startScope->name.c_str())));
            return context->report(diag, notes);
        }

        if (oneMatch.oneOverload->scope == identifierRef->startScope)
        {
            Diagnostic diag{prevNode, Fmt(Err(Err0097), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), prevNode->token.ctext(), symbol->name.c_str())};
            diag.addRange(identifier->token, Fmt(Nte(Nte1073), prevNode->typeInfo->getDisplayNameC()));
            Vector<const Diagnostic*> notes;
            notes.push_back(Diagnostic::note(Fmt(Nte(Nte1001), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), prevNode->token.ctext(), symbol->name.c_str())));
            notes.push_back(Diagnostic::hereIs(oneMatch.oneOverload->overload));
            notes.push_back(Diagnostic::note(Fmt(Nte(Nte1080), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), identifierRef->startScope->name.c_str())));
            return context->report(diag, notes);
        }

        Diagnostic diag{prevNode, Fmt(Err(Err0097), Naming::kindName(prevNode->resolvedSymbolName->kind).c_str(), prevNode->token.ctext(), symbol->name.c_str())};
        diag.addRange(identifier->token, Fmt(Nte(Nte1073), prevNode->typeInfo->getDisplayNameC()));
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
        return context->report({identifier, Fmt(Err(Err0187), symbol->name.c_str(), identifier->typeInfo->getDisplayNameC())});
    }

    // A.X and A is a slice : missing index
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        identifier->typeInfo->isSlice() &&
        identifier->parent->kind != AstNodeKind::ArrayPointerIndex &&
        identifier->parent == identifierRef &&
        identifierRef->childs.back() != identifier)
    {
        return context->report({identifier, Fmt(Err(Err0180), symbol->name.c_str(), identifier->typeInfo->getDisplayNameC())});
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
                    context->job->nodes.push_back(idNode);
                    if (i == 0 && identifier->identifierExtension)
                    {
                        idNode->identifierExtension->scopeUpMode  = identifier->identifierExtension->scopeUpMode;
                        idNode->identifierExtension->scopeUpValue = identifier->identifierExtension->scopeUpValue;
                    }

                    idNode->specFlags |= AstIdentifier::SPECFLAG_FROM_USING;
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

                idNode->specFlags |= AstIdentifier::SPECFLAG_FROM_USING;

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
                context->job->nodes.push_back(idNode);
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

    // Global identifier call. Must be a call to a mixin function, that's it...
    auto idRef = identifier->identifierRef();
    if (idRef && idRef->specFlags & AstIdentifierRef::SPECFLAG_GLOBAL)
    {
        if (identifier->callParameters)
            return context->report({identifier, identifier->token, Fmt(Err(Err0087), identifier->token.ctext(), Naming::aKindName(symbolKind).c_str())});
        return context->report({identifier, Fmt(Err(Err0776), identifier->token.ctext(), Naming::aKindName(symbolKind).c_str())});
    }

    SWAG_CHECK(warnDeprecated(context, identifier));

    if (symbolKind != SymbolKind::Variable && symbolKind != SymbolKind::Function)
    {
        if (isStatementIdentifier(identifier))
        {
            Diagnostic diag{identifier, Fmt(Err(Err0096), Naming::kindName(identifier->resolvedSymbolName->kind).c_str(), identifier->token.ctext()), Nte(Nte1026)};
            return context->report(diag);
        }
    }

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
            Diagnostic diag{idRef->previousResolvedNode, Fmt(Err(Err0481), idRef->previousResolvedNode->typeInfo->getDisplayNameC())};
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
            Diagnostic diag{identifier, identifier->token, Err(Err1128)};
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
            typeNode->specFlags |= AstType::SPECFLAG_HAS_STRUCT_PARAMETERS;
            typeNode->specFlags |= AstTypeExpression::SPECFLAG_DONE_GEN;
            identifier->semFlags |= SEMFLAG_ONCE;
            Ast::removeFromParent(identifier->parent);
            Ast::addChildBack(typeNode, identifier->parent);
            typeNode->identifier = identifier->parent;
            context->job->nodes.pop_back();
            context->job->nodes.pop_back();
            context->job->nodes.push_back(typeNode);
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
            context->job->waitOverloadCompleted(parentStructNode->resolvedSymbolOverload);
            if (context->result == ContextResult::Pending)
                return true;
        }

        overload->flags |= OVERLOAD_USED;

        // Be sure usage is valid
        auto ownerFct = identifier->ownerFct;
        if (ownerFct)
        {
            auto fctAttributes = ownerFct->attributeFlags;
            if (!(fctAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER) && !(ownerFct->flags & AST_RUN_BLOCK))
            {
                Diagnostic diag{identifier, Fmt(Err(Err0091), Naming::kindName(overload->node).c_str(), overload->node->token.ctext(), ownerFct->token.ctext())};
                auto       note = Diagnostic::note(overload->node, overload->node->token, Fmt(Nte(Nte1070), Naming::kindName(overload->node).c_str()));
                return context->report(diag, note);
            }
        }

        // Transform the variable to a constant node
        if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        {
            if (overload->node->isConstantGenTypeInfo())
                identifier->flags |= AST_VALUE_IS_GENTYPEINFO;
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
                        Diagnostic diag(identifier, identifier->token, Fmt(Err(Err0092), overload->node->token.ctext()));
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
                Diagnostic diag{identifier, identifier->token, Err(Err0094)};
                return context->report(diag, Diagnostic::hereIs(overload));
            }

            // From now this is considered as a function, not a lambda
            auto funcType           = (TypeInfoFuncAttr*) typeInfo->clone();
            funcType->kind          = TypeInfoKind::FuncAttr;
            identifier->typeInfo    = funcType;
            identifier->byteCodeFct = ByteCodeGenJob::emitLambdaCall;

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
                Diagnostic diag{idRef, Err(Err0096)};
                diag.hint = Nte(Nte1026);
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
                Diagnostic diag{prev, Fmt(Err(Err0097), Naming::kindName(prev->resolvedSymbolOverload->node).c_str(), prev->token.ctext(), identifier->token.ctext())};
                diag.addRange(identifier->token, Fmt(Nte(Nte1073), prev->typeInfo->getDisplayNameC()));
                return context->report(diag, Diagnostic::hereIs(funcDecl));
            }
        }

        identifier->flags |= AST_SIDE_EFFECTS;

        // Be sure it's () and not {}
        if (identifier->callParameters && (identifier->callParameters->specFlags & AstFuncCallParams::SPECFLAG_CALL_FOR_STRUCT))
            return context->report({identifier->callParameters, Fmt(Err(Err0098), identifier->token.ctext())});

        // Capture syntax
        if (identifier->callParameters && !identifier->callParameters->aliasNames.empty())
        {
            if (!(overload->node->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN)))
            {
                auto       cp = identifier->callParameters;
                Diagnostic diag{cp->sourceFile, cp->aliasNames.front().startLocation, cp->aliasNames.back().endLocation, Fmt(Err(Err0099), identifier->token.ctext())};
                diag.addRange(identifier->token, Nte(Nte1078));
                return context->report(diag);
            }
        }

        // Now we need to be sure that the function is now complete
        // If not, we need to wait for it
        context->job->waitOverloadCompleted(overload);
        if (context->result == ContextResult::Pending)
            return true;

        if (identifier->token.text == g_LangSpec->name_opInit)
            return context->report({identifier, identifier->token, Err(Err0100)});
        if (identifier->token.text == g_LangSpec->name_opDrop)
            return context->report({identifier, identifier->token, Err(Err0101)});
        if (identifier->token.text == g_LangSpec->name_opPostCopy)
            return context->report({identifier, identifier->token, Err(Err0103)});
        if (identifier->token.text == g_LangSpec->name_opPostMove)
            return context->report({identifier, identifier->token, Err(Err0104)});

        // Be sure this is not a 'forward' decl
        if (funcDecl->isEmptyFct() && !funcDecl->isForeign() && identifier->token.text[0] != '@')
        {
            Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0105), identifier->token.ctext())};
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
            if (context->result != ContextResult::Done)
                return true;
        }

        // Be sure the call is valid
        if ((identifier->token.text[0] != '@') && !funcDecl->isForeign())
        {
            auto ownerFct = identifier->ownerFct;
            if (ownerFct)
            {
                auto fctAttributes = ownerFct->attributeFlags;

                if (!(fctAttributes & ATTRIBUTE_COMPILER) && (funcDecl->attributeFlags & ATTRIBUTE_COMPILER) && !(identifier->flags & AST_RUN_BLOCK))
                {
                    Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0107), funcDecl->token.ctext(), ownerFct->token.ctext())};
                    auto       note = Diagnostic::note(overload->node, overload->node->token, Nte(Nte1064));
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
                    Diagnostic diag(identifier, identifier->token, Fmt(Err(Err0109), overload->node->token.ctext()));
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
            Diagnostic diag{identifier, identifier->token, Err(Err0094)};
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
                    context->job->waitFuncDeclFullResolve(funcDecl);
                    if (context->result != ContextResult::Done)
                        return true;

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

                    identifier->byteCodeFct = ByteCodeGenJob::emitPassThrough;
                    return true;
                }
            }
        }

        identifier->kind = AstNodeKind::FuncCall;

        // @print behaves like a normal function, so we want an emitCall in that case
        if (identifier->token.text[0] == '@' && identifier->tokenId != TokenId::IntrinsicPrint)
        {
            dealWithIntrinsic(context, identifier);
            identifier->byteCodeFct = ByteCodeGenJob::emitIntrinsic;
        }
        else if (funcDecl->isForeign())
            identifier->byteCodeFct = ByteCodeGenJob::emitForeignCall;
        else
            identifier->byteCodeFct = ByteCodeGenJob::emitCall;

        // Try/Assume
        if (identifier->hasExtOwner() &&
            identifier->extOwner()->ownerTryCatchAssume &&
            (identifier->typeInfo->flags & TYPEINFO_CAN_THROW))
        {
            switch (identifier->extOwner()->ownerTryCatchAssume->kind)
            {
            case AstNodeKind::Try:
                identifier->setBcNotifAfter(ByteCodeGenJob::emitTry);
                break;
            case AstNodeKind::TryCatch:
                identifier->setBcNotifAfter(ByteCodeGenJob::emitTryCatch);
                break;
            case AstNodeKind::Assume:
                identifier->setBcNotifAfter(ByteCodeGenJob::emitAssume);
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

void SemanticJob::setupContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload, uint32_t flags)
{
    auto node = context->node;

    // Fresh start on generic types
    oneTryMatch.symMatchContext.genericReplaceTypes.clear();
    oneTryMatch.symMatchContext.mapGenericTypesIndex.clear();

    auto& toCheck = context->job->tmpNodes;
    toCheck.clear();

    // If we are inside a struct, then we can inherit the generic concrete types of that struct
    if (node->ownerStructScope)
        toCheck.push_back(node->ownerStructScope->owner);

    // If function A in a struct calls function B in the same struct, then we can inherit the match types of function A
    // when instantiating function B
    if (node->ownerFct && node->ownerStructScope && node->ownerFct->ownerStructScope == symOverload->node->ownerStructScope)
        toCheck.push_back(node->ownerFct);

    // We do not want function to deduce their generic type from context, as the generic type can be deduced from the
    // parameters
    if (node->ownerFct && !symOverload->typeInfo->isFuncAttr())
        toCheck.push_back(node->ownerFct);

    // Except for a second try
    if (node->ownerFct && symOverload->typeInfo->isFuncAttr() && flags & MIP_SECOND_GENERIC_TRY)
        toCheck.push_back(node->ownerFct);

    // With A.B form, we try to get generic parameters from A if they exist
    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
        if (identifier->identifierRef()->startScope)
            toCheck.push_back(identifier->identifierRef()->startScope->owner);
    }

    // Except that with using, B could be in fact in another struct than A.
    // In that case we have a dependentVar, so replace what needs to be replaced.
    // What a mess...
    if (oneTryMatch.dependentVarLeaf)
    {
        if (oneTryMatch.dependentVarLeaf->typeInfo && oneTryMatch.dependentVarLeaf->typeInfo->isStruct())
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(oneTryMatch.dependentVarLeaf->typeInfo, TypeInfoKind::Struct);
            toCheck.push_back(typeStruct->declNode);
        }
    }

    // Collect from the owner structure
    for (auto one : toCheck)
    {
        if (one->kind == AstNodeKind::FuncDecl)
        {
            auto nodeFunc = CastAst<AstFuncDecl>(one, AstNodeKind::FuncDecl);
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(nodeFunc->typeInfo, TypeInfoKind::FuncAttr);

            oneTryMatch.symMatchContext.genericReplaceTypes.reserve(typeFunc->replaceTypes.size());
            for (auto oneReplace : typeFunc->replaceTypes)
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceValues.reserve(typeFunc->replaceValues.size());
            for (auto oneReplace : typeFunc->replaceValues)
                oneTryMatch.symMatchContext.genericReplaceValues[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceFrom.reserve(typeFunc->replaceFrom.size());
            for (auto oneReplace : typeFunc->replaceFrom)
                oneTryMatch.symMatchContext.genericReplaceFrom[oneReplace.first] = oneReplace.second;
        }
        else if (one->kind == AstNodeKind::StructDecl)
        {
            auto nodeStruct = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(nodeStruct->typeInfo, TypeInfoKind::Struct);

            oneTryMatch.symMatchContext.genericReplaceTypes.reserve(typeStruct->replaceTypes.size());
            for (auto oneReplace : typeStruct->replaceTypes)
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceValues.reserve(typeStruct->replaceValues.size());
            for (auto oneReplace : typeStruct->replaceValues)
                oneTryMatch.symMatchContext.genericReplaceValues[oneReplace.first] = oneReplace.second;

            oneTryMatch.symMatchContext.genericReplaceFrom.reserve(typeStruct->replaceFrom.size());
            for (auto oneReplace : typeStruct->replaceFrom)
                oneTryMatch.symMatchContext.genericReplaceFrom[oneReplace.first] = oneReplace.second;
        }
    }
}

bool SemanticJob::isFunctionButNotACall(SemanticContext* context, AstNode* node, SymbolName* symbol)
{
    AstIdentifier* id = nullptr;
    if (node && node->kind == AstNodeKind::Identifier)
    {
        id = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (id != id->identifierRef()->childs.back())
            return false;
    }

    if (node && node->parent && node->parent->parent)
    {
        auto grandParent = node->parent->parent;
        if (symbol->kind == SymbolKind::Attribute)
        {
            if (grandParent->kind != AstNodeKind::AttrUse)
                return true;
        }
        else if (symbol->kind == SymbolKind::Function)
        {
            if (symbol->kind == SymbolKind::Function)
            {
                if (grandParent->kind == AstNodeKind::MakePointer && node == node->parent->childs.back())
                {
                    if (id && id->callParameters)
                        return false;
                    return true;
                }

                if (grandParent->kind == AstNodeKind::MakePointerLambda && node == node->parent->childs.back())
                    return true;
            }

            if (grandParent->kind == AstNodeKind::TypeAlias ||
                grandParent->kind == AstNodeKind::NameAlias ||
                (grandParent->kind == AstNodeKind::IntrinsicDefined) ||
                (grandParent->kind == AstNodeKind::IntrinsicLocation) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicStringOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicNameOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicRunes) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicTypeOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicDeclType) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicKindOf))
            {
                return true;
            }
        }
    }

    return false;
}

static bool areGenericReplaceTypesIdentical(TypeInfo* typeInfo, OneGenericMatch& match)
{
    if (typeInfo->kind != TypeInfoKind::FuncAttr)
        return false;

    auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr);
    if (match.genericReplaceTypes.size() != typeFunc->replaceTypes.size())
        return false;

    for (auto rt : match.genericReplaceTypes)
    {
        auto it = typeFunc->replaceTypes.find(rt.first);
        if (it == typeFunc->replaceTypes.end())
            return false;
        if (rt.second != it->second)
            return false;
    }

    return true;
}

bool SemanticJob::matchIdentifierParameters(SemanticContext* context, VectorNative<OneTryMatch*>& overloads, AstNode* node, uint32_t flags)
{
    bool  justCheck        = flags & MIP_JUST_CHECK;
    auto  job              = context->job;
    auto& matches          = job->cacheMatches;
    auto& genericMatches   = job->cacheGenericMatches;
    auto& genericMatchesSI = job->cacheGenericMatchesSI;

    job->clearMatch();
    job->clearGenericMatch();
    job->bestSignatureInfos.clear();

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
                auto match              = job->getOneMatch();
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
                    job->waitSymbolNoLock(symbol);
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
                    oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC;
            }
        }

        // This way, a special cast can be done for the first parameter of a function
        if (oneOverload.ufcs)
            oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_UFCS;

        // When in a @typeof or @kindof, if we specify a type without generic parameters, then
        // we want to match the generic version, and not an instantiated one
        if (context->node->parent && context->node->parent->parent)
        {
            if (context->node->parent->parent->kind == AstNodeKind::IntrinsicProp && context->node->parent->parent->tokenId == TokenId::IntrinsicTypeOf)
                oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC;
            if (context->node->parent->parent->kind == AstNodeKind::IntrinsicProp && context->node->parent->parent->tokenId == TokenId::IntrinsicKindOf)
                oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC;
        }

        // We collect type replacements depending on where the identifier is
        setupContextualGenericTypeReplacement(context, oneOverload, overload, flags);

        oneOverload.symMatchContext.semContext = context;
        context->castFlagsResult               = 0;

        if (rawTypeInfo->isStruct())
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
            typeInfo->match(oneOverload.symMatchContext);
            if (context->result == ContextResult::Pending)
                return true;
        }
        else if (rawTypeInfo->isInterface())
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Interface);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->isFuncAttr())
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->isLambdaClosure())
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::LambdaClosure);
            typeInfo->match(oneOverload.symMatchContext);
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
            typeInfo->match(oneOverload.symMatchContext);
        }
        else
        {
            SWAG_ASSERT(false);
        }

        if (context->result != ContextResult::Done)
            return true;

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
            if (job->bestSignatureInfos.badSignatureParameterIdx == -1 ||
                (oneOverload.symMatchContext.badSignatureInfos.badSignatureParameterIdx > job->bestSignatureInfos.badSignatureParameterIdx))
            {
                job->bestMatchResult    = oneOverload.symMatchContext.result;
                job->bestSignatureInfos = oneOverload.symMatchContext.badSignatureInfos;
                job->bestOverload       = overload;
            }
        }

        if (oneOverload.symMatchContext.result == MatchResult::Ok)
        {
            if (overload->flags & OVERLOAD_GENERIC)
            {
                // Be sure that we would like to instantiate
                bool asMatch = false;
                if (node && node->kind == AstNodeKind::Identifier)
                {
                    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
                    if (!identifier->genericParameters && node->parent && node->parent->parent)
                    {
                        auto grandParent = node->parent->parent;
                        if (grandParent->kind == AstNodeKind::IntrinsicDefined ||
                            (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicNameOf))
                            asMatch = true;
                    }
                }

                if (asMatch)
                {
                    auto match              = job->getOneMatch();
                    match->symbolOverload   = overload;
                    match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
                    match->dependentVar     = dependentVar;
                    match->ufcs             = oneOverload.ufcs;
                    match->secondTry        = oneOverload.secondTry;
                    match->oneOverload      = &oneOverload;
                    match->flags            = oneOverload.symMatchContext.flags;
                    matches.push_back(match);
                }
                else
                {
                    auto* match                        = job->getOneGenericMatch();
                    match->flags                       = oneOverload.symMatchContext.flags;
                    match->symbolName                  = symbol;
                    match->symbolOverload              = overload;
                    match->genericParametersCallTypes  = std::move(oneOverload.symMatchContext.genericParametersCallTypes);
                    match->genericParametersCallValues = std::move(oneOverload.symMatchContext.genericParametersCallValues);
                    match->genericParametersCallFrom   = std::move(oneOverload.symMatchContext.genericParametersCallFrom);
                    match->genericReplaceTypes         = std::move(oneOverload.symMatchContext.genericReplaceTypes);
                    match->genericReplaceValues        = std::move(oneOverload.symMatchContext.genericReplaceValues);
                    match->genericReplaceFrom          = std::move(oneOverload.symMatchContext.genericReplaceFrom);
                    match->genericParametersGenTypes   = std::move(oneOverload.symMatchContext.genericParametersGenTypes);
                    match->parameters                  = std::move(oneOverload.symMatchContext.parameters);
                    match->solvedParameters            = std::move(oneOverload.symMatchContext.solvedParameters);
                    match->genericParameters           = genericParameters;
                    match->numOverloadsWhenChecked     = oneOverload.cptOverloads;
                    match->numOverloadsInitWhenChecked = oneOverload.cptOverloadsInit;
                    match->secondTry                   = oneOverload.secondTry;
                    if (overload->node->flags & AST_HAS_SELECT_IF && overload->node->kind == AstNodeKind::FuncDecl)
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
                    auto match              = job->getOneMatch();
                    match->symbolOverload   = overload;
                    match->solvedParameters = std::move(oneOverload.symMatchContext.solvedParameters);
                    match->dependentVar     = dependentVar;
                    match->ufcs             = oneOverload.ufcs;
                    match->secondTry        = oneOverload.secondTry;
                    match->autoOpCast       = oneOverload.symMatchContext.autoOpCast;
                    match->oneOverload      = &oneOverload;
                    match->typeWasForced    = typeWasForced;
                    match->flags            = oneOverload.symMatchContext.flags;

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
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(filterMatchesInContext(context, matches));
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(filterGenericMatches(context, matches, genericMatches));
    if (context->result != ContextResult::Done)
        return true;
    SWAG_CHECK(filterGenericMatches(context, matches, genericMatchesSI));
    if (context->result != ContextResult::Done)
        return true;

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
        return cannotMatchIdentifierError(context, overloads, node);
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
            checkCanInstantiateGenericSymbol(context, *g);
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
        SWAG_CHECK(instantiateGenericSymbol(context, *genericMatches[0], forStruct));
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
            return duplicatedSymbolError(context, node->sourceFile, node->token, symbol->kind, symbol->name, match->symbolOverload->symbol->kind, match->symbolOverload->node);
        }

        return true;
    }

    // Ambiguity with generics
    if (genericMatches.size() > 1)
    {
        if (justCheck)
            return false;

        auto                      symbol  = overloads[0]->overload->symbol;
        auto                      errNode = node ? node : context->node;
        Diagnostic                diag{errNode, errNode->token, Fmt(Err(Err0115), Naming::kindName(symbol->kind).c_str(), symbol->name.c_str())};
        Vector<const Diagnostic*> notes;
        for (auto match : genericMatches)
        {
            auto overload = match->symbolOverload;
            auto couldBe  = Fmt("could be of type '%s'", overload->typeInfo->getDisplayNameC());

            VectorNative<TypeInfoParam*> params;
            for (auto og : match->genericReplaceTypes)
            {
                auto p      = g_TypeMgr->makeParam();
                p->name     = og.first.c_str();
                p->typeInfo = og.second;
                params.push_back(p);
            }

            auto note                   = Diagnostic::note(overload->node, overload->node->token, couldBe);
            note->showRange             = false;
            note->showMultipleCodeLines = false;
            notes.push_back(note);
        }

        context->report(diag, notes);
        return false;
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

        return cannotMatchIdentifierError(context, overloads, node);
    }

    // There is more than one possible match, and this is an identifier for a name alias.
    // We are fine
    if (matches.size() > 1 && node && (node->flags & AST_NAME_ALIAS))
        return true;

    // There is more than one possible match
    if (matches.size() > 1)
    {
        if (justCheck)
            return false;
        if (!node)
            node = context->node;
        auto symbol = overloads[0]->overload->symbol;

        if (flags & MIP_FOR_GHOSTING)
        {
            AstNode*   otherNode = nullptr;
            SymbolKind otherKind = SymbolKind::Invalid;
            for (auto match : matches)
            {
                if (match->symbolOverload->node != node && !match->symbolOverload->node->isParentOf(node))
                {
                    otherKind = match->symbolOverload->symbol->kind;
                    otherNode = match->symbolOverload->node;
                    break;
                }
            }

            SWAG_ASSERT(otherNode);
            return duplicatedSymbolError(context, node->sourceFile, node->token, symbol->kind, symbol->name, otherKind, otherNode);
        }

        Diagnostic                diag{node, node->token, Fmt(Err(Err0116), symbol->name.c_str())};
        Vector<const Diagnostic*> notes;
        for (auto match : matches)
        {
            auto        overload = match->symbolOverload;
            Diagnostic* note     = nullptr;

            if (overload->typeInfo->isFuncAttr())
            {
                if (overload->typeInfo->flags & TYPEINFO_FROM_GENERIC)
                {
                    auto         typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                    AstFuncDecl* funcNode = CastAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
                    auto         orgNode  = funcNode->originalGeneric ? funcNode->originalGeneric : overload->typeInfo->declNode;
                    auto         couldBe  = Fmt(Nte(Nte0045), orgNode->typeInfo->getDisplayNameC());
                    note                  = Diagnostic::note(overload->node, overload->node->token, couldBe);
                    note->remarks.push_back(Fmt(Nte(Nte0047), overload->typeInfo->getDisplayNameC()));
                }
                else
                {
                    auto couldBe = Fmt(Nte(Nte0048), overload->typeInfo->getDisplayNameC());
                    note         = Diagnostic::note(overload->node, overload->node->token, couldBe);
                }

                if (!overload->typeInfo->isLambdaClosure())
                    note->showRange = false;
            }
            else if (overload->typeInfo->isStruct())
            {
                auto couldBe    = Fmt(Nte(Nte0049), overload->typeInfo->getDisplayNameC());
                note            = Diagnostic::note(overload->node, overload->node->token, couldBe);
                note->showRange = false;
            }
            else
            {
                auto concreteType = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
                auto couldBe      = Fmt(Nte(Nte0050), Naming::aKindName(match->symbolOverload).c_str(), concreteType->getDisplayNameC());
                note              = Diagnostic::note(overload->node, overload->node->token, couldBe);
            }

            note->noteHeader = "could be";
            notes.push_back(note);
        }

        return context->report(diag, notes);
    }

    return true;
}

void SemanticJob::checkCanInstantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch)
{
    auto symbol = firstMatch.symbolName;

    // Be sure number of overloads has not changed since then
    if (firstMatch.numOverloadsWhenChecked != symbol->overloads.size())
    {
        context->result = ContextResult::NewChilds;
        return;
    }

    if (firstMatch.numOverloadsInitWhenChecked != symbol->cptOverloadsInit)
    {
        context->result = ContextResult::NewChilds;
        return;
    }

    // Cannot instantiate if the type is incomplete
    Generic::waitForGenericParameters(context, firstMatch);
    if (context->result != ContextResult::Done)
        return;
}

bool SemanticJob::instantiateGenericSymbol(SemanticContext* context, OneGenericMatch& firstMatch, bool forStruct)
{
    auto       job               = context->job;
    auto       node              = context->node;
    auto&      matches           = job->cacheMatches;
    auto       symbol            = firstMatch.symbolName;
    auto       genericParameters = firstMatch.genericParameters;
    ScopedLock lk(symbol->mutex);

    // If we are inside a generic function (not instantiated), then we are done, we
    // cannot instantiate (can occure when evaluating function body of an incomplete short lammbda
    if (node->ownerFct && node->ownerFct->flags & AST_IS_GENERIC)
        return true;

    checkCanInstantiateGenericSymbol(context, firstMatch);
    if (context->result != ContextResult::Done)
        return true;

    if (forStruct)
    {
        // Be sure we have generic parameters if there's an automatic match
        if (!genericParameters && (firstMatch.flags & SymbolMatchContext::MATCH_GENERIC_AUTO))
        {
            SWAG_ASSERT(!firstMatch.genericParametersCallTypes.empty());
            auto identifier               = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
            identifier->genericParameters = Ast::newFuncCallGenParams(node->sourceFile, node);
            genericParameters             = identifier->genericParameters;
            for (int i = 0; i < firstMatch.genericParametersCallTypes.size(); i++)
            {
                auto param          = firstMatch.genericParametersCallTypes[i];
                auto callParam      = Ast::newFuncCallParam(node->sourceFile, genericParameters);
                callParam->typeInfo = param;

                if (firstMatch.genericParametersCallValues[i])
                {
                    callParam->allocateComputedValue();
                    *callParam->computedValue = *firstMatch.genericParametersCallValues[i];
                }
            }
        }

        // We can instantiate the struct because it's no more generic, and we have generic parameters to replace
        if (!(node->flags & AST_IS_GENERIC) && genericParameters)
        {
            bool alias = false;
            SWAG_CHECK(Generic::instantiateStruct(context, genericParameters, firstMatch, alias));
            if (alias)
            {
                auto oneMatch            = job->getOneMatch();
                oneMatch->symbolOverload = firstMatch.symbolOverload;
                matches.push_back(oneMatch);
            }
        }

        // The new struct is no more generic without generic parameters.
        // It happens each time we reference a generic struct without generic parameters, like impl 'Array', @typeof(Array), self* Array or even
        // a variable type (in that case we will try later to instantiate it with default generic parameters).
        // So we match a generic struct as a normal match without instantiation (for now).
        else if (!(node->flags & AST_IS_GENERIC))
        {
            auto oneMatch            = job->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->flags |= AST_IS_GENERIC;
        }

        // The new struct is still generic and we have generic parameters
        else
        {
            SWAG_ASSERT(genericParameters);
            auto oneMatch            = job->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->flags |= AST_IS_GENERIC;

            // :DupGen
            // We generate a new struct with the wanted generic parameters to have those names for replacement.
            auto newStructType = CastTypeInfo<TypeInfoStruct>(firstMatch.symbolOverload->typeInfo, TypeInfoKind::Struct);
            if (newStructType->genericParameters.size() == genericParameters->childs.size() && genericParameters->childs.size())
            {
                auto typeWasForced = firstMatch.symbolOverload->typeInfo->clone();
                newStructType      = CastTypeInfo<TypeInfoStruct>(typeWasForced, TypeInfoKind::Struct);
                for (size_t i = 0; i < genericParameters->childs.size(); i++)
                {
                    newStructType->genericParameters[i]->name     = genericParameters->childs[i]->typeInfo->name;
                    newStructType->genericParameters[i]->typeInfo = genericParameters->childs[i]->typeInfo;
                }

                typeWasForced->forceComputeName();
                oneMatch->typeWasForced = typeWasForced;
            }
        }
    }
    else
    {
        SWAG_CHECK(Generic::instantiateFunction(context, genericParameters, firstMatch));
    }

    return true;
}

bool SemanticJob::ufcsSetFirstParam(SemanticContext* context, AstIdentifierRef* identifierRef, OneMatch& match)
{
    auto symbol       = match.symbolOverload->symbol;
    auto dependentVar = match.dependentVar;
    auto node         = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);

    auto fctCallParam = Ast::newNode<AstFuncCallParam>(nullptr, AstNodeKind::FuncCallParam, node->sourceFile, nullptr);
    if (!node->callParameters)
        node->callParameters = Ast::newFuncCallParams(context->sourceFile, node);

    SWAG_CHECK(checkIsConcrete(context, identifierRef->previousResolvedNode));

    // Insert variable in first position. Need to update child
    // rank of all brothers.
    node->callParameters->childs.push_front(fctCallParam);

    fctCallParam->parent   = node->callParameters;
    fctCallParam->typeInfo = identifierRef->previousResolvedNode->typeInfo;
    fctCallParam->token    = identifierRef->previousResolvedNode->token;
    fctCallParam->inheritTokenLocation(node);
    fctCallParam->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;
    fctCallParam->inheritOwners(node->callParameters);
    fctCallParam->flags |= AST_TO_UFCS | AST_GENERATED;
    fctCallParam->inheritOrFlag(node, AST_IN_MIXIN);

    // If this is a closure, then parameter index 0 is for the embedded struct.
    // So ufcs will have index 1.
    if (match.symbolOverload->typeInfo->isClosure())
        fctCallParam->indexParam = 1;

    SWAG_ASSERT(match.solvedParameters.size() > 0);
    SWAG_ASSERT(match.solvedParameters[0]->index == 0);
    fctCallParam->resolvedParameter = match.solvedParameters[0];

    auto idRef = Ast::newIdentifierRef(node->sourceFile, fctCallParam);
    if (symbol->kind == SymbolKind::Variable)
    {
        if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
        {
            // Function that returns an interface, used as an ufcs.
            // Ex: var cfg = @compiler().getBuildCfg()
            // :SpecUfcsNode
            identifierRef->previousResolvedNode->flags |= AST_TO_UFCS;
            fctCallParam->specUfcsNode = identifierRef->previousResolvedNode;
            auto id                    = Ast::newIdentifier(node->sourceFile, Fmt("__8tmp_%d", g_UniqueID.fetch_add(1)), idRef, idRef);
            id->flags |= AST_NO_BYTECODE;
        }
        else
        {
            // Call from a lambda, on a variable : we need to keep the original variable, and put the UFCS one in its own identifierref
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            for (auto child : identifierRef->childs)
            {
                auto copyChild = Ast::cloneRaw(child, idRef);

                // We want to generate bytecode for the expression on the left only if the lambda is dereferenced from a struct/itf
                // Otherwise the left expression is only used for scoping
                if (!identifierRef->startScope ||
                    identifierRef->startScope != match.symbolOverload->node->ownerStructScope)
                {
                    child->flags |= AST_NO_BYTECODE;
                    copyChild->flags |= AST_UFCS_FCT;
                }

                if (child == identifierRef->previousResolvedNode)
                {
                    copyChild->flags |= AST_TO_UFCS;
                    break;
                }
            }
        }
    }
    else
    {
        identifierRef->previousResolvedNode->flags |= AST_UFCS_FCT;

        // If ufcs comes from a using var, then we must make a reference to the using var in
        // the first call parameter
        if (dependentVar == identifierRef->previousResolvedNode)
        {
            for (auto child : dependentVar->childs)
            {
                auto copyChild = Ast::newIdentifier(node->sourceFile, child->token.text.empty() ? dependentVar->token.text : child->token.text, idRef, idRef);
                copyChild->inheritOwners(fctCallParam);
                copyChild->inheritOrFlag(idRef, AST_IN_MIXIN);
                if (!child->resolvedSymbolOverload)
                {
                    copyChild->resolvedSymbolName     = dependentVar->resolvedSymbolOverload->symbol;
                    copyChild->resolvedSymbolOverload = dependentVar->resolvedSymbolOverload;
                    copyChild->typeInfo               = dependentVar->typeInfo;

                    // In case of a parameter of an inlined function, we will have to find the real OVERLOAD_VAR_INLINE variable
                    // :InlineUsingParam
                    if (dependentVar->kind == AstNodeKind::FuncDeclParam && copyChild->ownerInline && copyChild->ownerInline->parametersScope)
                    {
                        // Really, but REALLY not sure about that fix !! Seems really like a hack...
                        if (!copyChild->isSameStackFrame(copyChild->resolvedSymbolOverload))
                        {
                            auto sym = copyChild->ownerInline->parametersScope->symTable.find(dependentVar->resolvedSymbolOverload->symbol->name);
                            if (sym)
                            {
                                ScopedLock lk(sym->mutex);
                                SWAG_ASSERT(sym->overloads.size() == 1);
                                copyChild->resolvedSymbolOverload = sym->overloads[0];
                                copyChild->resolvedSymbolName     = copyChild->resolvedSymbolOverload->symbol;
                                copyChild->typeInfo               = copyChild->resolvedSymbolOverload->typeInfo;
                            }
                        }
                    }
                }
                else
                {
                    copyChild->resolvedSymbolName     = child->resolvedSymbolOverload->symbol;
                    copyChild->resolvedSymbolOverload = child->resolvedSymbolOverload;
                    copyChild->typeInfo               = child->typeInfo;
                }

                copyChild->byteCodeFct = ByteCodeGenJob::emitIdentifier;
                copyChild->flags |= AST_TO_UFCS | AST_L_VALUE;
                copyChild->flags |= AST_UFCS_FCT;
            }
        }
        else
        {
            // Copy all previous references to the one we want to pass as parameter
            // X.Y.call(...) => X.Y.call(X.Y, ...)
            // We copy instead of moving in case this will be evaluated another time (inline)
            for (auto child : identifierRef->childs)
            {
                auto copyChild = Ast::cloneRaw(child, idRef);
                child->flags |= AST_NO_BYTECODE;
                if (child == identifierRef->previousResolvedNode)
                {
                    copyChild->flags |= AST_TO_UFCS;
                    copyChild->flags |= AST_UFCS_FCT;
                    break;
                }
            }
        }
    }

    idRef->inheritAndFlag1(AST_CONST_EXPR);
    fctCallParam->inheritAndFlag1(AST_CONST_EXPR);

    identifierRef->previousResolvedNode->flags |= AST_FROM_UFCS;
    return true;
}

TypeInfoEnum* SemanticJob::findEnumTypeInContext(SemanticContext* context, TypeInfo* typeInfo)
{
    while (true)
    {
        typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_FUNC | CONCRETE_FORCEALIAS);
        if (!typeInfo)
            return nullptr;

        if (typeInfo->isArray())
        {
            auto typeArr = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            typeInfo     = typeArr->finalType;
            continue;
        }

        if (typeInfo->isSlice())
        {
            auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
            typeInfo       = typeSlice->pointedType;
            continue;
        }

        if (typeInfo->isPointer())
        {
            auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
            typeInfo         = typePointer->pointedType;
            continue;
        }

        if (typeInfo->isEnum())
        {
            return CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
        }

        return nullptr;
    }

    return nullptr;
}

bool SemanticJob::findEnumTypeInContext(SemanticContext* context, AstNode* node, VectorNative<TypeInfoEnum*>& result, VectorNative<std::pair<AstNode*, TypeInfoEnum*>>& has)
{
    result.clear();
    has.clear();

    // If this is a parameter of a function call, we will try to deduce the type with a function signature
    auto fctCallParam = node->findParent(AstNodeKind::FuncCallParam);
    if (fctCallParam &&
        fctCallParam->parent->kind == AstNodeKind::FuncCallParams &&
        fctCallParam->parent->parent->kind == AstNodeKind::Identifier)
    {
        VectorNative<OneSymbolMatch> symbolMatch;

        auto idref = CastAst<AstIdentifierRef>(fctCallParam->parent->parent->parent, AstNodeKind::IdentifierRef);
        auto id    = CastAst<AstIdentifier>(fctCallParam->parent->parent, AstNodeKind::Identifier);

        context->silentError++;
        auto found = findIdentifierInScopes(context, symbolMatch, idref, id);
        context->silentError--;
        if (context->result != ContextResult::Done)
            return true;

        if (!found || symbolMatch.empty())
            return true;

        // Be sure symbols have been solved, because we need the types to be deduced
        for (const auto& sm : symbolMatch)
        {
            auto symbol = sm.symbol;
            if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Variable)
                continue;

            ScopedLock ls(symbol->mutex);
            if (symbol->cptOverloads)
            {
                context->job->waitSymbolNoLock(symbol);
                return true;
            }
        }

        for (const auto& sm : symbolMatch)
        {
            auto symbol = sm.symbol;
            if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Variable)
                continue;

            for (auto& overload : symbol->overloads)
            {
                auto concrete = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCEALIAS);
                if (!concrete->isFuncAttr() && !concrete->isLambdaClosure())
                    continue;
                if (concrete->isGeneric() || concrete->isFromGeneric())
                    continue;

                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(concrete, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);

                // If there's only one corresponding type in the function, then take it
                // If it's not the correct parameter, the match will not be done, so we do not really care here
                VectorNative<TypeInfoEnum*> subResult;
                for (auto param : typeFunc->parameters)
                {
                    auto typeEnum = findEnumTypeInContext(context, param->typeInfo);
                    if (!typeEnum)
                        continue;
                    has.push_back_once({param->declNode, typeEnum});
                    if (typeEnum->contains(node->token.text))
                        subResult.push_back_once(typeEnum);
                }

                if (subResult.size() == 1)
                {
                    result.push_back_once(subResult.front());
                }

                // More that one possible type (at least two different enums with the same identical requested name in the function signature)
                // We are not lucky...
                else if (subResult.size() > 1)
                {
                    int enumIdx = 0;
                    for (size_t i = 0; i < fctCallParam->parent->childs.size(); i++)
                    {
                        auto child = fctCallParam->parent->childs[i];
                        if (child == fctCallParam)
                            break;
                        if (child->typeInfo && child->typeInfo->getCA()->isEnum())
                            enumIdx++;
                        else if (child->kind == AstNodeKind::IdentifierRef && child->specFlags & AstIdentifierRef::SPECFLAG_AUTO_SCOPE)
                            enumIdx++;
                    }

                    for (auto p : typeFunc->parameters)
                    {
                        auto concreteP = TypeManager::concreteType(p->typeInfo, CONCRETE_FORCEALIAS);
                        if (concreteP->isEnum())
                        {
                            if (!enumIdx)
                            {
                                auto typeEnum = findEnumTypeInContext(context, concreteP);
                                if (typeEnum)
                                    has.push_back_once({p->declNode, typeEnum});
                                if (typeEnum && typeEnum->contains(node->token.text))
                                    result.push_back_once(typeEnum);
                                break;
                            }

                            enumIdx--;
                        }
                    }
                }
            }
        }
    }
    else
    {
        auto fctReturn = node->findParent(AstNodeKind::Return);
        if (fctReturn)
        {
            auto funcNode = getFunctionForReturn(fctReturn);
            if (funcNode->returnType)
            {
                auto typeInfo = TypeManager::concreteType(funcNode->returnType->typeInfo, CONCRETE_FUNC | CONCRETE_FORCEALIAS);
                if (typeInfo->isEnum())
                {
                    auto typeEnum = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
                    has.push_back_once({nullptr, typeEnum});
                    if (typeEnum->contains(node->token.text))
                    {
                        result.push_back(typeEnum);
                        return true;
                    }
                }
            }
        }
    }

    if (!result.empty())
        return true;

    // We go up in the hierarchy until we find a statement, or a contextual type to return
    bool canScanChilds = true;
    auto parent        = node->parent;
    while (parent)
    {
        if (canScanChilds && (parent->kind == AstNodeKind::BinaryOp ||
                              parent->kind == AstNodeKind::FactorOp ||
                              parent->kind == AstNodeKind::AffectOp ||
                              parent->kind == AstNodeKind::VarDecl ||
                              parent->kind == AstNodeKind::ConstDecl ||
                              parent->kind == AstNodeKind::FuncDeclParam ||
                              parent->kind == AstNodeKind::Switch))
        {
            if (parent->kind != AstNodeKind::FactorOp)
                canScanChilds = false;

            SharedLock lk(parent->mutex);
            for (auto c : parent->childs)
            {
                auto typeInfoChild = c->typeInfo;

                // Take first child of an expression list
                if (c->kind == AstNodeKind::ExpressionList)
                {
                    auto expr     = CastAst<AstExpressionList>(c, AstNodeKind::ExpressionList);
                    typeInfoChild = expr->childs[0]->typeInfo;
                }

                auto typeEnum = findEnumTypeInContext(context, typeInfoChild);
                if (typeEnum)
                {
                    has.push_back_once({c, typeEnum});
                    if (typeEnum->contains(node->token.text))
                        result.push_back_once(typeEnum);
                }
            }
        }
        else
        {
            SharedLock lk(parent->mutex);
            auto       typeEnum = findEnumTypeInContext(context, parent->typeInfo);
            if (typeEnum)
            {
                has.push_back_once({parent, typeEnum});
                if (typeEnum->contains(node->token.text))
                    result.push_back_once(typeEnum);
            }
        }

        if (!result.empty())
            return true;

        if (parent->kind == AstNodeKind::Statement ||
            parent->kind == AstNodeKind::SwitchCaseBlock ||
            parent->kind == AstNodeKind::FuncDecl ||
            parent->kind == AstNodeKind::File ||
            parent->kind == AstNodeKind::Module ||
            parent->kind == AstNodeKind::FuncDeclParam ||
            parent->kind == AstNodeKind::VarDecl ||
            parent->kind == AstNodeKind::ConstDecl)
            break;

        parent = parent->parent;
    }

    return true;
}

void SemanticJob::addDependentSymbol(VectorNative<OneSymbolMatch>& symbols, SymbolName* symName, Scope* scope, uint32_t asflags)
{
    for (auto& ds : symbols)
    {
        if (ds.symbol == symName)
            return;
    }

    OneSymbolMatch osm;
    osm.symbol  = symName;
    osm.scope   = scope;
    osm.asFlags = asflags;
    symbols.push_back(osm);
}

bool SemanticJob::findIdentifierInScopes(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    return findIdentifierInScopes(context, context->job->cacheDependentSymbols, identifierRef, node);
}

bool SemanticJob::findIdentifierInScopes(SemanticContext* context, VectorNative<OneSymbolMatch>& dependentSymbols, AstIdentifierRef* identifierRef, AstIdentifier* node)
{
    auto job = context->job;

    // When this is "retval" type, no need to do fancy things, we take the corresponding function
    // return symbol. This will avoid some ambiguous resolutions with multiple tuples/structs.
    if (node->token.text == g_LangSpec->name_retval)
    {
        // Be sure this is correct
        SWAG_CHECK(resolveRetVal(context));
        auto fctDecl = node->ownerInline ? node->ownerInline->func : node->ownerFct;
        SWAG_ASSERT(fctDecl);
        auto typeFct = CastTypeInfo<TypeInfoFuncAttr>(fctDecl->typeInfo, TypeInfoKind::FuncAttr);
        SWAG_ASSERT(typeFct->returnType->isStruct());
        addDependentSymbol(dependentSymbols, typeFct->returnType->declNode->resolvedSymbolName, nullptr, 0);
        return true;
    }

    // #self
    if (node->token.text == g_LangSpec->name_sharpself)
    {
        SWAG_VERIFY(node->ownerFct, context->report({node, Err(Err0348)}));
        AstNode* parent = node;
        while ((parent->ownerFct->attributeFlags & ATTRIBUTE_SHARP_FUNC) && parent->ownerFct->parent->ownerFct)
            parent = parent->ownerFct->parent;
        SWAG_VERIFY(parent, context->report({parent, Err(Err0348)}));

        if (parent->ownerScope->kind == ScopeKind::Struct || parent->ownerScope->kind == ScopeKind::Enum)
        {
            parent = parent->ownerScope->owner;
            node->flags |= AST_CAN_MATCH_INCOMPLETE;
        }
        else
        {
            SWAG_VERIFY(parent->ownerFct, context->report({parent, Err(Err0348)}));
            parent = parent->ownerFct;

            // Force scope
            if (!node->callParameters && node != identifierRef->childs.back())
            {
                node->semFlags |= SEMFLAG_FORCE_SCOPE;
                node->typeInfo                      = g_TypeMgr->typeInfoVoid;
                identifierRef->previousResolvedNode = node;
                if (node->ownerInline)
                    identifierRef->startScope = node->ownerInline->parametersScope;
                else
                    identifierRef->startScope = node->ownerFct->scope;
                node->flags |= AST_NO_BYTECODE;
                return true;
            }
        }

        addDependentSymbol(dependentSymbols, parent->resolvedSymbolName, nullptr, 0);
        return true;
    }

    auto& scopeHierarchy     = job->cacheScopeHierarchy;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
    auto  crc                = node->token.text.hash();

    bool forceEnd  = false;
    bool checkWait = false;

    // We make 2 tries at max : one try with the previous symbol scope (A.B), and one try with the collected scope
    // hierarchy. We need this because even if A.B does not resolve (B is not in A), B(A) can be a match because of UFCS
    for (int oneTry = 0; oneTry < 2 && !forceEnd; oneTry++)
    {
        auto startScope = identifierRef->startScope;
        if (!startScope || oneTry == 1)
        {
            uint32_t collectFlags = COLLECT_ALL;
            auto     scopeUpMode  = node->identifierExtension ? node->identifierExtension->scopeUpMode : IdentifierScopeUpMode::None;
            auto     scopeUpValue = node->identifierExtension ? node->identifierExtension->scopeUpValue : TokenParse{};

            startScope = node->ownerScope;

            // :AutoScope
            // Auto scoping depending on the context
            // We scan the parent hierarchy for an already defined type that can be used for scoping
            if ((identifierRef->specFlags & AstIdentifierRef::SPECFLAG_AUTO_SCOPE) && !(identifierRef->specFlags & AstIdentifierRef::SPECFLAG_WITH_SCOPE))
            {
                VectorNative<TypeInfoEnum*>                      typeEnum;
                VectorNative<std::pair<AstNode*, TypeInfoEnum*>> hasEnum;
                SWAG_CHECK(findEnumTypeInContext(context, identifierRef, typeEnum, hasEnum));
                if (context->result == ContextResult::Pending)
                    return true;

                // More than one match : ambiguous
                if (typeEnum.size() > 1)
                {
                    Diagnostic diag{identifierRef, Fmt(Err(Err0080), node->token.ctext())};
                    for (auto t : typeEnum)
                        diag.remarks.push_back(Fmt(Nte(Nte0086), t->getDisplayNameC()));
                    return context->report(diag);
                }

                // One single match : we are done
                if (typeEnum.size() == 1)
                {
                    identifierRef->startScope = typeEnum[0]->scope;
                    scopeHierarchy.clear();
                    addAlternativeScopeOnce(scopeHierarchy, typeEnum[0]->scope);
                }

                // No match, we will try 'with'
                else
                {
                    auto withNodeP = node->findParent(AstNodeKind::With);
                    if (!withNodeP)
                    {
                        if (hasEnum.size())
                        {
                            Diagnostic                diag{identifierRef, Fmt(Err(Err0144), node->token.ctext(), hasEnum[0].second->getDisplayNameC())};
                            Vector<const Diagnostic*> notes;
                            notes.push_back(Diagnostic::note(findClosestMatchesMsg(context, node, {{hasEnum[0].second->scope}})));
                            if (hasEnum[0].first)
                                notes.push_back(Diagnostic::note(hasEnum[0].first, Diagnostic::isType(hasEnum[0].first)));
                            notes.push_back(Diagnostic::hereIs(hasEnum[0].second->declNode));
                            return context->report(diag, notes);
                        }

                        Diagnostic diag{identifierRef, Fmt(Err(Err0881), node->token.ctext())};
                        return context->report(diag);
                    }

                    auto withNode = CastAst<AstWith>(withNodeP, AstNodeKind::With);

                    // Prepend the 'with' identifier, and reevaluate
                    for (int wi = (int) withNode->id.size() - 1; wi >= 0; wi--)
                    {
                        auto id = Ast::newIdentifier(context->sourceFile, withNode->id[wi], identifierRef, identifierRef);
                        id->flags |= AST_GENERATED;
                        id->specFlags |= AstIdentifier::SPECFLAG_FROM_WITH;
                        id->allocateIdentifierExtension();
                        id->identifierExtension->alternateEnum    = hasEnum.empty() ? nullptr : hasEnum[0].second;
                        id->identifierExtension->fromAlternateVar = withNode->childs.front();
                        id->inheritTokenLocation(identifierRef);
                        identifierRef->childs.pop_back();
                        Ast::addChildFront(identifierRef, id);
                        identifierRef->specFlags |= AstIdentifierRef::SPECFLAG_WITH_SCOPE;
                        context->job->nodes.push_back(id);
                    }

                    context->result = ContextResult::NewChilds;
                    return true;
                }
            }
            else
            {
                SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, collectFlags, scopeUpMode, &scopeUpValue));
            }

            // Be sure this is the last try
            forceEnd = true;
        }
        else
        {
            addAlternativeScopeOnce(scopeHierarchy, startScope);

            // No need to go further if we have found the symbol in the parent identifierRef scope (if specified).
            auto symbol = startScope->symTable.find(node->token.text, crc);
            if (symbol)
            {
                addDependentSymbol(dependentSymbols, symbol, startScope, 0);
            }
            else
            {
                // Only deal with previous scope if the previous node wants to
                bool addAlternative = true;
                if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->semFlags & SEMFLAG_FORCE_SCOPE)
                    addAlternative = false;

                if (addAlternative)
                {
                    // A namespace scope can in fact be shared between multiple nodes, so the 'owner' is not
                    // relevant and we should not use it
                    if (startScope->kind != ScopeKind::Namespace && startScope->owner->extension)
                    {
                        collectAlternativeScopes(startScope->owner, scopeHierarchy);
                        collectAlternativeScopeVars(startScope->owner, scopeHierarchy, scopeHierarchyVars);
                    }
                }
            }
        }

        if (!dependentSymbols.empty())
            break;

        // Search symbol in all the scopes of the hierarchy
        while (scopeHierarchy.size())
        {
            for (size_t i = 0; i < scopeHierarchy.size(); i++)
            {
                const auto& as = scopeHierarchy[i];
                if (!as.scope)
                {
                    scopeHierarchy.erase_unordered(i);
                    i--;
                    continue;
                }

                if (as.scope->symTable.tryRead())
                {
                    auto symbol = as.scope->symTable.findNoLock(node->token.text, crc);
                    as.scope->symTable.endRead();
                    if (symbol)
                        addDependentSymbol(dependentSymbols, symbol, as.scope, as.flags);
                    scopeHierarchy.erase_unordered(i);
                    i--;
                }
            }
        }

        if (!dependentSymbols.empty())
            break;

        // If can happen than types are not completed when collecting hierarchies, so we can miss some scopes (because of using).
        // If the types are completed between the collect and the waitTypeCompleted, then we won't find the missing hierarchies,
        // because we won't parse again.
        // So here, if we do not find symbols, then we restart until all types are completed.
        if (!checkWait)
        {
            // If we dereference something, be sure the owner has been completed
            if (identifierRef->startScope)
            {
                job->waitTypeCompleted(identifierRef->startScope->owner->typeInfo);
                if (context->result == ContextResult::Pending)
                    return true;
            }

            // Same if dereference is implied by a using var
            for (auto& sv : scopeHierarchyVars)
            {
                job->waitTypeCompleted(sv.scope->owner->typeInfo);
                if (context->result == ContextResult::Pending)
                    return true;
            }

            forceEnd  = false;
            checkWait = true;
            oneTry--;
            continue;
        }

        checkWait = false;

        // We raise an error if we have tried to resolve with the normal scope hierarchy, and not just the scope
        // from the previous symbol
        if (oneTry || forceEnd || !identifierRef->startScope || !identifierRef->previousResolvedNode)
        {
            if (identifierRef->flags & AST_SILENT_CHECK)
                return true;
            unknownIdentifier(context, identifierRef, node);
            return false;
        }

        node->semFlags |= SEMFLAG_FORCE_UFCS;
    }

    return true;
}

bool SemanticJob::getUsingVar(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** result, AstNode** resultLeaf)
{
    auto  job                = context->job;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;

    if (scopeHierarchyVars.empty())
        return true;

    // Not for a global symbol
    if (overload->flags & OVERLOAD_VAR_GLOBAL)
        return true;

    auto kind = overload->symbol->kind;
    switch (kind)
    {
    case SymbolKind::Enum:
    case SymbolKind::Struct:
    case SymbolKind::TypeAlias:
    case SymbolKind::Interface:
        return true;
    default:
        break;
    }

    auto                        symbol   = overload->symbol;
    auto                        symScope = symbol->ownerTable->scope;
    Vector<AlternativeScopeVar> toCheck;

    // Collect all matches
    bool hasWith = false;
    bool hasUfcs = false;
    for (auto& dep : scopeHierarchyVars)
    {
        bool getIt = false;

        // This is a function, and first parameter matches the using var
        bool okForUfcs = false;
        if (symbol->kind == SymbolKind::Function)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
            if (typeInfo->parameters.size())
            {
                auto firstParam = typeInfo->parameters.front()->typeInfo;
                if (firstParam->isSame(dep.node->typeInfo, CASTFLAG_EXACT))
                    okForUfcs = true;
            }
        }

        // Exact same scope
        if (dep.scope == symScope || dep.scope->getFullName() == symScope->getFullName())
            getIt = true;

        // The symbol scope is an 'impl' inside a struct (impl for)
        else if (symScope->kind == ScopeKind::Impl && symScope->parentScope == dep.scope)
            getIt = true;

        // For mtd sub functions and ufcs
        else if (okForUfcs)
        {
            hasUfcs = true;
            getIt   = true;
        }

        if (getIt)
        {
            if (dep.node->parent->kind == AstNodeKind::With)
                hasWith = true;
            dep.flags |= okForUfcs ? ALTSCOPE_UFCS : 0;
            toCheck.push_back(dep);
        }
    }

    // With has priority over using
    if (hasWith)
    {
        for (auto& dep : toCheck)
        {
            SWAG_ASSERT(dep.node->parent);
            bool isWith = dep.node->parent->kind == AstNodeKind::With;
            if (!isWith)
                dep.node = nullptr;
        }
    }

    // If something has an ufcs match, then remove all those that don't
    if (hasUfcs)
    {
        for (auto& dep : toCheck)
        {
            if (!(dep.flags & ALTSCOPE_UFCS))
                dep.node = nullptr;
        }
    }

    // Get one
    AstNode* dependentVar     = nullptr;
    AstNode* dependentVarLeaf = nullptr;
    for (auto& dep : toCheck)
    {
        if (!dep.node)
            continue;
        if (dep.node == dependentVar)
            continue;

        if (dependentVar)
        {
            if (dep.node->isGeneratedSelf())
            {
                Diagnostic diag{dependentVar, Fmt(Err(Err0117), dependentVar->typeInfo->getDisplayNameC())};
                diag.hint  = Nte(Nte1081);
                auto note  = Diagnostic::note(dep.node->ownerFct, dep.node->ownerFct->token, Nte(Nte0056));
                auto note1 = Diagnostic::note(Nte(Nte0143));
                return context->report(diag, note, note1);
            }
            else
            {
                Diagnostic diag{dep.node, Fmt(Err(Err0117), dependentVar->typeInfo->getDisplayNameC())};
                diag.hint  = Nte(Nte1081);
                auto note  = Diagnostic::note(dependentVar, Nte(Nte0021));
                auto note1 = Diagnostic::note(Nte(Nte0143));
                return context->report(diag, note, note1);
            }
        }

        dependentVar     = dep.node;
        dependentVarLeaf = dep.leafNode;

        // This way the ufcs can trigger even with an implicit 'using' var (typically for a 'using self')
        if (!identifierRef->previousResolvedNode)
        {
            if (symbol->kind == SymbolKind::Function)
            {
                // Be sure we have a missing parameter in order to try ufcs
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
                bool canTry   = canTryUfcs(context, typeFunc, node->callParameters, dependentVar, false);
                if (context->result == ContextResult::Pending)
                    return true;
                if (canTry)
                {
                    identifierRef->resolvedSymbolOverload = dependentVar->resolvedSymbolOverload;
                    identifierRef->resolvedSymbolName     = dependentVar->resolvedSymbolOverload->symbol;
                    identifierRef->previousResolvedNode   = dependentVar;
                }
            }
        }
    }

    *result     = dependentVar;
    *resultLeaf = dependentVarLeaf;
    return true;
}

bool SemanticJob::canTryUfcs(SemanticContext* context, TypeInfoFuncAttr* typeFunc, AstFuncCallParams* parameters, AstNode* ufcsNode, bool nodeIsExplicit)
{
    if (!ufcsNode || typeFunc->parameters.empty())
        return false;

    // Be sure the identifier we want to use in ufcs emits code, otherwise we cannot use it.
    // This can happen if we have already changed the ast, but the nodes are reavaluated later
    // (because of an inline for example)
    if (ufcsNode->flags & AST_NO_BYTECODE)
        return false;

    // If we have an explicit node, then we can try. Anyway we will also try without...
    if (nodeIsExplicit)
        return true;

    // Compare first function parameter with ufcsNode type.
    bool cmpTypeUfcs = TypeManager::makeCompatibles(context,
                                                    typeFunc->parameters[0]->typeInfo,
                                                    ufcsNode->typeInfo,
                                                    nullptr,
                                                    ufcsNode,
                                                    CASTFLAG_JUST_CHECK | CASTFLAG_UFCS);
    if (context->result != ContextResult::Done)
        return false;

    // In case ufcsNode is not explicit (using var), then be sure that first parameter type matches.
    if (!cmpTypeUfcs)
        return false;

    return true;
}

bool SemanticJob::getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam)
{
    auto symbol = overload->symbol;

    bool canDoUfcs = false;
    if (symbol->kind == SymbolKind::Function)
        canDoUfcs = true;
    if (symbol->kind == SymbolKind::Variable && overload->typeInfo->isLambdaClosure())
        canDoUfcs = node->callParameters;
    if (isFunctionButNotACall(context, node, symbol))
        canDoUfcs = false;

    if (canDoUfcs)
    {
        // If a variable is defined just before a function call, then this can be an UFCS (uniform function call syntax)
        if (identifierRef->resolvedSymbolName && identifierRef->previousResolvedNode)
        {
            bool canTry = false;

            // Before was a variable
            if (identifierRef->resolvedSymbolName->kind == SymbolKind::Variable)
                canTry = true;
            // Before was an enum value
            else if (identifierRef->resolvedSymbolName->kind == SymbolKind::EnumValue)
                canTry = true;
            // Before was a function call
            else if (identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
                     identifierRef->previousResolvedNode &&
                     identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
                canTry = true;
            // Before was an inlined function call
            else if (identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
                     identifierRef->previousResolvedNode &&
                     identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier &&
                     identifierRef->previousResolvedNode->childs.size() &&
                     identifierRef->previousResolvedNode->childs.front()->kind == AstNodeKind::FuncCallParams &&
                     identifierRef->previousResolvedNode->childs.back()->kind == AstNodeKind::Inline)
                canTry = true;

            if (canTry)
            {
                SWAG_ASSERT(identifierRef->previousResolvedNode);
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::LambdaClosure);
                canTry        = canTryUfcs(context, typeFunc, node->callParameters, identifierRef->previousResolvedNode, true);
                if (context->result == ContextResult::Pending)
                    return true;
                if (canTry)
                    *ufcsFirstParam = identifierRef->previousResolvedNode;
                SWAG_VERIFY(node->callParameters, context->report({node, Fmt(Err(Err0020), typeFunc->getDisplayNameC())}));
            }
        }
    }

    if (canDoUfcs && symbol->kind == SymbolKind::Variable)
    {
        bool fine = false;

        if (identifierRef->resolvedSymbolName &&
            identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
            identifierRef->previousResolvedNode &&
            identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier &&
            identifierRef->previousResolvedNode->flags & AST_INLINED)
        {
            fine = true;
        }

        if (identifierRef->resolvedSymbolName &&
            identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
            identifierRef->previousResolvedNode &&
            identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
        {
            fine = true;
        }

        if (!fine)
        {
            if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind != SymbolKind::Variable)
            {
                auto       subNode = identifierRef->previousResolvedNode ? identifierRef->previousResolvedNode : node;
                Diagnostic diag{subNode, subNode->token, Fmt(Err(Err0124), identifierRef->resolvedSymbolName->name.c_str(), Naming::aKindName(identifierRef->resolvedSymbolName->kind).c_str())};
                diag.addRange(node->token, Nte(Nte1079));
                return context->report(diag);
            }
        }
    }

    return true;
}

bool SemanticJob::appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload)
{
    if (!(node->semFlags & SEMFLAG_LAST_PARAM_CODE) && (overload->symbol->kind == SymbolKind::Function))
    {
        node->semFlags |= SEMFLAG_LAST_PARAM_CODE;

        // If last parameter is of type code, and the call last parameter is not, then take the next statement
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->parameters.empty() && typeFunc->parameters.back()->typeInfo->isCode())
        {
            if (node->callParameters && node->callParameters->childs.size() < typeFunc->parameters.size())
            {
                auto parent = node->parent;
                if (parent->parent->kind == AstNodeKind::Catch ||
                    parent->parent->kind == AstNodeKind::Try ||
                    parent->parent->kind == AstNodeKind::Assume ||
                    parent->parent->kind == AstNodeKind::TryCatch)
                    parent = parent->parent;

                auto parentIdx = parent->childParentIdx();
                if (parentIdx != parent->parent->childs.size() - 1)
                {
                    auto brother = parent->parent->childs[parentIdx + 1];
                    if (brother->kind == AstNodeKind::Statement)
                    {
                        auto brotherParent = brother->parent;
                        switch (brotherParent->kind)
                        {
                        case AstNodeKind::Loop:
                        case AstNodeKind::If:
                        case AstNodeKind::CompilerIf:
                        case AstNodeKind::While:
                        {
                            Diagnostic diag{node, node->token, Fmt(Err(Err0686), Naming::kindName(overload).c_str(), overload->node->token.ctext(), brotherParent->token.ctext())};
                            return context->report(diag, Diagnostic::hereIs(overload->node));
                        }
                        default:
                            break;
                        }

                        auto fctCallParam = Ast::newFuncCallParam(context->sourceFile, node->callParameters);
                        auto codeNode     = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerCode, node->sourceFile, fctCallParam);
                        codeNode->flags |= AST_NO_BYTECODE;

                        Ast::removeFromParent(brother);
                        Ast::addChildBack(codeNode, brother);
                        auto typeCode     = makeType<TypeInfoCode>();
                        typeCode->content = brother;
                        brother->flags |= AST_NO_SEMANTIC;
                        fctCallParam->semFlags |= SEMFLAG_AUTO_CODE_PARAM;
                        fctCallParam->typeInfo = typeCode;
                        codeNode->typeInfo     = typeCode;
                    }
                }
            }
        }
    }

    return true;
}

bool SemanticJob::fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* identifier, SymbolOverload* overload, AstNode* ufcsFirstParam)
{
    auto symbol         = overload->symbol;
    auto symbolKind     = symbol->kind;
    auto callParameters = identifier->callParameters;

    auto typeRef = TypeManager::concreteType(overload->typeInfo, CONCRETE_FORCEALIAS);

    if (identifier->isSilentCall())
    {
        SWAG_ASSERT(typeRef->isArray());
        auto typeArr = CastTypeInfo<TypeInfoArray>(overload->typeInfo, TypeInfoKind::Array);
        typeRef      = TypeManager::concreteType(typeArr->finalType, CONCRETE_FORCEALIAS);
    }

    // :ClosureForceFirstParam
    // A closure has always a first parameter of type *void
    if (typeRef->isClosure() && identifier->callParameters)
    {
        if (!(identifier->specFlags & AstIdentifier::SPECFLAG_CLOSURE_FIRST_PARAM))
        {
            Ast::constructNode(&context->job->closureFirstParam);
            context->job->closureFirstParam.kind     = AstNodeKind::FuncCallParam;
            context->job->closureFirstParam.typeInfo = g_TypeMgr->makePointerTo(g_TypeMgr->typeInfoVoid);
            symMatchContext.parameters.push_back(&context->job->closureFirstParam);
            symMatchContext.flags |= SymbolMatchContext::MATCH_CLOSURE_PARAM;
        }
    }

    if (ufcsFirstParam)
        symMatchContext.parameters.push_back(ufcsFirstParam);

    if (callParameters || ufcsFirstParam)
    {
        if (symbolKind != SymbolKind::Attribute &&
            symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            symbolKind != SymbolKind::TypeAlias &&
            !identifier->isSilentCall() &&
            !symbol->overloads[0]->typeInfo->isKindGeneric() &&
            !TypeManager::concretePtrRefType(symbol->overloads[0]->typeInfo, CONCRETE_FORCEALIAS)->isLambdaClosure())
        {
            if (symbolKind == SymbolKind::Variable)
            {
                Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0125), identifier->token.ctext(), symbol->overloads[0]->typeInfo->getDisplayNameC())};
                return context->report(diag, Diagnostic::hereIs(symbol->overloads[0]));
            }
            else
            {
                Diagnostic diag{identifier, identifier->token, Fmt(Err(Err0127), identifier->token.ctext(), Naming::aKindName(symbol->kind).c_str())};
                return context->report(diag, Diagnostic::hereIs(symbol->overloads[0]));
            }
        }
    }

    if (callParameters)
    {
        auto childCount = callParameters->childs.size();
        for (size_t i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(callParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.parameters.push_back(oneParam);

            // Be sure all interfaces of the structure have been solved, in case a cast to an interface is necessary to match
            // a function
            // :WaitInterfaceReg
            TypeInfoStruct* typeStruct = nullptr;
            if (oneParam->typeInfo->isStruct())
                typeStruct = CastTypeInfo<TypeInfoStruct>(oneParam->typeInfo, TypeInfoKind::Struct);
            else if (oneParam->typeInfo->isPointerTo(TypeInfoKind::Struct))
            {
                auto typePtr = CastTypeInfo<TypeInfoPointer>(oneParam->typeInfo, TypeInfoKind::Pointer);
                typeStruct   = CastTypeInfo<TypeInfoStruct>(typePtr->pointedType, TypeInfoKind::Struct);
            }

            if (typeStruct)
            {
                context->job->waitAllStructInterfacesReg(oneParam->typeInfo);
                if (context->result == ContextResult::Pending)
                    return true;
            }

            // Variadic parameter must be the last one
            if (i != childCount - 1)
            {
                if (oneParam->typeInfo->isVariadic() ||
                    oneParam->typeInfo->isTypedVariadic() ||
                    oneParam->typeInfo->isCVariadic())
                {
                    return context->report({oneParam, Err(Err0734)});
                }
            }
        }
    }

    return true;
}

bool SemanticJob::fillMatchContextGenericParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload)
{
    auto genericParameters = node->genericParameters;

    // User generic parameters
    if (genericParameters)
    {
        auto symbol     = overload->symbol;
        auto symbolKind = symbol->kind;

        node->inheritOrFlag(genericParameters, AST_IS_GENERIC);
        if (symbolKind != SymbolKind::Function &&
            symbolKind != SymbolKind::Struct &&
            symbolKind != SymbolKind::Interface &&
            symbolKind != SymbolKind::TypeAlias)
        {
            auto       firstNode = symbol->nodes.front();
            Diagnostic diag{genericParameters, Fmt(Err(Err0130), node->token.ctext(), Naming::aKindName(symbol->kind).c_str())};
            return context->report(diag, Diagnostic::hereIs(firstNode, false, true));
        }

        auto childCount = genericParameters->childs.size();
        for (size_t i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(genericParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.genericParameters.push_back(oneParam);
            symMatchContext.genericParametersCallTypes.push_back(oneParam->typeInfo);
            symMatchContext.genericParametersCallValues.push_back(oneParam->computedValue);
            symMatchContext.genericParametersCallFrom.push_back(oneParam);
        }
    }

    return true;
}

bool SemanticJob::filterMatches(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    auto node         = context->node;
    auto countMatches = matches.size();

    // Sometimes we don't care about multiple symbols with the same name
    if (countMatches > 1 && node->parent && node->parent->parent)
    {
        auto grandParent = node->parent->parent;
        if (grandParent->kind == AstNodeKind::IntrinsicDefined ||
            (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->tokenId == TokenId::IntrinsicNameOf))
        {
            matches.count = 1;
            return true;
        }
    }

    for (size_t i = 0; i < countMatches; i++)
    {
        auto curMatch = matches[i];
        auto over     = curMatch->symbolOverload;
        auto overSym  = over->symbol;

        // Take care of #validif/#validifx
        if (overSym->kind == SymbolKind::Function &&
            !(context->node->flags & AST_IN_VALIDIF) &&
            !(context->node->attributeFlags & ATTRIBUTE_MATCH_VALIDIF_OFF))
        {
            auto funcDecl = CastAst<AstFuncDecl>(over->node, AstNodeKind::FuncDecl);
            if (funcDecl->validif)
            {
                SWAG_CHECK(solveValidIf(context, curMatch, funcDecl));
                if (context->result != ContextResult::Done)
                    return true;
                if (curMatch->remove)
                    continue;
            }
        }

        // Do not match "self"
        if (overSym->kind == SymbolKind::Function &&
            context->node->attributeFlags & ATTRIBUTE_MATCH_SELF_OFF)
        {
            if (node->ownerInline)
            {
                if (node->ownerInline->func->resolvedSymbolOverload == over)
                {
                    matches[i]->remove = true;
                    continue;
                }
            }
            else if (node->ownerFct && node->ownerFct->resolvedSymbolOverload == over)
            {
                matches[i]->remove = true;
                continue;
            }
        }

        if (countMatches == 1)
            return true;

        // In case of an alias, we take the first one, which should be the 'closest' one.
        // Not sure this is true, perhaps one day will have to change the way we find it.
        if (overSym->name[0] == '@' && overSym->name.find(g_LangSpec->name_atalias) == 0)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (i != j)
                    matches[j]->remove = true;
            }

            break;
        }

        // Priority to 'secondTry' false
        if (curMatch->secondTry)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->secondTry)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a 'moveref' call
        if (curMatch->flags & CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!(matches[j]->flags & CASTFLAG_RESULT_AUTO_MOVE_OPAFFECT))
                {
                    matches[j]->remove = true;
                }
            }
        }

        // Priority to a guess 'moveref' call
        if (curMatch->flags & CASTFLAG_RESULT_GUESS_MOVE)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!(matches[j]->flags & CASTFLAG_RESULT_GUESS_MOVE))
                {
                    matches[j]->remove = true;
                }
            }
        }

        // Priority to a match without an auto cast
        if (curMatch->autoOpCast)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->autoOpCast)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a non empty function
        if (over->node->isEmptyFct())
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!matches[j]->symbolOverload->node->isEmptyFct() &&
                    matches[j]->symbolOverload->symbol == curMatch->symbolOverload->symbol)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a local var/parameter versus a function
        if (over->typeInfo->isFuncAttr())
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->symbolOverload->flags & (OVERLOAD_VAR_LOCAL | OVERLOAD_VAR_FUNC_PARAM | OVERLOAD_VAR_INLINE))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority if no CASTFLAG_RESULT_STRUCT_CONVERT
        if (curMatch->oneOverload && curMatch->oneOverload->symMatchContext.flags & CASTFLAG_RESULT_STRUCT_CONVERT)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->oneOverload && !(matches[j]->oneOverload->symMatchContext.flags & CASTFLAG_RESULT_STRUCT_CONVERT))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a concrete type versus a generic one
        auto lastOverloadType = overSym->ownerTable->scope->owner->typeInfo;
        if (lastOverloadType && lastOverloadType->isGeneric())
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                auto newOverloadType = matches[j]->symbolOverload->symbol->ownerTable->scope->owner->typeInfo;
                if (newOverloadType && !newOverloadType->isGeneric())
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // If we are referencing an interface name, then interface can come from the interface definition and
        // from the sub scope inside a struct (OVERLOAD_IMPL_IN_STRUCT).
        if (over->flags & OVERLOAD_IMPL_IN_STRUCT)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!(matches[j]->symbolOverload->flags & OVERLOAD_IMPL_IN_STRUCT))
                {
                    // If interface name is alone, then we take in priority the interface definition, not the impl block
                    if (node == node->parent->childs.front())
                        curMatch->remove = true;
                    // If interface name is not alone (like X.ITruc), then we take in priority the sub scope
                    else
                        matches[j]->remove = true;
                }
            }
        }

        // Priority to a user generic parameters, instead of a copy one
        if (over->node->flags & AST_GENERATED_GENERIC_PARAM)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (!(matches[j]->symbolOverload->node->flags & AST_GENERATED_GENERIC_PARAM))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to the same stack frame
        if (!node->isSameStackFrame(over))
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (node->isSameStackFrame(matches[j]->symbolOverload))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to lambda call in a parameter over a function outside the actual function
        if (over->typeInfo->isLambdaClosure())
        {
            auto callParams = over->node->findParent(AstNodeKind::FuncCallParams);
            if (callParams)
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->symbol->kind == SymbolKind::Function)
                    {
                        auto nodeFct = matches[j]->symbolOverload->node;
                        if (!callParams->ownerFct->isParentOf(nodeFct))
                        {
                            matches[j]->remove = true;
                        }
                    }
                }
            }
        }

        // Priority to the same inline scope
        if (node->ownerInline)
        {
            if (!node->ownerInline->scope->isParentOf(over->node->ownerScope))
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    auto nodeToKeep = matches[j]->symbolOverload->node;
                    if (node->ownerInline->scope->isParentOf(nodeToKeep->ownerScope))
                    {
                        auto inMixin = nodeToKeep->findParent(AstNodeKind::CompilerMixin);
                        if (inMixin)
                        {
                            auto inMacro = inMixin->findParent(AstNodeKind::CompilerMacro);
                            if (inMacro)
                            {
                                break;
                            }
                        }

                        curMatch->remove = true;
                        break;
                    }
                }
            }
        }

        // Priority to not a namespace (??)
        if (curMatch->symbolOverload->symbol->kind == SymbolKind::Namespace)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (matches[j]->symbolOverload->symbol->kind != SymbolKind::Namespace)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Closure variable has a priority over a "out of scope" one
        if (curMatch->symbolOverload->symbol->kind == SymbolKind::Variable)
        {
            for (size_t j = 0; j < countMatches; j++)
            {
                if (i == j)
                    continue;
                if (matches[j]->symbolOverload->symbol->kind == SymbolKind::Variable && (matches[j]->symbolOverload->flags & OVERLOAD_VAR_CAPTURE))
                {
                    if (curMatch->symbolOverload->node->ownerScope->isParentOf(matches[j]->symbolOverload->node->ownerScope))
                    {
                        curMatch->remove = true;
                        break;
                    }
                }
            }
        }

        // If we didn't match with ufcs, then priority to a match that do not start with 'self'
        if (!curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            auto typeFunc0 = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeFunc0->parameters.empty() && typeFunc0->parameters[0]->typeInfo->isSelf())
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->isFuncAttr())
                    {
                        auto typeFunc1 = CastTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (typeFunc1->parameters.empty() || !typeFunc1->parameters[0]->typeInfo->isSelf())
                        {
                            curMatch->remove = true;
                            break;
                        }
                    }
                }
            }
        }

        // If we did match with ufcs, then priority to a match that starts with 'self'
        if (curMatch->ufcs && over->typeInfo->isFuncAttr())
        {
            auto typeFunc0 = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters.empty() || !typeFunc0->parameters[0]->typeInfo->isSelf())
            {
                for (size_t j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->isFuncAttr())
                    {
                        auto typeFunc1 = CastTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (!typeFunc1->parameters.empty() && (typeFunc1->parameters[0]->typeInfo->isSelf()))
                        {
                            curMatch->remove = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    // Eliminate all matches tag as 'remove'
    for (size_t i = 0; i < matches.size(); i++)
    {
        if (matches[i]->remove)
        {
            matches[i] = matches.back();
            matches.pop_back();
            i--;
        }
    }

    return true;
}

static int scopeCost(Scope* from, Scope* to)
{
    // Not sure this is really correct, because we do not take care of 'using'
    int cost = 0;
    while (from && from != to)
    {
        cost += 1;
        from = from->parentScope;
    }

    return cost;
}

bool SemanticJob::filterGenericMatches(SemanticContext* context, VectorNative<OneMatch*>& matches, VectorNative<OneGenericMatch*>& genMatches)
{
    // We have a match and more than one generic match
    // We need to be sure than instantiated another generic match will not be better than keeping
    // the already instantiated one
    if (genMatches.size() > 1 && matches.size() == 1)
    {
        auto   idCost       = scopeCost(context->node->ownerScope, matches[0]->symbolOverload->node->ownerScope);
        auto   bestIsIdCost = true;
        size_t bestGenId    = 0;

        for (size_t i = 0; i < genMatches.size(); i++)
        {
            auto& p    = genMatches[i];
            auto  cost = scopeCost(context->node->ownerScope, p->symbolOverload->node->ownerScope);
            if (cost < idCost)
            {
                bestIsIdCost = false;
                bestGenId    = i;
                idCost       = cost;
            }
        }

        if (!bestIsIdCost)
        {
            matches.clear();
            auto temp = genMatches[bestGenId];
            genMatches.clear();
            genMatches.push_back(temp);
        }
    }

    // Take the most "specialized" generic match, i.e. the one with the more 'genericReplaceTypes'
    if (genMatches.size() > 1)
    {
        size_t bestS = 0;
        for (size_t i = 0; i < genMatches.size(); i++)
        {
            bestS = max(bestS, genMatches[i]->genericReplaceTypes.size());
        }

        for (size_t i = 0; i < genMatches.size(); i++)
        {
            if (genMatches[i]->genericReplaceTypes.size() < bestS)
            {
                genMatches[i] = genMatches.back();
                genMatches.pop_back();
                i--;
            }
        }
    }

    for (size_t i = 0; i < genMatches.size(); i++)
    {
        // 'secondTry' is less prio than first ufcs try
        if (genMatches[i]->secondTry)
        {
            for (size_t j = 0; j < genMatches.size(); j++)
            {
                if (!genMatches[j]->secondTry)
                {
                    genMatches[i] = genMatches.back();
                    genMatches.pop_back();
                    i--;
                    break;
                }
            }
        }
    }

    // We eliminate all generic matches that will create an already existing match.
    // This way we can keep some matches with autoOpCast if we have no choice (we will not try to
    // instantiate 'without' autoOpCast, because it would result in a autoOpCast too.
    if (matches.size() && genMatches.size())
    {
        VectorNative<OneGenericMatch*> newGenericMatches;
        newGenericMatches.reserve((int) genMatches.size());
        for (size_t im = 0; im < matches.size(); im++)
        {
            for (size_t i = 0; i < genMatches.size(); i++)
            {
                auto same = areGenericReplaceTypesIdentical(matches[im]->oneOverload->overload->typeInfo, *genMatches[i]);
                if (!same)
                    newGenericMatches.push_back_once(genMatches[i]);
            }
        }

        genMatches = std::move(newGenericMatches);
    }

    return true;
}

bool SemanticJob::filterMatchesInContext(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    if (matches.size() <= 1)
        return true;

    for (size_t i = 0; i < matches.size(); i++)
    {
        auto                                             oneMatch = matches[i];
        auto                                             over     = oneMatch->symbolOverload;
        VectorNative<TypeInfoEnum*>                      typeEnum;
        VectorNative<std::pair<AstNode*, TypeInfoEnum*>> hasEnum;
        SWAG_CHECK(findEnumTypeInContext(context, over->node, typeEnum, hasEnum));
        if (context->result != ContextResult::Done)
            return true;

        if (typeEnum.size() == 1 && typeEnum[0]->scope == oneMatch->scope)
        {
            matches.clear();
            matches.push_back(oneMatch);
            return true;
        }

        // We try to eliminate a function if it does not match a contextual generic match
        // Because if we call a generic function without generic parameters, we can have multiple matches
        // if the generic type has not been deduced from parameters (if any).
        if (over->symbol->kind == SymbolKind::Function)
        {
            auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc->replaceTypes.size())
            {
                auto                   node = context->node;
                VectorNative<AstNode*> toCheck;

                // Pick contextual generic type replacements
                if (node->ownerFct)
                    toCheck.push_back(node->ownerFct);
                if (node->ownerInline)
                    toCheck.push_back(node->ownerInline->func);

                for (auto c : toCheck)
                {
                    auto typeFuncCheck = CastTypeInfo<TypeInfoFuncAttr>(c->typeInfo, TypeInfoKind::FuncAttr);
                    if (typeFuncCheck->replaceTypes.size() != typeFunc->replaceTypes.size())
                        continue;
                    for (auto& it : typeFunc->replaceTypes)
                    {
                        auto it1 = typeFuncCheck->replaceTypes.find(it.first);
                        if (it1 == typeFuncCheck->replaceTypes.end())
                            continue;
                        if (it.second != it1->second)
                            oneMatch->remove = true;
                    }
                }

                if (oneMatch->remove)
                {
                    matches[i] = matches.back();
                    matches.pop_back();
                    i--;
                }
            }
        }
    }

    return true;
}

bool SemanticJob::solveValidIf(SemanticContext* context, OneMatch* oneMatch, AstFuncDecl* funcDecl)
{
    ScopedLock lk0(funcDecl->funcMutex);
    ScopedLock lk1(funcDecl->mutex);

    // Be sure block has been solved
    if (!(funcDecl->specFlags & AstFuncDecl::SPECFLAG_PARTIAL_RESOLVE))
    {
        funcDecl->dependentJobs.add(context->job);
        context->job->setPending(JobWaitKind::SemPartialResolve, funcDecl->resolvedSymbolName, funcDecl, nullptr);
        return true;
    }

    // Execute #validif/#validifx block
    auto expr = funcDecl->validif->childs.back();

    // #validifx is evaluated for each call, so we remove the AST_VALUE_COMPUTED computed flag.
    // #validif is evaluated once, so keep it.
    if (funcDecl->validif->kind == AstNodeKind::CompilerValidIfx)
        expr->flags &= ~AST_VALUE_COMPUTED;

    if (!expr->hasComputedValue())
    {
        auto node                  = context->node;
        context->validIfParameters = oneMatch->oneOverload->callParameters;

        ErrCxtStepKind type;
        if (funcDecl->validif->kind == AstNodeKind::CompilerValidIfx)
            type = ErrCxtStepKind::ValidIfx;
        else
            type = ErrCxtStepKind::ValidIf;

        PushErrCxtStep ec(context, node, type, nullptr);
        auto           result      = executeCompilerNode(context, expr, false);
        context->validIfParameters = nullptr;
        if (!result)
            return false;
        if (context->result != ContextResult::Done)
            return true;
    }

    // Result
    if (!expr->computedValue->reg.b)
    {
        oneMatch->remove                              = true;
        oneMatch->oneOverload->symMatchContext.result = MatchResult::ValidIfFailed;
        return true;
    }

    if (funcDecl->content && funcDecl->content->flags & AST_NO_SEMANTIC)
    {
        funcDecl->content->flags &= ~AST_NO_SEMANTIC;

        // Need to restart semantic on instantiated function and on its content,
        // because the #validif has passed
        // It's safe to create a job with the content as it has been fully evaluated.
        // It's NOT safe for the function itself as the job that deals with it can be
        // still running
        auto job          = Allocator::alloc<SemanticJob>();
        job->sourceFile   = context->sourceFile;
        job->module       = context->sourceFile->module;
        job->dependentJob = context->job->dependentJob;
        job->nodes.push_back(funcDecl->content);
        job->context.errCxtSteps.insert(job->context.errCxtSteps.begin(),
                                        context->job->context.errCxtSteps.begin(),
                                        context->job->context.errCxtSteps.end());

        // This comes from a generic instantiation. Add context
        if (oneMatch->oneOverload->overload->typeInfo->isFromGeneric())
        {
            ErrorCxtStep expNode;

            auto typeFunc        = CastTypeInfo<TypeInfoFuncAttr>(oneMatch->oneOverload->overload->typeInfo, TypeInfoKind::FuncAttr);
            expNode.node         = context->node;
            expNode.replaceTypes = typeFunc->replaceTypes;
            if (expNode.node->hasExtMisc() && expNode.node->extMisc()->exportNode)
                expNode.node = expNode.node->extMisc()->exportNode;
            expNode.type = ErrCxtStepKind::Generic;
            job->context.errCxtSteps.push_back(expNode);
        }

        // To avoid a race condition with the job that is currently dealing with the funcDecl,
        // we will reevaluate it with a semanticAfterFct trick
        funcDecl->content->allocateExtension(ExtensionKind::Semantic);
        auto sem = funcDecl->content->extension->semantic;
        SWAG_ASSERT(!sem->semanticAfterFct ||
                    sem->semanticAfterFct == SemanticJob::resolveFuncDeclAfterSI ||
                    sem->semanticAfterFct == SemanticJob::resolveScopedStmtAfter);
        sem->semanticAfterFct = SemanticJob::resolveFuncDeclAfterSI;

        g_ThreadMgr.addJob(job);
    }

    return true;
}

bool SemanticJob::filterSymbols(SemanticContext* context, AstIdentifier* node)
{
    auto  job              = context->job;
    auto  identifierRef    = node->identifierRef();
    auto& dependentSymbols = job->cacheDependentSymbols;

    if (dependentSymbols.size() == 1)
        return true;

    for (auto& p : dependentSymbols)
    {
        auto oneSymbol = p.symbol;
        if (p.remove)
            continue;

        // A variable inside a scopefile has priority
        if (p.asFlags & ALTSCOPE_FILE_PRIV)
        {
            for (auto& p1 : dependentSymbols)
            {
                if (!(p1.asFlags & ALTSCOPE_FILE_PRIV))
                    p1.remove = true;
            }
        }

        // A variable which is name as a function...
        if (!node->callParameters &&
            oneSymbol->kind == SymbolKind::Function &&
            !isFunctionButNotACall(context, node, oneSymbol))
        {
            p.remove = true;
            continue;
        }

        // A function call which is named as a variable, and the variable is not a lambda
        if (node->callParameters &&
            oneSymbol->kind == SymbolKind::Variable)
        {
            bool ok = false;
            for (auto& o : oneSymbol->overloads)
            {
                if (o->typeInfo->isLambdaClosure())
                {
                    ok = true;
                    break;
                }
            }

            if (!ok)
            {
                p.remove = true;
                continue;
            }
        }

        // If a generic type does not come from a 'using', it has priority
        if (!(p.asFlags & ALTSCOPE_USING) && p.symbol->kind == SymbolKind::GenericType)
        {
            for (auto& p1 : dependentSymbols)
            {
                if (p1.asFlags & ALTSCOPE_USING && p1.symbol->kind == SymbolKind::GenericType)
                    p1.remove = true;
            }
        }

        // Reference to a variable inside a struct, without a direct explicit reference
        bool isValid = true;
        if (oneSymbol->kind != SymbolKind::Function &&
            oneSymbol->kind != SymbolKind::GenericType &&
            oneSymbol->kind != SymbolKind::Struct &&
            oneSymbol->kind != SymbolKind::Enum &&
            (oneSymbol->overloads.size() != 1 || !(oneSymbol->overloads[0]->flags & OVERLOAD_COMPUTED_VALUE)) &&
            oneSymbol->ownerTable->scope->kind == ScopeKind::Struct &&
            !identifierRef->startScope)
        {
            isValid                  = false;
            auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
            for (auto& dep : scopeHierarchyVars)
            {
                if (dep.scope->getFullName() == oneSymbol->ownerTable->scope->getFullName())
                {
                    isValid = true;
                    break;
                }
            }
        }

        p.remove = !isValid;
    }

    // Eliminate all matches tag as 'remove'
    for (size_t i = 0; i < dependentSymbols.size(); i++)
    {
        if (dependentSymbols[i].remove)
        {
            dependentSymbols[i] = dependentSymbols.back();
            dependentSymbols.pop_back();
            i--;
        }
    }

    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context)
{
    auto node = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier, AstNodeKind::FuncCall);
    return resolveIdentifier(context, node, RI_ZERO);
}

bool SemanticJob::needToCompleteSymbol(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol)
{
    if (symbol->kind != SymbolKind::Struct && symbol->kind != SymbolKind::Interface)
        return true;
    if (identifier->callParameters || identifier->genericParameters)
        return true;
    if (symbol->overloads.size() != 1)
        return true;

    // If this is a generic type, and it's from an instance, we must wait, because we will
    // have to instantiate that symbol too
    if (identifier->ownerStructScope && (identifier->ownerStructScope->owner->flags & AST_FROM_GENERIC) && symbol->overloads[0]->typeInfo->isGeneric())
        return true;
    if (identifier->ownerFct && (identifier->ownerFct->flags & AST_FROM_GENERIC) && symbol->overloads[0]->typeInfo->isGeneric())
        return true;

    // If a structure is referencing itself, we will match the incomplete symbol for now
    if (identifier->flags & AST_STRUCT_MEMBER)
        return false;

    // We can also do an incomplete match with the identifier of an Impl block
    if (identifier->flags & AST_CAN_MATCH_INCOMPLETE)
        return false;

    // If identifier is in a pointer type expression, can incomplete resolve
    if (identifier->parent->parent && identifier->parent->parent->kind == AstNodeKind::TypeExpression)
    {
        auto typeExprNode = CastAst<AstTypeExpression>(identifier->parent->parent, AstNodeKind::TypeExpression);
        if (typeExprNode->typeFlags & TYPEFLAG_IS_PTR)
            return false;

        if (typeExprNode->parent && typeExprNode->parent->kind == AstNodeKind::TypeExpression)
        {
            typeExprNode = CastAst<AstTypeExpression>(typeExprNode->parent, AstNodeKind::TypeExpression);
            if (typeExprNode->typeFlags & TYPEFLAG_IS_PTR)
                return false;
        }
    }

    return true;
}

bool SemanticJob::needToWaitForSymbol(SemanticContext* context, AstIdentifier* identifier, SymbolName* symbol)
{
    if (!symbol->cptOverloads && !(symbol->flags & SYMBOL_ATTRIBUTE_GEN))
        return false;

    // For a name alias, we wait everything to be done...
    if (identifier->flags & AST_NAME_ALIAS)
        return true;

    // This is enough to resolve, as we just need parameters, and that case means that some functions
    // do not know their return type yet (short lambdas)
    if (symbol->kind == SymbolKind::Function && symbol->overloads.size() == symbol->cptOverloadsInit)
        return false;

    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context, AstIdentifier* identifier, uint32_t riFlags)
{
    auto  job                = context->job;
    auto& scopeHierarchy     = job->cacheScopeHierarchy;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
    auto& dependentSymbols   = job->cacheDependentSymbols;
    auto  identifierRef      = identifier->identifierRef();

    identifier->byteCodeFct = ByteCodeGenJob::emitIdentifier;

    // Current file scope
    if (context->sourceFile && context->sourceFile->scopeFile && identifier->token.text == context->sourceFile->scopeFile->name)
    {
        SWAG_VERIFY(identifier == identifierRef->childs.front(), context->report({identifier, Err(Err0132)}));
        identifierRef->startScope = context->sourceFile->scopeFile;
        return true;
    }

    // If previous identifier was generic, then stop evaluation
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->typeInfo->isKindGeneric())
    {
        // Just take the generic type for now
        identifier->typeInfo    = g_TypeMgr->typeInfoUndefined;
        identifierRef->typeInfo = identifierRef->previousResolvedNode->typeInfo;
        return true;
    }

    // Already solved
    if ((identifier->flags & AST_FROM_GENERIC) && identifier->typeInfo && !identifier->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        if (identifier->resolvedSymbolOverload)
        {
            OneMatch oneMatch;
            oneMatch.symbolOverload = identifier->resolvedSymbolOverload;
            oneMatch.scope          = identifier->resolvedSymbolOverload->node->ownerScope;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, identifier, oneMatch));
        }

        return true;
    }

    // Compute dependencies if not already done
    if (identifier->semanticState == AstNodeResolveState::ProcessingChilds || (riFlags & RI_FOR_GHOSTING) || (riFlags & RI_FOR_ZERO_GHOSTING))
    {
        scopeHierarchy.clear();
        scopeHierarchyVars.clear();
        dependentSymbols.clear();
    }

    if (dependentSymbols.empty())
    {
        if (identifier->isSilentCall())
        {
            OneSymbolMatch sm;
            sm.symbol = identifierRef->resolvedSymbolName;
            dependentSymbols.push_back(sm);
        }
        else
        {
            SWAG_CHECK(findIdentifierInScopes(context, identifierRef, identifier));
            if (context->result != ContextResult::Done)
            {
                dependentSymbols.clear();
                return true;
            }
        }

        // Because of #self
        if (identifier->semFlags & SEMFLAG_FORCE_SCOPE)
            return true;

        if (dependentSymbols.empty())
        {
            SWAG_ASSERT(identifierRef->flags & AST_SILENT_CHECK);
            return true;
        }
    }

    // Filter symbols
    SWAG_CHECK(filterSymbols(context, identifier));
    if (dependentSymbols.empty())
        return context->report({identifier, Fmt(Err(Err0133), identifier->token.ctext())});

    // If we have multiple symbols, we need to check that no one can be solved as incomplete, otherwhise it
    // can lead to ambiguities, or even worse, take the wrong one.
    if (dependentSymbols.size() > 1)
    {
        // A struct and an interface, this is legit
        bool fine = false;
        if (dependentSymbols.size() == 2 && dependentSymbols[0].symbol->kind == SymbolKind::Struct && dependentSymbols[1].symbol->kind == SymbolKind::Interface)
            fine = true;
        else if (dependentSymbols.size() == 2 && dependentSymbols[0].symbol->kind == SymbolKind::Interface && dependentSymbols[1].symbol->kind == SymbolKind::Struct)
            fine = true;

        if (!fine)
        {
            for (auto& p : dependentSymbols)
            {
                auto symbol = p.symbol;
                if (!needToCompleteSymbol(context, identifier, symbol))
                {
                    Diagnostic diag{identifier, Fmt(Err(Err0116), identifier->token.ctext())};

                    Vector<const Diagnostic*> notes;
                    for (auto& p1 : dependentSymbols)
                    {
                        auto note                   = Diagnostic::note(p1.symbol->nodes[0], p1.symbol->nodes[0]->token, "could be");
                        note->showRange             = false;
                        note->showMultipleCodeLines = false;
                        notes.push_back(note);
                    }

                    context->report(diag, notes);
                    return false;
                }
            }
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto& p : dependentSymbols)
    {
        auto symbol = p.symbol;

        // First test, with just a SharedLock for contention
        {
            SharedLock lkn(symbol->mutex);
            if (!needToWaitForSymbol(context, identifier, symbol))
                continue;
        }

        {
            // Do the test again, this time with a lock
            ScopedLock lkn(symbol->mutex);
            if (!needToWaitForSymbol(context, identifier, symbol))
                continue;

            // Can we make a partial match ?
            if (needToCompleteSymbol(context, identifier, symbol))
            {
                job->waitSymbolNoLock(symbol);
                return true;
            }

            // Be sure that we have at least a registered incomplete symbol
            SWAG_ASSERT(symbol->overloads.size() == 1);
            if (!(symbol->overloads[0]->flags & OVERLOAD_INCOMPLETE))
            {
                job->waitSymbolNoLock(symbol);
                return true;
            }
        }

        // Partial resolution
        identifier->resolvedSymbolName     = symbol;
        identifier->resolvedSymbolOverload = symbol->overloads[0];
        identifier->typeInfo               = identifier->resolvedSymbolOverload->typeInfo;

        setIdentifierAccess(identifier, symbol->overloads[0]);

        // In case identifier is part of a reference, need to initialize it
        if (identifier != identifier->identifierRef()->childs.back())
            SWAG_CHECK(setupIdentifierRef(context, identifier));

        return true;
    }

    auto orgResolvedSymbolOverload = identifierRef->resolvedSymbolOverload;
    auto orgResolvedSymbolName     = identifierRef->resolvedSymbolName;
    auto orgPreviousResolvedNode   = identifierRef->previousResolvedNode;

    // In case of a reevaluation, ufcsFirstParam will be null, but we still want to cast for ufcs
    bool hasForcedUfcs = false;
    if (identifier->callParameters && !identifier->callParameters->childs.empty() && identifier->callParameters->childs.front()->flags & AST_TO_UFCS)
        hasForcedUfcs = true;

    while (true)
    {
        // Collect all overloads to solve. We collect also the number of overloads when the collect is done, in
        // case that number changes (other thread) during the resolution. Because if the number of overloads differs
        // at one point in the process (for a given symbol), then this will invalidate the resolution
        // (number of overloads can change when instantiating a generic)
        auto& toSolveOverload = job->cacheToSolveOverload;
        toSolveOverload.clear();
        for (auto& p : dependentSymbols)
        {
            auto       symbol = p.symbol;
            SharedLock lk(symbol->mutex);
            for (auto over : symbol->overloads)
            {
                OneOverload t;
                t.overload         = over;
                t.scope            = p.scope;
                t.cptOverloads     = (uint32_t) symbol->overloads.size();
                t.cptOverloadsInit = (uint32_t) symbol->cptOverloadsInit;
                toSolveOverload.push_back(t);
            }
        }

        // Can happen if a symbol is inside a disabled #if for example
        if (toSolveOverload.empty())
        {
            if (identifierRef->flags & AST_SILENT_CHECK)
                return true;
            return context->report({identifier, Fmt(Err(Err0133), identifier->token.ctext())});
        }

        auto& listTryMatch = job->cacheListTryMatch;
        job->clearTryMatch();

        for (auto& oneOver : toSolveOverload)
        {
            auto symbolOverload                   = oneOver.overload;
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;

            // Is there a using variable associated with the symbol to solve ?
            AstNode* dependentVar     = nullptr;
            AstNode* dependentVarLeaf = nullptr;
            SWAG_CHECK(getUsingVar(context, identifierRef, identifier, symbolOverload, &dependentVar, &dependentVarLeaf));
            if (context->result == ContextResult::Pending)
                return true;

            // Get the ufcs first parameter if we can
            AstNode* ufcsFirstParam = nullptr;

            // The ufcs parameter has already been set in we are evaluating an identifier for the second time
            // (when we inline a function call)
            if (!identifier->callParameters || identifier->callParameters->childs.empty() || !(identifier->callParameters->childs.front()->flags & AST_TO_UFCS))
            {
                SWAG_CHECK(getUfcs(context, identifierRef, identifier, symbolOverload, &ufcsFirstParam));
                if (context->result == ContextResult::Pending)
                    return true;
                if ((identifier->semFlags & SEMFLAG_FORCE_UFCS) && !ufcsFirstParam)
                    continue;
            }

            // If the last parameter of a function is of type 'code', and the last call parameter is not,
            // then we take the next statement, after the function, and put it as the last parameter
            SWAG_CHECK(appendLastCodeStatement(context, identifier, symbolOverload));

            // Will try with ufcs, and will try without
            for (int tryUfcs = 0; tryUfcs < 2; tryUfcs++)
            {
                auto  tryMatch        = job->getTryMatch();
                auto& symMatchContext = tryMatch->symMatchContext;

                tryMatch->genericParameters = identifier->genericParameters;
                tryMatch->callParameters    = identifier->callParameters;
                tryMatch->dependentVar      = dependentVar;
                tryMatch->dependentVarLeaf  = dependentVarLeaf;
                tryMatch->overload          = symbolOverload;
                tryMatch->scope             = oneOver.scope;
                tryMatch->ufcs              = ufcsFirstParam || hasForcedUfcs;
                tryMatch->cptOverloads      = oneOver.cptOverloads;
                tryMatch->cptOverloadsInit  = oneOver.cptOverloadsInit;
                tryMatch->secondTry         = tryUfcs == 1;

                SWAG_CHECK(fillMatchContextCallParameters(context, symMatchContext, identifier, symbolOverload, ufcsFirstParam));
                if (context->result != ContextResult::Done)
                    return true;
                SWAG_CHECK(fillMatchContextGenericParameters(context, symMatchContext, identifier, symbolOverload));

                bool notACall = isFunctionButNotACall(context, identifier, symbolOverload->symbol);
                if (identifier->forceTakeAddress() && notACall)
                    symMatchContext.flags |= SymbolMatchContext::MATCH_FOR_LAMBDA;

                listTryMatch.push_back(tryMatch);

                if (!ufcsFirstParam)
                    break;
                if (identifier->semFlags & SEMFLAG_FORCE_UFCS)
                    break;
                ufcsFirstParam = nullptr;
            }
        }

        if (listTryMatch.empty())
        {
            job->cacheMatches.clear();
            break;
        }

        uint32_t mipFlags = 0;
        if (riFlags & RI_FOR_GHOSTING)
            mipFlags |= MIP_FOR_GHOSTING;
        if (riFlags & RI_FOR_ZERO_GHOSTING)
            mipFlags |= MIP_FOR_ZERO_GHOSTING;
        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, identifier, mipFlags));

        if (context->result == ContextResult::Pending)
        {
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;
            return true;
        }

        if (context->result == ContextResult::NewChilds1)
        {
            identifierRef->resolvedSymbolOverload = orgResolvedSymbolOverload;
            identifierRef->resolvedSymbolName     = orgResolvedSymbolName;
            identifierRef->previousResolvedNode   = orgPreviousResolvedNode;
            context->result                       = ContextResult::NewChilds;
            return true;
        }

        // The number of overloads for a given symbol has been changed by another thread, which
        // means that we need to restart everything...
        if (context->result != ContextResult::NewChilds)
            break;

        context->result = ContextResult::Done;
    }

    if (job->cacheMatches.empty())
    {
        // We want to force the ufcs
        if (identifier->semFlags & SEMFLAG_FORCE_UFCS)
        {
            if (identifierRef->flags & AST_SILENT_CHECK)
                return true;
            unknownIdentifier(context, identifierRef, CastAst<AstIdentifier>(identifier, AstNodeKind::Identifier));
            return false;
        }

        return false;
    }

    if (riFlags & (RI_FOR_GHOSTING | RI_FOR_ZERO_GHOSTING))
        return true;

    // Name alias with overloads (more than one match)
    if (identifier->flags & AST_NAME_ALIAS && job->cacheMatches.size() > 1)
    {
        identifier->resolvedSymbolName     = job->cacheMatches[0]->symbolOverload->symbol;
        identifier->resolvedSymbolOverload = nullptr;
        return true;
    }

    // Deal with ufcs. Now that the match is done, we will change the ast in order to
    // add the ufcs parameters to the function call parameters
    auto& match = job->cacheMatches[0];
    if (match->ufcs && !hasForcedUfcs)
    {
        // Do not change AST if this is code inside a generic function
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            if (match->dependentVar && !identifierRef->previousResolvedNode)
            {
                identifierRef->resolvedSymbolOverload = match->dependentVar->resolvedSymbolOverload;
                identifierRef->resolvedSymbolName     = match->dependentVar->resolvedSymbolOverload->symbol;
                identifierRef->previousResolvedNode   = match->dependentVar;
            }

            SWAG_CHECK(ufcsSetFirstParam(context, identifierRef, *match));
        }
    }

    if (identifier->isSilentCall())
        identifier->typeInfo = identifierRef->typeInfo;
    else if (match->typeWasForced)
        identifier->typeInfo = match->typeWasForced;
    else
        identifier->typeInfo = match->symbolOverload->typeInfo;

    SWAG_CHECK(setSymbolMatch(context, identifierRef, identifier, *match));
    return true;
}

void SemanticJob::collectAlternativeScopes(AstNode* startNode, VectorNative<AlternativeScope>& scopes)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<AlternativeScope> toAdd;
    VectorNative<Scope*>           done;

    if (startNode->hasExtMisc())
    {
        SharedLock lk(startNode->extMisc()->mutexAltScopes);
        toAdd.append(startNode->extMisc()->alternativeScopes);
    }

    while (!toAdd.empty())
    {
        auto it0 = toAdd.back();
        toAdd.pop_back();

        if (!done.contains(it0.scope))
        {
            done.push_back(it0.scope);
            addAlternativeScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->kind == ScopeKind::Struct)
            {
                SharedLock lk(it0.scope->owner->mutex);
                if (it0.scope->owner->hasExtMisc())
                {
                    // We register the sub scope with the original "node" (top level), because the original node will in the end
                    // become the dependentVar node, and will be converted by cast to the correct type.
                    {
                        SharedLock lk1(it0.scope->owner->extMisc()->mutexAltScopes);
                        for (auto& it1 : it0.scope->owner->extMisc()->alternativeScopes)
                            toAdd.push_back({it1.scope, it1.flags});
                    }
                }
            }
        }
    }
}

void SemanticJob::collectAlternativeScopeVars(AstNode* startNode, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<AlternativeScopeVar> toAdd;
    VectorNative<Scope*>              done;

    if (startNode->hasExtMisc())
    {
        SharedLock lk(startNode->extMisc()->mutexAltScopes);
        toAdd.append(startNode->extMisc()->alternativeScopesVars);
    }

    while (!toAdd.empty())
    {
        auto it0 = toAdd.back();
        toAdd.pop_back();
        scopesVars.push_back(it0);

        if (!done.contains(it0.scope))
        {
            done.push_back(it0.scope);
            addAlternativeScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->kind == ScopeKind::Struct)
            {
                SharedLock lk(it0.scope->owner->mutex);
                if (it0.scope->owner->hasExtMisc())
                {
                    // We register the sub scope with the original "node" (top level), because the original node will in the end
                    // become the dependentVar node, and will be converted by cast to the correct type.
                    {
                        SharedLock lk1(it0.scope->owner->extMisc()->mutexAltScopes);
                        for (auto& it1 : it0.scope->owner->extMisc()->alternativeScopesVars)
                            toAdd.push_back({it0.node, it1.node, it1.scope, it1.flags});
                    }

                    // If this is a struct that comes from a generic, we need to also register the generic scope in order
                    // to be able to find generic functions to instantiate
                    SWAG_ASSERT(it0.scope->owner->typeInfo->isStruct());
                    auto typeStruct = CastTypeInfo<TypeInfoStruct>(it0.scope->owner->typeInfo, TypeInfoKind::Struct);
                    if (typeStruct->fromGeneric)
                    {
                        auto structDecl = CastAst<AstStruct>(typeStruct->fromGeneric->declNode, AstNodeKind::StructDecl);
                        addAlternativeScopeOnce(scopes, structDecl->scope, 0);
                    }
                }
            }
        }
    }
}

void SemanticJob::addAlternativeScopeOnce(VectorNative<AlternativeScope>& scopes, Scope* scope, uint32_t flags)
{
    if (hasAlternativeScope(scopes, scope))
        return;
    addAlternativeScope(scopes, scope, flags);
}

bool SemanticJob::hasAlternativeScope(VectorNative<AlternativeScope>& scopes, Scope* scope)
{
    for (auto& as : scopes)
    {
        if (as.scope == scope)
            return true;
    }

    return false;
}

void SemanticJob::addAlternativeScope(VectorNative<AlternativeScope>& scopes, Scope* scope, uint32_t flags)
{
    if (!scope)
        return;

    AlternativeScope as;
    as.scope = scope;
    as.flags = flags;
    if (scope->flags & SCOPE_FILE_PRIV)
        as.flags |= ALTSCOPE_FILE_PRIV;
    scopes.push_back(as);
}

void SemanticJob::collectAlternativeScopeHierarchy(SemanticContext*                   context,
                                                   VectorNative<AlternativeScope>&    scopes,
                                                   VectorNative<AlternativeScopeVar>& scopesVars,
                                                   AstNode*                           startNode,
                                                   uint32_t                           flags,
                                                   IdentifierScopeUpMode              scopeUpMode,
                                                   TokenParse*                        scopeUpValue)
{
    // Add registered alternative scopes of the current node
    if (startNode->hasExtMisc() && !startNode->extMisc()->alternativeScopes.empty())
    {
        auto  job       = context->job;
        auto& toProcess = job->scopesToProcess;
        for (auto& as : startNode->extMisc()->alternativeScopes)
        {
            if (!hasAlternativeScope(scopes, as.scope))
            {
                addAlternativeScope(scopes, as.scope, as.flags);
                addAlternativeScopeOnce(toProcess, as.scope, as.flags);
            }
        }

        // Can only take a using var if in the same function
        if (startNode->ownerFct == context->node->ownerFct || startNode == context->node->ownerFct)
        {
            collectAlternativeScopeVars(startNode, scopes, scopesVars);
        }
    }

    // An inline block contains a specific scope that contains the parameters.
    // That scope does not have a parent, so the hierarchy scan will stop at it.
    if (startNode->kind == AstNodeKind::Inline && !(flags & COLLECT_NO_INLINE_PARAMS))
    {
        auto inlineNode = CastAst<AstInline>(startNode, AstNodeKind::Inline);
        SWAG_ASSERT(inlineNode->parametersScope);
        addAlternativeScopeOnce(scopes, inlineNode->parametersScope);
    }

    if (startNode->kind == AstNodeKind::CompilerMacro)
    {
        if (scopeUpMode == IdentifierScopeUpMode::None)
        {
            while (startNode->parent->kind != AstNodeKind::Inline && // Need to test on parent to be able to add alternative scopes of the inline block
                   startNode->kind != AstNodeKind::FuncDecl)
            {
                startNode = startNode->parent;
            }

            flags |= COLLECT_NO_INLINE_PARAMS;
        }

        scopeUpMode = IdentifierScopeUpMode::None;

        // Macro in a sub function, stop there
        if (startNode->kind == AstNodeKind::FuncDecl)
            return;
    }

    // If we are in an inline block, jump right to the function parent
    // Not that the function parent can be null in case of inlined expression in a global for example (compile time execution)
    else if (startNode->kind == AstNodeKind::Inline)
    {
        auto inlineBlock = CastAst<AstInline>(startNode, AstNodeKind::Inline);
        if (!(inlineBlock->func->attributeFlags & ATTRIBUTE_MIXIN))
        {
            if (scopeUpMode == IdentifierScopeUpMode::None)
            {
                while (startNode && startNode->kind != AstNodeKind::FuncDecl)
                    startNode = startNode->parent;
            }

            scopeUpMode = IdentifierScopeUpMode::None;
        }
    }

    if (!startNode)
        return;

    if (startNode->hasExtMisc() && startNode->extMisc()->alternativeNode)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->extMisc()->alternativeNode, flags, scopeUpMode, scopeUpValue);
    else if (startNode->parent)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->parent, flags, scopeUpMode, scopeUpValue);

    // Mixin block, collect alternative scopes from the original source tree (with the user code, before
    // making the inline)
    if (startNode->hasExtMisc() &&
        startNode->extMisc()->alternativeNode &&
        startNode->parent->kind == AstNodeKind::CompilerMixin)
    {
        // We authorize mixin code to access the parameters of the Swag.mixin function, except if there's a #macro block
        // in the way.
        while (startNode->kind != AstNodeKind::Inline &&
               startNode->kind != AstNodeKind::CompilerMacro &&
               startNode->kind != AstNodeKind::FuncDecl)
        {
            startNode = startNode->parent;
        }

        if (startNode->kind == AstNodeKind::Inline)
        {
            auto inlineNode = CastAst<AstInline>(startNode, AstNodeKind::Inline);
            SWAG_ASSERT(inlineNode->parametersScope);
            addAlternativeScopeOnce(scopes, inlineNode->parametersScope);
        }
    }
}

bool SemanticJob::collectScopeHierarchy(SemanticContext*                   context,
                                        VectorNative<AlternativeScope>&    scopes,
                                        VectorNative<AlternativeScopeVar>& scopesVars,
                                        AstNode*                           startNode,
                                        uint32_t                           flags,
                                        IdentifierScopeUpMode              scopeUpMode,
                                        TokenParse*                        scopeUpValue)
{
    auto  job        = context->job;
    auto& toProcess  = job->scopesToProcess;
    auto  sourceFile = context->sourceFile;

    toProcess.clear();
    scopes.clear();

    // Get alternative scopes from the node hierarchy
    collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode, flags, scopeUpMode, scopeUpValue);

    auto startScope = startNode->ownerScope;
    if (startScope)
    {
        // For a backtick, do not collect scope until we find an inline block
        if (scopeUpMode == IdentifierScopeUpMode::Count)
        {
            for (uint64_t i = 0; i < scopeUpValue->literalValue.u8; i++)
            {
                while (startScope && startScope->kind != ScopeKind::Inline && startScope->kind != ScopeKind::Macro)
                    startScope = startScope->parentScope;

                if (!startScope && i == 0)
                {
                    Diagnostic diag{context->node, *scopeUpValue, Err(Err0136)};
                    return context->report(diag);
                }

                if (!startScope && i)
                {
                    Diagnostic diag{context->node, *scopeUpValue, Fmt(Err(Err0576), scopeUpValue->literalValue.u8)};
                    return context->report(diag);
                }

                startScope = startScope->parentScope;
            }

            scopeUpMode = IdentifierScopeUpMode::None;
        }

        addAlternativeScope(scopes, startScope);
        addAlternativeScope(toProcess, startScope);
    }

    // Add current file scope
    addAlternativeScope(scopes, context->sourceFile->scopeFile);
    addAlternativeScope(toProcess, context->sourceFile->scopeFile);

    // Add bootstrap
    SWAG_ASSERT(g_Workspace->bootstrapModule);
    addAlternativeScope(scopes, g_Workspace->bootstrapModule->scopeRoot);
    addAlternativeScope(toProcess, g_Workspace->bootstrapModule->scopeRoot);

    // Add runtime, except for the bootstrap
    if (!sourceFile->isBootstrapFile)
    {
        SWAG_ASSERT(g_Workspace->runtimeModule);
        addAlternativeScope(scopes, g_Workspace->runtimeModule->scopeRoot);
        addAlternativeScope(toProcess, g_Workspace->runtimeModule->scopeRoot);
    }

    for (size_t i = 0; i < toProcess.size(); i++)
    {
        auto& as    = toProcess[i];
        auto  scope = as.scope;
        if (!scope)
            continue;

        // For an inline scope, jump right to the function
        if (scope->kind == ScopeKind::Inline)
        {
            while (scope && scope->kind != ScopeKind::Function)
                scope = scope->parentScope;
            if (!scope)
                continue;
        }

        // For a macro scope, jump right to the inline
        else if (scope->kind == ScopeKind::Macro)
        {
            while (scope && scope->kind != ScopeKind::Inline)
                scope = scope->parentScope;
            if (!scope)
                continue;
            scope = scope->parentScope;
        }

        // Add parent scope
        if (scope->parentScope)
        {
            if (scope->parentScope->kind == ScopeKind::Struct && (flags & COLLECT_NO_STRUCT))
                continue;

            if (!hasAlternativeScope(scopes, scope->parentScope))
            {
                addAlternativeScope(scopes, scope->parentScope);
                addAlternativeScope(toProcess, scope->parentScope);
            }
        }
    }

    SWAG_VERIFY(scopeUpMode == IdentifierScopeUpMode::None, context->report({startNode, Err(Err0136)}));

    return true;
}

bool SemanticJob::checkCanThrow(SemanticContext* context)
{
    auto node = context->node;

    // For a try/throw inside an inline block, take the original function, except if it is flagged with 'Swag.CalleeReturn'
    if (node->ownerInline)
    {
        if (!(node->ownerInline->func->attributeFlags & ATTRIBUTE_CALLEE_RETURN) && !(node->flags & AST_IN_MIXIN))
            node->semFlags |= SEMFLAG_EMBEDDED_RETURN;
    }

    auto parentFct = (node->semFlags & SEMFLAG_EMBEDDED_RETURN) ? node->ownerInline->func : node->ownerFct;

    if (parentFct->isSpecialFunctionName())
        return context->report({node, node->token, Fmt(Err(Err0137), node->token.ctext(), node->token.ctext(), parentFct->token.ctext())});

    if (!(parentFct->typeInfo->flags & TYPEINFO_CAN_THROW) && !(parentFct->attributeFlags & ATTRIBUTE_SHARP_FUNC))
        return context->report({node, node->token, Fmt(Err(Err0138), node->token.ctext(), node->token.ctext(), parentFct->token.ctext())});

    return true;
}

bool SemanticJob::checkCanCatch(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Try, AstNodeKind::Catch, AstNodeKind::TryCatch, AstNodeKind::Assume);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);

    for (auto c : identifierRef->childs)
    {
        if (!c->resolvedSymbolOverload)
            continue;
        if (c->resolvedSymbolOverload->symbol->kind == SymbolKind::Function || c->resolvedSymbolOverload->typeInfo->isLambdaClosure())
            return true;
    }

    auto lastChild = identifierRef->childs.back();
    return context->report({node, node->token, Fmt(Err(Err0139), node->token.ctext(), lastChild->token.ctext(), Naming::aKindName(lastChild->resolvedSymbolName->kind).c_str())});
}

bool SemanticJob::resolveTryBlock(SemanticContext* context)
{
    SWAG_CHECK(checkCanThrow(context));
    return true;
}

bool SemanticJob::resolveTry(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Try);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanThrow(context));
    SWAG_CHECK(checkCanCatch(context));

    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;

    return true;
}

bool SemanticJob::resolveTryCatch(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::TryCatch);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->specFlags |= AstFuncDecl::SPECFLAG_REG_GET_CONTEXT;

    node->setBcNotifBefore(ByteCodeGenJob::emitInitStackTrace);
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);

    return true;
}

bool SemanticJob::resolveCatch(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Catch);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->specFlags |= AstFuncDecl::SPECFLAG_REG_GET_CONTEXT;

    node->allocateExtension(ExtensionKind::ByteCode);
    node->setBcNotifBefore(ByteCodeGenJob::emitInitStackTrace);
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;

    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->flags &= ~AST_DISCARD;
    node->inheritComputedValue(identifierRef);

    return true;
}

bool SemanticJob::resolveAssume(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Assume);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));

    node->allocateExtension(ExtensionKind::ByteCode);
    node->setBcNotifBefore(ByteCodeGenJob::emitInitStackTrace);
    node->typeInfo = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;

    return true;
}

bool SemanticJob::resolveThrow(SemanticContext* context)
{
    auto node      = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Throw);
    auto back      = node->childs.back();
    node->typeInfo = back->typeInfo;

    SWAG_CHECK(checkCanThrow(context));
    SWAG_CHECK(TypeManager::makeCompatibles(context, g_TypeMgr->typeInfoString, node, back, CASTFLAG_AUTO_OPCAST | CASTFLAG_CONCRETE_ENUM));
    node->byteCodeFct = ByteCodeGenJob::emitThrow;
    return true;
}
