#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "SourceFile.h"
#include "Module.h"

bool Generic::updateGenericParameters(SemanticContext* context, bool doType, bool doNode, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match)
{
    for (int i = 0; i < typeGenericParameters.size(); i++)
    {
        auto param = typeGenericParameters[i];

        if (doType)
        {
            // If the user has specified a generic type, take it
            if (callGenericParameters && i < callGenericParameters->childs.size())
            {
                auto genParam   = callGenericParameters->childs[i];
                param->typeInfo = genParam->typeInfo;
                if (genParam->computedValue)
                {
                    param->allocateComputedValue();
                    *param->value = *genParam->computedValue;
                }

                if ((genParam->flags & AST_VALUE_COMPUTED) && !(genParam->flags & AST_VALUE_IS_TYPEINFO))
                    param->flags |= TYPEINFO_DEFINED_VALUE;
            }

            // If we have a calltype filled with the match, take it
            else if (match.genericParametersCallTypes[i])
            {
                param->typeInfo = match.genericParametersCallTypes[i];
            }

            // Take the type as it is in the instantiated struct/func
            else
            {
                SWAG_ASSERT(i < nodeGenericParameters.size());
                param->typeInfo = nodeGenericParameters[i]->typeInfo;
            }
        }

        SWAG_ASSERT(param->typeInfo);

        // Value
        auto it1 = match.genericReplaceValues.find(param->namedParam);
        if (it1 != match.genericReplaceValues.end())
        {
            param->allocateComputedValue();
            *param->value = *it1->second.first;
        }

        // We should not instantiate with unresolved types
        auto genGen = match.genericParametersGenTypes[i];
        if (genGen->kind == TypeInfoKind::Generic)
            param->typeInfo = TypeManager::solidifyUntyped(param->typeInfo);

        if (doNode)
        {
            auto nodeParam = nodeGenericParameters[i];
            nodeParam->setFlagsValueIsComputed();
            nodeParam->kind = AstNodeKind::ConstDecl;
            nodeParam->flags |= AST_FROM_GENERIC;
            if (param->value)
            {
                nodeParam->allocateComputedValue();
                *nodeParam->computedValue = *param->value;
            }
        }
    }

    return true;
}

