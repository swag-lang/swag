#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node = context->node;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    node->typeInfo              = node->token.literalType;
    node->computedValue.reg = node->token.literalValue;
    node->computedValue.text    = node->token.text;
    context->result             = SemanticResult::Done;
    return true;
}
