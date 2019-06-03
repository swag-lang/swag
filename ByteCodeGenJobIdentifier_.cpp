#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node = context->node;
    switch (node->resolvedSymbolName->kind)
    {
    case SymbolKind::Namespace:
        return true;
    }

    return internalError(context);
}
