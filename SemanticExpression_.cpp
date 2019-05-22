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

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node = static_cast<AstNode*>(context->node);
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    node->typeInfo  = node->token.literalType;
    context->result = SemanticResult::Done;
    return true;
}
