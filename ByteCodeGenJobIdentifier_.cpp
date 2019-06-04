#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node = context->node;
    switch (node->resolvedSymbolName->kind)
    {
    case SymbolKind::Namespace:
        return true;
    }

    return internalError(context, "emitIdentifier");
}
