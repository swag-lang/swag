#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "SourceFile.h"
#include "ThreadManager.h"

bool Generic::InstanciateStruct(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto        job              = context->job;
    auto&       dependentSymbols = job->cacheDependentSymbols;
    scoped_lock lock(dependentSymbols[0]->mutex);

    CloneContext cloneContext;

    // Types replacements
    for (int i = 0; i < match.genericParametersCallTypes.size(); i++)
    {
        auto callType = match.genericParametersCallTypes[i];
        auto genType  = match.genericParametersGenTypes[i];
        if (callType != genType)
            cloneContext.replaceTypes[genType] = callType;
    }

    auto symbol     = match.symbolOverload;
    auto sourceNode = symbol->node;
    auto structNode = CastAst<AstStruct>(sourceNode->clone(cloneContext), AstNodeKind::StructDecl);
    structNode->flags &= ~AST_IS_GENERIC;
    structNode->flags |= AST_FROM_GENERIC;
    Ast::addChild(sourceNode->parent, structNode);

    auto newType = static_cast<TypeInfoStruct*>(symbol->typeInfo->clone());
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

    // Replace generic types in the struct childs
    for (int i = 0; i < newType->childs.size(); i++)
    {
        auto child = newType->childs[i];
        auto it    = cloneContext.replaceTypes.find(child);
        if (it != cloneContext.replaceTypes.end())
            newType->childs[i] = it->second;
    }

    // Need to wait for the struct to be semantic resolved
    dependentSymbols[0]->cptOverloads++;
    dependentSymbols[0]->dependentJobs.push_back(context->job);
    g_ThreadMgr.addPendingJob(context->job);
    context->result = SemanticResult::Pending;

    // Run semantic on that struct
    auto sourceFile = context->sourceFile;
    job             = g_Pool_semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    job->nodes.push_back(structNode);
    g_ThreadMgr.addJob(job);

    return true;
}

bool Generic::InstanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto        job              = context->job;
    auto&       dependentSymbols = job->cacheDependentSymbols;
    scoped_lock lock(dependentSymbols[0]->mutex);

    CloneContext cloneContext;

    // Types replacements
    for (int i = 0; i < match.genericParametersCallTypes.size(); i++)
    {
        auto callType = match.genericParametersCallTypes[i];
        auto genType  = match.genericParametersGenTypes[i];
        if (callType != genType)
            cloneContext.replaceTypes[genType] = callType;
    }

    auto symbol     = match.symbolOverload;
    auto sourceNode = symbol->node;
    auto funcNode   = CastAst<AstFuncDecl>(sourceNode->clone(cloneContext), AstNodeKind::FuncDecl);
    funcNode->flags |= AST_FROM_GENERIC;
    funcNode->content->flags &= ~AST_DISABLED;
    Ast::addChild(sourceNode->parent, funcNode);

    auto newType = static_cast<TypeInfoFuncAttr*>(symbol->typeInfo->clone());
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
    dependentSymbols[0]->cptOverloads++;
    dependentSymbols[0]->dependentJobs.push_back(context->job);
    g_ThreadMgr.addPendingJob(context->job);
    context->result = SemanticResult::Pending;

    // Run semantic on that function
    auto sourceFile = context->sourceFile;
    job             = g_Pool_semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    job->nodes.push_back(funcNode);
    g_ThreadMgr.addJob(job);

    return true;
}