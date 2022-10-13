#include "pch.h"
#include "SemanticJob.h"
#include "ByteCodeGenJob.h"
#include "Ast.h"
#include "Workspace.h"
#include "Generic.h"
#include "TypeManager.h"
#include "ThreadManager.h"
#include "Module.h"
#include "ErrorIds.h"
#include "LanguageSpec.h"

bool SemanticJob::preResolveIdentifierRef(SemanticContext* context)
{
    auto node = CastAst<AstIdentifierRef>(context->node, AstNodeKind::IdentifierRef);

    // When duplicating an identifier ref, and solve it again, we need to be sure that all that
    // stuff is reset
    node->typeInfo             = nullptr;
    node->previousResolvedNode = nullptr;
    node->startScope           = nullptr;
    return true;
}

bool SemanticJob::resolveIdentifierRef(SemanticContext* context)
{
    auto node                    = static_cast<AstIdentifierRef*>(context->node);
    auto childBack               = node->childs.back();
    node->resolvedSymbolName     = childBack->resolvedSymbolName;
    node->resolvedSymbolOverload = childBack->resolvedSymbolOverload;
    node->typeInfo               = childBack->typeInfo;
    if (!(node->flags & AST_IS_NAMED))
        node->token.text = childBack->token.text;
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

    if (childBack->flags & AST_VALUE_COMPUTED)
        node->inheritComputedValue(childBack);
    node->inheritOrFlag(childBack, AST_L_VALUE | AST_R_VALUE | AST_TRANSIENT | AST_VALUE_IS_TYPEINFO | AST_SIDE_EFFECTS);

    if (childBack->semFlags & AST_SEM_IS_CONST_ASSIGN)
        node->flags |= AST_IS_CONST;

    // Symbol is in fact a constant value : no need for bytecode
    if (node->resolvedSymbolOverload && (node->resolvedSymbolOverload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        node->setFlagsValueIsComputed();
        *node->computedValue = node->resolvedSymbolOverload->computedValue;
        node->flags |= AST_NO_BYTECODE_CHILDS;

        // If literal is stored in a data segment, then it's still a left value (we can take the address for example)
        if (!node->computedValue->storageSegment || node->computedValue->storageOffset == 0xFFFFFFFF)
            node->flags &= ~AST_L_VALUE;
    }

    return true;
}

bool SemanticJob::setupIdentifierRef(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return true;

    // If type of previous one was const, then we force this node to be const (cannot change it)
    if (node->parent->typeInfo && node->parent->typeInfo->isConst())
        node->flags |= AST_IS_CONST;
    if (node->typeInfo->kind == TypeInfoKind::Reference && node->typeInfo->isConst())
        node->flags |= AST_IS_CONST;
    auto overload = node->resolvedSymbolOverload;
    if (overload && overload->flags & OVERLOAD_CONST_ASSIGN)
        node->semFlags |= AST_SEM_IS_CONST_ASSIGN;

    if (node->parent->kind != AstNodeKind::IdentifierRef)
        return true;

    auto identifierRef = CastAst<AstIdentifierRef>(node->parent, AstNodeKind::IdentifierRef);

    // If we cannot assign previous, and this was AST_IS_CONST_ASSIGN_INHERIT, then we cannot assign
    // this one either
    if (identifierRef->previousResolvedNode && (identifierRef->previousResolvedNode->semFlags & AST_SEM_IS_CONST_ASSIGN_INHERIT))
    {
        node->semFlags |= AST_SEM_IS_CONST_ASSIGN;
        node->semFlags |= AST_SEM_IS_CONST_ASSIGN_INHERIT;
    }

    typeInfo                            = TypeManager::concreteType(typeInfo, CONCRETE_ALIAS);
    identifierRef->typeInfo             = typeInfo;
    identifierRef->previousResolvedNode = node;
    identifierRef->startScope           = nullptr;

    // Before making the type concrete
    if (node->typeInfo->kind == TypeInfoKind::Enum)
        identifierRef->startScope = CastTypeInfo<TypeInfoEnum>(node->typeInfo, node->typeInfo->kind)->scope;

    switch (typeInfo->kind)
    {
    case TypeInfoKind::Reference:
    {
        auto typeReference = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        auto subType       = TypeManager::concreteReferenceType(typeReference->pointedType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        else if (subType->kind == TypeInfoKind::Interface)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->itable->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto subType     = TypeManager::concreteReferenceType(typePointer->pointedType);
        if (subType->kind == TypeInfoKind::Struct || subType->kind == TypeInfoKind::Interface)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::TypeListArray:
    case TypeInfoKind::TypeListTuple:
        identifierRef->startScope = CastTypeInfo<TypeInfoList>(typeInfo, typeInfo->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Interface:
    case TypeInfoKind::Struct:
        identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(typeInfo, typeInfo->kind)->scope;
        node->typeInfo            = typeInfo;
        break;

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto subType   = TypeManager::concreteReferenceType(typeArray->finalType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto subType   = TypeManager::concreteReferenceType(typeSlice->pointedType);
        if (subType->kind == TypeInfoKind::Struct)
            identifierRef->startScope = CastTypeInfo<TypeInfoStruct>(subType, subType->kind)->scope;
        node->typeInfo = typeInfo;
        break;
    }
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

    switch (identifier->token.id)
    {
    case TokenId::IntrinsicAssert:
    {
        if (module->mustOptimizeBC(context->node))
        {
            // Remove assert(true)
            SWAG_ASSERT(identifier->callParameters && !identifier->callParameters->childs.empty());
            auto param = identifier->callParameters->childs.front();
            if ((param->flags & AST_VALUE_COMPUTED) && param->computedValue->reg.b)
                identifier->flags |= AST_NO_BYTECODE;
        }
        break;
    }
    }
}

void SemanticJob::resolvePendingLambdaTyping(AstFuncCallParam* nodeCall, OneMatch* oneMatch, int i)
{
    auto funcDecl = CastAst<AstFuncDecl>(nodeCall->typeInfo->declNode, AstNodeKind::FuncDecl);
    SWAG_ASSERT(!(funcDecl->flags & AST_IS_GENERIC));
    auto typeUndefinedFct = CastTypeInfo<TypeInfoFuncAttr>(funcDecl->typeInfo, TypeInfoKind::FuncAttr);
    auto concreteType     = TypeManager::concreteReferenceType(oneMatch->solvedParameters[i]->typeInfo);
    auto typeDefinedFct   = CastTypeInfo<TypeInfoFuncAttr>(concreteType, TypeInfoKind::Lambda);

    // Replace every parameters types
    for (int paramIdx = 0; paramIdx < typeUndefinedFct->parameters.size(); paramIdx++)
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
    for (int paramIdx = 0; paramIdx < typeUndefinedFct->genericParameters.size(); paramIdx++)
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
    funcDecl->semFlags &= ~AST_SEM_PENDING_LAMBDA_TYPING;
    g_ThreadMgr.addJob(funcDecl->pendingLambdaJob);
}

bool SemanticJob::createTmpVarStruct(SemanticContext* context, AstIdentifier* identifier)
{
    auto sourceFile = identifier->sourceFile;
    auto callP      = identifier->callParameters;
    identifier->flags |= AST_R_VALUE | AST_GENERATED | AST_NO_BYTECODE;

    // Be sure it's the NAME{} syntax
    if (!(identifier->callParameters->flags & AST_CALL_FOR_STRUCT))
        return context->report(callP, Fmt(Err(Err0082), identifier->typeInfo->getDisplayNameC()));

    auto varParent = identifier->identifierRef->parent;
    while (varParent->kind == AstNodeKind::ExpressionList)
        varParent = varParent->parent;

    // Declare a variable
    auto varNode = Ast::newVarDecl(sourceFile, Fmt("__1tmp_%d", g_UniqueID.fetch_add(1)), varParent);

    // Inherit alternative scopes.
    if (identifier->parent->extension)
        varNode->addAlternativeScopes(identifier->parent->extension->alternativeScopes);

    // If we are in a const declaration, that temporary variable should be a const too...
    if (identifier->parent->parent->kind == AstNodeKind::ConstDecl)
        varNode->kind = AstNodeKind::ConstDecl;

    // At global scope, this should be a constant declaration, not a variable, as we cannot assign a global variable to
    // another global variable at compile time
    else if (identifier->ownerScope->isGlobalOrImpl())
        varNode->kind = AstNodeKind::ConstDecl;

    auto typeNode = Ast::newTypeExpression(sourceFile, varNode);
    typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
    varNode->flags |= AST_GENERATED;
    varNode->type = typeNode;
    CloneContext cloneContext;
    cloneContext.parent     = typeNode;
    cloneContext.cloneFlags = CLONE_RAW;
    typeNode->identifier    = identifier->identifierRef->clone(cloneContext);
    auto back               = CastAst<AstIdentifier>(typeNode->identifier->childs.back(), AstNodeKind::Identifier);
    back->flags &= ~AST_NO_BYTECODE;
    back->flags |= AST_IN_TYPE_VAR_DECLARATION;

    // Call parameters have already been evaluated, so do not reevaluate them again
    back->callParameters->flags |= AST_NO_SEMANTIC;

    // If this is in a return expression, then force the identifier type to be retval
    if (context->node->parent && context->node->parent->inSimpleReturn())
        typeNode->typeFlags |= TYPEFLAG_RETVAL;

    // And make a reference to that variable
    auto identifierRef = identifier->identifierRef;
    identifierRef->childs.clear();
    auto idNode = Ast::newIdentifier(sourceFile, varNode->token.text, identifierRef, identifierRef);
    idNode->flags |= AST_R_VALUE | AST_TRANSIENT;

    // Reset parsing
    identifierRef->startScope = nullptr;

    // The variable will be inserted after its reference (below), so we need to inverse the order of evaluation.
    // Seems a little bit like a hack. Not sure this will always work.
    varParent->flags |= AST_REVERSE_SEMANTIC;

    // Add the 2 nodes to the semantic
    context->job->nodes.pop_back();
    context->job->nodes.push_back(idNode);
    context->job->nodes.push_back(varNode);
    context->result = ContextResult::NewChilds;

    return true;
}

bool SemanticJob::setSymbolMatchCallParams(SemanticContext* context, AstIdentifier* identifier, OneMatch& oneMatch)
{
    if (!identifier->callParameters)
        return true;

    auto sourceFile   = context->sourceFile;
    auto typeInfoFunc = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);

    // :ClosureForceFirstParam
    // Add a first dummy parameter in case of closure
    if (typeInfoFunc->isClosure() && !(identifier->doneFlags & AST_DONE_CLOSURE_FIRST_PARAM))
    {
        auto fcp = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
        Ast::removeFromParent(fcp);
        Ast::addChildFront(identifier->callParameters, fcp);
        fcp->setFlagsValueIsComputed();
        fcp->computedValue->reg.pointer = nullptr;
        fcp->typeInfo                   = g_TypeMgr->typeInfoNull;
        fcp->flags |= AST_GENERATED;
        identifier->doneFlags |= AST_DONE_CLOSURE_FIRST_PARAM;

        auto node = Ast::newNode<AstNode>(nullptr, AstNodeKind::Literal, context->sourceFile, fcp);
        node->setFlagsValueIsComputed();
        node->flags |= AST_GENERATED;
        node->typeInfo = g_TypeMgr->typeInfoNull;
    }

    sortParameters(identifier->callParameters);
    auto maxParams = identifier->callParameters->childs.size();
    for (int idx = 0; idx < maxParams; idx++)
    {
        auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[idx], AstNodeKind::FuncCallParam);

        int i = idx;
        if (idx < typeInfoFunc->parameters.size() - 1 || !(typeInfoFunc->flags & (TYPEINFO_VARIADIC | TYPEINFO_C_VARIADIC)))
            i = nodeCall->indexParam;

        // This is a lambda that was waiting for a match to have its types, and to continue solving its content
        if (nodeCall->typeInfo->kind == TypeInfoKind::Lambda && (nodeCall->typeInfo->declNode->semFlags & AST_SEM_PENDING_LAMBDA_TYPING))
            resolvePendingLambdaTyping(nodeCall, &oneMatch, i);

        uint32_t castFlags = CASTFLAG_AUTO_OPCAST | CASTFLAG_ACCEPT_PENDING | CASTFLAG_PARAMS;
        if (i == 0 && oneMatch.ufcs)
            castFlags |= CASTFLAG_UFCS;

        TypeInfo* toType = nullptr;
        if (i < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[i])
        {
            toType = oneMatch.solvedParameters[i]->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, toType, nullptr, nodeCall, castFlags));
            if (context->result != ContextResult::Done)
                return true;
        }
        else if (oneMatch.solvedParameters.size() && oneMatch.solvedParameters.back() && oneMatch.solvedParameters.back()->typeInfo->kind == TypeInfoKind::TypedVariadic)
        {
            toType = oneMatch.solvedParameters.back()->typeInfo;
            SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters.back()->typeInfo, nullptr, nodeCall));
            if (context->result != ContextResult::Done)
                return true;
        }

        // For a variadic parameter, we need to generate the concrete typeinfo for the corresponding 'any' type
        if (i >= typeInfoFunc->parameters.size() - 1 && (typeInfoFunc->flags & TYPEINFO_VARIADIC))
        {
            auto  module       = sourceFile->module;
            auto& typeTable    = module->typeTable;
            auto  concreteType = TypeManager::concreteType(nodeCall->typeInfo, CONCRETE_FUNC);
            nodeCall->allocateComputedValue();
            nodeCall->computedValue->storageSegment = getConstantSegFromContext(nodeCall);
            SWAG_CHECK(typeTable.makeConcreteTypeInfo(context, concreteType, nodeCall->computedValue->storageSegment, &nodeCall->computedValue->storageOffset));
        }

        // If passing a closure
        // :FctCallParamClosure
        auto toTypeRef = TypeManager::concreteType(toType, CONCRETE_ALIAS);
        auto makePtrL  = nodeCall->childs.empty() ? nullptr : nodeCall->childs.front();

        if (makePtrL && toTypeRef && toTypeRef->isClosure())
        {
            bool convert = false;
            if (makePtrL->kind == AstNodeKind::MakePointer || makePtrL->kind == AstNodeKind::MakePointerLambda)
                convert = true;
            if (makePtrL->typeInfo && makePtrL->typeInfo->isLambda())
                convert = true;
            if (makePtrL->typeInfo == g_TypeMgr->typeInfoNull)
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
                }
                else if (makePtrL->typeInfo == g_TypeMgr->typeInfoNull)
                {
                    nodeCall->flags &= ~AST_VALUE_COMPUTED;
                    makePtrL->flags |= AST_NO_BYTECODE;
                    Ast::removeFromParent(makePtrL);
                }
                else
                {
                    makePtrL->semFlags |= AST_SEM_ONCE;
                    Ast::removeFromParent(makePtrL);
                    Ast::addChildBack(varNode, makePtrL);
                    varNode->assignment = makePtrL;
                }

                varNode->type           = Ast::newTypeExpression(sourceFile, varNode);
                varNode->type->typeInfo = toType;
                varNode->type->flags |= AST_NO_SEMANTIC;

                auto idRef = Ast::newIdentifierRef(sourceFile, varNode->token.text, nodeCall);
                idRef->allocateExtension();
                idRef->extension->exportNode = makePtrL;

                // Add the 2 nodes to the semantic
                context->job->nodes.push_back(idRef);
                context->job->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, and the new variable, which is a child,
                // will be reevaluated too, so twice because of the push above. So we set a special flag to not reevaluate
                // it twice.
                varNode->semFlags |= AST_SEM_ONCE;

                context->result = ContextResult::NewChilds;
            }
        }
    }

    // Deal with opAffect automatic conversion
    // :opAffectParam
    for (int i = 0; i < maxParams; i++)
    {
        auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);

        if (nodeCall->extension && nodeCall->extension->resolvedUserOpSymbolOverload)
        {
            auto overload = nodeCall->extension->resolvedUserOpSymbolOverload;
            if (overload->symbol->name == g_LangSpec->name_opAffect || overload->symbol->name == g_LangSpec->name_opAffectSuffix)
            {
                SWAG_ASSERT(nodeCall->extension->resolvedUserOpSymbolOverload);
                SWAG_ASSERT(nodeCall->castedTypeInfo);
                nodeCall->extension->resolvedUserOpSymbolOverload = nullptr;
                nodeCall->castedTypeInfo                          = nullptr;

                auto varNode = Ast::newVarDecl(sourceFile, Fmt("__2tmp_%d", g_UniqueID.fetch_add(1)), identifier);
                varNode->inheritTokenLocation(nodeCall);

                // Put child front, because emitCall wants the parameters to be the last
                Ast::removeFromParent(varNode);
                Ast::addChildFront(identifier, varNode);

                CloneContext cloneContext;
                cloneContext.parent      = varNode;
                cloneContext.parentScope = identifier->ownerScope;

                auto typeExpr      = Ast::newTypeExpression(sourceFile, varNode);
                typeExpr->typeInfo = nodeCall->typeInfo;
                typeExpr->flags |= AST_NO_SEMANTIC;
                varNode->type = typeExpr;

                auto assign = nodeCall->childs.front();
                if (assign->kind == AstNodeKind::Cast)
                    assign = assign->childs.back();
                varNode->assignment = assign->clone(cloneContext);

                Ast::removeFromParent(nodeCall);

                auto newParam        = Ast::newFuncCallParam(sourceFile, identifier->callParameters);
                newParam->indexParam = nodeCall->indexParam;
                Ast::removeFromParent(newParam);
                Ast::insertChild(identifier->callParameters, newParam, i);
                Ast::newIdentifierRef(sourceFile, varNode->token.text, newParam);

                // We want to export the original parameter, not the temporary variable reference
                newParam->allocateExtension();
                newParam->extension->exportNode = nodeCall;

                // Add the 2 nodes to the semantic
                context->job->nodes.push_back(newParam);
                context->job->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, so the new variable, which is a child of
                // that identifier, will be reevaluated too (so twice because of the push above).
                // So we set a special flag to not reevaluate it twice.
                varNode->semFlags |= AST_SEM_ONCE;

                context->result = ContextResult::NewChilds;
            }
        }
    }

    // Deal with default values for structs and uncomputed values
    // We need to add a temporary variable initialized with the default value, and reference
    // that temporary variable as a new function call parameter
    if (typeInfoFunc->parameters.size() && maxParams < typeInfoFunc->parameters.size())
    {
        auto funcDecl = CastAst<AstFuncDecl>(typeInfoFunc->declNode, AstNodeKind::FuncDecl);
        for (int i = 0; i < funcDecl->parameters->childs.size(); i++)
        {
            auto funcParam = CastAst<AstVarDecl>(funcDecl->parameters->childs[i], AstNodeKind::FuncDeclParam);
            if (!funcParam->assignment)
                continue;
            switch (funcParam->assignment->token.id)
            {
            case TokenId::CompilerCallerLocation:
            case TokenId::CompilerCallerFunction:
            case TokenId::CompilerBuildCfg:
            case TokenId::CompilerArch:
            case TokenId::CompilerOs:
            case TokenId::CompilerSwagOs:
            case TokenId::CompilerBackend:
                continue;
            }

            auto typeParam = TypeManager::concreteReference(funcParam->typeInfo);
            if (typeParam->kind != TypeInfoKind::Struct &&
                typeParam->kind != TypeInfoKind::TypeListTuple &&
                typeParam->kind != TypeInfoKind::TypeListArray &&
                !typeParam->isNative(NativeTypeKind::Any) &&
                !typeParam->isClosure() &&
                (funcParam->assignment->flags & AST_VALUE_COMPUTED))
                continue;

            bool covered = false;
            for (int j = 0; j < maxParams; j++)
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
                    varNode->type = funcParam->type->clone(cloneContext);

                // Need to test sizeof because assignement can be @{}. In that case, we just reference
                // the temporary variable
                if (funcParam->assignment->typeInfo->sizeOf && funcParam->assignment->typeInfo != g_TypeMgr->typeInfoNull)
                {
                    varNode->assignment = funcParam->assignment->clone(cloneContext);
                    varNode->assignment->inheritOwners(identifier);
                }

                auto newParam = Ast::newFuncCallParam(sourceFile, identifier->callParameters);

                // Make it a named param, in case some other default "normal" parameters are before (because in that case
                // we let the emitCall to deal with those default parameters)
                newParam->namedParam = funcParam->token.text;

                newParam->indexParam = i;
                newParam->flags |= AST_GENERATED;
                Ast::newIdentifierRef(sourceFile, varNode->token.text, newParam);

                // Add the 2 nodes to the semantic
                context->job->nodes.push_back(newParam);
                context->job->nodes.push_back(varNode);

                // If call is inlined, then the identifier will be reevaluated, and the new variable, which is a child,
                // will be reevaluated too, so twice because of the push above. So we set a special flag to not reevaluate
                // it twice.
                varNode->semFlags |= AST_SEM_ONCE;

                context->result = ContextResult::NewChilds;
            }
        }
    }

    return true;
}