TypeInfo* Generic::doTypeSubstitution(map<Utf8, TypeInfo*>& replaceTypes, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (replaceTypes.empty())
        return typeInfo;
    if (!(typeInfo->flags & TYPEINFO_GENERIC))
        return typeInfo;

    // Search if the type has a corresponding replacement
    auto it = replaceTypes.find(typeInfo->name);
    if (it != replaceTypes.end())
    {
        // We can have a match on a lambda for a function attribute, when function has been generated
        // In that case, we want to be sure that the kind is function
        if (typeInfo->kind == TypeInfoKind::FuncAttr && it->second->kind == TypeInfoKind::Lambda)
        {
            auto t  = it->second->clone();
            t->kind = TypeInfoKind::FuncAttr;
            return t;
        }

        // Do not substitute with unconverted TypeList
        if (it->second->kind != TypeInfoKind::TypeListArray)
            return it->second;
    }

    // When type is a compound, we do substitution in the raw type
    switch (typeInfo->kind)
    {
    case TypeInfoKind::TypedVariadic:
    {
        auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        auto newType      = doTypeSubstitution(replaceTypes, typeVariadic->rawType);
        if (newType != typeVariadic->rawType)
        {
            typeVariadic          = CastTypeInfo<TypeInfoVariadic>(typeVariadic->clone(), TypeInfoKind::TypedVariadic);
            typeVariadic->rawType = newType;
            typeVariadic->removeGenericFlag();
            typeVariadic->forceComputeName();
            return typeVariadic;
        }

        break;
    }

    case TypeInfoKind::Alias:
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        auto newType   = doTypeSubstitution(replaceTypes, typeAlias->rawType);
        if (newType != typeAlias->rawType)
        {
            typeAlias          = CastTypeInfo<TypeInfoAlias>(typeAlias->clone(), TypeInfoKind::Alias);
            typeAlias->rawType = newType;
            typeAlias->removeGenericFlag();
            typeAlias->forceComputeName();
            return typeAlias;
        }

        break;
    }

    case TypeInfoKind::Reference:
    {
        auto typeRef = CastTypeInfo<TypeInfoReference>(typeInfo, TypeInfoKind::Reference);
        auto newType = doTypeSubstitution(replaceTypes, typeRef->pointedType);
        if (newType != typeRef->pointedType)
        {
            typeRef              = CastTypeInfo<TypeInfoReference>(typeRef->clone(), TypeInfoKind::Reference);
            typeRef->pointedType = newType;
            typeRef->removeGenericFlag();
            typeRef->forceComputeName();
            return typeRef;
        }

        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto newType     = doTypeSubstitution(replaceTypes, typePointer->pointedType);
        if (newType != typePointer->pointedType)
        {
            typePointer              = CastTypeInfo<TypeInfoPointer>(typePointer->clone(), TypeInfoKind::Pointer);
            typePointer->pointedType = newType;
            typePointer->removeGenericFlag();
            typePointer->forceComputeName();
            return typePointer;
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto      typeArray      = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto      newPointedType = doTypeSubstitution(replaceTypes, typeArray->pointedType);
        TypeInfo* newFinalType;
        if (typeArray->pointedType == typeArray->finalType)
            newFinalType = newPointedType;
        else
            newFinalType = doTypeSubstitution(replaceTypes, typeArray->finalType);
        if (newPointedType != typeArray->pointedType || newFinalType != typeArray->finalType)
        {
            typeArray              = CastTypeInfo<TypeInfoArray>(typeArray->clone(), TypeInfoKind::Array);
            typeArray->pointedType = newPointedType;
            typeArray->finalType   = newFinalType;
            typeArray->removeGenericFlag();
            typeArray->forceComputeName();
            return typeArray;
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto newType   = doTypeSubstitution(replaceTypes, typeSlice->pointedType);
        if (newType != typeSlice->pointedType)
        {
            typeSlice              = CastTypeInfo<TypeInfoSlice>(typeSlice->clone(), TypeInfoKind::Slice);
            typeSlice->pointedType = newType;
            typeSlice->removeGenericFlag();
            typeSlice->forceComputeName();
            return typeSlice;
        }

        break;
    }

    case TypeInfoKind::Lambda:
    case TypeInfoKind::FuncAttr:
    {
        TypeInfoFuncAttr* newLambda  = nullptr;
        auto              typeLambda = CastTypeInfo<TypeInfoFuncAttr>(typeInfo, TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);

        auto newType = doTypeSubstitution(replaceTypes, typeLambda->returnType);
        if (newType != typeLambda->returnType)
        {
            newLambda = CastTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), typeLambda->kind);
            newLambda->removeGenericFlag();
            newLambda->returnType   = newType;
            newLambda->replaceTypes = replaceTypes;
        }

        auto numParams = typeLambda->parameters.size();
        for (int idx = 0; idx < numParams; idx++)
        {
            auto param = typeLambda->parameters[idx];
            newType    = doTypeSubstitution(replaceTypes, param->typeInfo);

            // If generic parameter is a closure, but the instantiated type is a lambda, then a conversion will
            // take place.
            // But we want to be sure that the closure stays a closure in ordre for the conversion to take place.
            // So transform the replaced lambda type by a closure one.
            if (param->typeInfo->isClosure() && newType->isLambda())
            {
                newType = newType->clone();
                newType->flags |= TYPEINFO_CLOSURE;
                newType->sizeOf = SWAG_LIMIT_CLOSURE_SIZEOF;
                newType->flags |= TYPEINFO_RETURN_BY_COPY | TYPEINFO_CLOSURE;

                auto newParam      = g_TypeMgr->makeParam();
                newParam->typeInfo = g_TypeMgr->typeInfoPointers[(int) NativeTypeKind::Void];
                auto newTypeFct    = CastTypeInfo<TypeInfoFuncAttr>(newType, newType->kind);
                newTypeFct->parameters.push_front(newParam);
            }

            if (newType != param->typeInfo)
            {
                if (!newLambda)
                {
                    newLambda = CastTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                    newLambda->removeGenericFlag();
                    newLambda->replaceTypes = replaceTypes;
                }

                auto newParam      = newLambda->parameters[idx];
                newParam->typeInfo = newType;
            }
        }

        if (newLambda)
        {
            newLambda->forceComputeName();
            return newLambda;
        }

        break;
    }
    }

    return typeInfo;
}

