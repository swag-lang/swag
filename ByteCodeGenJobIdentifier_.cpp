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
        if (resolved->typeInfo->isNative(NativeType::String) || resolved->typeInfo->kind == TypeInfoKind::Slice)
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::RAFromStackParam64, node->resultRegisterRC[1]);
            inst->b.u32 = resolved->storageOffset + 8;
            inst->c.u32 = resolved->storageIndex + 1;
            inst        = emitInstruction(context, ByteCodeOp::RAFromStackParam64, node->resultRegisterRC[0]);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;
        }
        else
        {
            auto inst   = emitInstruction(context, ByteCodeOp::RAFromStackParam64, node->resultRegisterRC);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;
        }

        return true;
    }

    // Variable from the data segment
    if (resolved->flags & OVERLOAD_VAR_GLOBAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (resolved->typeInfo->kind == TypeInfoKind::Array)
        {
            emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (node->flags & AST_LEFT_EXPRESSION)
        {
            emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (resolved->typeInfo->isNative(NativeType::String) || resolved->typeInfo->kind == TypeInfoKind::Slice)
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
            emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
        }
        else
        {
            ByteCodeInstruction* inst;
            switch (resolved->typeInfo->sizeOf)
            {
            case 1:
                inst = emitInstruction(context, ByteCodeOp::RAFromDataSeg8, node->resultRegisterRC);
                break;
            case 2:
                inst = emitInstruction(context, ByteCodeOp::RAFromDataSeg16, node->resultRegisterRC);
                break;
            case 4:
                inst = emitInstruction(context, ByteCodeOp::RAFromDataSeg32, node->resultRegisterRC);
                break;
            case 8:
                inst = emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitIdentifier, invalid global variable sizeof");
                break;
            }

            inst->b.u32 = resolved->storageOffset;
        }

        return true;
    }

    // Variable from the stack
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (resolved->typeInfo->kind == TypeInfoKind::Array)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (node->flags & AST_LEFT_EXPRESSION)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (resolved->typeInfo->isNative(NativeType::String) || resolved->typeInfo->kind == TypeInfoKind::Slice)
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
            emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
        }
        else
        {
            ByteCodeInstruction* inst;
            switch (resolved->typeInfo->sizeOf)
            {
            case 1:
                inst = emitInstruction(context, ByteCodeOp::RAFromStack8, node->resultRegisterRC);
                break;
            case 2:
                inst = emitInstruction(context, ByteCodeOp::RAFromStack16, node->resultRegisterRC);
                break;
            case 4:
                inst = emitInstruction(context, ByteCodeOp::RAFromStack32, node->resultRegisterRC);
                break;
            case 8:
                inst = emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitIdentifier, invalid local variable sizeof");
            }

            inst->b.u32 = resolved->storageOffset;
        }

        return true;
    }

    return internalError(context, "emitIdentifier");
}
