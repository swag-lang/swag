#include "pch.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Global.h"
#include "ThreadManager.h"
#include "FileSemanticJob.h"

Pool<FileSemanticJob> g_Pool_fileSemanticJob;

SemanticJob* FileSemanticJob::newSemanticJob(SourceFile* sourceFile)
{
    auto job        = g_Pool_semanticJob.alloc();
    job->sourceFile = sourceFile;
    return job;
}

JobResult FileSemanticJob::execute()
{
    context.job                     = this;
    context.sourceFile              = sourceFile;
    context.errorContext.sourceFile = sourceFile;
    context.result                  = SemanticResult::Done;

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:
            switch (node->kind)
            {
            case AstNodeKind::AttrUse:
            {
                auto job = newSemanticJob(sourceFile);
                nodes.pop_back();

                // Will deal with the next node too, as we want to stitch the attributes and the next declaration
                // if necessary
                if (!nodes.empty())
                {
                    auto nextNode = nodes.back();
                    job->nodes.push_back(nextNode);
                    nodes.pop_back();
                }

                job->nodes.push_back(node);
                g_ThreadMgr.addJob(job);
                continue;
            }
            break;

            case AstNodeKind::VarDecl:		
			case AstNodeKind::LetDecl:
			case AstNodeKind::ConstDecl:
            case AstNodeKind::TypeAlias:
            case AstNodeKind::EnumDecl:
            case AstNodeKind::StructDecl:
            case AstNodeKind::FuncDecl:
            case AstNodeKind::CompilerAssert:
            case AstNodeKind::CompilerPrint:
            case AstNodeKind::CompilerRun:
            case AstNodeKind::AttrDecl:
            case AstNodeKind::Impl:
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