Job* Generic::end(SemanticContext* context, Job* job, SymbolName* symbol, AstNode* newNode, bool waitSymbol)
{
    // Need to wait for the struct/function to be semantic resolved
    symbol->cptOverloads++;
    symbol->cptOverloadsInit++;
    if (waitSymbol && context->result == ContextResult::Done)
        job->waitSymbolNoLock(symbol);

    // Run semantic on that struct/function
    auto sourceFile = context->sourceFile;
    auto newJob     = SemanticJob::newJob(job->dependentJob, sourceFile, newNode, false);

    // Store stack of instantiation contexts
    auto srcCxt  = context;
    auto destCxt = &newJob->context;
    destCxt->errorContextStack.insert(destCxt->errorContextStack.end(), srcCxt->errorContextStack.begin(), srcCxt->errorContextStack.end());

    // New context
    JobContext::ErrorContext expNode;
    expNode.node = context->node;
    if (expNode.node->extension && expNode.node->extension->exportNode)
        expNode.node = expNode.node->extension->exportNode;
    expNode.type = JobContext::ErrorContextType::Generic;
    destCxt->errorContextStack.push_back(expNode);

    return newJob;
}

void Generic::waitForGenericParameters(SemanticContext* context, OneGenericMatch& match)
{
    for (auto it : match.genericReplaceTypes)
    {
        auto typeInfo = it.second;
        auto declNode = typeInfo->declNode;
        if (!declNode)
            continue;
        if (!declNode->resolvedSymbolOverload)
            continue;
        if (declNode->resolvedSymbolOverload->symbol == match.symbolName)
            continue;
        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct);
            if (typeStruct->fromGeneric)
                continue;
        }

        context->job->waitOverloadCompleted(declNode->resolvedSymbolOverload);
        if (context->result == ContextResult::Pending)
            return;

        SWAG_ASSERT(typeInfo->sizeOf > 0);
    }
}

bool Generic::instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto node = context->node;
    SWAG_VERIFY(!match.genericReplaceTypes.empty(), context->report(node, Fmt(Err(Err0039), node->token.ctext())));

    // Be sure all methods have been registered, because we need opDrop & co to be known, as we need
    // to instantiate them also (because those functions can be called by the compiler itself, not by the user)
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(match.symbolOverload->typeInfo, match.symbolOverload->typeInfo->kind);
    context->job->waitAllStructSpecialMethods(typeStruct);
    if (context->result != ContextResult::Done)
        return true;
    context->job->waitAllStructInterfaces(typeStruct);
    if (context->result != ContextResult::Done)
        return true;

    // Types replacements
    CloneContext cloneContext;
    cloneContext.replaceTypes = move(match.genericReplaceTypes);

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;

    // Can be a type alias
    // In that case, we need to retrieve the real struct
    auto genericStructType = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, overload->typeInfo->kind);
    auto sourceSymbol      = match.symbolName;
    SWAG_VERIFY(sourceNode->kind == AstNodeKind::StructDecl, context->report(node, Fmt(Err(Err0040), node->token.ctext())));

    // Make a new type
    auto newType = CastTypeInfo<TypeInfoStruct>(genericStructType->clone(), genericStructType->kind);
    newType->removeGenericFlag();
    newType->fromGeneric = genericStructType;

    // Replace generic types in the struct generic parameters
    auto sourceNodeStruct = CastAst<AstStruct>(sourceNode, AstNodeKind::StructDecl);
    SWAG_CHECK(updateGenericParameters(context, true, false, newType->genericParameters, sourceNodeStruct->genericParameters->childs, genericParameters, match));
    newType->forceComputeName();

    // Add the struct type replacement now, in case the struct has a field to replace
    cloneContext.replaceTypes[overload->typeInfo->name] = newType;

    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, structNode);

    newType->scope           = structNode->scope;
    newType->declNode        = structNode;
    newType->replaceTypes    = cloneContext.replaceTypes;
    structNode->typeInfo     = newType;
    structNode->ownerGeneric = context->node;

    // Replace generic values in the struct generic parameters
    SWAG_CHECK(updateGenericParameters(context, false, true, newType->genericParameters, structNode->genericParameters->childs, genericParameters, match));

    auto structJob = end(context, context->job, sourceSymbol, structNode, true);

    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserDropFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostCopyFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostMoveFct);

    // Force instantiation of all special functions
    for (auto method : newType->methods)
    {
        if (method->declNode->isSpecialFunctionName())
        {
            auto specFunc = CastAst<AstFuncDecl>(method->declNode, AstNodeKind::FuncDecl);
            if (specFunc != genericStructType->opUserDropFct &&
                specFunc != genericStructType->opUserPostCopyFct &&
                specFunc != genericStructType->opUserPostMoveFct &&
                !specFunc->genericParameters)
            {
                instantiateSpecialFunc(context, structJob, cloneContext, &specFunc);
            }
        }
    }

    // Instantiate generic interfaces
    cloneContext.ownerStructScope   = structNode->scope;
    newType->cptRemainingInterfaces = (uint32_t) genericStructType->interfaces.size();
    for (auto itf : genericStructType->interfaces)
    {
        auto typeItf = (TypeInfoParam*) itf->clone();
        newType->interfaces.push_back(typeItf);

        SWAG_ASSERT(itf->declNode);
        auto newItf       = itf->declNode->clone(cloneContext);
        typeItf->declNode = newItf;
        newItf->flags |= AST_FROM_GENERIC;

        auto implJob = SemanticJob::newJob(context->job->dependentJob, context->sourceFile, newItf, false);
        structJob->addDependentJob(implJob);
    }

    context->job->jobsToAdd.push_back(structJob);
    return true;
}

