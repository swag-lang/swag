#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "TypeManager.h"
#include "Module.h"

void Generic::computeTypeReplacements(CloneContext& cloneContext, OneGenericMatch& match)
{
    for (int i = 0; i < match.genericParametersCallTypes.size(); i++)
    {
        auto callType = match.genericParametersCallTypes[i];
        auto genType  = match.genericParametersGenTypes[i];
        if (callType != genType)
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

void Generic::end(SemanticContext* context, AstNode* newNode)
{
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  symbol           = dependentSymbols[0];

    // Need to wait for the struct to be semantic resolved
    symbol->cptOverloads++;
    job->waitForSymbol(symbol);

    // Run semantic on that struct
    auto sourceFile = context->sourceFile;
    job             = g_Pool_semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    job->nodes.push_back(newNode);

    // Store stack of instantiation contexts
    auto& srcCxt  = context->errorContext;
    auto& destCxt = job->context.errorContext;
    destCxt.genericInstanceTree.insert(destCxt.genericInstanceTree.begin(), srcCxt.genericInstanceTree.begin(), srcCxt.genericInstanceTree.end());
    destCxt.genericInstanceTreeFile.insert(destCxt.genericInstanceTreeFile.begin(), srcCxt.genericInstanceTreeFile.begin(), srcCxt.genericInstanceTreeFile.end());
    destCxt.genericInstanceTree.push_back(context->node);
    destCxt.genericInstanceTreeFile.push_back(context->sourceFile);

    g_ThreadMgr.addJob(job);
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

bool Generic::instanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    CloneContext cloneContext;

    // Types replacements
    computeTypeReplacements(cloneContext, match);

    // Clone original node
    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_DISABLED;
    Ast::addChild(sourceNode->parent, structNode);

    // Make a new type
    auto newType = static_cast<TypeInfoStruct*>(overload->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    newType->scope       = structNode->scope;
    structNode->typeInfo = newType;

    // Replace generic types and values in the struct generic parameters
    updateGenericParameters(newType->genericParameters, structNode->genericParameters->childs, genericParameters, match);

    // Clone opInit
    auto newOpInit     = CastAst<AstFuncDecl>(structNode->defaultOpInit->clone(cloneContext), AstNodeKind::FuncDecl);
    newType->opInitFct = newOpInit;
    newOpInit->flags |= AST_FROM_GENERIC | AST_DISABLED;
    Ast::addChild(structNode, newType->opInitFct);

    end(context, structNode);
    return true;
}

void Generic::doTypeSubstitution(SemanticContext* context, CloneContext& cloneContext, TypeInfo** typeInfo)
{
    auto oldType = *typeInfo;
    if (!oldType)
        return;

    auto      sourceFile = context->sourceFile;
    auto      module     = sourceFile->module;
    TypeInfo* newType    = nullptr;

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
            *typeInfo = module->typeTable.registerType(typeAlias);
        }

        break;
    }

    case TypeInfoKind::Pointer:
    {
        auto typePointer = CastTypeInfo<TypeInfoPointer>(oldType, TypeInfoKind::Pointer);
        newType          = typePointer->pointedType;
        doTypeSubstitution(context, cloneContext, &newType);
        if (newType != typePointer->pointedType)
        {
            typePointer              = static_cast<TypeInfoPointer*>(typePointer->clone());
            typePointer->pointedType = newType;
            typePointer->flags &= ~TYPEINFO_GENERIC;
            typePointer->computeName();
            *typeInfo = module->typeTable.registerType(typePointer);
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
            *typeInfo = module->typeTable.registerType(typeArray);
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
            *typeInfo = module->typeTable.registerType(typeSlice);
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
            *typeInfo = module->typeTable.registerType(newLambda);
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
    funcNode->content->flags &= ~AST_DISABLED;
    Ast::addChild(sourceNode->parent, funcNode);

    // Generate and initialize a new type
    auto newType = static_cast<TypeInfoFuncAttr*>(overload->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    funcNode->typeInfo = newType;

    // Replace generic types with their real values in the function parameters
    for (int i = 0; i < newType->parameters.size(); i++)
    {
        auto param = newType->parameters[i];
        doTypeSubstitution(context, cloneContext, &param->typeInfo);
    }

    // Replace generic types and values in the struct generic parameters
    updateGenericParameters(newType->genericParameters, funcNode->genericParameters->childs, genericParameters, match);

    end(context, funcNode);
    return true;
}