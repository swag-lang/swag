#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGen.h"
#include "TypeInfo.h"
#include "SymTable.h"

bool ByteCodeGen::emitIdentifier(ByteCodeGenContext* context, AstNode* node)
{
    return internalError(context, node);
}
