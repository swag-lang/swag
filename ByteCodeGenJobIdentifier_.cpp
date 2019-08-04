#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "TypeInfo.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "SourceFile.h"
#include "Ast.h"

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    node->typeInfo         = node->childs.back()->typeInfo;
    return true;
}

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);

    // Direct index in a tuple
    if (node->flags & AST_IDENTIFIER_IS_INTEGER)
    {
        node->resultRegisterRC = identifier->identifierRef->resultRegisterRC;
        if (!g_CommandLine.optimizeByteCode || node->computedValue.reg.u32 > 0)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointerVB, node->resultRegisterRC);
            inst->b.u32 = node->computedValue.reg.u32;
        }

        emitStructDeRef(context);
        return true;
    }

    auto resolved     = node->resolvedSymbolOverload;
    auto resolvedName = node->resolvedSymbolName;
    if (resolvedName->kind == SymbolKind::Namespace || resolvedName->kind == SymbolKind::Struct)
        return true;
    if (node->resolvedSymbolName->kind != SymbolKind::Variable)
        return internalError(context, "emitIdentifier");

    auto typeInfo = TypeManager::concreteType(resolved->typeInfo);

    // Function parameter : it's a register on the stack
    if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if ((node->flags & AST_TAKE_ADDRESS) && typeInfo->kind != TypeInfoKind::Lambda)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::RARefFromStackParam, node->resultRegisterRC);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;
        }
        else if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
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

        identifier->identifierRef->typeInfo         = node->typeInfo;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        return true;
    }

    // Variable from the data segment
    if (resolved->flags & OVERLOAD_VAR_GLOBAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (typeInfo->kind == TypeInfoKind::Array ||
            typeInfo->kind == TypeInfoKind::TypeList ||
            typeInfo->kind == TypeInfoKind::Struct)
        {
            emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (node->flags & AST_TAKE_ADDRESS)
        {
            emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
            emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
        }
        else
        {
            ByteCodeInstruction* inst;
            switch (typeInfo->sizeOf)
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

        identifier->identifierRef->typeInfo         = node->typeInfo;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        return true;
    }

    // Variable from the stack
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if (typeInfo->kind == TypeInfoKind::Array ||
            typeInfo->kind == TypeInfoKind::TypeList ||
            typeInfo->kind == TypeInfoKind::Struct)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (node->flags & AST_TAKE_ADDRESS)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
            emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
        }
        else
        {
            ByteCodeInstruction* inst;
            switch (typeInfo->sizeOf)
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

        identifier->identifierRef->typeInfo         = node->typeInfo;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        return true;
    }

    // Reference to an array
    if (resolved->flags & OVERLOAD_COMPUTED_VALUE)
    {
        if (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            reserveRegisterRC(context, node->resultRegisterRC, 2);
            node->parent->resultRegisterRC      = node->resultRegisterRC;
            auto inst                           = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC[0], node->resultRegisterRC[1]);
            auto storageOffset                  = node->resolvedSymbolOverload->storageOffset;
            inst->c.u64                         = ((uint64_t) storageOffset << 32) | (uint32_t) typeArray->count;
            identifier->identifierRef->typeInfo = identifier->typeInfo;
            return true;
        }
    }

    // Direct index in a structure
    if (identifier->identifierRef->typeInfo->kind == TypeInfoKind::Struct)
    {
        node->resultRegisterRC = identifier->identifierRef->resultRegisterRC;
        if (!g_CommandLine.optimizeByteCode || node->resolvedSymbolOverload->storageOffset > 0)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointerVB, node->resultRegisterRC);
            inst->b.u32 = node->resolvedSymbolOverload->storageOffset;
        }

        if (!(node->flags & AST_TAKE_ADDRESS))
            emitStructDeRef(context);

        identifier->identifierRef->typeInfo = identifier->typeInfo;
        return true;
    }

    return internalError(context, "emitIdentifier");
}