static bool isStatementIdentifier(AstIdentifier* identifier)
{
    // :SilentCall
    if (identifier->token.text.empty())
        return false;

    auto checkParent = identifier->identifierRef->parent;
    if (checkParent->kind == AstNodeKind::Try || checkParent->kind == AstNodeKind::Catch || checkParent->kind == AstNodeKind::Assume)
        checkParent = checkParent->parent;
    if (checkParent->kind == AstNodeKind::Statement ||
        checkParent->kind == AstNodeKind::StatementNoScope ||
        checkParent->kind == AstNodeKind::Defer ||
        checkParent->kind == AstNodeKind::SwitchCaseBlock)
    {
        // If this is the last identifier
        if (identifier->childParentIdx == identifier->identifierRef->childs.size() - 1)
            return true;

        // If this is not the last identifier, and it's not a function call
        auto back = identifier->identifierRef->childs.back();
        if (back->kind == AstNodeKind::Identifier && !((AstIdentifier*) back)->callParameters)
            return true;
    }

    return false;
}

bool SemanticJob::setSymbolMatch(SemanticContext* context, AstIdentifierRef* parent, AstIdentifier* identifier, OneMatch& oneMatch)
{
    auto symbol       = oneMatch.symbolOverload->symbol;
    auto overload     = oneMatch.symbolOverload;
    auto dependentVar = oneMatch.dependentVar;
    auto sourceFile   = context->sourceFile;

    // Test x.toto with x not a struct (like a native type for example), but toto is known, so
    // no error was raised before
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        overload->typeInfo->kind != TypeInfoKind::Lambda &&
        !parent->startScope &&
        parent->previousResolvedNode &&
        !identifier->token.text.empty() && // :SilentCall
        !parent->previousResolvedNode->typeInfo->isPointerTo(TypeInfoKind::Struct) &&
        parent->previousResolvedNode->typeInfo->kind != TypeInfoKind::Struct)
    {
        return context->report(parent->previousResolvedNode, Fmt(Err(Err0085), parent->previousResolvedNode->token.ctext(), parent->previousResolvedNode->typeInfo->getDisplayNameC()));
    }

    // If a variable on the left has only been used for scoping, and not evaluated as an ufcs source, then this is an
    // an error too, cause it's too strange.
    // x.toto() with toto taking no argument for example, but toto is 'in' x scope.
    if (symbol &&
        symbol->kind == SymbolKind::Function &&
        parent->startScope &&
        parent->previousResolvedNode &&
        parent->previousResolvedNode->resolvedSymbolName &&
        parent->previousResolvedNode->resolvedSymbolName->kind == SymbolKind::Variable &&
        !(parent->previousResolvedNode->flags & AST_FROM_UFCS))
    {
        if (parent->previousResolvedNode->kind == AstNodeKind::Identifier && parent->previousResolvedNode->specFlags & AST_SPEC_IDENTIFIER_FROM_WITH)
            return context->report(parent->previousResolvedNode, Fmt(Err(Err0310), parent->previousResolvedNode->token.ctext(), symbol->name.c_str(), parent->startScope->name.c_str()));
        return context->report(parent->previousResolvedNode, Fmt(Err(Err0086), parent->previousResolvedNode->token.ctext(), symbol->name.c_str(), parent->startScope->name.c_str()));
    }

    // A.X and A is an array : missing index
    if (symbol &&
        symbol->kind == SymbolKind::Variable &&
        identifier->typeInfo->kind == TypeInfoKind::Array &&
        identifier->parent->kind != AstNodeKind::ArrayPointerIndex &&
        identifier->parent == parent &&
        parent->childs.back() != identifier)
    {
        return context->report(Hint::isType(identifier->typeInfo), {identifier, Fmt(Err(Err0187), symbol->name.c_str())});
    }

    // Reapply back the values of the match to the call parameter node
    for (auto& pp : oneMatch.paramParameters)
    {
        pp.param->indexParam        = pp.indexParam;
        pp.param->resolvedParameter = pp.resolvedParameter;
    }

    // Direct reference to a constexpr typeinfo
    if (parent->previousResolvedNode &&
        (parent->previousResolvedNode->flags & AST_VALUE_IS_TYPEINFO) &&
        symbol->kind == SymbolKind::Variable)
    {
        if (derefLiteralStruct(context, parent, overload))
        {
            parent->previousResolvedNode = context->node;
            return true;
        }

        identifier->flags |= AST_R_VALUE;
    }

    // Direct reference to a constexpr structure
    if (parent->previousResolvedNode &&
        (parent->previousResolvedNode->flags & AST_VALUE_COMPUTED) &&
        parent->previousResolvedNode->typeInfo->kind == TypeInfoKind::Struct &&
        symbol->kind == SymbolKind::Variable)
    {
        if (derefLiteralStruct(context, parent, overload))
        {
            parent->previousResolvedNode       = context->node;
            identifier->resolvedSymbolName     = overload->symbol;
            identifier->resolvedSymbolOverload = overload;
            return true;
        }

        identifier->flags |= AST_R_VALUE;
    }

    // Direct reference of a struct field inside a const array
    if (parent->previousResolvedNode &&
        parent->previousResolvedNode->kind == AstNodeKind::ArrayPointerIndex &&
        parent->previousResolvedNode->typeInfo->kind == TypeInfoKind::Struct &&
        symbol->kind == SymbolKind::Variable)
    {
        auto arrayNode = CastAst<AstArrayPointerIndex>(parent->previousResolvedNode, AstNodeKind::ArrayPointerIndex);
        auto arrayOver = arrayNode->array->resolvedSymbolOverload;
        if (arrayOver && (arrayOver->flags & OVERLOAD_COMPUTED_VALUE))
        {
            if (arrayNode->access->flags & AST_VALUE_COMPUTED)
            {
                auto typePtr = CastTypeInfo<TypeInfoArray>(arrayNode->array->typeInfo, TypeInfoKind::Array);
                SWAG_ASSERT(arrayOver->computedValue.storageOffset != UINT32_MAX);
                SWAG_ASSERT(arrayOver->computedValue.storageSegment);
                auto ptr = arrayOver->computedValue.storageSegment->address(arrayOver->computedValue.storageOffset);
                ptr += arrayNode->access->computedValue->reg.u64 * typePtr->finalType->sizeOf;
                if (derefLiteralStruct(context, ptr, overload, &sourceFile->module->constantSegment))
                {
                    parent->previousResolvedNode       = context->node;
                    identifier->resolvedSymbolName     = overload->symbol;
                    identifier->resolvedSymbolOverload = overload;
                    return true;
                }

                identifier->flags |= AST_R_VALUE;
            }
        }
    }

    // If this a L or R value
    if (overload->flags & OVERLOAD_VAR_STRUCT)
    {
        if (symbol->kind != SymbolKind::GenericType)
        {
            if (parent->previousResolvedNode)
            {
                if (parent->previousResolvedNode->flags & AST_R_VALUE)
                    identifier->flags |= AST_L_VALUE | AST_R_VALUE;
                else
                    identifier->flags |= (parent->previousResolvedNode->flags & AST_L_VALUE);
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
        parent->resolvedSymbolName     = symbol;
        parent->resolvedSymbolOverload = overload;
    }

    identifier->resolvedSymbolName     = symbol;
    identifier->resolvedSymbolOverload = overload;

    if (identifier->typeInfo->flags & TYPEINFO_GENERIC)
        identifier->flags |= AST_IS_GENERIC;

    // Symbol is linked to a using var : insert the variable name before the symbol
    // Except if symbol is a constant !
    if (!(overload->flags & OVERLOAD_COMPUTED_VALUE))
    {
        if (dependentVar && parent->kind == AstNodeKind::IdentifierRef && symbol->kind != SymbolKind::Function)
        {
            auto idRef = CastAst<AstIdentifierRef>(parent, AstNodeKind::IdentifierRef);
            if (dependentVar->kind == AstNodeKind::IdentifierRef)
            {
                for (int i = (int) dependentVar->childs.size() - 1; i >= 0; i--)
                {
                    auto child  = dependentVar->childs[i];
                    auto idNode = Ast::newIdentifier(dependentVar->sourceFile, child->token.text, idRef, nullptr);
                    idNode->inheritOrFlag(idRef, AST_IN_MIXIN);
                    idNode->inheritTokenLocation(idRef);
                    Ast::addChildFront(idRef, idNode);
                    context->job->nodes.push_back(idNode);
                    if (i == 0)
                        idNode->specFlags |= identifier->specFlags & AST_SPEC_IDENTIFIER_BACKTICK;
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

                idNode->specFlags |= identifier->specFlags & AST_SPEC_IDENTIFIER_BACKTICK;
                Ast::insertChild(idRef, idNode, newParent->childParentIdx);
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
        typeAlias = TypeManager::concreteType(identifier->typeInfo, CONCRETE_ALIAS);
        if (typeAlias->kind == TypeInfoKind::Struct)
            symbolKind = SymbolKind::Struct;
    }

    // Global identifier call. Must be a call to a mixin function, that's it...
    if (identifier->identifierRef && identifier->identifierRef->flags & AST_GLOBAL_CALL)
        return context->report(identifier, Fmt(Err(Err0087), identifier->token.ctext(), SymTable::getArticleKindName(symbolKind)));

    checkDeprecated(context, identifier);

    switch (symbolKind)
    {
    case SymbolKind::GenericType:
        SWAG_CHECK(setupIdentifierRef(context, identifier, identifier->typeInfo));
        break;

    case SymbolKind::Namespace:
        parent->startScope = CastTypeInfo<TypeInfoNamespace>(identifier->typeInfo, identifier->typeInfo->kind)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::Enum:
        parent->startScope = CastTypeInfo<TypeInfoEnum>(identifier->typeInfo, identifier->typeInfo->kind)->scope;
        identifier->flags |= AST_CONST_EXPR;
        break;

    case SymbolKind::EnumValue:
        SWAG_CHECK(setupIdentifierRef(context, identifier, TypeManager::concreteType(identifier->typeInfo)));
        identifier->setFlagsValueIsComputed();
        identifier->flags |= AST_R_VALUE;
        *identifier->computedValue = identifier->resolvedSymbolOverload->computedValue;
        break;

    case SymbolKind::Struct:
    case SymbolKind::Interface:
        if (!(overload->flags & OVERLOAD_IMPL_IN_STRUCT))
            SWAG_CHECK(setupIdentifierRef(context, identifier, identifier->typeInfo));
        parent->startScope = CastTypeInfo<TypeInfoStruct>(typeAlias, typeAlias->kind)->scope;

        if (!identifier->callParameters)
            identifier->flags |= AST_CONST_EXPR;

        // Be sure it's the NAME{} syntax
        if (identifier->callParameters && !(identifier->flags & AST_GENERATED) && !(identifier->callParameters->flags & AST_CALL_FOR_STRUCT))
            return context->report(identifier, Fmt(Err(Err0082), identifier->typeInfo->getDisplayNameC()));

        // Need to make all types compatible, in case a cast is necessary
        if (identifier->callParameters)
        {
            sortParameters(identifier->callParameters);
            auto maxParams = identifier->callParameters->childs.size();
            for (int i = 0; i < maxParams; i++)
            {
                auto nodeCall = CastAst<AstFuncCallParam>(identifier->callParameters->childs[i], AstNodeKind::FuncCallParam);
                int  idx      = nodeCall->indexParam;
                if (idx < oneMatch.solvedParameters.size() && oneMatch.solvedParameters[idx])
                    SWAG_CHECK(TypeManager::makeCompatibles(context, oneMatch.solvedParameters[idx]->typeInfo, nullptr, nodeCall, CASTFLAG_TRY_COERCE | CASTFLAG_FORCE_UNCONST));
            }
        }

        // A struct with parameters is in fact the creation of a temporary variable
        if (identifier->callParameters && !(identifier->flags & AST_GENERATED))
        {
            if (!(identifier->flags & AST_IN_TYPE_VAR_DECLARATION) && !(identifier->flags & AST_IN_FUNC_DECL_PARAMS))
            {
                if (identifier->parent->parent->kind == AstNodeKind::VarDecl || identifier->parent->parent->kind == AstNodeKind::ConstDecl)
                {
                    // Optim if we have var = Struct{}
                    // In that case, no need to generate a temporary variable. We just consider Struct{} as the type definition
                    // of 'var'
                    auto varNode = CastAst<AstVarDecl>(identifier->parent->parent, AstNodeKind::VarDecl, AstNodeKind::ConstDecl);
                    if (varNode->assignment == identifier->parent)
                    {
                        if (!varNode->type)
                        {
                            auto typeNode       = Ast::newTypeExpression(sourceFile, varNode);
                            varNode->type       = typeNode;
                            varNode->assignment = nullptr;
                            typeNode->flags |= AST_HAS_STRUCT_PARAMETERS;
                            typeNode->specFlags |= AST_SPEC_TYPEEXPRESSION_DONEGEN;
                            identifier->semFlags |= AST_SEM_ONCE;
                            Ast::removeFromParent(identifier->parent);
                            Ast::addChildBack(typeNode, identifier->parent);
                            typeNode->identifier = identifier->parent;
                            context->job->nodes.pop_back();
                            context->job->nodes.pop_back();
                            context->job->nodes.push_back(typeNode);
                            context->result = ContextResult::NewChilds;
                            return true;
                        }
                    }
                }
                else if (identifier->parent->parent->kind == AstNodeKind::TypeExpression && identifier->parent->parent->specFlags & AST_SPEC_TYPEEXPRESSION_DONEGEN)
                {
                    return true;
                }

                SWAG_CHECK(createTmpVarStruct(context, identifier));
                return true;
            }
        }

        break;

    case SymbolKind::Variable:
    {
        // If this is a struct variable, and it's referenced (identifierRef has a startScope), then we
        // wait for the struct container to be solved. We do not want the semantic to continue with
        // an unsolved struct, because that means that storageOffset has not been computed yet
        // (and in some cases we can go to the bytecode generation with the struct not solved).
        if (overload->flags & OVERLOAD_VAR_STRUCT && identifier->identifierRef->startScope)
        {
            auto parentStructNode = identifier->identifierRef->startScope->owner;
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
                return context->report(identifier, Fmt(Err(Err0091), AstNode::getKindName(overload->node).c_str(), overload->node->token.ctext(), ownerFct->getDisplayNameC()));
            }
        }

        // Transform the variable to a constant node
        if (overload->flags & OVERLOAD_COMPUTED_VALUE)
        {
            if (overload->node->flags & AST_VALUE_IS_TYPEINFO)
                identifier->flags |= AST_VALUE_IS_TYPEINFO;
            identifier->setFlagsValueIsComputed();
            *identifier->computedValue = overload->computedValue;

            // If constant is inside an expression, everything before should be constant too.
            // Otherwise that means that we reference a constant threw a variable
            //
            // x := y.Constant where y is a variable
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
            for (int i = 0; i < (int) child->childParentIdx; i++)
            {
                auto brother = checkParent->childs[i];
                if (!(brother->flags & AST_VALUE_COMPUTED) &&
                    brother->resolvedSymbolOverload &&
                    brother->resolvedSymbolOverload->symbol->kind == SymbolKind::Variable)
                {
                    brother->flags |= AST_NO_BYTECODE;
                }
            }
        }

        // Setup parent if necessary
        auto typeInfo = TypeManager::concreteType(identifier->typeInfo, CONCRETE_ALL & ~CONCRETE_FORCEALIAS);
        SWAG_CHECK(setupIdentifierRef(context, identifier, typeInfo));

        // If this is a 'code' variable, when passing code from one macro to another, then do not generate bytecode
        // for it, as this is not really a variable
        if (typeInfo->kind == TypeInfoKind::Code)
            identifier->flags |= AST_NO_BYTECODE;

        // Lambda call
        if (typeInfo->kind == TypeInfoKind::Lambda && identifier->callParameters)
        {
            auto typeInfoRet = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::Lambda)->returnType;

            // Check return value
            if (!typeInfoRet->isNative(NativeTypeKind::Void))
            {
                if (isStatementIdentifier(identifier))
                {
                    if (!(overload->node->attributeFlags & ATTRIBUTE_DISCARDABLE) && !(identifier->flags & AST_DISCARD))
                    {
                        PushErrHint errh(Hnt(Hnt0023));
                        Diagnostic  diag(identifier, Fmt(Err(Err0092), overload->node->token.ctext()));
                        Diagnostic  note(overload->node, Nte(Nte0039), DiagnosticLevel::Note);
                        return context->report(diag, &note);
                    }
                    else
                    {
                        identifier->flags |= AST_DISCARD;
                    }
                }
            }
            else if (typeInfoRet->isNative(NativeTypeKind::Void) && (identifier->flags & AST_DISCARD))
            {
                Diagnostic diag{identifier, Err(Err0094)};
                Diagnostic note{overload->node, Nte(Nte0039), DiagnosticLevel::Note};
                return context->report(Hint::isType(typeInfo), diag, &note);
            }

            // From now this is considered as a function, not a lambda
            auto funcType           = typeInfo->clone();
            funcType->kind          = TypeInfoKind::FuncAttr;
            identifier->typeInfo    = funcType;
            identifier->byteCodeFct = ByteCodeGenJob::emitLambdaCall;

            // Need to make all types compatible, in case a cast is necessary
            SWAG_CHECK(setSymbolMatchCallParams(context, identifier, oneMatch));

            // For a return by copy, need to reserve room on the stack for the return result
            auto returnType = TypeManager::concreteType(identifier->typeInfo);
            if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
            {
                identifier->flags |= AST_TRANSIENT;
                identifier->allocateComputedValue();
                identifier->computedValue->storageOffset = identifier->ownerScope->startStackSize;
                identifier->ownerScope->startStackSize += returnType->sizeOf;
                SemanticJob::setOwnerMaxStackSize(identifier, identifier->ownerScope->startStackSize);
            }
        }
        else
        {
            if (isStatementIdentifier(identifier))
            {
                return context->report(identifier, Fmt(Err(Err0096), identifier->token.ctext()));
            }
        }

        if (identifier->forceTakeAddress())
            identifier->resolvedSymbolOverload->flags |= OVERLOAD_CAN_CHANGE;

        break;
    }

    case SymbolKind::Function:
    {
        // Be sure that we didn't use a variable as a 'scope'
        if (identifier->childParentIdx)
        {
            auto prev = identifier->identifierRef->childs[identifier->childParentIdx - 1];
            if (prev->resolvedSymbolName && prev->resolvedSymbolName->kind == SymbolKind::Variable && !(prev->flags & AST_FROM_UFCS))
            {
                return context->report(Hnt(Hnt0026), {prev, Fmt(Err(Err0097), AstNode::getKindName(prev->resolvedSymbolOverload->node).c_str(), prev->token.ctext(), identifier->token.ctext())});
            }
        }

        identifier->flags |= AST_SIDE_EFFECTS;

        // Be sure it's () and not {}
        if (identifier->callParameters && (identifier->callParameters->flags & AST_CALL_FOR_STRUCT))
            return context->report(identifier->callParameters, Fmt(Err(Err0098), identifier->token.ctext()));

        // Capture syntax
        if (identifier->callParameters && !identifier->callParameters->aliasNames.empty())
        {
            if (!(overload->node->attributeFlags & (ATTRIBUTE_MACRO | ATTRIBUTE_MIXIN)))
            {
                auto cp = identifier->callParameters;
                return context->report({cp->sourceFile, cp->aliasNames.front().startLocation, cp->aliasNames.back().endLocation, Fmt(Err(Err0099), identifier->token.ctext())});
            }
        }

        // Now we need to be sure that the function is now complete
        // If not, we need to wait for it
        context->job->waitOverloadCompleted(overload);
        if (context->result == ContextResult::Pending)
            return true;

        if (identifier->token.text == g_LangSpec->name_opInit)
            return context->report(identifier, Err(Err0100));
        if (identifier->token.text == g_LangSpec->name_opDrop)
            return context->report(identifier, Err(Err0101));
        if (identifier->token.text == g_LangSpec->name_opPostCopy)
            return context->report(identifier, Err(Err0103));
        if (identifier->token.text == g_LangSpec->name_opPostMove)
            return context->report(identifier, Err(Err0104));

        // Be sure this is not a 'forward' decl
        if (overload->node->flags & AST_EMPTY_FCT && !(overload->node->attributeFlags & ATTRIBUTE_FOREIGN) && identifier->token.text[0] != '@')
        {
            Diagnostic diag{identifier, Fmt(Err(Err0105), identifier->token.ctext())};
            Diagnostic note{overload->node, Nte(Nte0033), DiagnosticLevel::NotePack};
            return context->report(diag, &note);
        }

        identifier->flags |= AST_L_VALUE | AST_R_VALUE;

        // Need to make all types compatible, in case a cast is necessary
        if (!identifier->ownerFct || !(identifier->ownerFct->flags & AST_IS_GENERIC))
        {
            SWAG_CHECK(setSymbolMatchCallParams(context, identifier, oneMatch));
            if (context->result != ContextResult::Done)
                return true;
        }

        // Be sure the call is valid
        if ((identifier->token.text[0] != '@') && !(overload->node->attributeFlags & ATTRIBUTE_FOREIGN))
        {
            auto ownerFct = identifier->ownerFct;
            if (ownerFct)
            {
                auto fctAttributes = ownerFct->attributeFlags;

                if (!(fctAttributes & ATTRIBUTE_COMPILER) && (overload->node->attributeFlags & ATTRIBUTE_COMPILER) && !(identifier->flags & AST_RUN_BLOCK))
                {
                    Diagnostic note{overload->node, Fmt(Nte(Nte0029), overload->node->token.ctext()), DiagnosticLevel::NotePack};
                    return context->report({identifier, Fmt(Err(Err0107), overload->node->token.ctext(), ownerFct->getDisplayNameC())}, &note);
                }

                if (!(fctAttributes & ATTRIBUTE_TEST_FUNC) && (overload->node->attributeFlags & ATTRIBUTE_TEST_FUNC))
                {
                    Diagnostic note{overload->node, Fmt(Nte(Nte0029), overload->node->token.ctext()), DiagnosticLevel::NotePack};
                    return context->report({identifier, Fmt(Err(Err0108), overload->node->token.ctext(), ownerFct->getDisplayNameC())}, &note);
                }
            }
        }

        // This is for a lambda
        if (identifier->forceTakeAddress())
        {
            // The makePointer will deal with the real make lambda thing
            identifier->flags |= AST_NO_BYTECODE;
            break;
        }

        // The function call is constexpr if the function is, and all parameters are
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
                auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(identifier->typeInfo, TypeInfoKind::FuncAttr);
                auto returnType = TypeManager::concreteReferenceType(typeFunc->returnType);

                // :CheckConstExprFuncReturnType
                if (returnType &&
                    !returnType->isNative(NativeTypeKind::String) &&
                    !returnType->isNativeIntegerOrRune() &&
                    !returnType->isNativeFloat() &&
                    !returnType->isNative(NativeTypeKind::Bool) &&
                    !(returnType->flags & TYPEINFO_RETURN_BY_COPY) && // Treated later (as errors)
                    !(identifier->semFlags & AST_SEM_EXEC_RET_STACK))
                {
                    identifier->flags &= ~AST_CONST_EXPR;
                }
            }
        }

        auto returnType = TypeManager::concreteType(identifier->typeInfo, CONCRETE_ALL & ~CONCRETE_FORCEALIAS);

        // Check return value
        if (!returnType->isNative(NativeTypeKind::Void))
        {
            if (isStatementIdentifier(identifier))
            {
                if (!(overload->node->attributeFlags & ATTRIBUTE_DISCARDABLE) && !(identifier->flags & AST_DISCARD))
                {
                    PushErrHint errh(Hnt(Hnt0023));
                    Diagnostic  diag(identifier, Fmt(Err(Err0109), overload->node->token.ctext()));
                    Diagnostic  note(overload->node, Nte(Nte0033), DiagnosticLevel::NotePack);
                    return context->report(diag, &note);
                }
                else
                {
                    identifier->flags |= AST_DISCARD;
                }
            }
        }
        else if (returnType->isNative(NativeTypeKind::Void) && (identifier->flags & AST_DISCARD))
        {
            Diagnostic diag{identifier, Err(Err0094)};
            Diagnostic note{overload->node, Nte(Nte0033), DiagnosticLevel::NotePack};
            return context->report(Hint::isType(identifier->typeInfo), diag, &note);
        }

        if (overload->node->mustInline() && !(identifier->specFlags & AST_SPEC_IDENTIFIER_NO_INLINE))
        {
            // Mixin and macros must be inlined here, because no call is possible
            bool forceInline = false;
            if (overload->node->attributeFlags & (ATTRIBUTE_MIXIN | ATTRIBUTE_MACRO))
                forceInline = true;

            // Expand inline function. Do not expand an inline call inside a function marked as inline.
            // The expansion will be done at the lowest level possible
            if (!identifier->ownerFct || !identifier->ownerFct->mustInline() || forceInline)
            {
                // Need to wait for function full semantic resolve
                auto funcDecl = static_cast<AstFuncDecl*>(overload->node);
                context->job->waitFuncDeclFullResolve(funcDecl);
                if (context->result != ContextResult::Done)
                    return true;

                // First pass, we inline the function.
                // The identifier for the function call will be reresolved later when the content
                // of the inline os done.
                if (!(identifier->doneFlags & AST_DONE_INLINED))
                {
                    identifier->doneFlags |= AST_DONE_INLINED;

                    // In case of an inline call inside an inline function, the identifier kind has been changed to
                    // AstNodeKind::FuncCall in the original function. So we restore it to be a simple identifier.
                    identifier->kind = AstNodeKind::Identifier;

                    SWAG_CHECK(makeInline(context, funcDecl, identifier));
                }
                else
                {
                    SWAG_CHECK(setupIdentifierRef(context, identifier, returnType));
                }

                identifier->byteCodeFct = ByteCodeGenJob::emitPassThrough;
                return true;
            }
        }

        identifier->kind = AstNodeKind::FuncCall;

        // @print behaves like a normal function, so we want an emitCall in that case
        if (identifier->token.text[0] == '@' && identifier->token.id != TokenId::IntrinsicPrint)
        {
            dealWithIntrinsic(context, identifier);
            identifier->byteCodeFct = ByteCodeGenJob::emitIntrinsic;
        }
        else if (overload->node->attributeFlags & ATTRIBUTE_FOREIGN)
        {
            identifier->byteCodeFct = ByteCodeGenJob::emitForeignCall;
        }
        else
            identifier->byteCodeFct = ByteCodeGenJob::emitCall;

        // Try/Assume
        if (identifier->extension && identifier->extension->ownerTryCatchAssume && (identifier->typeInfo->flags & TYPEINFO_CAN_THROW))
        {
            identifier->allocateExtension();
            auto extension = identifier->extension;
            switch (extension->ownerTryCatchAssume->kind)
            {
            case AstNodeKind::Try:
                extension->byteCodeAfterFct = ByteCodeGenJob::emitTry;
                break;
            case AstNodeKind::Assume:
                extension->byteCodeAfterFct = ByteCodeGenJob::emitAssume;
                break;
            }
        }

        // Setup parent if necessary
        if (returnType->kind == TypeInfoKind::Struct)
        {
            identifier->semFlags |= AST_SEM_IS_CONST_ASSIGN_INHERIT;
            identifier->semFlags |= AST_SEM_IS_CONST_ASSIGN;
        }

        SWAG_CHECK(setupIdentifierRef(context, identifier, returnType));

        // For a return by copy, need to reserve room on the stack for the return result
        if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            identifier->flags |= AST_TRANSIENT;
            identifier->allocateComputedValue();
            identifier->computedValue->storageOffset = identifier->ownerScope->startStackSize;
            identifier->ownerScope->startStackSize += returnType->sizeOf;
            SemanticJob::setOwnerMaxStackSize(identifier, identifier->ownerScope->startStackSize);
        }

        break;
    }
    }

    return true;
}

