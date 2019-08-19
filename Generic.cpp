#include "pch.h"
#include "SemanticJob.h"
#include "Generic.h"
#include "Ast.h"
#include "SymTable.h"
#include "Scope.h"
#include "SourceFile.h"
#include "ThreadManager.h"

bool Generic::InstanciateFunction(SemanticContext* context, AstNode* genericParameters, OneGenericMatch& match)
{
    auto symbol     = match.symbolOverload;
    auto sourceNode = symbol->node;
    auto funcNode   = CastAst<AstFuncDecl>(sourceNode->clone(), AstNodeKind::FuncDecl);
    funcNode->flags &= ~AST_IS_GENERIC;
	funcNode->flags |= AST_FROM_GENERIC;
    funcNode->genericParameters = nullptr;

    Ast::addChild(sourceNode->parent, funcNode);

    auto newType = static_cast<TypeInfoFuncAttr*>(symbol->typeInfo->clone());
    newType->flags &= ~TYPEINFO_GENERIC;
    funcNode->typeInfo = newType;

    for (int i = 0; i < newType->genericParameters.size(); i++)
    {
        auto param = newType->genericParameters[i];
        if (param->typeInfo)
            param->typeInfo->release();
        param->typeInfo     = genericParameters->childs[i]->typeInfo;
        param->genericValue = genericParameters->childs[i]->computedValue;
    }

    // Need to wait for the function to be semantic resolved
    auto  job              = context->job;
    auto& dependentSymbols = job->cacheDependentSymbols;
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