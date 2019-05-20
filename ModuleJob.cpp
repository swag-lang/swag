#include "pch.h"
#include "Job.h"
#include "Module.h"
#include "ModuleJob.h"
#include "Ast.h"
#include "ThreadManager.h"
#include "Global.h"
#include "PoolFactory.h"
#include "SourceFile.h"

bool ModuleJob::semanticNode(SourceFile* file, AstNode* node)
{
    switch (node->type)
    {
    case AstNodeType::RootFile:
        for (auto child : node->childs)
            semanticNode(file, child);
        break;

    case AstNodeType::VarDecl:
    {
        auto job = file->m_poolFactory->m_semanticJob.alloc();
        job->nodes.clear();
        job->nodes.push_back(node);
        g_ThreadMgr.addJob(job);
    }
    break;

    default:
        assert(false);
    }

    return true;
}

bool ModuleJob::execute()
{
    // One ast root to rule them all
    module->m_astRoot = Ast::newNode(&g_Pool.m_astNode, AstNodeType::RootModule);
    for (auto file : module->m_files)
    {
        if (file->m_buildPass < BuildPass::Semantic)
            continue;
        Ast::addChild(module->m_astRoot, file->m_astRoot, false);
        semanticNode(file, file->m_astRoot);
    }

    return true;
}