void Generic::instantiateSpecialFunc(SemanticContext* context, Job* structJob, CloneContext& cloneContext, AstFuncDecl** specialFct)
{
    auto funcNode = *specialFct;
    if (!funcNode)
        return;

    // Clone original node
    AstFuncDecl* newFunc = CastAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
    if (newFunc->genericParameters)
    {
        newFunc->flags |= AST_IS_GENERIC;
        newFunc->content->flags |= AST_NO_SEMANTIC;
    }
    else
    {
        newFunc->flags |= AST_FROM_GENERIC;
        newFunc->content->flags &= ~AST_NO_SEMANTIC;
    }

    if (newFunc->selectIf)
        newFunc->content->flags |= AST_NO_SEMANTIC;

    Ast::addChildBack(funcNode->parent, newFunc);
    *specialFct = newFunc;

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->flags & TYPEINFO_GENERIC)
    {
        newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->removeGenericFlag();
        newFunc->typeInfo = newTypeFunc;
    }

    newTypeFunc->forceComputeName();

    ScopedLock lk(newFunc->resolvedSymbolName->mutex);
    auto       newJob = end(context, context->job, newFunc->resolvedSymbolName, newFunc, false);
    structJob->dependentJobs.add(newJob);
}

bool Generic::instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool selectIf)
{
    auto       node             = context->node;
    AstStruct* contextualStruct = nullptr;
    AstNode*   contextualNode   = nullptr;

    // Get the contextual structure call if it exists
    if (context->node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
        if (identifier->identifierRef->resolvedSymbolOverload)
        {
            auto concreteType = TypeManager::concreteType(identifier->identifierRef->resolvedSymbolOverload->typeInfo);
            if (concreteType->kind == TypeInfoKind::Struct)
            {
                contextualNode   = identifier->identifierRef->previousResolvedNode;
                contextualStruct = CastAst<AstStruct>(concreteType->declNode, AstNodeKind::StructDecl);
            }
        }
    }

    // We should have types to replace
    bool noReplaceTypes = match.genericReplaceTypes.empty();
    if (noReplaceTypes)
    {
        // If we do not have types to replace, perhaps we will be able to deduce them in case of a
        // function call parameter
        if (!node->findParent(AstNodeKind::FuncCallParam))
        {
            if (contextualNode)
                return context->report(contextualNode, Fmt(Err(Err0041), node->token.ctext()));
            return context->report(node, Fmt(Err(Err0042), node->token.ctext()));
        }
    }

    for (auto& v : match.genericReplaceTypes)
    {
        if (v.second->kind == TypeInfoKind::TypeListTuple)
        {
            auto tpt = CastTypeInfo<TypeInfoList>(v.second, TypeInfoKind::TypeListTuple);
            int  idx = 0;
            for (auto p : match.parameters)
            {
                if (p->typeInfo == tpt)
                {
                    auto typeDest = CastTypeInfo<TypeInfoStruct>(match.solvedParameters[idx]->typeInfo, TypeInfoKind::Struct);
                    SWAG_CHECK(TypeManager::convertLiteralTupleToStructType(context, typeDest, p));
                    SWAG_ASSERT(context->result != ContextResult::Done);
                    return true;
                }

                idx++;
            }
        }
    }

    // Types replacements
    CloneContext cloneContext;
    cloneContext.replaceTypes = move(match.genericReplaceTypes);

    // We replace all types and generic types with undefined for now
    if (noReplaceTypes)
    {
        auto typeFunc = CastTypeInfo<TypeInfoFuncAttr>(match.symbolOverload->node->typeInfo, TypeInfoKind::FuncAttr);
        for (auto p : typeFunc->genericParameters)
            cloneContext.replaceTypes[p->typeInfo->name] = g_TypeMgr->typeInfoUndefined;
        for (auto p : typeFunc->parameters)
        {
            if (p->typeInfo->flags & TYPEINFO_GENERIC)
                cloneContext.replaceTypes[p->typeInfo->name] = g_TypeMgr->typeInfoUndefined;
        }
    }

    // Clone original node
    auto         overload = match.symbolOverload;
    auto         funcNode = overload->node;
    AstFuncDecl* newFunc  = CastAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
    newFunc->flags |= AST_FROM_GENERIC;

    // If this is for testing a #selectif match, we must not evaluate the function content until the
    // #selectif has passed
    if (selectIf)
        newFunc->content->flags |= AST_NO_SEMANTIC;
    else
        newFunc->content->flags &= ~AST_NO_SEMANTIC;

    auto p = funcNode->parent;
    while (p && p->kind == AstNodeKind::AttrUse)
        p = p->parent;
    Ast::addChildBack(p, newFunc);

    // If we are calling the function in a struct context (struct.func), then add the struct as
    // an alternative scope
    if (contextualStruct)
        newFunc->addAlternativeScope(contextualStruct->scope);

    // If we are in a function, inherit also the scopes from the function.
    // Be carreful that if the call is inside a #macro, we do not want to inherit the function (3550)
    if (node->ownerFct && node->ownerFct->extension && node->ownerFct->extension->alternativeScopes.size() && !node->findParent(AstNodeKind::CompilerMacro))
        newFunc->addAlternativeScopes(node->ownerFct->extension->alternativeScopes);

    // Inherit alternative scopes from the generic function
    if (funcNode->extension && funcNode->extension->alternativeScopes.size())
        newFunc->addAlternativeScopes(funcNode->extension->alternativeScopes);

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->flags & TYPEINFO_GENERIC || noReplaceTypes)
    {
        newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->removeGenericFlag();
        newTypeFunc->declNode     = newFunc;
        newTypeFunc->replaceTypes = cloneContext.replaceTypes;
        newFunc->typeInfo         = newTypeFunc;
        if (noReplaceTypes)
            newTypeFunc->flags |= TYPEINFO_UNDEFINED;
    }

    // Replace generic types and values in the function generic parameters
    SWAG_CHECK(updateGenericParameters(context, true, true, newTypeFunc->genericParameters, newFunc->genericParameters->childs, genericParameters, match));
    newTypeFunc->forceComputeName();

    auto job = end(context, context->job, match.symbolName, newFunc, true);
    context->job->jobsToAdd.push_back(job);

    return true;
}

