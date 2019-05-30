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

SemanticJob* ModuleSemanticJob::newSemanticJob(SourceFile* sourceFile)
{
    auto job        = sourceFile->poolFactory->semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
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

    return true;
}

bool ModuleSemanticJob::doSemanticNode(SourceFile* sourceFile, AstNode* node)
{
    vector<AstNode*> nodes;
    for (int i = (int) node->childs.size() - 1; i >= 0; i--)
        nodes.push_back(node->childs[i]);

    while (!nodes.empty())
    {
        node = nodes.back();
        nodes.pop_back();

        switch (node->kind)
        {
        case AstNodeKind::Namespace:
            SWAG_CHECK(doSemanticNamespace(sourceFile, node));
            for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                nodes.push_back(node->childs[i]);
            break;

        case AstNodeKind::AttrUse:
        {
            auto job = newSemanticJob(sourceFile);

            // Will deal with the next node too, as we want to stitch the attributes and the next declaration
			// if necessary
            if (!nodes.empty())
            {
                job->nodes.push_back(nodes.back());
                nodes.pop_back();
            }

			job->nodes.push_back(node);
            g_ThreadMgr.addJob(job);
        }
        break;

        case AstNodeKind::VarDecl:
        case AstNodeKind::TypeDecl:
        case AstNodeKind::EnumDecl:
        case AstNodeKind::FuncDecl:
        case AstNodeKind::CompilerAssert:
        case AstNodeKind::CompilerPrint:
        case AstNodeKind::CompilerRun:
        case AstNodeKind::AttrDecl:
        {
            auto job = newSemanticJob(sourceFile);
            job->nodes.push_back(node);
            g_ThreadMgr.addJob(job);
        }
        break;

        default:
            sourceFile->report({sourceFile, node->token, "not yet implemented ! (doSemanticNode)"});
            return false;
        }
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
