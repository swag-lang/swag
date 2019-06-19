#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
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

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node     = context->node;
    auto typeInfo = node->childs.front()->typeInfo;

    auto ptrType         = g_Pool_typeInfoPointer.alloc();
    ptrType->ptrCount    = 1;
    ptrType->pointedType = typeInfo;
    ptrType->sizeOf      = sizeof(void*);
    ptrType->name        = "*" + typeInfo->name;
    node->typeInfo       = g_TypeMgr.registerType(ptrType);

    return true;
}
