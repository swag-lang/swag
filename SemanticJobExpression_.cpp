#include "pch.h"
#include "SemanticJob.h"
#include "Diagnostic.h"
#include "TypeInfo.h"
#include "TypeManager.h"
#include "Global.h"
#include "SourceFile.h"
#include "ByteCodeGenJob.h"

bool SemanticJob::resolveLiteral(SemanticContext* context)
{
    auto node         = context->node;
    node->byteCodeFct = &ByteCodeGenJob::emitLiteral;
    node->flags |= AST_CONST_EXPR | AST_VALUE_COMPUTED;
    node->typeInfo           = node->token.literalType;
    node->computedValue.reg  = node->token.literalValue;
    node->computedValue.text = node->token.text;
    return true;
}

bool SemanticJob::resolveMakePointer(SemanticContext* context)
{
    auto node       = context->node;
    auto child      = node->childs.front();
    auto typeInfo   = child->typeInfo;
    auto sourceFile = context->sourceFile;

    SWAG_VERIFY(child->kind == AstNodeKind::IdentifierRef, sourceFile->report({sourceFile, child, "invalid address expression"}));
    node->byteCodeFct = &ByteCodeGenJob::emitMakePointer;

    auto ptrType         = g_Pool_typeInfoPointer.alloc();
    ptrType->ptrCount    = 1;
    ptrType->pointedType = typeInfo;
    ptrType->sizeOf      = sizeof(void*);
    ptrType->name        = "*" + typeInfo->name;
    node->typeInfo       = g_TypeMgr.registerType(ptrType);

    return true;
}
