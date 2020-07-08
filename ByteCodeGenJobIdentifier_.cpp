#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Module.h"

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitIdentifier(ByteCodeGenContext* context)
{
    auto node = context->node;
    if (!(node->flags & AST_L_VALUE) && !(node->flags & AST_R_VALUE))
        return true;

    auto identifier = CastAst<AstIdentifier>(node, AstNodeKind::Identifier);
    auto resolved   = node->resolvedSymbolOverload;
    auto typeInfo   = TypeManager::concreteReference(resolved->typeInfo);
    typeInfo        = TypeManager::concreteType(typeInfo);
    SWAG_ASSERT(typeInfo->kind != TypeInfoKind::Generic);

    // Will be done in the variable declaration
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (node->flags & AST_IN_TYPE_VAR_DECLARATION)
            return true;
    }

    // Reference to a constant
    if (resolved->flags & OVERLOAD_COMPUTED_VALUE)
    {
        if (typeInfo->kind == TypeInfoKind::Array)
        {
            auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
            reserveRegisterRC(context, node->resultRegisterRC, 2);
            node->parent->resultRegisterRC = node->resultRegisterRC;
            auto inst                      = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC[0], node->resultRegisterRC[1]);
            SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
            auto storageOffset = node->resolvedSymbolOverload->storageOffset;
            inst->c.u64        = ((uint64_t) storageOffset << 32) | (uint32_t) typeArray->count;
            return true;
        }

        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            node->resultRegisterRC         = reserveRegisterRC(context);
            node->parent->resultRegisterRC = node->resultRegisterRC;
            auto inst                      = emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, node->resultRegisterRC);
            SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
            inst->b.u32 = node->resolvedSymbolOverload->storageOffset;
            return true;
        }

        if (typeInfo->isNative(NativeTypeKind::String))
        {
            // Not yet stored in the constant segment
            unique_lock lk(node->resolvedSymbolName->mutex);
            if (node->resolvedSymbolOverload->storageOffset == UINT32_MAX)
                node->resolvedSymbolOverload->storageOffset = context->sourceFile->module->constantSegment.addString(node->resolvedSymbolOverload->computedValue.text);

            reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
            node->parent->resultRegisterRC = node->resultRegisterRC;
            auto inst                      = emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, node->resultRegisterRC);
            SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
            inst->b.u32 = node->resolvedSymbolOverload->storageOffset;
            emitInstruction(context, ByteCodeOp::CopyRAVB32, node->resultRegisterRC[1], (uint32_t) node->resolvedSymbolOverload->computedValue.text.length());
            return true;
        }

        return internalError(context, "emitIdentifier, invalid constant type");
    }

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
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
            auto inst   = emitInstruction(context, ByteCodeOp::RAFromStackParam64, node->resultRegisterRC[0]);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;
            inst        = emitInstruction(context, ByteCodeOp::RAFromStackParam64, node->resultRegisterRC[1]);
            inst->b.u32 = resolved->storageOffset + 8;
            inst->c.u32 = resolved->storageIndex + 1;
        }
        else
        {
            SWAG_ASSERT(typeInfo->numRegisters() == 1);
            auto inst   = emitInstruction(context, ByteCodeOp::RAFromStackParam64, node->resultRegisterRC);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;

            if (typeInfo->kind == TypeInfoKind::Interface || typeInfo->isPointerTo(TypeInfoKind::Interface))
            {
                if (node->flags & AST_FROM_UFCS)
                    emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u32 = sizeof(void*);
                else if (node->flags & AST_TO_UFCS)
                    emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
            }
        }

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
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
            if (resolved->flags & OVERLOAD_VAR_BSS)
                emitInstruction(context, ByteCodeOp::RARefFromBssSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
            else
                emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (node->flags & AST_TAKE_ADDRESS)
        {
            if (resolved->flags & OVERLOAD_VAR_BSS)
                emitInstruction(context, ByteCodeOp::RARefFromBssSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
            else
                emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            if (resolved->flags & OVERLOAD_VAR_BSS)
            {
                emitInstruction(context, ByteCodeOp::RAFromBssSeg64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
                emitInstruction(context, ByteCodeOp::RAFromBssSeg64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
            }
            else
            {
                emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
                emitInstruction(context, ByteCodeOp::RAFromDataSeg64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
            }

            transformResultToLinear2(context, node);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface || typeInfo->isPointerTo(TypeInfoKind::Interface))
        {
            emitInstruction(context, ByteCodeOp::RARefFromDataSeg, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
            if (typeInfo->kind == TypeInfoKind::Interface || typeInfo->isPointerTo(TypeInfoKind::Interface))
            {
                if (node->flags & AST_FROM_UFCS)
                    emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u32 = sizeof(void*);
                else if (node->flags & AST_TO_UFCS)
                    emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
            }
        }
        else
        {
            ByteCodeInstruction* inst;
            if (resolved->flags & OVERLOAD_VAR_BSS)
            {
                switch (typeInfo->sizeOf)
                {
                case 1:
                    inst = emitInstruction(context, ByteCodeOp::RAFromBssSeg8, node->resultRegisterRC);
                    break;
                case 2:
                    inst = emitInstruction(context, ByteCodeOp::RAFromBssSeg16, node->resultRegisterRC);
                    break;
                case 4:
                    inst = emitInstruction(context, ByteCodeOp::RAFromBssSeg32, node->resultRegisterRC);
                    break;
                case 8:
                    inst = emitInstruction(context, ByteCodeOp::RAFromBssSeg64, node->resultRegisterRC);
                    break;
                default:
                    return internalError(context, "emitIdentifier, invalid global variable sizeof");
                    break;
                }
            }
            else
            {
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
            }

            inst->b.u32 = resolved->storageOffset;
        }

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
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
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
            emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
            emitInstruction(context, ByteCodeOp::RAFromStack64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
        }
        else if (typeInfo->kind == TypeInfoKind::Interface)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
            if (node->flags & AST_FROM_UFCS)
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u32 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS)
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
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

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Reference inside a struct
    if (resolved->flags & OVERLOAD_VAR_STRUCT)
    {
        SWAG_ASSERT(!(resolved->flags & OVERLOAD_VAR_INLINE));
        node->resultRegisterRC = identifier->identifierRef->resultRegisterRC;
        SWAG_VERIFY(node->resultRegisterRC.size() > 0, internalError(context, format("emitIdentifier, cannot reference identifier '%s'", identifier->name.c_str()).c_str()));
        if (node->resolvedSymbolOverload->storageOffset > 0)
            emitInstruction(context, ByteCodeOp::IncPointerVB, node->resultRegisterRC)->b.u32 = node->resolvedSymbolOverload->storageOffset;
        if (!(node->flags & AST_TAKE_ADDRESS))
            emitStructDeRef(context);

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Reference to an inline parameter : the registers are directly stored in the overload symbol
    if (resolved->flags & OVERLOAD_VAR_INLINE)
    {
        // We need to copy register, and not use it directly, because the register can be changed by
        // some code after (like when dereferencing something)
        reserveRegisterRC(context, node->resultRegisterRC, resolved->registers.size());
        for (int i = 0; i < node->resultRegisterRC.size(); i++)
            emitInstruction(context, ByteCodeOp::CopyRARB, node->resultRegisterRC[i], resolved->registers[i]);

        SWAG_VERIFY(node->resultRegisterRC.size() > 0, internalError(context, format("emitIdentifier, cannot reference identifier '%s'", identifier->name.c_str()).c_str()));
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    return internalError(context, "emitIdentifier");
}
