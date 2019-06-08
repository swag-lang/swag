#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto resolved   = node->resolvedSymbolOverload;

    switch (node->resolvedSymbolName->kind)
    {
    case SymbolKind::Namespace:
        return true;
    case SymbolKind::Variable:
    {
        if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
        {
            node->resultRegisterRC                                                            = sourceFile->module->reserveRegisterRC();
            emitInstruction(context, ByteCodeOp::RCxFromStack, node->resultRegisterRC)->b.s32 = resolved->stackOffset;
            return true;
        }
		break;
    }
    }

    return internalError(context, "emitIdentifier");
}
