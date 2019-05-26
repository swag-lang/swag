#include "pch.h"
#include "Job.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "Global.h"
#include "PoolFactory.h"
#include "SourceFile.h"
#include "Diagnostic.h"

SemanticJob* ModuleSemanticJob::newSemanticJob(SourceFile* sourceFile, AstNode* node)
{
    auto job        = sourceFile->poolFactory->semanticJob.alloc();
    job->astRoot    = node;
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    job->nodes.push_back(node);
    return job;
}

bool ModuleSemanticJob::doSemanticNamespace(SourceFile* sourceFile, AstNode* node)
{
    // We need to check that the namespace name is not defined in the scope hierarchy, to avoid
    // symbol hidding
    auto scope = node->scope->parentScope;
    while (scope)
    {
        SWAG_CHECK(scope->symTable->checkHiddenSymbol(sourceFile, node->token, node->name, node->typeInfo, SymbolKind::Namespace));
        scope = scope->parentScope;
    }

    for (auto child : node->childs)
        doSemanticNode(sourceFile, child);

    return true;
}

bool ModuleSemanticJob::doSemanticNode(SourceFile* sourceFile, AstNode* node)
{
    switch (node->type)
    {
    case AstNodeType::File:
        for (auto child : node->childs)
            doSemanticNode(sourceFile, child);
        break;

    case AstNodeType::Namespace:
        SWAG_CHECK(doSemanticNamespace(sourceFile, node));
        break;

    case AstNodeType::VarDecl:
    case AstNodeType::TypeDecl:
    case AstNodeType::EnumDecl:
    case AstNodeType::CompilerAssert:
    case AstNodeType::CompilerPrint:
    case AstNodeType::CompilerRun:
    {
        auto job = newSemanticJob(sourceFile, node);
        g_ThreadMgr.addJob(job);
    }
    break;

    default:
        sourceFile->report({sourceFile, node->token, "not yet implemented ! (doSemanticNode)"});
        return false;
    }

    return true;
}

JobResult ModuleSemanticJob::execute()
{
    for (auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;
        doSemanticNode(file, file->astRoot);
    }

    return JobResult::ReleaseJob;
}
