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

bool SemanticJob::execute()
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
                nodes.insert(nodes.end(), node->childs.begin(), node->childs.end());
                break;
            }

        case AstNodeSemanticState::ProcessingChilds:
            context.node = node;
            SWAG_CHECK(node->semanticFct(&context));
            if (context.result == SemanticResult::Pending)
                break;
            nodes.pop_back();
            break;
        }
    }

    return true;
}
