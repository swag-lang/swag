#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "TypeManager.h"

bool Generic::updateGenericParameters(SemanticContext* context, bool doType, bool doNode, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match)
{
    for (int i = 0; i < typeGenericParameters.size(); i++)
    {
        auto param = typeGenericParameters[i];

        if (doType)
        {
            // If the user has specified a generic type, take it
            if (callGenericParameters)
            {
                param->typeInfo = callGenericParameters->childs[i]->typeInfo;
                param->value    = callGenericParameters->childs[i]->computedValue;
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

        // We should not instantiate with unresolved types
        auto genGen = match.genericParametersGenTypes[i];
        if (genGen->kind == TypeInfoKind::Generic)
        {
            if (param->typeInfo->flags & TYPEINFO_UNTYPED_INTEGER)
            {
                auto symbol = match.symbolName;
                return context->report({context->node, format("cannot instantiate generic %s '%s' with an untyped integer, you need to specify a type\n", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())});
            }

            if (param->typeInfo->flags & TYPEINFO_UNTYPED_FLOAT)
            {
                auto symbol = match.symbolName;
                return context->report({context->node, format("cannot instantiate generic %s '%s' with an untyped float, you need to specify a type\n", SymTable::getNakedKindName(symbol->kind), symbol->name.c_str())});
            }
        }

        if (doNode)
        {
            auto nodeParam           = nodeGenericParameters[i];
            nodeParam->kind          = AstNodeKind::ConstDecl;
            nodeParam->computedValue = param->value;
            nodeParam->setFlagsValueIsComputed();
            nodeParam->flags |= AST_FROM_GENERIC;
        }
    }

    return true;
}

TypeInfo* Generic::doTypeSubstitution(CloneContext& cloneContext, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (!(typeInfo->flags & TYPEINFO_GENERIC))
        return typeInfo;

    // Search if the type has a corresponding replacement
    auto it = cloneContext.replaceTypes.find(typeInfo->name);
    if (it != cloneContext.replaceTypes.end())
    {
        // We can have a match on a lambda for a function attribute, when function has been generated
        // In that case, we want to be sure that the kind is function
        if (typeInfo->kind == TypeInfoKind::FuncAttr && it->second->kind == TypeInfoKind::Lambda)
        {
            auto t  = it->second->clone();
            t->kind = TypeInfoKind::FuncAttr;
            return t;
        }

        return it->second;
    }

    // When type is a compound, we do substitution in the raw type
    switch (typeInfo->kind)
    {
    case TypeInfoKind::TypedVariadic:
    {
        auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(typeInfo, TypeInfoKind::TypedVariadic);
        auto newType      = doTypeSubstitution(cloneContext, typeVariadic->rawType);
        if (newType != typeVariadic->rawType)
        {
            typeVariadic          = CastTypeInfo<TypeInfoVariadic>(typeVariadic->clone(), TypeInfoKind::TypedVariadic);
            typeVariadic->rawType = newType;
            typeVariadic->flags &= ~TYPEINFO_GENERIC;
            typeVariadic->forceComputeName();
            return typeVariadic;
        }

        break;
    }

    case TypeInfoKind::Alias:
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        auto newType   = doTypeSubstitution(cloneContext, typeAlias->rawType);
        if (newType != typeAlias->rawType)
        {
            typeAlias          = CastTypeInfo<TypeInfoAlias>(typeAlias->clone(), TypeInfoKind::Alias);
            typeAlias->rawType = newType;
            typeAlias->flags &= ~TYPEINFO_GENERIC;
            typeAlias->forceComputeName();
            return typeAlias;
        }

        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(typeInfo, TypeInfoKind::Pointer);
        auto newType     = doTypeSubstitution(cloneContext, typePointer->finalType);
        if (newType != typePointer->finalType)
        {
            typePointer            = CastTypeInfo<TypeInfoPointer>(typePointer->clone(), TypeInfoKind::Pointer);
            typePointer->finalType = newType;
            typePointer->flags &= ~TYPEINFO_GENERIC;
            typePointer->forceComputeName();
            typePointer->computePointedType();
            return typePointer;
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto      typeArray      = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto      newPointedType = doTypeSubstitution(cloneContext, typeArray->pointedType);
        TypeInfo* newFinalType;
        if (typeArray->pointedType == typeArray->finalType)
            newFinalType = newPointedType;
        else
            newFinalType = doTypeSubstitution(cloneContext, typeArray->finalType);
        if (newPointedType != typeArray->pointedType || newFinalType != typeArray->finalType)
        {
            typeArray              = CastTypeInfo<TypeInfoArray>(typeArray->clone(), TypeInfoKind::Array);
            typeArray->pointedType = newPointedType;
            typeArray->finalType   = newFinalType;
            typeArray->flags &= ~TYPEINFO_GENERIC;
            typeArray->forceComputeName();
            return typeArray;
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);
        auto newType   = doTypeSubstitution(cloneContext, typeSlice->pointedType);
        if (newType != typeSlice->pointedType)
        {
            typeSlice              = CastTypeInfo<TypeInfoSlice>(typeSlice->clone(), TypeInfoKind::Slice);
            typeSlice->pointedType = newType;
            typeSlice->flags &= ~TYPEINFO_GENERIC;
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

        auto newType = doTypeSubstitution(cloneContext, typeLambda->returnType);
        if (newType != typeLambda->returnType)
        {
            newLambda = CastTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), typeLambda->kind);
            newLambda->flags &= ~TYPEINFO_GENERIC;
            newLambda->returnType   = newType;
            newLambda->replaceTypes = cloneContext.replaceTypes;
        }

        auto numParams = typeLambda->parameters.size();
        for (int idx = 0; idx < numParams; idx++)
        {
            auto param = CastTypeInfo<TypeInfoParam>(typeLambda->parameters[idx], TypeInfoKind::Param);
            newType    = doTypeSubstitution(cloneContext, param->typeInfo);
            if (newType != param->typeInfo)
            {
                if (!newLambda)
                {
                    newLambda = CastTypeInfo<TypeInfoFuncAttr>(typeLambda->clone(), TypeInfoKind::FuncAttr, TypeInfoKind::Lambda);
                    newLambda->flags &= ~TYPEINFO_GENERIC;
                    newLambda->replaceTypes = cloneContext.replaceTypes;
                }

                auto newParam      = CastTypeInfo<TypeInfoParam>(newLambda->parameters[idx], TypeInfoKind::Param);
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
    if (waitSymbol)
        job->waitForSymbolNoLock(symbol);

    // Run semantic on that struct/function
    auto sourceFile = context->sourceFile;
    auto newJob     = SemanticJob::newJob(job->dependentJob, sourceFile, newNode, false);

    // Store stack of instantiation contexts
    auto srcCxt  = context;
    auto destCxt = &newJob->context;
    destCxt->expansionNode.append(srcCxt->expansionNode);
    destCxt->expansionNode.push_back(context->node);

    return newJob;
}

bool Generic::instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto node = context->node;
    SWAG_VERIFY(!match.genericReplaceTypes.empty(), context->report({node, node->token, format("cannot instantiate generic struct '%s', missing contextual types replacements", node->token.text.c_str())}));

    // Be sure all methods have been registered, because we need opDrop & co to be known, as we need
    // to instantiate them also (because those functions can be called by the compiler itself, not by the user)
    auto typeStruct = CastTypeInfo<TypeInfoStruct>(match.symbolOverload->typeInfo, match.symbolOverload->typeInfo->kind);
    context->job->waitForAllStructMethods(typeStruct);
    if (context->result != ContextResult::Done)
        return true;
    context->job->waitForAllStructInterfaces(typeStruct);
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
    SWAG_VERIFY(sourceNode->kind == AstNodeKind::StructDecl, context->report({node, node->token, format("partial type alias for generic struct instantation is not supported", node->token.text.c_str())}));

    // Make a new type
    auto newType = CastTypeInfo<TypeInfoStruct>(genericStructType->clone(), genericStructType->kind);
    newType->flags &= ~TYPEINFO_GENERIC;
    newType->fromGeneric = genericStructType;

    // Replace generic types and values in the struct generic parameters
    auto sourceNodeStruct = CastAst<AstStruct>(sourceNode, AstNodeKind::StructDecl);
    SWAG_CHECK(updateGenericParameters(context, true, false, newType->genericParameters, sourceNodeStruct->genericParameters->childs, genericParameters, match));
    newType->forceComputeName();

    // Add the struct type replacement now, in case the struct has a field to replace
    cloneContext.replaceTypes[overload->typeInfo->name] = newType;

    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    structNode->replaceTypes = cloneContext.replaceTypes;
    Ast::addChildBack(sourceNode->parent, structNode);

    newType->scope           = structNode->scope;
    newType->declNode        = structNode;
    structNode->typeInfo     = newType;
    structNode->ownerGeneric = context->node;

    // Replace generic types and values in the struct generic parameters
    SWAG_CHECK(updateGenericParameters(context, false, true, newType->genericParameters, structNode->genericParameters->childs, genericParameters, match));

    auto structJob = end(context, context->job, sourceSymbol, structNode, true);

    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserDropFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostCopyFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostMoveFct);

    // Force instantiation of all special functions
    for (auto method : newType->methods)
    {
        if (method->declNode->flags & AST_IS_SPECIAL_FUNC)
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
    auto newFunc = CastAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
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

    Ast::addChildBack(funcNode->parent, newFunc);
    *specialFct = newFunc;

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->flags & TYPEINFO_GENERIC)
    {
        newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->flags &= ~TYPEINFO_GENERIC;
        newFunc->typeInfo = newTypeFunc;
    }

    // Replace generic types and values in the function generic parameters
    newTypeFunc->forceComputeName();

    scoped_lock lk(newFunc->resolvedSymbolName->mutex);
    auto        newJob = end(context, context->job, newFunc->resolvedSymbolName, newFunc, false);
    structJob->dependentJobs.add(newJob);
}

bool Generic::instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto       node             = context->node;
    AstStruct* contextualStruct = nullptr;
    AstNode*   contextualNode   = nullptr;

    // Get the contextual structure call if it exists
    if (context->node->kind == AstNodeKind::Identifier)
    {
        auto identifier = CastAst<AstIdentifier>(context->node, AstNodeKind::Identifier);
        if (identifier->identifierRef->resolvedSymbolOverload && identifier->identifierRef->resolvedSymbolOverload->typeInfo->kind == TypeInfoKind::Struct)
        {
            contextualNode   = identifier->identifierRef->previousResolvedNode;
            contextualStruct = CastAst<AstStruct>(identifier->identifierRef->resolvedSymbolOverload->typeInfo->declNode, AstNodeKind::StructDecl);
        }
    }

    // We should have types to replace
    bool noReplaceTypes = false;
    if (match.genericReplaceTypes.empty())
    {
        noReplaceTypes = true;

        // If we do not have types to replace, perhaps we will be able to deduce them in case of a
        // function call parameter
        auto parent = node->parent;
        while (parent && parent->kind != AstNodeKind::FuncCallParam)
            parent = parent->parent;

        if (!parent)
        {
            if (contextualNode)
                return context->report({contextualNode, contextualNode->token, format("cannot instantiate generic function '%s', missing generic parameters", node->token.text.c_str())});
            else
                return context->report({node, node->token, format("cannot instantiate generic function '%s', missing contextual types replacements", node->token.text.c_str())});
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
            cloneContext.replaceTypes[p->typeInfo->name] = g_TypeMgr.typeInfoUndefined;
        for (auto p : typeFunc->parameters)
        {
            if (p->typeInfo->flags & TYPEINFO_GENERIC)
                cloneContext.replaceTypes[p->typeInfo->name] = g_TypeMgr.typeInfoUndefined;
        }
    }

    // Clone original node
    auto overload = match.symbolOverload;
    auto funcNode = overload->node;
    auto newFunc  = CastAst<AstFuncDecl>(funcNode->clone(cloneContext), AstNodeKind::FuncDecl);
    newFunc->flags |= AST_FROM_GENERIC;
    newFunc->content->flags &= ~AST_NO_SEMANTIC;
    newFunc->replaceTypes = cloneContext.replaceTypes;
    Ast::addChildBack(funcNode->parent, newFunc);

    // If we are calling the function in a struct context (struct.func), then add the struct as
    // an alternative scope
    if (contextualStruct)
    {
        newFunc->allocateExtension();
        newFunc->extension->alternativeScopes.push_back(contextualStruct->scope);
    }

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo, newFunc->typeInfo->kind);
    if (newTypeFunc->flags & TYPEINFO_GENERIC || noReplaceTypes)
    {
        newTypeFunc = CastTypeInfo<TypeInfoFuncAttr>(newFunc->typeInfo->clone(), newFunc->typeInfo->kind);
        newTypeFunc->flags &= ~TYPEINFO_GENERIC;
        newTypeFunc->declNode = newFunc;
        newFunc->typeInfo     = newTypeFunc;
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
                            return context->report({node, format("cannot instantiate variable because type '%s' is generic", node->typeInfo->name.c_str())});

                        auto child          = Ast::newFuncCallParam(context->sourceFile, identifier->genericParameters);
                        cloneContext.parent = child;
                        param->assignment->clone(cloneContext);
                    }

                    idRef->flags &= ~AST_IS_GENERIC;
                    identifier->flags &= ~AST_IS_GENERIC;
                    node->flags &= ~AST_IS_GENERIC;
                    node->type->flags &= ~AST_IS_GENERIC;
                    context->result     = ContextResult::NewChilds;
                    node->semanticState = AstNodeResolveState::Enter;
                    return true;
                }
            }
        }
    }

    return context->report({node, format("cannot instantiate variable because type '%s' is generic", node->typeInfo->name.c_str())});
}