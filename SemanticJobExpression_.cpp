#include "pch.h"
#include "SemanticJob.h"
#include "Global.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "TypeManager.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node = context->node;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    node->typeInfo           = node->token.literalType;
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    return true;
}

bool SemanticJob::resolveCast(SemanticContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto typeNode   = node->childs[0];
    auto exprNode   = node->childs[1];

    SWAG_CHECK(TypeManager::makeCompatibles(sourceFile, typeNode->typeInfo, exprNode));
    node->typeInfo = typeNode->typeInfo;
    node->byteCodeFct = &ByteCodeGenJob::emitCast;

    node->inheritAndFlag(exprNode, AST_CONST_EXPR);
    node->inheritComputedValue(exprNode);

    return true;
}
