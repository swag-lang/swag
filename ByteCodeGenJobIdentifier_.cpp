#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "TypeInfo.h"
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
    auto node     = context->node;
    auto resolved = node->resolvedSymbolOverload;

    if (node->resolvedSymbolName->kind == SymbolKind::Namespace)
        return true;
    if (node->resolvedSymbolName->kind != SymbolKind::Variable)
        return internalError(context, "emitIdentifier");

    // Function parameter : it's a register on the stack
    if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (resolved->typeInfo->isNative(NativeType::String))
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::RCxFromStackParam64, node->resultRegisterRC[1]);
            inst->b.s32 = resolved->storageOffset + 8;
            inst->c.s32 = resolved->storageIndex + 1;
            inst        = emitInstruction(context, ByteCodeOp::RCxFromStackParam64, node->resultRegisterRC[0]);
            inst->b.s32 = resolved->storageOffset;
            inst->c.s32 = resolved->storageIndex;
        }
        else
        {
            auto inst   = emitInstruction(context, ByteCodeOp::RCxFromStackParam64, node->resultRegisterRC);
            inst->b.s32 = resolved->storageOffset;
            inst->c.s32 = resolved->storageIndex;
        }

        return true;
    }

    // Variable from the data segment
    if (resolved->flags & OVERLOAD_VAR_GLOBAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (node->flags & AST_LEFT_EXPRESSION)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::RCxRefFromDataSeg, node->resultRegisterRC);
            inst->b.s32 = resolved->storageOffset;
        }
        else if (resolved->typeInfo->isNative(NativeType::String))
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::RCxFromDataSeg64, node->resultRegisterRC[1]);
            inst->b.s32 = resolved->storageOffset + 8;
            inst        = emitInstruction(context, ByteCodeOp::RCxFromDataSeg64, node->resultRegisterRC[0]);
            inst->b.s32 = resolved->storageOffset;
        }
        else
        {
            ByteCodeInstruction* inst;
            switch (resolved->typeInfo->sizeOf)
            {
            case 1:
                inst = emitInstruction(context, ByteCodeOp::RCxFromDataSeg8, node->resultRegisterRC);
                break;
            case 2:
                inst = emitInstruction(context, ByteCodeOp::RCxFromDataSeg16, node->resultRegisterRC);
                break;
            case 4:
                inst = emitInstruction(context, ByteCodeOp::RCxFromDataSeg32, node->resultRegisterRC);
                break;
            case 8:
                inst = emitInstruction(context, ByteCodeOp::RCxFromDataSeg64, node->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitIdentifier, invalid global variable sizeof");
                break;
            }

            inst->b.s32 = resolved->storageOffset;
        }

        return true;
    }

    // Variable from the stack
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (resolved->typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(resolved->typeInfo, TypeInfoKind::Array);
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::RCxRefFromStack, node->resultRegisterRC[0])->b.s32 = resolved->storageOffset;
            emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, node->resultRegisterRC[1])->a.u32  = typeArray->size;
        }
        else if (node->flags & AST_LEFT_EXPRESSION)
        {
            emitInstruction(context, ByteCodeOp::RCxRefFromStack, node->resultRegisterRC)->b.s32 = resolved->storageOffset;
        }
        else if (resolved->typeInfo->isNative(NativeType::String))
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::RCxFromStack64, node->resultRegisterRC[1])->b.s32 = resolved->storageOffset + 8;
            emitInstruction(context, ByteCodeOp::RCxFromStack64, node->resultRegisterRC[0])->b.s32 = resolved->storageOffset;
        }
        else
        {
            ByteCodeInstruction* inst;
            switch (resolved->typeInfo->sizeOf)
            {
            case 1:
                inst = emitInstruction(context, ByteCodeOp::RCxFromStack8, node->resultRegisterRC);
                break;
            case 2:
                inst = emitInstruction(context, ByteCodeOp::RCxFromStack16, node->resultRegisterRC);
                break;
            case 4:
                inst = emitInstruction(context, ByteCodeOp::RCxFromStack32, node->resultRegisterRC);
                break;
            case 8:
                inst = emitInstruction(context, ByteCodeOp::RCxFromStack64, node->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitIdentifier, invalid local variable sizeof");
            }

            inst->b.s32 = resolved->storageOffset;
        }

        return true;
    }

    return internalError(context, "emitIdentifier");
}
