#include "pch.h"
#include "Job.h"
#include "Module.h"
#include "ModuleSemanticJob.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "Global.h"
#include "PoolFactory.h"
#include "SourceFile.h"

SemanticJob* ModuleSemanticJob::newSemanticJob(SourceFile* file, AstNode* node)
{
    auto job        = file->poolFactory->semanticJob.alloc();
    job->astRoot    = node;
    job->module     = module;
    job->sourceFile = file;
    job->nodes.push_back(node);
    return job;
}

bool ModuleSemanticJob::doSemanticNode(SourceFile* file, AstNode* node)
{
    switch (node->type)
    {
    case AstNodeType::File:
    case AstNodeType::Namespace:
        for (auto child : node->childs)
            doSemanticNode(file, child);
        break;

    case AstNodeType::VarDecl:
    {
        auto job = newSemanticJob(file, node);
        g_ThreadMgr.addJob(job);
    }
    break;

    default:
        assert(false);
    }

    return true;
}

bool ModuleSemanticJob::execute()
{
    for (auto file : module->files)
    {
        if (file->buildPass < BuildPass::Semantic)
            continue;
        doSemanticNode(file, file->astRoot);
    }

    return true;
}
