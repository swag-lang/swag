#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "TypeManager.h"

bool Generic::InstanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;
    auto  symbol           = dependentSymbols[0];

    CloneContext cloneContext;

    // Types replacements
    for (int i = 0; i < match.genericParametersCallTypes.size(); i++)
    {
        auto callType = match.genericParametersCallTypes[i];
        auto genType  = match.genericParametersGenTypes[i];
        if (callType != genType)
            cloneContext.replaceTypes[genType] = callType;
    }

    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;

    AstStruct*      structNode = nullptr;
    TypeInfoStruct* typeStruct = nullptr;
    if (overload->typeInfo->kind == TypeInfoKind::Struct)
    {
        structNode = CastAst<AstStruct>(sourceNode, AstNodeKind::StructDecl);
        typeStruct = CastTypeInfo<TypeInfoStruct>(overload->typeInfo, TypeInfoKind::Struct);
    }
    else
    {
        SWAG_ASSERT(false);
    }

    structNode = CastAst<AstStruct>(structNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags &= ~AST_IS_GENERIC;
    structNode->flags |= AST_FROM_GENERIC;
    structNode->content->flags &= ~AST_DISABLED;
    Ast::addChild(sourceNode->parent, structNode);

    auto newType = static_cast<TypeInfoStruct*>(typeStruct->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    newType->scope       = structNode->scope;
    structNode->typeInfo = newType;

    // Replace generic types and values in the struct generic parameters
    for (int i = 0; i < newType->genericParameters.size(); i++)
    {
        auto param      = newType->genericParameters[i];
        param->typeInfo = match.genericParametersCallTypes[i];
        if (genericParameters)
        {
            param->typeInfo     = genericParameters->childs[i]->typeInfo;
            param->genericValue = genericParameters->childs[i]->computedValue;
        }

        auto nodeParam           = structNode->genericParameters->childs[i];
        nodeParam->kind          = AstNodeKind::ConstDecl;
        nodeParam->computedValue = param->genericValue;
        nodeParam->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }

    // Need to wait for the struct to be semantic resolved
    symbol->cptOverloads++;
    job->waitForSymbol(symbol);

    // Clone opInit
    auto newOpInit     = CastAst<AstFuncDecl>(structNode->defaultOpInit->clone(cloneContext), AstNodeKind::FuncDecl);
    newType->opInitFct = newOpInit;
    newOpInit->flags |= AST_FROM_GENERIC | AST_DISABLED;
    Ast::addChild(structNode, newType->opInitFct);

    // Run semantic on that struct
    auto sourceFile = context->sourceFile;
    job             = g_Pool_semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    job->nodes.push_back(structNode);

    // Store stack of instantiation contexts
    auto& srcCxt  = context->errorContext;
    auto& destCxt = job->context.errorContext;
    destCxt.genericInstanceTree.insert(destCxt.genericInstanceTree.begin(), srcCxt.genericInstanceTree.begin(), srcCxt.genericInstanceTree.end());
    destCxt.genericInstanceTreeFile.insert(destCxt.genericInstanceTreeFile.begin(), srcCxt.genericInstanceTreeFile.begin(), srcCxt.genericInstanceTreeFile.end());
    destCxt.genericInstanceTree.push_back(context->node);
    destCxt.genericInstanceTreeFile.push_back(context->sourceFile);

    g_ThreadMgr.addJob(job);

    return true;
}

bool Generic::InstanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;

    CloneContext cloneContext;

    // Types replacements
    for (int i = 0; i < match.genericParametersCallTypes.size(); i++)
    {
        auto callType = match.genericParametersCallTypes[i];
        auto genType  = match.genericParametersGenTypes[i];
        if (callType != genType)
            cloneContext.replaceTypes[genType] = callType;

        // For a struct, each generic parameter must be swapped too
        if (callType->kind == TypeInfoKind::Struct)
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

    auto overload   = match.symbolOverload;
    auto sourceNode = overload->node;
    auto funcNode   = CastAst<AstFuncDecl>(sourceNode->clone(cloneContext), AstNodeKind::FuncDecl);
    funcNode->flags |= AST_FROM_GENERIC;
    funcNode->content->flags &= ~AST_DISABLED;
    Ast::addChild(sourceNode->parent, funcNode);

    auto newType = static_cast<TypeInfoFuncAttr*>(overload->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    funcNode->typeInfo = newType;

    // Replace generic types with their real values in the function parameters
    for (int i = 0; i < newType->parameters.size(); i++)
    {
        auto param = newType->parameters[i];
        auto it    = cloneContext.replaceTypes.find(param->typeInfo);
        if (it != cloneContext.replaceTypes.end())
            param->typeInfo = it->second;
    }

    // Replace generic types and values in the function generic parameters
    for (int i = 0; i < newType->genericParameters.size(); i++)
    {
        auto param      = newType->genericParameters[i];
        param->typeInfo = match.genericParametersCallTypes[i];

        if (genericParameters)
        {
            param->typeInfo     = genericParameters->childs[i]->typeInfo;
            param->genericValue = genericParameters->childs[i]->computedValue;
        }

        auto nodeParam           = funcNode->genericParameters->childs[i];
        nodeParam->kind          = AstNodeKind::ConstDecl;
        nodeParam->computedValue = param->genericValue;
        nodeParam->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    }

    // Need to wait for the function to be semantic resolved
    auto symbol = dependentSymbols[0];
    symbol->cptOverloads++;
    job->waitForSymbol(symbol);

    // Run semantic on that function
    auto sourceFile = context->sourceFile;
    job             = g_Pool_semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    job->nodes.push_back(funcNode);

    // Store stack of instantiation contexts
    auto& srcCxt  = context->errorContext;
    auto& destCxt = job->context.errorContext;
    destCxt.genericInstanceTree.insert(destCxt.genericInstanceTree.begin(), srcCxt.genericInstanceTree.begin(), srcCxt.genericInstanceTree.end());
    destCxt.genericInstanceTreeFile.insert(destCxt.genericInstanceTreeFile.begin(), srcCxt.genericInstanceTreeFile.begin(), srcCxt.genericInstanceTreeFile.end());
    destCxt.genericInstanceTree.push_back(context->node);
    destCxt.genericInstanceTreeFile.push_back(context->sourceFile);

    g_ThreadMgr.addJob(job);

    return true;
}