void SemanticJob::setupContextualGenericTypeReplacement(SemanticContext* context, OneTryMatch& oneTryMatch, SymbolOverload* symOverload, uint32_t flags)
{
    auto node = context->node;

    // Fresh start on generic types
    oneTryMatch.symMatchContext.genericReplaceTypes.clear();
    oneTryMatch.symMatchContext.mapGenericTypesIndex.clear();

    VectorNative<AstNode*> toCheck;

    // If we are inside a struct, then we can inherit the generic concrete types of that struct
    if (node->ownerStructScope)
        toCheck.push_back(node->ownerStructScope->owner);

    // If function A in a struct calls function B in the same struct, then we can inherit the match types of function A
    // when instantiating function B
    if (node->ownerFct && node->ownerStructScope && node->ownerFct->ownerStructScope == symOverload->node->ownerStructScope)
        toCheck.push_back(node->ownerFct);

    // We do not want function to deduce their generic type from context, as the generic type can be deduced from the
    // parameters
    if (node->ownerFct && symOverload->typeInfo->kind != TypeInfoKind::FuncAttr)
        toCheck.push_back(node->ownerFct);

    // Except for a second try
    if (node->ownerFct && symOverload->typeInfo->kind == TypeInfoKind::FuncAttr && flags & MIP_SECOND_GENERIC_TRY)
        toCheck.push_back(node->ownerFct);

    // With A.B form, we try to get generic parameters from A if they exist
    if (node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
        if (identifier->identifierRef->startScope)
            toCheck.push_back(identifier->identifierRef->startScope->owner);
    }

    // Except that with using, B could be in fact in another struct than A.
    // In that case we have a dependentVar, so replace what needs to be replaced.
    // What a mess...
    if (oneTryMatch.dependentVarLeaf)
    {
        if (oneTryMatch.dependentVarLeaf->typeInfo && oneTryMatch.dependentVarLeaf->typeInfo->kind == TypeInfoKind::Struct)
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
            for (auto oneReplace : typeFunc->replaceTypes)
            {
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;
            }
        }

        if (one->kind == AstNodeKind::StructDecl)
        {
            auto nodeStruct = CastAst<AstStruct>(one, AstNodeKind::StructDecl);
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(nodeStruct->typeInfo, TypeInfoKind::Struct);
            for (auto oneReplace : typeStruct->replaceTypes)
            {
                oneTryMatch.symMatchContext.genericReplaceTypes[oneReplace.first] = oneReplace.second;
            }
        }
    }
}

