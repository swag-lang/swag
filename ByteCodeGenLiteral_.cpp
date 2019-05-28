#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGen.h"

bool ByteCodeGen::emitLiteral(ByteCodeGenContext* context, AstNode* node)
{
    auto  typeInfo = node->typeInfo;
    auto& out      = context->bc->out;

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeType::S32:
            addInstruction(context, ByteCodeNodeId::PushS32);
			out.addS32(node->computedValue.variant.s32);
            return true;
        }
    }

    return internalError(context, node);
}
