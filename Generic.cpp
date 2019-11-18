#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "ThreadManager.h"

void Generic::computeTypeReplacements(CloneContext& cloneContext, OneGenericMatch& match)
{
    auto numGens = match.genericParametersCallTypes.size();
    for (int i = 0; i < numGens; i++)
    {
        auto callType = match.genericParametersCallTypes[i];
        auto genType  = match.genericParametersGenTypes[i];

        // Cast from struct to pointer
        if (callType->kind == TypeInfoKind::Struct && genType->kind == TypeInfoKind::Pointer)
        {
            auto genTypePointer                                  = CastTypeInfo<TypeInfoPointer>(genType, TypeInfoKind::Pointer);
            cloneContext.replaceTypes[genTypePointer->finalType] = callType;
            genType                                              = genTypePointer->finalType;
        }

        // Else replace 1 to 1
        else if (callType != genType)
            cloneContext.replaceTypes[genType] = callType;

        // For a struct, each generic parameter must be swapped too
        if (callType->kind == TypeInfoKind::Struct && genType->kind == TypeInfoKind::Struct)
        {
            auto callTypeStruct = CastTypeInfo<TypeInfoStruct>(callType, TypeInfoKind::Struct);
            auto genTypeStruct  = CastTypeInfo<TypeInfoStruct>(genType, TypeInfoKind::Struct);
            for (int j = 0; j < callTypeStruct->genericParameters.size(); j++)
            {
                auto genTypeParam  = CastTypeInfo<TypeInfoParam>(genTypeStruct->genericParameters[j], TypeInfoKind::Param);
                auto callTypeParam = CastTypeInfo<TypeInfoParam>(callTypeStruct->genericParameters[j], TypeInfoKind::Param);

                cloneContext.replaceTypes[genTypeParam->typeInfo] = callTypeParam->typeInfo;
            }
        }
    }
}

void Generic::end(SemanticContext* context, AstNode* newNode, bool waitSymbol)
{
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  symbol           = dependentSymbols[0];

    // Need to wait for the struct to be semantic resolved
    symbol->cptOverloads++;
	symbol->cptOverloadsInit++;
    if (waitSymbol)
        job->waitForSymbolNoLock(symbol);

    // Run semantic on that struct
    auto sourceFile = context->sourceFile;
    auto newJob     = SemanticJob::newJob(sourceFile, newNode, false);

    // Store stack of instantiation contexts
    auto srcCxt  = context;
    auto destCxt = &newJob->context;
    destCxt->expansionNode.insert(destCxt->expansionNode.begin(), srcCxt->expansionNode.begin(), srcCxt->expansionNode.end());
    destCxt->expansionNode.push_back(context->node);

    g_ThreadMgr.addJob(newJob);
}

void Generic::updateGenericParameters(vector<TypeInfoParam*>& typeGenericParameters, vector<AstNode*>& nodeGenericParameters, AstNode* callGenericParameters, OneGenericMatch& match)
{
    for (int i = 0; i < typeGenericParameters.size(); i++)
    {
        auto param = typeGenericParameters[i];

        if (callGenericParameters)
        {
            param->typeInfo = callGenericParameters->childs[i]->typeInfo;
            param->value    = callGenericParameters->childs[i]->computedValue;
        }
        else
        {
            param->typeInfo = match.genericParametersCallTypes[i];
        }

        auto nodeParam           = nodeGenericParameters[i];
        nodeParam->kind          = AstNodeKind::ConstDecl;
        nodeParam->computedValue = param->value;
        nodeParam->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }
}

bool Generic::instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match, bool waitSymbol)
{
    CloneContext cloneContext;

    // Types replacements
    computeTypeReplacements(cloneContext, match);

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, structNode);

    // Make a new type
    auto newType = static_cast<TypeInfoStruct*>(overload->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    newType->scope       = structNode->scope;
    structNode->typeInfo = newType;

    // Replace generic types and values in the struct generic parameters
    updateGenericParameters(newType->genericParameters, structNode->genericParameters->childs, genericParameters, match);

    end(context, structNode, waitSymbol);
    return true;
}

