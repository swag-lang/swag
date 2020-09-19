#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "ThreadManager.h"

bool Generic::updateGenericParameters(SemanticContext* context, VectorNative<TypeInfoParam*>& typeGenericParameters, VectorNative<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match)
{
    for (int i = 0; i < typeGenericParameters.size(); i++)
    {
        auto param = typeGenericParameters[i];

        // If the user has specified a generic type, take it
        if (callGenericParameters)
        {
            param->typeInfo = callGenericParameters->childs[i]->typeInfo;
            param->value    = callGenericParameters->childs[i]->computedValue;
        }

        // If we have a calltype filled with the match, take it
        else if(match.genericParametersCallTypes[i])
        {
            param->typeInfo = match.genericParametersCallTypes[i];
        }

        // Take the type as it is in the instantiated struct/func
        else
        {
            SWAG_ASSERT(i < nodeGenericParameters.size());
            param->typeInfo = nodeGenericParameters[i]->typeInfo;
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

        auto nodeParam           = nodeGenericParameters[i];
        nodeParam->kind          = AstNodeKind::ConstDecl;
        nodeParam->computedValue = param->value;
        nodeParam->setFlagsValueIsComputed();
        nodeParam->flags |= AST_FROM_GENERIC;
    }

    return true;
}

TypeInfo* Generic::doTypeSubstitution(CloneContext& cloneContext, TypeInfo* typeInfo)
{
    if (!typeInfo)
        return nullptr;
    if (!(typeInfo->flags & TYPEINFO_GENERIC))
        return typeInfo;

    auto it = cloneContext.replaceTypes.find(typeInfo->name);
    if (it != cloneContext.replaceTypes.end())
        return it->second;

    // When type is a compound, we do substitution in the raw type
    switch (typeInfo->kind)
    {
    case TypeInfoKind::Alias:
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(typeInfo, TypeInfoKind::Alias);
        auto newType   = doTypeSubstitution(cloneContext, typeAlias->rawType);
        if (newType != typeAlias->rawType)
        {
            typeAlias          = static_cast<TypeInfoAlias*>(typeAlias->clone());
            typeAlias->rawType = newType;
            typeAlias->flags &= ~TYPEINFO_GENERIC;
            typeAlias->computeName();
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
            typePointer            = static_cast<TypeInfoPointer*>(typePointer->clone());
            typePointer->finalType = newType;
            typePointer->flags &= ~TYPEINFO_GENERIC;
            typePointer->computeName();
            typePointer->computePointedType();
            return typePointer;
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        auto newType   = doTypeSubstitution(cloneContext, typeArray->pointedType);
        if (newType != typeArray->pointedType)
        {
            typeArray              = static_cast<TypeInfoArray*>(typeArray->clone());
            typeArray->pointedType = newType;
            typeArray->flags &= ~TYPEINFO_GENERIC;
            typeArray->computeName();
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
            typeSlice              = static_cast<TypeInfoSlice*>(typeSlice->clone());
            typeSlice->pointedType = newType;
            typeSlice->flags &= ~TYPEINFO_GENERIC;
            typeSlice->computeName();
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
            newLambda = static_cast<TypeInfoFuncAttr*>(typeLambda->clone());
            newLambda->flags &= ~TYPEINFO_GENERIC;
            newLambda->returnType = newType;
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
                    newLambda = static_cast<TypeInfoFuncAttr*>(typeLambda->clone());
                    newLambda->flags &= ~TYPEINFO_GENERIC;
                }

                auto newParam      = static_cast<TypeInfoParam*>(newLambda->parameters[idx]);
                newParam->typeInfo = newType;
            }
        }

        if (newLambda)
        {
            newLambda->computeName();
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

bool Generic::instantiateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, InstantiateContext& instContext)
{
    auto node = context->node;
    SWAG_VERIFY(!match.genericReplaceTypes.empty(), context->report({node, node->token, format("cannot instantiate generic struct '%s', missing contextual types replacements", node->name.c_str())}));

    // Be sure all methods have been registered, because we need opDrop & co to be known, as we need
    // to instantiate them also (because those functions can be called by the compiler itself, not by the user)
    context->job->waitForAllStructMethods((TypeInfoStruct*) match.symbolOverload->typeInfo);
    if (context->result != ContextResult::Done)
        return true;

    // Types replacements
    CloneContext cloneContext;
    cloneContext.replaceTypes = move(match.genericReplaceTypes);

    // We are batching the struct
    if (instContext.fromBake)
    {
        cloneContext.forceFlags = AST_FROM_BAKE;
        if (instContext.bakeIsPublic)
            cloneContext.forceAttributeFlags = ATTRIBUTE_PUBLIC;
    }

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, structNode);

    if (instContext.fromBake)
    {
        SWAG_ASSERT(!instContext.bakeName.empty());
        structNode->bakeName    = instContext.bakeName;
        structNode->scope->name = instContext.bakeName;
    }

    // Make a new type
    auto oldType = static_cast<TypeInfoStruct*>(overload->typeInfo);
    auto newType = static_cast<TypeInfoStruct*>(oldType->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    newType->scope           = structNode->scope;
    newType->declNode        = structNode;
    newType->fromGeneric     = oldType;
    structNode->typeInfo     = newType;
    structNode->ownerGeneric = context->node;

    // Replace generic types and values in the struct generic parameters
    SWAG_CHECK(updateGenericParameters(context, newType->genericParameters, structNode->genericParameters->childs, genericParameters, match));

    if (instContext.fromBake)
    {
        newType->nakedName = instContext.bakeName;
        newType->name      = newType->nakedName;
    }
    else
    {
        newType->forceComputeName();
    }

    auto structJob = end(context, context->job, match.symbolName, structNode, true);

    cloneContext.replaceTypes[overload->typeInfo->name] = newType;

    // Not sure this shouldn't be done for all, not just for bake
    if (instContext.fromBake)
    {
        cloneContext.parentScope      = structNode->scope;
        cloneContext.ownerStructScope = structNode->scope;
    }

    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserDropFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostCopyFct);
    instantiateSpecialFunc(context, structJob, cloneContext, &newType->opUserPostMoveFct);

    // Force instantiation of all special functions
    for (auto method : newType->methods)
    {
        if (method->node->flags & AST_IS_SPECIAL_FUNC)
        {
            auto specFunc = CastAst<AstFuncDecl>(method->node, AstNodeKind::FuncDecl);
            if (specFunc != oldType->opUserDropFct &&
                specFunc != oldType->opUserPostCopyFct &&
                specFunc != oldType->opUserPostMoveFct &&
                !specFunc->genericParameters)
            {
                instantiateSpecialFunc(context, structJob, cloneContext, &specFunc);
            }
        }
        else if (instContext.fromBake)
        {
            auto specFunc = CastAst<AstFuncDecl>(method->node, AstNodeKind::FuncDecl);
            instantiateSpecialFunc(context, structJob, cloneContext, &specFunc);
        }
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
    newFunc->flags |= AST_FROM_GENERIC;
    newFunc->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(funcNode->parent, newFunc);
    *specialFct = newFunc;

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = static_cast<TypeInfoFuncAttr*>(newFunc->typeInfo);
    if (newTypeFunc->flags & TYPEINFO_GENERIC)
    {
        newTypeFunc = static_cast<TypeInfoFuncAttr*>(newFunc->typeInfo->clone());
        newTypeFunc->flags &= ~TYPEINFO_GENERIC;
        newFunc->typeInfo = newTypeFunc;
    }

    // Replace generic types and values in the function generic parameters
    newTypeFunc->computeName();

    scoped_lock lk(newFunc->resolvedSymbolName->mutex);
    auto newJob = end(context, context->job, newFunc->resolvedSymbolName, newFunc, false);
    structJob->dependentJobs.add(newJob);
}

bool Generic::instantiateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, InstantiateContext& instContext)
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
    if (match.genericReplaceTypes.empty())
    {
        if (contextualNode)
            return context->report({contextualNode, contextualNode->token, format("cannot instantiate generic function '%s', missing generic parameters", node->name.c_str())});
        else
            return context->report({node, node->token, format("cannot instantiate generic function '%s', missing contextual types replacements", node->name.c_str())});
    }

    // Types replacements
    CloneContext cloneContext;
    cloneContext.replaceTypes = move(match.genericReplaceTypes);

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
        newFunc->alternativeScopes.push_back(contextualStruct->scope);

    // Generate and initialize a new type if the type is still generic
    // The type is still generic if the doTypeSubstitution didn't find any type to change
    // (for example if we have just generic value)
    TypeInfoFuncAttr* newTypeFunc = static_cast<TypeInfoFuncAttr*>(newFunc->typeInfo);
    if (newTypeFunc->flags & TYPEINFO_GENERIC)
    {
        newTypeFunc = static_cast<TypeInfoFuncAttr*>(newFunc->typeInfo->clone());
        newTypeFunc->flags &= ~TYPEINFO_GENERIC;
        newTypeFunc->declNode = newFunc;
        newFunc->typeInfo     = newTypeFunc;
    }

    // Replace generic types and values in the function generic parameters
    SWAG_CHECK(updateGenericParameters(context, newTypeFunc->genericParameters, newFunc->genericParameters->childs, genericParameters, match));
    newTypeFunc->computeName();

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