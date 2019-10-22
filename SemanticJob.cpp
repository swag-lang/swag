#include "pch.h"
#include "SemanticJob.h"
#include "SymTable.h"
#include "ThreadManager.h"

Pool<SemanticJob> g_Pool_semanticJob;

bool SemanticJob::internalError(SemanticContext* context, const char* msg, AstNode* node)
{
    if (!node)
        node = context->node;
    context->errorContext.report({context->sourceFile, node->token, format("internal compiler error during semantic (%s)", msg)});
    return false;
}

bool SemanticJob::checkTypeIsNative(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    SWAG_VERIFY(typeInfo->kind == TypeInfoKind::Native, notAllowed(context, node, typeInfo));
    return true;
}

bool SemanticJob::notAllowed(SemanticContext* context, AstNode* node, TypeInfo* typeInfo)
{
    return context->errorContext.report({node, format("operation not allowed on %s '%s'", TypeInfo::getNakedKindName(typeInfo), typeInfo->name.c_str())});
}

SemanticJob* SemanticJob::newJob(SourceFile* sourceFile, AstNode* rootNode, bool run)
{
    auto job        = g_Pool_semanticJob.alloc();
    job->sourceFile = sourceFile;
    job->nodes.push_back(rootNode);
    if (run)
        g_ThreadMgr.addJob(job);
    return job;
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->errorContext.report({context->sourceFile, context->node->token, msg});
    return false;
}

void SemanticJob::waitForSymbolNoLock(SymbolName* symbol)
{
    waitingSymbolSolved = symbol;
    setPending();
    symbol->dependentJobs.add(this);
    g_ThreadMgr.addPendingJob(this);
}

void SemanticJob::setPending()
{
    context.node->semanticPass++;
    context.result = SemanticResult::Pending;
}

JobResult SemanticJob::execute()
{
    scoped_lock lkExecute(executeMutex);

#ifdef SWAG_HAS_ASSERT
    g_diagnosticInfos.pass       = "SemanticJob";
    g_diagnosticInfos.sourceFile = sourceFile;
#endif

    context.job                     = this;
    context.sourceFile              = sourceFile;
    context.errorContext.sourceFile = sourceFile;
    context.result                  = SemanticResult::Done;

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;
#ifdef SWAG_HAS_ASSERT
        g_diagnosticInfos.node = node;
#endif

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:
            node->semanticState = AstNodeResolveState::ProcessingChilds;
            context.result      = SemanticResult::Done;

            if (node->semanticBeforeFct && !node->semanticBeforeFct(&context))
                return JobResult::ReleaseJob;

            if (!node->childs.empty() && !node->isDisabled())
            {
                for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                {
                    auto child = node->childs[i];
                    if (child->isDisabled())
                        continue;

                    // Top to bottom inheritance
                    if (node->kind == AstNodeKind::Statement)
                        child->parentAttributes = node->parentAttributes;

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
                if (context.result == SemanticResult::NewChilds)
                    continue;
            }

            node->semanticState = AstNodeResolveState::PostChilds;

        case AstNodeResolveState::PostChilds:
            if (node->semanticAfterFct)
            {
                if (!node->semanticAfterFct(&context))
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
