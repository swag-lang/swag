#include "pch.h"
#include "SemanticJob.h"
#include "Ast.h"
#include "AstNode.h"
#include "Utf8.h"
#include "Global.h"
#include "TypeInfo.h"
#include "Diagnostic.h"
#include "SourceFile.h"
#include "Scope.h"
#include "TypeManager.h"

SymTable::SymTable()
{
    memset(mapNames, 0, sizeof(mapNames));
}

bool SemanticJob::error(SemanticContext* context, const Utf8& msg)
{
    context->sourceFile->report({context->sourceFile, context->node->token, msg});
    return false;
}

JobResult SemanticJob::execute()
{
    SemanticContext context;
    context.job        = this;
    context.sourceFile = sourceFile;

    while (!nodes.empty())
    {
        auto node = nodes.back();
        switch (node->semanticState)
        {
        case AstNodeSemanticState::Enter:
            node->semanticState = AstNodeSemanticState::ProcessingChilds;
            if (!node->childs.empty())
            {
                for (int i = (int) node->childs.size() - 1; i >= 0; i--)
                {
                    auto child        = node->childs[i];
                    child->attributes = node->attributes;
                    nodes.push_back(child);
                }

                break;
            }

        case AstNodeSemanticState::ProcessingChilds:
            if (node->semanticFct)
            {
                context.node = node;
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