bool Generic::instantiateDefaultGeneric(SemanticContext* context, AstVarDecl* node)
{
    if (node->typeInfo->kind == TypeInfoKind::Struct && node->type && node->type->kind == AstNodeKind::TypeExpression)
    {
        auto typeExpr = CastAst<AstTypeExpression>(node->type, AstNodeKind::TypeExpression);
        if (typeExpr->identifier)
        {
            auto typeStruct = CastTypeInfo<TypeInfoStruct>(node->typeInfo, TypeInfoKind::Struct);
            auto nodeStruct = CastAst<AstStruct>(typeStruct->declNode, AstNodeKind::StructDecl);
            if (nodeStruct->genericParameters)
            {
                auto idRef      = CastAst<AstIdentifierRef>(typeExpr->identifier, AstNodeKind::IdentifierRef);
                auto identifier = CastAst<AstIdentifier>(idRef->childs.back(), AstNodeKind::Identifier);
                if (!identifier->genericParameters)
                {
                    identifier->genericParameters = Ast::newFuncCallParams(context->sourceFile, identifier);
                    identifier->genericParameters->flags |= AST_NO_BYTECODE;

                    CloneContext cloneContext;
                    for (auto p : nodeStruct->genericParameters->childs)
                    {
                        auto param = CastAst<AstVarDecl>(p, AstNodeKind::FuncDeclParam);
                        if (!param->assignment)
                            return context->report(node, Fmt(Err(Err0312), node->typeInfo->getDisplayNameC()));

                        auto child          = Ast::newFuncCallParam(context->sourceFile, identifier->genericParameters);
                        cloneContext.parent = child;
                        param->assignment->clone(cloneContext);
                    }

                    idRef->flags &= ~AST_IS_GENERIC;
                    identifier->flags &= ~AST_IS_GENERIC;
                    node->flags &= ~AST_IS_GENERIC;
                    node->type->flags &= ~AST_IS_GENERIC;

                    // Force the reevaluation of the variable and its childs
                    context->result     = ContextResult::NewChilds;
                    node->semanticState = AstNodeResolveState::Enter;
                    Ast::visit(node, [](AstNode* n)
                               { n->semFlags &= ~AST_SEM_ONCE; });
                    return true;
                }
            }
        }
    }

    return context->report(node, Fmt(Err(Err0312), node->typeInfo->getDisplayNameC()));
}