void Generic::doTypeSubstitution(SemanticContext* context, CloneContext& cloneContext, TypeInfo** typeInfo)
{
    auto oldType = *typeInfo;
    if (!oldType)
        return;

    TypeInfo* newType = nullptr;

    auto it = cloneContext.replaceTypes.find(oldType);
    if (it != cloneContext.replaceTypes.end())
    {
        *typeInfo = it->second;
        return;
    }

    // When type is a compound, we do substitution in the raw type
    switch (oldType->kind)
    {
    case TypeInfoKind::Alias:
    {
        auto typeAlias = CastTypeInfo<TypeInfoAlias>(oldType, TypeInfoKind::Alias);
        newType        = typeAlias->rawType;
        doTypeSubstitution(context, cloneContext, &newType);
        if (newType != typeAlias->rawType)
        {
            typeAlias          = static_cast<TypeInfoAlias*>(typeAlias->clone());
            typeAlias->rawType = newType;
            typeAlias->flags &= ~TYPEINFO_GENERIC;
            typeAlias->computeName();
            *typeInfo = typeAlias;
        }

        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(oldType, TypeInfoKind::Pointer);
        newType          = typePointer->finalType;
        doTypeSubstitution(context, cloneContext, &newType);
        if (newType != typePointer->finalType)
        {
            typePointer            = static_cast<TypeInfoPointer*>(typePointer->clone());
            typePointer->finalType = newType;
            typePointer->flags &= ~TYPEINFO_GENERIC;
            typePointer->computeName();
            typePointer->pointedType = typePointer->computePointedType();
            *typeInfo                = typePointer;
        }

        break;
    }

    case TypeInfoKind::Array:
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(oldType, TypeInfoKind::Array);
        newType        = typeArray->pointedType;
        doTypeSubstitution(context, cloneContext, &newType);
        if (newType != typeArray->pointedType)
        {
            typeArray              = static_cast<TypeInfoArray*>(typeArray->clone());
            typeArray->pointedType = newType;
            typeArray->flags &= ~TYPEINFO_GENERIC;
            typeArray->computeName();
            *typeInfo = typeArray;
        }

        break;
    }

    case TypeInfoKind::Slice:
    {
        auto typeSlice = CastTypeInfo<TypeInfoSlice>(oldType, TypeInfoKind::Slice);
        newType        = typeSlice->pointedType;
        doTypeSubstitution(context, cloneContext, &newType);
        if (newType != typeSlice->pointedType)
        {
            typeSlice              = static_cast<TypeInfoSlice*>(typeSlice->clone());
            typeSlice->pointedType = newType;
            typeSlice->flags &= ~TYPEINFO_GENERIC;
            typeSlice->computeName();
            *typeInfo = typeSlice;
        }

        break;
    }

    case TypeInfoKind::Lambda:
    {
        TypeInfoFuncAttr* newLambda  = nullptr;
        auto              typeLambda = CastTypeInfo<TypeInfoFuncAttr>(oldType, TypeInfoKind::Lambda);

        newType = typeLambda->returnType;
        doTypeSubstitution(context, cloneContext, &newType);
        if (newType != typeLambda->returnType)
        {
            newLambda = static_cast<TypeInfoFuncAttr*>(typeLambda->clone());
            newLambda->flags &= ~TYPEINFO_GENERIC;
            newLambda->returnType = newType;
        }

        for (int idx = 0; idx < typeLambda->parameters.size(); idx++)
        {
            auto param = CastTypeInfo<TypeInfoParam>(typeLambda->parameters[idx], TypeInfoKind::Param);
            newType    = param->typeInfo;
            doTypeSubstitution(context, cloneContext, &newType);
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
            *typeInfo = newLambda;
        }

        break;
    }
    }
}

bool Generic::instanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    CloneContext cloneContext;

    // Types replacements
    computeTypeReplacements(cloneContext, match);

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto funcNode   = CastAst<AstFuncDecl>(sourceNode->clone(cloneContext), AstNodeKind::FuncDecl);
    funcNode->flags |= AST_FROM_GENERIC;
    funcNode->content->flags &= ~AST_NO_SEMANTIC;
    Ast::addChildBack(sourceNode->parent, funcNode);

    // Generate and initialize a new type
    auto newType = static_cast<TypeInfoFuncAttr*>(overload->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    funcNode->typeInfo = newType;

    // Replace return type
    doTypeSubstitution(context, cloneContext, &newType->returnType);

    // Replace generic types with their real values in the function parameters
    auto numParams = newType->parameters.size();
    for (int i = 0; i < numParams; i++)
    {
        auto param = newType->parameters[i];
        doTypeSubstitution(context, cloneContext, &param->typeInfo);
    }

    // Replace generic types and values in the struct generic parameters
    updateGenericParameters(newType->genericParameters, funcNode->genericParameters->childs, genericParameters, match);

    end(context, funcNode, true);
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
            auto nodeStruct = CastAst<AstStruct>(typeStruct->structNode, AstNodeKind::StructDecl);
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