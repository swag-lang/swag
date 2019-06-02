#include "pch.h"
#include "FileSemanticJob.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "Global.h"
#include "ThreadManager.h"

SemanticJob* FileSemanticJob::newSemanticJob(SourceFile* sourceFile)
{
    auto job        = sourceFile->poolFactory->semanticJob.alloc();
    job->module     = sourceFile->module;
    job->sourceFile = sourceFile;
    return job;
}

JobResult FileSemanticJob::execute()
{
    SemanticContext context;
    context.job        = this;
    context.sourceFile = sourceFile;

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:
            switch (node->kind)
            {
            case AstNodeKind::Namespace:
                if (!resolveNamespace(&context))
                    return JobResult::ReleaseJob;
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
                nodes.pop_back();
                continue;
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
                nodes.pop_back();
                continue;
            }
            break;
            }

            node->semanticState = AstNodeResolveState::ProcessingChilds;
            if (!node->childs.empty())
            {
                for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                {
                    auto child = node->childs[i];
                    nodes.push_back(child);
                }

                break;
            }

        case AstNodeResolveState::ProcessingChilds:
            if (node->semanticFct)
            {
                if (!node->semanticFct(&context))
                    return JobResult::ReleaseJob;

                if (context.result == SemanticResult::Pending)
                    return JobResult::KeepJobAlive;
            }

            nodes.pop_back();
            break;
        }
    }

    return JobResult::ReleaseJob;
}
