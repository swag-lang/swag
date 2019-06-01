#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeNodeId.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SymTable.h"
#include "SourceFile.h"
#include "PoolFactory.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    return internalError(context);
}