bool SemanticJob::isFunctionButNotACall(SemanticContext* context, AstNode* node, SymbolName* symbol)
{
    if (node && node->kind == AstNodeKind::Identifier)
    {
        auto id = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (id != id->identifierRef->childs.back())
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
                    return true;
                if (grandParent->kind == AstNodeKind::MakePointerLambda && node == node->parent->childs.back())
                    return true;
            }

            if (grandParent->kind == AstNodeKind::Alias ||
                (grandParent->kind == AstNodeKind::CompilerSpecialFunction && grandParent->token.id == TokenId::CompilerLocation) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicStringOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicNameOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicRunes) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicTypeOf) ||
                (grandParent->kind == AstNodeKind::IntrinsicProp && grandParent->token.id == TokenId::IntrinsicKindOf))
            {
                return true;
            }
        }
    }

    return false;
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
                overload->typeInfo->kind != TypeInfoKind::Lambda)
            {
                auto match              = job->getOneMatch();
                match->symbolOverload   = overload;
                match->scope            = oneMatch->scope;
                match->solvedParameters = move(oneOverload.symMatchContext.solvedParameters);
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

        // If this is a type alias that already has a generic instance, accept to not have generic
        // parameters on the source symbol
        if (rawTypeInfo->kind == TypeInfoKind::Alias)
        {
            rawTypeInfo = TypeManager::concreteType(rawTypeInfo, CONCRETE_ALIAS);
            if (rawTypeInfo->kind == TypeInfoKind::Struct)
            {
                auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
                if (!(typeInfo->flags & TYPEINFO_GENERIC))
                    oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_ACCEPT_NO_GENERIC;
            }
        }

        // This way, a special cast can be done for the first parameter of a function
        if (oneOverload.ufcs)
            oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_UFCS;

        // When in a @typeof or @kindof, if we specify a type without generic parameters, then
        // we want to match the generic version, and not an instantiated one
        if (context->node->parent->parent->kind == AstNodeKind::IntrinsicProp && context->node->parent->parent->token.id == TokenId::IntrinsicTypeOf)
            oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC;
        if (context->node->parent->parent->kind == AstNodeKind::IntrinsicProp && context->node->parent->parent->token.id == TokenId::IntrinsicKindOf)
            oneOverload.symMatchContext.flags |= SymbolMatchContext::MATCH_DO_NOT_ACCEPT_NO_GENERIC;

        // We collect type replacements depending on where the identifier is
        setupContextualGenericTypeReplacement(context, oneOverload, overload, flags);

        oneOverload.symMatchContext.semContext = context;
        if (rawTypeInfo->kind == TypeInfoKind::Struct)
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Struct);
            typeInfo->match(oneOverload.symMatchContext);
            if (context->result == ContextResult::Pending)
                return true;
        }
        else if (rawTypeInfo->kind == TypeInfoKind::Interface)
        {
            forStruct     = true;
            auto typeInfo = CastTypeInfo<TypeInfoStruct>(rawTypeInfo, TypeInfoKind::Interface);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::FuncAttr);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::Lambda)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(rawTypeInfo, TypeInfoKind::Lambda);
            typeInfo->match(oneOverload.symMatchContext);
        }
        else if (rawTypeInfo->kind == TypeInfoKind::Generic)
        {
            oneOverload.symMatchContext.result = MatchResult::Ok;
        }
        else if (rawTypeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArr   = CastTypeInfo<TypeInfoArray>(rawTypeInfo, TypeInfoKind::Array);
            auto typeFinal = TypeManager::concreteReferenceType(typeArr->finalType);
            auto typeInfo  = CastTypeInfo<TypeInfoFuncAttr>(typeFinal, TypeInfoKind::Lambda);
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
                if (callParameters)
                    return context->report(callParameters, Err(Err0114));
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
        if (emptyParams && (oneOverload.symMatchContext.result == MatchResult::NotEnoughParameters || oneOverload.symMatchContext.result == MatchResult::MissingSomeParameters))
        {
            if (symbol->kind == SymbolKind::Variable)
                oneOverload.symMatchContext.result = MatchResult::Ok;
        }

        // In case of errors, remember the 'best' signature in order to generate the best possible
        // accurate error. We find the longest match (the one that failed on the right most parameter)
        if (oneOverload.symMatchContext.result != MatchResult::Ok)
        {
            if (job->bestSignatureInfos.badSignatureParameterIdx == -1 ||
                (oneOverload.symMatchContext.badSignatureInfos.badSignatureParameterIdx > job->bestSignatureInfos.badSignatureParameterIdx) ||
                (oneOverload.symMatchContext.result == MatchResult::BadGenericMatch && job->bestMatchResult == MatchResult::BadSignature))
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
                auto* match                        = job->getOneGenericMatch();
                match->flags                       = oneOverload.symMatchContext.flags;
                match->symbolName                  = symbol;
                match->symbolOverload              = overload;
                match->genericParametersCallTypes  = move(oneOverload.symMatchContext.genericParametersCallTypes);
                match->genericParametersGenTypes   = move(oneOverload.symMatchContext.genericParametersGenTypes);
                match->genericReplaceTypes         = move(oneOverload.symMatchContext.genericReplaceTypes);
                match->genericReplaceTypesFrom     = move(oneOverload.symMatchContext.genericReplaceTypesFrom);
                match->genericReplaceValues        = move(oneOverload.symMatchContext.genericReplaceValues);
                match->genericParameters           = genericParameters;
                match->parameters                  = move(oneOverload.symMatchContext.parameters);
                match->solvedParameters            = move(oneOverload.symMatchContext.solvedParameters);
                match->numOverloadsWhenChecked     = oneOverload.cptOverloads;
                match->numOverloadsInitWhenChecked = oneOverload.cptOverloadsInit;
                if (overload->node->flags & AST_HAS_SELECT_IF)
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
                    auto match              = job->getOneMatch();
                    match->symbolOverload   = overload;
                    match->solvedParameters = move(oneOverload.symMatchContext.solvedParameters);
                    match->dependentVar     = dependentVar;
                    match->ufcs             = oneOverload.ufcs;
                    match->oneOverload      = &oneOverload;

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

    // All choices were removed because of #selectif
    if (!genericMatches.size() && genericMatchesSI.size() && matches.empty() && prevMatchesCount)
    {
        if (justCheck)
            return false;
        return cannotMatchIdentifierError(context, overloads, node);
    }

    // Multi instantiation in case of #selectif
    if (genericMatchesSI.size() && matches.empty() && !prevMatchesCount)
    {
        if (justCheck)
            return true;

        set<SymbolName*> symbols;
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
                SWAG_CHECK(Generic::instantiateFunction(context, g->genericParameters, *g, true));
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

            auto       symbol = overloads[0]->overload->symbol;
            auto       match  = matches[0];
            Diagnostic diag{node, Fmt(Err(Err0886), symbol->name.c_str())};
            auto       note = new Diagnostic{match->symbolOverload->node, Nte(Nte0036), DiagnosticLevel::Note};
            return context->report(diag, note);
        }

        return true;
    }

    // Ambiguity with generics
    if (genericMatches.size() > 1)
    {
        if (justCheck)
            return false;

        auto                      symbol = overloads[0]->overload->symbol;
        Diagnostic                diag{node ? node : context->node, Fmt(Err(Err0115), SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())};
        vector<const Diagnostic*> notes;
        for (auto match : genericMatches)
        {
            auto overload = match->symbolOverload;
            auto couldBe  = Fmt("could be of type `%s`", overload->typeInfo->getDisplayNameC());

            VectorNative<TypeInfoParam*> params;
            for (auto og : match->genericReplaceTypes)
            {
                auto p        = g_TypeMgr->makeParam();
                p->namedParam = og.first.c_str();
                p->typeInfo   = og.second;
                params.push_back(p);
            }

            auto with = Ast::computeGenericParametersReplacement(params);
            if (!with.empty())
            {
                couldBe += " ... ";
                couldBe += with;
            }

            auto note                   = new Diagnostic{overload->node, couldBe, DiagnosticLevel::Note};
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
    for (int i = 0; i < overloads.size(); i++)
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
            Diagnostic  diag{node, Fmt(Err(Err0886), symbol->name.c_str())};
            Diagnostic* note = nullptr;
            for (auto match : matches)
            {
                if (match->symbolOverload->node != node && !match->symbolOverload->node->isParentOf(node))
                {
                    note = new Diagnostic{match->symbolOverload->node, Nte(Nte0036), DiagnosticLevel::Note};
                    break;
                }
            }

            SWAG_ASSERT(note);
            context->report(diag, note);
        }
        else
        {
            Diagnostic                diag{node, Fmt(Err(Err0116), symbol->name.c_str())};
            vector<const Diagnostic*> notes;
            for (auto match : matches)
            {
                auto        overload = match->symbolOverload;
                Diagnostic* note     = nullptr;

                if (overload->typeInfo->kind == TypeInfoKind::FuncAttr)
                {
                    if (overload->typeInfo->flags & TYPEINFO_FROM_GENERIC)
                    {
                        auto         typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                        AstFuncDecl* funcNode = CastAst<AstFuncDecl>(typeFunc->declNode, AstNodeKind::FuncDecl);
                        auto         orgNode  = funcNode->originalGeneric ? funcNode->originalGeneric : overload->typeInfo->declNode;
                        auto         couldBe  = Fmt(Nte(Nte0045), orgNode->typeInfo->getDisplayNameC());
                        couldBe += Ast::computeGenericParametersReplacement(typeFunc->genericParameters);
                        note = new Diagnostic{overload->node, couldBe, DiagnosticLevel::NotePack};
                        note->remarks.push_back(Fmt(Nte(Nte0047), overload->typeInfo->getDisplayNameC()));
                    }
                    else
                    {
                        auto couldBe = Fmt(Nte(Nte0048), overload->typeInfo->getDisplayNameC());
                        note         = new Diagnostic{overload->node, couldBe, DiagnosticLevel::NotePack};
                    }
                }
                else if (overload->typeInfo->kind == TypeInfoKind::Struct)
                {
                    auto couldBe    = Fmt(Nte(Nte0049), overload->typeInfo->getDisplayNameC());
                    note            = new Diagnostic{overload->node, couldBe, DiagnosticLevel::NotePack};
                    auto typeStruct = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
                    note->remarks.push_back(Ast::computeGenericParametersReplacement(typeStruct->genericParameters));
                }
                else
                {
                    auto couldBe = Fmt(Nte(Nte0050), SymTable::getArticleKindName(match->symbolOverload->symbol->kind), overload->typeInfo->getDisplayNameC());
                    note         = new Diagnostic{overload->node, couldBe, DiagnosticLevel::Note};
                }

                note->noteHeader = "could be";
                notes.push_back(note);
            }

            context->report(diag, notes);
        }

        return false;
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
            for (auto param : firstMatch.genericParametersCallTypes)
            {
                auto callParam      = Ast::newFuncCallParam(node->sourceFile, genericParameters);
                callParam->typeInfo = param;
            }
        }

        if (!(node->flags & AST_IS_GENERIC) && genericParameters)
        {
            SWAG_CHECK(Generic::instantiateStruct(context, genericParameters, firstMatch));
        }
        else
        {
            auto oneMatch            = job->getOneMatch();
            oneMatch->symbolOverload = firstMatch.symbolOverload;
            matches.push_back(oneMatch);
            node->flags |= AST_IS_GENERIC;
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
    for (auto c : node->callParameters->childs)
        c->childParentIdx++;
    node->callParameters->childs.push_front(fctCallParam);

    fctCallParam->parent   = node->callParameters;
    fctCallParam->typeInfo = identifierRef->previousResolvedNode->typeInfo;
    fctCallParam->token    = identifierRef->previousResolvedNode->token;
    fctCallParam->inheritTokenLocation(node);
    fctCallParam->byteCodeFct = ByteCodeGenJob::emitFuncCallParam;
    fctCallParam->inheritOwners(node->callParameters);
    fctCallParam->flags |= AST_TO_UFCS | AST_GENERATED;
    fctCallParam->inheritOrFlag(node, AST_IN_MIXIN);

    SWAG_ASSERT(match.solvedParameters.size() > 0);
    SWAG_ASSERT(match.solvedParameters[0]->index == 0);
    fctCallParam->resolvedParameter = match.solvedParameters[0];

    auto idRef = Ast::newIdentifierRef(node->sourceFile, fctCallParam);
    if (symbol->kind == SymbolKind::Variable)
    {
        // Call from a lambda, on a variable : we need to keep the original variable, and put the UFCS one in its own identifierref
        // Copy all previous references to the one we want to pass as parameter
        // X.Y.call(...) => X.Y.call(X.Y, ...)
        for (auto child : identifierRef->childs)
        {
            auto copyChild = Ast::cloneRaw(child, idRef);

            // We want to generate bytecode for the expression on the left only if the lambda is dereferenced from a struct/itf
            // Otherwise the left expression is only used for scoping
            if (!identifierRef->startScope || identifierRef->startScope != match.symbolOverload->node->ownerStructScope)
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
    typeInfo = TypeManager::concreteReferenceType(typeInfo, CONCRETE_FUNC);
    if (!typeInfo || typeInfo->kind != TypeInfoKind::Enum)
        return nullptr;
    return CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
}

bool SemanticJob::findEnumTypeInContext(SemanticContext* context, AstNode* node, TypeInfoEnum** res, bool genError)
{
    bool done   = false;
    auto parent = node->parent;

    // If this is a parameter of a function call, we will try to deduce the type with a function signature
    auto fctCallParam = node->findParent(AstNodeKind::FuncCallParam);
    if (fctCallParam &&
        fctCallParam->parent->kind == AstNodeKind::FuncCallParams &&
        fctCallParam->parent->parent->kind == AstNodeKind::Identifier)
    {
        AstIdentifierRef*            idref = CastAst<AstIdentifierRef>(fctCallParam->parent->parent->parent, AstNodeKind::IdentifierRef);
        AstIdentifier*               id    = CastAst<AstIdentifier>(fctCallParam->parent->parent, AstNodeKind::Identifier);
        VectorNative<OneSymbolMatch> symbolMatch;
        context->silentError++;

        auto found = findIdentifierInScopes(context, symbolMatch, idref, id);
        context->silentError--;
        if (found && symbolMatch.size() == 1)
        {
            auto symbol = symbolMatch.front().symbol;
            if (symbol->kind != SymbolKind::Function && symbol->kind != SymbolKind::Variable)
                return true;

            {
                ScopedLock ls(symbol->mutex);
                if (symbol->cptOverloads)
                {
                    context->job->waitSymbolNoLock(symbol);
                    return true;
                }
            }

            VectorNative<TypeInfoEnum*> result;
            for (auto& overload : symbol->overloads)
            {
                if (overload->typeInfo->kind != TypeInfoKind::FuncAttr && overload->typeInfo->kind != TypeInfoKind::Lambda)
                    continue;

                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);

                // If there's only one corresponding type in the function, then take it
                // If it's not the correct parameter, the match will not be done, so we do not really care here
                VectorNative<TypeInfoEnum*> subResult;
                for (auto param : typeFunc->parameters)
                {
                    auto typeEnum = findEnumTypeInContext(context, param->typeInfo);
                    if (typeEnum && typeEnum->contains(node->token.text))
                        subResult.push_back_once(typeEnum);
                }

                if (subResult.size() == 1)
                {
                    result.push_back_once(subResult.front());
                }

                // More that one possible type (at least two different enums with the same identical requested name in the function signature)
                // We are not lucky...
                else
                {
                    int enumIdx = 0;
                    for (int i = 0; i < fctCallParam->parent->childs.size(); i++)
                    {
                        auto child = fctCallParam->parent->childs[i];
                        if (child == fctCallParam)
                            break;
                        if (child->typeInfo && child->typeInfo->kind == TypeInfoKind::Enum)
                            enumIdx++;
                        else if (child->kind == AstNodeKind::IdentifierRef && child->specFlags & AST_SPEC_IDENTIFIERREF_AUTO_SCOPE)
                            enumIdx++;
                    }

                    for (auto p : typeFunc->parameters)
                    {
                        if (p->typeInfo->kind == TypeInfoKind::Enum)
                        {
                            if (!enumIdx)
                            {
                                auto typeEnum = findEnumTypeInContext(context, p->typeInfo);
                                if (typeEnum && typeEnum->contains(node->token.text))
                                    result.push_back_once(typeEnum);
                                break;
                            }

                            enumIdx--;
                        }
                    }
                }
            }

            if (result.size() == 1)
            {
                *res = result.front();
                return true;
            }
        }
        else
        {
            if (genError)
            {
                unknownIdentifier(context, idref, id);
                return false;
            }

            *res = nullptr;
            return true;
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
                auto typeInfo = TypeManager::concreteReferenceType(funcNode->returnType->typeInfo, CONCRETE_FUNC);
                if (typeInfo->kind == TypeInfoKind::Enum)
                {
                    *res = CastTypeInfo<TypeInfoEnum>(typeInfo, TypeInfoKind::Enum);
                    return true;
                }
            }
        }
    }

    // We go up in the hierarchy until we find a statement, or a contextual type to return
    while (parent && !done)
    {
        if (parent->kind == AstNodeKind::Statement)
            break;
        if (parent->kind == AstNodeKind::SwitchCaseBlock)
            break;

        for (auto c : parent->childs)
        {
            auto cType = findEnumTypeInContext(context, c->typeInfo);
            if (cType)
            {
                *res = cType;
                return true;
            }
        }

        parent = parent->parent;
    }

    *res = nullptr;
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
        SWAG_ASSERT(typeFct->returnType->kind == TypeInfoKind::Struct);
        addDependentSymbol(dependentSymbols, typeFct->returnType->declNode->resolvedSymbolName, nullptr, 0);
        return true;
    }

    // #self
    if (node->token.text == g_LangSpec->name_sharpself)
    {
        SWAG_VERIFY(node->ownerFct, context->report(node, Err(Err0348)));
        AstNode* parent = node;
        while ((parent->ownerFct->attributeFlags & ATTRIBUTE_SHARP_FUNC) && parent->ownerFct->parent->ownerFct)
            parent = parent->ownerFct->parent;
        SWAG_VERIFY(parent, context->report(parent, Err(Err0348)));

        if (parent->ownerScope->kind == ScopeKind::Struct || parent->ownerScope->kind == ScopeKind::Enum)
        {
            parent = parent->ownerScope->owner;
            node->flags |= AST_CAN_MATCH_INCOMPLETE;
        }
        else
        {
            SWAG_VERIFY(parent->ownerFct, context->report(parent, Err(Err0348)));
            parent = parent->ownerFct;

            // Force scope
            if (!node->callParameters && node != identifierRef->childs.back())
            {
                node->semFlags |= AST_SEM_FORCE_SCOPE;
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

            // Pass through the first inline if there's a back tick before the name
            if (node->specFlags & AST_SPEC_IDENTIFIER_BACKTICK)
                collectFlags = COLLECT_BACKTICK;

            startScope = node->ownerScope;

            // :AutoScope
            // Auto scoping depending on the context
            // We scan the parent hierarchy for an already defined type that can be used for scoping
            if (identifierRef->specFlags & AST_SPEC_IDENTIFIERREF_AUTO_SCOPE && !(identifierRef->doneFlags & AST_DONE_SPEC_SCOPE))
            {
                TypeInfoEnum* typeEnum = nullptr;
                SWAG_CHECK(findEnumTypeInContext(context, identifierRef, &typeEnum, true));
                if (context->result == ContextResult::Pending)
                    return true;
                if (typeEnum)
                {
                    identifierRef->startScope = typeEnum->scope;
                    scopeHierarchy.clear();
                    addAlternativeScopeOnce(scopeHierarchy, typeEnum->scope);
                }
                else
                {
                    auto withNodeP = node->findParent(AstNodeKind::With);
                    SWAG_VERIFY(withNodeP, context->report(identifierRef, Fmt(Err(Err0881), node->token.text.c_str())));
                    auto withNode = CastAst<AstWith>(withNodeP, AstNodeKind::With);

                    // Prepend the 'with' identifier, and reevaluate
                    for (int wi = (int) withNode->id.size() - 1; wi >= 0; wi--)
                    {
                        auto id = Ast::newIdentifier(context->sourceFile, withNode->id[wi], identifierRef, identifierRef);
                        id->flags |= AST_GENERATED;
                        id->specFlags |= AST_SPEC_IDENTIFIER_FROM_WITH;
                        id->inheritTokenLocation(identifierRef);
                        identifierRef->childs.pop_back();
                        Ast::addChildFront(identifierRef, id);
                        identifierRef->doneFlags |= AST_DONE_SPEC_SCOPE;
                        context->job->nodes.push_back(id);
                    }

                    context->result = ContextResult::NewChilds;
                    return true;
                }
            }
            else
            {
                SWAG_CHECK(collectScopeHierarchy(context, scopeHierarchy, scopeHierarchyVars, node, collectFlags));
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
                if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->semFlags & AST_SEM_FORCE_SCOPE)
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
            for (int i = 0; i < scopeHierarchy.size(); i++)
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

        node->semFlags |= AST_SEM_FORCE_UFCS;
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
    }

    auto                        symbol   = overload->symbol;
    auto                        symScope = symbol->ownerTable->scope;
    vector<AlternativeScopeVar> toCheck;

    // Collect all matches
    bool hasWith = false;
    for (auto& dep : scopeHierarchyVars)
    {
        bool getIt = false;

        // Exact same scope
        if (dep.scope == symScope || dep.scope->getFullName() == symScope->getFullName())
            getIt = true;

        // The symbol scope is an 'impl' inside a struct (impl for)
        else if (symScope->kind == ScopeKind::Impl && symScope->parentScope == dep.scope)
            getIt = true;

        // For mtd sub functions and ufcs
        else if (symbol->kind == SymbolKind::Function)
        {
            auto typeInfo = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
            if (typeInfo->parameters.size())
            {
                auto firstParam = typeInfo->parameters.front()->typeInfo;
                if (firstParam->isSame(dep.node->typeInfo, ISSAME_EXACT))
                    getIt = true;
            }
        }

        if (getIt)
        {
            if (dep.node->parent->kind == AstNodeKind::With)
                hasWith = true;
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
            if (dep.node->specFlags & AST_SPEC_DECLPARAM_GENERATED_SELF)
            {
                Diagnostic diag{dependentVar, Fmt(Err(Err0117), dependentVar->typeInfo->getDisplayNameC())};
                Diagnostic note{dep.node, Nte(Nte0056), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            else
            {
                Diagnostic diag{dep.node, Fmt(Err(Err0117), dependentVar->typeInfo->getDisplayNameC())};
                Diagnostic note{dependentVar, Nte(Nte0036), DiagnosticLevel::Note};
                return context->report(diag, &note);
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
    if (!ufcsNode || typeFunc->parameters.size() == 0)
        return false;

    // Be sure the identifier we want to use in ufcs emits code, otherwise we cannot use it.
    // This can happen if we have already changed the ast, but the nodes are reavaluated later
    // (because of an inline for example)
    if (ufcsNode->flags & AST_NO_BYTECODE)
        return false;

    // Compare first function parameter with ufcsNode type.
    bool cmpFirstParam = TypeManager::makeCompatibles(context, typeFunc->parameters[0]->typeInfo, ufcsNode->typeInfo, nullptr, ufcsNode, CASTFLAG_JUST_CHECK | CASTFLAG_NO_ERROR | CASTFLAG_UFCS);
    if (context->result == ContextResult::Pending)
        return false;

    // In case ufcsNode is not explicit (using var), then be sure that first parameter type matches.
    if (!nodeIsExplicit && !cmpFirstParam)
        return false;

    auto numParams = parameters ? parameters->childs.size() : 0;

    // Take care of closure first dummy parameter (do not count it)
    if (typeFunc->isClosure())
        numParams++;

    if (numParams < typeFunc->parameters.size())
        return true;

    // In case of variadic functions, make ufcs if the first parameter is correct
    if (typeFunc->flags & TYPEINFO_VARIADIC)
    {
        if (cmpFirstParam)
            return true;
    }

    // As we have a variable on the left (or equivalent), force it, except when calling a lambda with the
    // right number of arguments (not sure all of thoses tests are bullet proof)
    if (typeFunc->kind == TypeInfoKind::Lambda)
        return false;

    return nodeIsExplicit;
}

bool SemanticJob::getUfcs(SemanticContext* context, AstIdentifierRef* identifierRef, AstIdentifier* node, SymbolOverload* overload, AstNode** ufcsFirstParam)
{
    auto symbol = overload->symbol;

    bool canDoUfcs = false;
    if (symbol->kind == SymbolKind::Function)
        canDoUfcs = true;
    if (symbol->kind == SymbolKind::Variable && overload->typeInfo->kind == TypeInfoKind::Lambda)
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
                     identifierRef->previousResolvedNode->kind == AstNodeKind::FuncCall)
                canTry = true;
            // Before was an inlined function call
            else if (identifierRef->resolvedSymbolName->kind == SymbolKind::Function &&
                     identifierRef->previousResolvedNode->kind == AstNodeKind::Identifier &&
                     identifierRef->previousResolvedNode->childs.size() &&
                     identifierRef->previousResolvedNode->childs.front()->kind == AstNodeKind::FuncCallParams &&
                     identifierRef->previousResolvedNode->childs.back()->kind == AstNodeKind::Inline)
                canTry = true;

            if (canTry)
            {
                SWAG_ASSERT(identifierRef->previousResolvedNode);
                auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                canTry        = canTryUfcs(context, typeFunc, node->callParameters, identifierRef->previousResolvedNode, true);
                if (context->result == ContextResult::Pending)
                    return true;
                if (canTry)
                    *ufcsFirstParam = identifierRef->previousResolvedNode;
                SWAG_VERIFY(node->callParameters, context->report(Hnt(Hnt0044), {node, Fmt(Err(Err0189), typeFunc->getDisplayNameC())}));
            }
        }
    }

    if (canDoUfcs && (symbol->kind == SymbolKind::Variable))
    {
        if (identifierRef->resolvedSymbolName && identifierRef->resolvedSymbolName->kind != SymbolKind::Variable)
        {
            auto subNode = identifierRef->previousResolvedNode ? identifierRef->previousResolvedNode : node;
            auto msg     = Fmt(Err(Err0124), identifierRef->resolvedSymbolName->name.c_str(), SymTable::getArticleKindName(identifierRef->resolvedSymbolName->kind));
            return context->report(subNode, msg);
        }
    }

    return true;
}

bool SemanticJob::appendLastCodeStatement(SemanticContext* context, AstIdentifier* node, SymbolOverload* overload)
{
    if (!(node->doneFlags & AST_DONE_LAST_PARAM_CODE) && (overload->symbol->kind == SymbolKind::Function))
    {
        node->doneFlags |= AST_DONE_LAST_PARAM_CODE;

        // If last parameter is of type code, and the call last parameter is not, then take the next statement
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(overload->typeInfo, TypeInfoKind::FuncAttr);
        if (!typeFunc->parameters.empty() && typeFunc->parameters.back()->typeInfo->kind == TypeInfoKind::Code)
        {
            if (node->callParameters && node->callParameters->childs.size() < typeFunc->parameters.size())
            {
                if (node->parent->childParentIdx != node->parent->parent->childs.size() - 1)
                {
                    auto brother = node->parent->parent->childs[node->parent->childParentIdx + 1];
                    if (brother->kind == AstNodeKind::Statement)
                    {
                        auto fctCallParam = Ast::newFuncCallParam(context->sourceFile, node->callParameters);
                        auto codeNode     = Ast::newNode<AstNode>(nullptr, AstNodeKind::CompilerCode, node->sourceFile, fctCallParam);
                        codeNode->flags |= AST_NO_BYTECODE;
                        Ast::removeFromParent(brother);
                        Ast::addChildBack(codeNode, brother);
                        auto typeCode     = allocType<TypeInfoCode>();
                        typeCode->content = brother;
                        brother->flags |= AST_NO_SEMANTIC;
                        fctCallParam->semFlags |= AST_SEM_AUTO_CODE_PARAM;
                        fctCallParam->typeInfo = typeCode;
                        codeNode->typeInfo     = typeCode;
                    }
                }
            }
        }
    }

    return true;
}

bool SemanticJob::fillMatchContextCallParameters(SemanticContext* context, SymbolMatchContext& symMatchContext, AstIdentifier* node, SymbolOverload* overload, AstNode* ufcsFirstParam)
{
    auto symbol         = overload->symbol;
    auto symbolKind     = symbol->kind;
    auto callParameters = node->callParameters;

    // :ClosureForceFirstParam
    // A closure has always a first parameter of type *void
    auto typeRef = TypeManager::concreteType(overload->typeInfo, CONCRETE_ALIAS);
    if (typeRef->isClosure() && node->callParameters)
    {
        if (!(node->doneFlags & AST_DONE_CLOSURE_FIRST_PARAM))
        {
            SWAG_VERIFY(!ufcsFirstParam, context->report(ufcsFirstParam, Err(Err0873)));
            context->job->closureFirstParam.kind     = AstNodeKind::FuncCallParam;
            context->job->closureFirstParam.typeInfo = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
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
            !node->token.text.empty() && // :SilentCall
            symbol->overloads[0]->typeInfo->kind != TypeInfoKind::Generic &&
            TypeManager::concreteType(symbol->overloads[0]->typeInfo, CONCRETE_ALIAS)->kind != TypeInfoKind::Lambda)
        {
            auto firstNode = symbol->nodes.front();
            if (symbolKind == SymbolKind::Variable)
            {
                Diagnostic diag{node, Fmt(Err(Err0125), node->token.ctext(), symbol->overloads[0]->typeInfo->getDisplayNameC())};
                Diagnostic note{firstNode->sourceFile, firstNode->token.startLocation, firstNode->token.endLocation, Fmt(Nte(Nte0040), node->token.ctext()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
            else
            {
                Diagnostic diag{node, Fmt(Err(Err0127), node->token.ctext(), SymTable::getArticleKindName(symbol->kind))};
                Diagnostic note{firstNode->sourceFile, firstNode->token.startLocation, firstNode->token.endLocation, Fmt(Nte(Nte0040), node->token.ctext()), DiagnosticLevel::Note};
                return context->report(diag, &note);
            }
        }
    }

    if (callParameters)
    {
        auto childCount = callParameters->childs.size();
        for (int i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(callParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.parameters.push_back(oneParam);

            // Be sure all interfaces of the structure have been solved, in case a cast to an interface is necessary to match
            // a function
            // :WaitInterfaceReg
            TypeInfoStruct* typeStruct = nullptr;
            if (oneParam->typeInfo->kind == TypeInfoKind::Struct)
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
                if (oneParam->typeInfo->kind == TypeInfoKind::Variadic ||
                    oneParam->typeInfo->kind == TypeInfoKind::TypedVariadic ||
                    oneParam->typeInfo->kind == TypeInfoKind::CVariadic)
                {
                    return context->report(oneParam, Err(Err0734));
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
            Diagnostic diag{genericParameters, Fmt(Err(Err0130), node->token.ctext(), SymTable::getArticleKindName(symbol->kind))};
            Diagnostic note{firstNode->sourceFile, firstNode->token.startLocation, firstNode->token.endLocation, Fmt(Nte(Nte0040), node->token.ctext()), DiagnosticLevel::Note};
            return context->report(diag, &note);
        }

        auto childCount = genericParameters->childs.size();
        for (int i = 0; i < childCount; i++)
        {
            auto oneParam = CastAst<AstFuncCallParam>(genericParameters->childs[i], AstNodeKind::FuncCallParam);
            symMatchContext.genericParameters.push_back(oneParam);
            symMatchContext.genericParametersCallTypes.push_back(oneParam->typeInfo);
        }
    }

    return true;
}

bool SemanticJob::filterMatches(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    auto node         = context->node;
    auto countMatches = matches.size();
    for (int i = 0; i < countMatches; i++)
    {
        auto curMatch = matches[i];
        auto over     = curMatch->symbolOverload;
        auto overSym  = over->symbol;

        // Take care of #selectif/#checkif
        if (overSym->kind == SymbolKind::Function &&
            !(context->node->flags & AST_IN_SELECTIF) &&
            !(context->node->attributeFlags & ATTRIBUTE_SELECTIF_OFF))
        {
            auto funcDecl = CastAst<AstFuncDecl>(over->node, AstNodeKind::FuncDecl);
            if (funcDecl->selectIf)
            {
                SWAG_CHECK(solveSelectIf(context, curMatch, funcDecl));
                if (context->result != ContextResult::Done)
                    return true;
                if (curMatch->remove)
                    continue;
            }
        }

        if (countMatches == 1)
            return true;

        // In case of an alias, we take the first one, which should be the 'closest' one.
        // Not sure this is true, perhaps one day will have to change the way we find it.
        if (overSym->name[0] == '@' && strstr(overSym->name.c_str(), g_LangSpec->name_atalias) == overSym->name.c_str())
        {
            for (int j = 0; j < countMatches; j++)
            {
                if (i != j)
                    matches[j]->remove = true;
            }

            break;
        }

        // Priority to a non empty function
        if (over->node->flags & AST_EMPTY_FCT)
        {
            for (int j = 0; j < countMatches; j++)
            {
                if (!(matches[j]->symbolOverload->node->flags & AST_EMPTY_FCT) &&
                    matches[j]->symbolOverload->symbol == curMatch->symbolOverload->symbol)
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to a local var/parameter versus a function
        if (over->typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            for (int j = 0; j < countMatches; j++)
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
            for (int j = 0; j < countMatches; j++)
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
        if (lastOverloadType && lastOverloadType->flags & TYPEINFO_GENERIC)
        {
            for (int j = 0; j < countMatches; j++)
            {
                auto newOverloadType = matches[j]->symbolOverload->symbol->ownerTable->scope->owner->typeInfo;
                if (newOverloadType && !(newOverloadType->flags & TYPEINFO_GENERIC))
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
            for (int j = 0; j < countMatches; j++)
            {
                if (!(matches[j]->symbolOverload->flags & OVERLOAD_IMPL_IN_STRUCT))
                {
                    // If interface name is alone, then we take in priority the interface definition, not the impl block
                    if (node->childParentIdx == 0)
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
            for (int j = 0; j < countMatches; j++)
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
            for (int j = 0; j < countMatches; j++)
            {
                if (node->isSameStackFrame(matches[j]->symbolOverload))
                {
                    curMatch->remove = true;
                    break;
                }
            }
        }

        // Priority to lambda call in a parameter over a function outside the actual function
        if (over->typeInfo->kind == TypeInfoKind::Lambda)
        {
            auto callParams = over->node->findParent(AstNodeKind::FuncCallParams);
            if (callParams)
            {
                for (int j = 0; j < countMatches; j++)
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
                for (int j = 0; j < countMatches; j++)
                {
                    if (node->ownerInline->scope->isParentOf(matches[j]->symbolOverload->node->ownerScope))
                    {
                        curMatch->remove = true;
                        break;
                    }
                }
            }
        }

        // Priority to not a namespace (??)
        if (curMatch->symbolOverload->symbol->kind == SymbolKind::Namespace)
        {
            for (int j = 0; j < countMatches; j++)
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
            for (int j = 0; j < countMatches; j++)
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
        if (!curMatch->ufcs && over->typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeFunc0 = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (!typeFunc0->parameters.empty() && typeFunc0->parameters[0]->typeInfo->flags & TYPEINFO_SELF)
            {
                for (int j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->kind == TypeInfoKind::FuncAttr)
                    {
                        auto typeFunc1 = CastTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (typeFunc1->parameters.empty() || !(typeFunc1->parameters[0]->typeInfo->flags & TYPEINFO_SELF))
                        {
                            curMatch->remove = true;
                            break;
                        }
                    }
                }
            }
        }

        // If we did match with ufcs, then priority to a match that starts with 'self'
        if (curMatch->ufcs && over->typeInfo->kind == TypeInfoKind::FuncAttr)
        {
            auto typeFunc0 = CastTypeInfo<TypeInfoFuncAttr>(over->typeInfo, TypeInfoKind::FuncAttr);
            if (typeFunc0->parameters.empty() || !(typeFunc0->parameters[0]->typeInfo->flags & TYPEINFO_SELF))
            {
                for (int j = 0; j < countMatches; j++)
                {
                    if (matches[j]->symbolOverload->typeInfo->kind == TypeInfoKind::FuncAttr)
                    {
                        auto typeFunc1 = CastTypeInfo<TypeInfoFuncAttr>(matches[j]->symbolOverload->typeInfo, TypeInfoKind::FuncAttr);
                        if (!typeFunc1->parameters.empty() && (typeFunc1->parameters[0]->typeInfo->flags & TYPEINFO_SELF))
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
    for (int i = 0; i < matches.size(); i++)
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
    if (genMatches.size() <= 1)
        return true;

    // We have a match and more than one generic match
    // We need to be sure than instantiated another generic match will not be better than keeping
    // the already instantiated one
    if (genMatches.size() > 1 && matches.size() == 1)
    {
        auto idCost       = scopeCost(context->node->ownerScope, matches[0]->symbolOverload->node->ownerScope);
        auto bestIsIdCost = true;
        int  bestGenId    = 0;

        for (int i = 0; i < genMatches.size(); i++)
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
            return true;
        }
    }

    // Take the most "specialized" one, i.e. the one with the more 'genericReplaceTypes'
    int bestS = -1;
    for (int i = 0; i < genMatches.size(); i++)
    {
        bestS = max(bestS, (int) genMatches[i]->genericReplaceTypes.size());
    }

    for (int i = 0; i < genMatches.size(); i++)
    {
        if (genMatches[i]->genericReplaceTypes.size() < bestS)
        {
            genMatches[i] = genMatches.back();
            genMatches.pop_back();
            i--;
        }
    }

    return true;
}

bool SemanticJob::filterMatchesInContext(SemanticContext* context, VectorNative<OneMatch*>& matches)
{
    if (matches.size() <= 1)
        return true;

    for (int i = 0; i < matches.size(); i++)
    {
        auto          oneMatch = matches[i];
        auto          over     = oneMatch->symbolOverload;
        TypeInfoEnum* typeEnum = nullptr;
        SWAG_CHECK(findEnumTypeInContext(context, over->node, &typeEnum, false));
        if (context->result != ContextResult::Done)
            return true;

        if (typeEnum && typeEnum->scope == oneMatch->scope)
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

bool SemanticJob::solveSelectIf(SemanticContext* context, OneMatch* oneMatch, AstFuncDecl* funcDecl)
{
    ScopedLock lk0(funcDecl->funcMutex);
    ScopedLock lk1(funcDecl->mutex);

    // Be sure block has been solved
    if (!(funcDecl->funcFlags & FUNC_FLAG_PARTIAL_RESOLVE))
    {
        funcDecl->dependentJobs.add(context->job);
        context->job->setPending(funcDecl->resolvedSymbolName, JobWaitKind::SemPartialResolve, funcDecl, nullptr);
        return true;
    }

    // Execute #selectif/#checkif block
    auto expr = funcDecl->selectIf->childs.back();

    // #checkif is evaluated for each call, so we remove the AST_VALUE_COMPUTED computed flag.
    // #selectif is evaluated once, so keep it.
    if (funcDecl->selectIf->kind == AstNodeKind::CompilerCheckIf)
        expr->flags &= ~AST_VALUE_COMPUTED;

    if (!(expr->flags & AST_VALUE_COMPUTED))
    {
        auto node                   = context->node;
        context->selectIfParameters = oneMatch->oneOverload->callParameters;

        JobContext::ErrorContextType type;
        if (funcDecl->selectIf->kind == AstNodeKind::CompilerCheckIf)
            type = JobContext::ErrorContextType::CheckIf;
        else
            type = JobContext::ErrorContextType::SelectIf;

        PushErrContext ec(context, node, type);
        auto           result       = executeCompilerNode(context, expr, false);
        context->selectIfParameters = nullptr;
        if (!result)
            return false;
        if (context->result != ContextResult::Done)
            return true;
    }

    // Result
    if (!expr->computedValue->reg.b)
    {
        oneMatch->remove                              = true;
        oneMatch->oneOverload->symMatchContext.result = MatchResult::SelectIfFailed;
        return true;
    }

    if (funcDecl->content && funcDecl->content->flags & AST_NO_SEMANTIC)
    {
        funcDecl->content->flags &= ~AST_NO_SEMANTIC;

        // Need to restart semantic on instantiated function and on its content,
        // because the #selectif has passed
        // It's safe to create a job with the content as it has been fully evaluated.
        // It's NOT safe for the function itself as the job that deals with it can be
        // still running
        auto job          = g_Allocator.alloc<SemanticJob>();
        job->sourceFile   = context->sourceFile;
        job->module       = context->sourceFile->module;
        job->dependentJob = context->job->dependentJob;
        job->nodes.push_back(funcDecl->content);

        // To avoid a race condition with the job that is currently dealing with the funcDecl,
        // we will reevaluate it with a semanticAfterFct trick
        funcDecl->content->allocateExtension();
        SWAG_ASSERT(!funcDecl->content->extension->semanticAfterFct || funcDecl->content->extension->semanticAfterFct == SemanticJob::resolveFuncDeclAfterSI);
        funcDecl->content->extension->semanticAfterFct = SemanticJob::resolveFuncDeclAfterSI;

        g_ThreadMgr.addJob(job);
    }

    return true;
}

bool SemanticJob::filterSymbols(SemanticContext* context, AstIdentifier* node)
{
    auto  job              = context->job;
    auto  identifierRef    = node->identifierRef;
    auto& dependentSymbols = job->cacheDependentSymbols;

    if (dependentSymbols.size() == 1)
        return true;

    for (auto& p : dependentSymbols)
    {
        auto oneSymbol = p.symbol;
        if (p.remove)
            continue;

        // A variable inside a scopefile has priority
        if (p.asFlags & ALTSCOPE_SCOPEFILE)
        {
            for (auto& p1 : dependentSymbols)
            {
                if (!(p1.asFlags & ALTSCOPE_SCOPEFILE))
                    p1.remove = true;
            }
        }

        // A variable which is name as a function for example...
        if (!node->callParameters &&
            oneSymbol->kind == SymbolKind::Function &&
            !isFunctionButNotACall(context, node, oneSymbol))
        {
            p.remove = true;
            continue;
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
    for (int i = 0; i < dependentSymbols.size(); i++)
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

bool SemanticJob::needToWaitForSymbol(SemanticContext* context, AstIdentifier* node, SymbolName* symbol, bool& needToWait)
{
    if (!symbol->cptOverloads && !(symbol->flags & SYMBOL_ATTRIBUTE_GEN))
        return false;

    // This is enough to resolve, as we just need parameters, and that case means that some functions
    // do not know their return type yet (short lambdas)
    if (symbol->kind == SymbolKind::Function && symbol->overloads.size() == symbol->cptOverloadsInit)
        return false;

    needToWait = true;
    if (symbol->kind == SymbolKind::Struct || symbol->kind == SymbolKind::Interface)
    {
        bool canIncomplete = false;

        // If a structure is referencing itself, we will match the incomplete symbol for now
        // We can also do an incomplete match with the identifier of an Impl block
        if ((node->flags & AST_STRUCT_MEMBER) || (node->flags & AST_CAN_MATCH_INCOMPLETE))
            canIncomplete = true;

        // If identifier is in a pointer type expression, can incomplete resolve
        if (node->parent->parent && node->parent->parent->kind == AstNodeKind::TypeExpression)
        {
            auto typeExprNode = CastAst<AstTypeExpression>(node->parent->parent, AstNodeKind::TypeExpression);
            if (typeExprNode->ptrCount)
                canIncomplete = true;
        }

        if (canIncomplete)
        {
            if (symbol->overloads.size() == 1 && (symbol->overloads[0]->flags & OVERLOAD_INCOMPLETE))
            {
                if (!node->callParameters && !node->genericParameters)
                {
                    needToWait                   = false;
                    node->resolvedSymbolName     = symbol;
                    node->resolvedSymbolOverload = symbol->overloads[0];
                    node->typeInfo               = node->resolvedSymbolOverload->typeInfo;

                    // If this is a generic type, and it's from an instante, we must wait, because we will
                    // have to instantiate that symbol too
                    if (node->ownerStructScope && (node->ownerStructScope->owner->flags & AST_FROM_GENERIC) && (node->typeInfo->flags & TYPEINFO_GENERIC))
                        needToWait = true;
                    if (node->ownerFct && (node->ownerFct->flags & AST_FROM_GENERIC) && (node->typeInfo->flags & TYPEINFO_GENERIC))
                        needToWait = true;
                }
            }
        }
    }

    return true;
}

bool SemanticJob::resolveIdentifier(SemanticContext* context, AstIdentifier* node, uint32_t riFlags)
{
    auto  job                = context->job;
    auto& scopeHierarchy     = job->cacheScopeHierarchy;
    auto& scopeHierarchyVars = job->cacheScopeHierarchyVars;
    auto& dependentSymbols   = job->cacheDependentSymbols;
    auto  identifierRef      = node->identifierRef;

    node->byteCodeFct = ByteCodeGenJob::emitIdentifier;

    // Current file scope
    if (context->sourceFile && context->sourceFile->scopeFile && node->token.text == context->sourceFile->scopeFile->name)
    {
        SWAG_VERIFY(node == identifierRef->childs.front(), context->report(node, Err(Err0132)));
        identifierRef->startScope = context->sourceFile->scopeFile;
        return true;
    }

    // If previous identifier was generic, then stop evaluation
    if (identifierRef->previousResolvedNode && identifierRef->previousResolvedNode->typeInfo->kind == TypeInfoKind::Generic)
    {
        // Just take the generic type for now
        node->typeInfo          = g_TypeMgr->typeInfoUndefined;
        identifierRef->typeInfo = identifierRef->previousResolvedNode->typeInfo;
        return true;
    }

    // Already solved
    if ((node->flags & AST_FROM_GENERIC) && node->typeInfo && !node->typeInfo->isNative(NativeTypeKind::Undefined))
    {
        if (node->resolvedSymbolOverload)
        {
            OneMatch oneMatch;
            oneMatch.symbolOverload = node->resolvedSymbolOverload;
            oneMatch.scope          = node->resolvedSymbolOverload->node->ownerScope;
            SWAG_CHECK(setSymbolMatch(context, identifierRef, node, oneMatch));
        }

        return true;
    }

    // Compute dependencies if not already done
    if (node->semanticState == AstNodeResolveState::ProcessingChilds || (riFlags & RI_FOR_GHOSTING) || (riFlags & RI_FOR_ZERO_GHOSTING))
    {
        scopeHierarchy.clear();
        scopeHierarchyVars.clear();
        dependentSymbols.clear();
    }

    if (dependentSymbols.empty())
    {
        // :SilentCall
        if (node->token.text.empty())
        {
            OneSymbolMatch sm = {0};
            sm.symbol         = identifierRef->resolvedSymbolName;
            dependentSymbols.push_back(sm);
        }
        else
        {
            SWAG_CHECK(findIdentifierInScopes(context, identifierRef, node));
            if (context->result != ContextResult::Done)
                return true;
        }

        // Because of #self
        if (node->semFlags & AST_SEM_FORCE_SCOPE)
            return true;

        if (dependentSymbols.empty())
        {
            SWAG_ASSERT(identifierRef->flags & AST_SILENT_CHECK);
            return true;
        }
    }

    // If one of my dependent symbol is not fully solved, we need to wait
    for (auto& p : dependentSymbols)
    {
        auto symbol     = p.symbol;
        bool needToWait = false;

        // First test, with just a SharedLock for contention
        {
            SharedLock lkn(symbol->mutex);
            if (!needToWaitForSymbol(context, node, symbol, needToWait))
                continue;
        }

        // If true, then do the test again, this time with a lock
        if (needToWait)
        {
            ScopedLock lkn(symbol->mutex);
            needToWait = false;
            if (!needToWaitForSymbol(context, node, symbol, needToWait))
                continue;
            if (needToWait)
            {
                job->waitSymbolNoLock(symbol);
            }
        }

        // In case identifier is part of a reference, need to initialize it
        if (!needToWait && node != node->identifierRef->childs.back())
            SWAG_CHECK(setupIdentifierRef(context, node, node->typeInfo));

        return true;
    }

    // Filter symbols
    SWAG_CHECK(filterSymbols(context, node));
    if (dependentSymbols.empty())
        return context->report(node, Fmt(Err(Err0133), node->token.ctext()));

    auto orgResolvedSymbolOverload = identifierRef->resolvedSymbolOverload;
    auto orgResolvedSymbolName     = identifierRef->resolvedSymbolName;
    auto orgPreviousResolvedNode   = identifierRef->previousResolvedNode;

    // In case of a reevaluation, ufcsFirstParam will be null, but we still want to cast for ufcs
    bool hasForcedUfcs = false;
    if (node->callParameters && !node->callParameters->childs.empty() && node->callParameters->childs.front()->flags & AST_TO_UFCS)
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
            return context->report(node, Fmt(Err(Err0133), node->token.ctext()));
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
            SWAG_CHECK(getUsingVar(context, identifierRef, node, symbolOverload, &dependentVar, &dependentVarLeaf));
            if (context->result == ContextResult::Pending)
                return true;

            // Get the ufcs first parameter if we can
            AstNode* ufcsFirstParam = nullptr;

            // The ufcs parameter has already been set in we are evaluating an identifier for the second time
            // (when we inline a function call)
            if (!node->callParameters || node->callParameters->childs.empty() || !(node->callParameters->childs.front()->flags & AST_TO_UFCS))
            {
                SWAG_CHECK(getUfcs(context, identifierRef, node, symbolOverload, &ufcsFirstParam));
                if (context->result == ContextResult::Pending)
                    return true;
                if ((node->semFlags & AST_SEM_FORCE_UFCS) && !ufcsFirstParam)
                    continue;
            }

            // If the last parameter of a function is of type 'code', and the last call parameter is not,
            // then we take the next statement, after the function, and put it as the last parameter
            SWAG_CHECK(appendLastCodeStatement(context, node, symbolOverload));

            auto  tryMatch        = job->getTryMatch();
            auto& symMatchContext = tryMatch->symMatchContext;

            tryMatch->genericParameters = node->genericParameters;
            tryMatch->callParameters    = node->callParameters;
            tryMatch->dependentVar      = dependentVar;
            tryMatch->dependentVarLeaf  = dependentVarLeaf;
            tryMatch->overload          = symbolOverload;
            tryMatch->scope             = oneOver.scope;
            tryMatch->ufcs              = ufcsFirstParam || hasForcedUfcs;

            tryMatch->cptOverloads     = oneOver.cptOverloads;
            tryMatch->cptOverloadsInit = oneOver.cptOverloadsInit;

            SWAG_CHECK(fillMatchContextCallParameters(context, symMatchContext, node, symbolOverload, ufcsFirstParam));
            if (context->result == ContextResult::Pending)
                return true;
            SWAG_CHECK(fillMatchContextGenericParameters(context, symMatchContext, node, symbolOverload));

            if (node->forceTakeAddress())
                symMatchContext.flags |= SymbolMatchContext::MATCH_FOR_LAMBDA;

            listTryMatch.push_back(tryMatch);
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
        SWAG_CHECK(matchIdentifierParameters(context, listTryMatch, node, mipFlags));
        if (context->result == ContextResult::Pending)
            return true;

        if (context->result == ContextResult::NewChilds1)
        {
            context->result = ContextResult::NewChilds;
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
        if (node->semFlags & AST_SEM_FORCE_UFCS)
        {
            unknownIdentifier(context, identifierRef, CastAst<AstIdentifier>(node, AstNodeKind::Identifier));
            return false;
        }

        return false;
    }

    if (riFlags & (RI_FOR_GHOSTING | RI_FOR_ZERO_GHOSTING))
        return true;

    // Deal with ufcs. Now that the match is done, we will change the ast in order to
    // add the ufcs parameters to the function call parameters
    auto& match = job->cacheMatches[0];
    if (match->ufcs && !hasForcedUfcs)
    {
        // Do not change AST if this is code inside a generic function
        if (!node->ownerFct || !(node->ownerFct->flags & AST_IS_GENERIC))
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

    // :SilentCall
    if (node->token.text.empty())
        node->typeInfo = identifierRef->typeInfo;
    else
        node->typeInfo = match->symbolOverload->typeInfo;

    SWAG_CHECK(setSymbolMatch(context, identifierRef, node, *match));
    return true;
}

void SemanticJob::collectAlternativeScopes(AstNode* startNode, VectorNative<AlternativeScope>& scopes)
{
    // Need to go deep for using vars, because we can have a using on a struct, which has also
    // a using itself, and so on...
    VectorNative<AlternativeScope> toAdd;
    VectorNative<Scope*>           done;

    {
        SharedLock lk(startNode->extension->mutexAltScopes);
        toAdd.append(startNode->extension->alternativeScopes);
    }

    while (!toAdd.empty())
    {
        auto it0 = toAdd.back();
        toAdd.pop_back();

        if (!done.contains(it0.scope))
        {
            done.push_back(it0.scope);
            addAlternativeScopeOnce(scopes, it0.scope, it0.flags);

            if (it0.scope && it0.scope->kind == ScopeKind::Struct && it0.scope->owner->extension)
            {
                // We register the sub scope with the original "node" (top level), because the original node will in the end
                // become the dependentVar node, and will be converted by cast to the correct type.
                {
                    SharedLock lk(it0.scope->owner->extension->mutexAltScopes);
                    for (auto& it1 : it0.scope->owner->extension->alternativeScopes)
                        toAdd.push_back({it1.scope, it1.flags});
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

    {
        SharedLock lk(startNode->extension->mutexAltScopes);
        toAdd.append(startNode->extension->alternativeScopesVars);
    }

    while (!toAdd.empty())
    {
        auto it0 = toAdd.back();
        toAdd.pop_back();

        if (!done.contains(it0.scope))
        {
            done.push_back(it0.scope);
            addAlternativeScopeOnce(scopes, it0.scope, it0.flags);
            scopesVars.push_back(it0);

            if (it0.scope && it0.scope->kind == ScopeKind::Struct && it0.scope->owner->extension)
            {
                // We register the sub scope with the original "node" (top level), because the original node will in the end
                // become the dependentVar node, and will be converted by cast to the correct type.
                {
                    SharedLock lk(it0.scope->owner->extension->mutexAltScopes);
                    for (auto& it1 : it0.scope->owner->extension->alternativeScopesVars)
                        toAdd.push_back({it0.node, it1.node, it1.scope, it1.flags});
                }

                // If this is a struct that comes from a generic, we need to also register the generic scope in order
                // to be able to find generic functions to instantiate
                SWAG_ASSERT(it0.scope->owner->typeInfo->kind == TypeInfoKind::Struct);
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
    AlternativeScope as;
    as.scope = scope;
    as.flags = flags;
    scopes.push_back(as);
}

void SemanticJob::collectAlternativeScopeHierarchy(SemanticContext* context, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars, AstNode* startNode, uint32_t flags)
{
    // Add registered alternative scopes of the current node
    if (startNode->extension && !startNode->extension->alternativeScopes.empty())
    {
        auto  job       = context->job;
        auto& toProcess = job->scopesToProcess;
        for (auto& as : startNode->extension->alternativeScopes)
        {
            if (!hasAlternativeScope(scopes, as.scope))
            {
                addAlternativeScope(scopes, as.scope, as.flags);
                addAlternativeScopeOnce(toProcess, as.scope);
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
    if (startNode->kind == AstNodeKind::Inline)
    {
        auto inlineNode = CastAst<AstInline>(startNode, AstNodeKind::Inline);
        SWAG_ASSERT(inlineNode->parametersScope);
        addAlternativeScopeOnce(scopes, inlineNode->parametersScope);
    }

    if (startNode->kind == AstNodeKind::CompilerMacro)
    {
        if (!(flags & COLLECT_BACKTICK))
        {
            while (startNode->parent->kind != AstNodeKind::Inline && // Need to test on parent to be able to add alternative scopes of the inline block
                   startNode->kind != AstNodeKind::CompilerInline &&
                   startNode->kind != AstNodeKind::FuncDecl)
            {
                startNode = startNode->parent;
            }
        }

        flags &= ~COLLECT_BACKTICK;

        // Macro in a sub function, stop there
        if (startNode->kind == AstNodeKind::FuncDecl)
            return;
    }

    // If we are in an inline block, jump right to the function parent
    else if (startNode->kind == AstNodeKind::CompilerInline)
    {
        if (!(flags & COLLECT_BACKTICK))
        {
            while (startNode && startNode->kind != AstNodeKind::FuncDecl)
                startNode = startNode->parent;
        }

        flags &= ~COLLECT_BACKTICK;
    }

    // If we are in an inline block, jump right to the function parent
    // Not that the function parent can be null in case of inlined expression in a global for example (compile time execution)
    else if (startNode->kind == AstNodeKind::Inline)
    {
        auto inlineBlock = CastAst<AstInline>(startNode, AstNodeKind::Inline);
        if (!(inlineBlock->func->attributeFlags & ATTRIBUTE_MIXIN))
        {
            if (!(flags & COLLECT_BACKTICK))
            {
                while (startNode && startNode->kind != AstNodeKind::FuncDecl)
                    startNode = startNode->parent;
            }

            flags &= ~COLLECT_BACKTICK;
        }
    }

    if (!startNode)
        return;

    if (startNode->extension && startNode->extension->alternativeNode)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->extension->alternativeNode, flags);
    else if (startNode->parent)
        collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode->parent, flags);

    // Mixin block, collect alternative scopes from the original source tree (with the user code, before
    // making the inline)
    if (startNode->extension && startNode->extension->alternativeNode && startNode->parent->kind == AstNodeKind::CompilerMixin)
    {
        // We authorize mixin code to access the parameters of the Swag.mixin function, except if there's a #macro block
        // in the way.
        while (startNode->kind != AstNodeKind::Inline &&
               startNode->kind != AstNodeKind::CompilerInline &&
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

bool SemanticJob::collectScopeHierarchy(SemanticContext* context, VectorNative<AlternativeScope>& scopes, VectorNative<AlternativeScopeVar>& scopesVars, AstNode* startNode, uint32_t flags)
{
    auto  job        = context->job;
    auto& toProcess  = job->scopesToProcess;
    auto  sourceFile = context->sourceFile;

    toProcess.clear();
    scopes.clear();

    // Get alternative scopes from the node hierarchy
    collectAlternativeScopeHierarchy(context, scopes, scopesVars, startNode, flags);

    auto startScope = startNode->ownerScope;
    if (startScope)
    {
        // For a backtick, do not collect scope until we find an inline block
        if (flags & COLLECT_BACKTICK)
        {
            while (startScope && startScope->kind != ScopeKind::Inline && startScope->kind != ScopeKind::Macro)
                startScope = startScope->parentScope;
            SWAG_VERIFY(startScope, context->report(context->node, Err(Err0136)));
            startScope = startScope->parentScope;
            flags &= ~COLLECT_BACKTICK;
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

    for (int i = 0; i < toProcess.size(); i++)
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
            while (scope && scope->parentScope->kind != ScopeKind::Inline)
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

    SWAG_VERIFY(!(flags & COLLECT_BACKTICK), context->report(startNode, Err(Err0136)));

    return true;
}

bool SemanticJob::checkCanThrow(SemanticContext* context)
{
    auto node = context->node;

    // For a try/throw inside an inline block, take the original function, except if it is flagged with 'Swag.noreturn'
    if (node->ownerInline)
    {
        if (!(node->ownerInline->func->attributeFlags & ATTRIBUTE_NO_RETURN) && !(node->flags & AST_IN_MIXIN))
            node->semFlags |= AST_SEM_EMBEDDED_RETURN;
    }

    auto parentFct = (node->semFlags & AST_SEM_EMBEDDED_RETURN) ? node->ownerInline->func : node->ownerFct;

    if (parentFct->isSpecialFunctionName())
        return context->report(node, Fmt(Err(Err0137), node->token.ctext(), parentFct->token.ctext()));

    if (!(parentFct->typeInfo->flags & TYPEINFO_CAN_THROW) && !(parentFct->attributeFlags & ATTRIBUTE_SHARP_FUNC))
        return context->report(node, Fmt(Err(Err0138), node->token.ctext(), parentFct->token.ctext()));

    return true;
}

bool SemanticJob::checkCanCatch(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Try, AstNodeKind::Catch, AstNodeKind::Assume);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);

    for (auto c : identifierRef->childs)
    {
        if (!c->resolvedSymbolOverload)
            continue;
        if (c->resolvedSymbolOverload->symbol->kind == SymbolKind::Function || c->resolvedSymbolOverload->typeInfo->kind == TypeInfoKind::Lambda)
            return true;
    }

    auto lastChild = identifierRef->childs.back();
    return context->report(node, Fmt(Err(Err0139), node->token.ctext(), lastChild->token.ctext(), SymTable::getArticleKindName(lastChild->resolvedSymbolName->kind)));
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

bool SemanticJob::resolveAssume(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Assume);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));

    node->allocateExtension();
    node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitInitStackTrace;
    node->typeInfo                     = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->inheritComputedValue(identifierRef);
    node->byteCodeFct = ByteCodeGenJob::emitPassThrough;

    return true;
}

bool SemanticJob::resolveCatch(SemanticContext* context)
{
    auto node          = CastAst<AstTryCatchAssume>(context->node, AstNodeKind::Catch);
    auto identifierRef = CastAst<AstIdentifierRef>(node->childs.front(), AstNodeKind::IdentifierRef);
    auto lastChild     = identifierRef->childs.back();

    SWAG_CHECK(checkCanCatch(context));
    SWAG_ASSERT(node->ownerFct);
    node->ownerFct->needRegisterGetContext = true;

    node->allocateExtension();
    node->extension->byteCodeBeforeFct = ByteCodeGenJob::emitInitStackTrace;
    node->byteCodeFct                  = ByteCodeGenJob::emitPassThrough;
    node->typeInfo                     = lastChild->typeInfo;
    node->flags |= identifierRef->flags;
    node->flags &= ~AST_DISCARD;
    node->inheritComputedValue(identifierRef);

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
