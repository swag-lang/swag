#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "SymTable.h"
#include "Global.h"

Pool<SemanticJob> g_Pool_semanticJob;

bool SemanticJob::internalError(SemanticContext* context, const char* msg)
{
    AstNode* node = context->node;
	context->errorContext.report({context->sourceFile, node->token, format("internal compiler error during semantic (%s)", msg)});
    return false;
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
	context->errorContext.report({context->sourceFile, context->node->token, msg});
    return false;
}

JobResult SemanticJob::execute()
{
    SemanticContext context;
    context.job                                  = this;
    context.sourceFile                           = sourceFile;
    context.errorContext.sourceFile              = sourceFile;
    context.errorContext.genericInstanceTree     = move(genericInstanceTree);
    context.errorContext.genericInstanceTreeFile = move(genericInstanceTreeFile);

    while (!nodes.empty())
    {
        auto node    = nodes.back();
        context.node = node;

        switch (node->semanticState)
        {
        case AstNodeResolveState::Enter:
            node->semanticState = AstNodeResolveState::ProcessingChilds;

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
        case AstNodeResolveState::SecondTry:
            if (node->semanticFct)
            {
                context.result = SemanticResult::Done;
                if (!node->semanticFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == SemanticResult::Pending)
                {
                    node->semanticState = AstNodeResolveState::SecondTry;
                    return JobResult::KeepJobAlive;
                }
            }

            node->semanticState = AstNodeResolveState::PostChilds;

        case AstNodeResolveState::PostChilds:
        case AstNodeResolveState::ThirdTry:
            if (node->semanticAfterFct)
            {
                if (!node->semanticAfterFct(&context))
                    return JobResult::ReleaseJob;
                if (context.result == SemanticResult::Pending)
                {
                    node->semanticState = AstNodeResolveState::ThirdTry;
                    return JobResult::KeepJobAlive;
                }
            }

            nodes.pop_back();
            break;
        }
    }

    return JobResult::ReleaseJob;
}
