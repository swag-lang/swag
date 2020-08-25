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

            SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
            emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, node->resultRegisterRC[0])->b.u32 = node->resolvedSymbolOverload->storageOffset;
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, node->resultRegisterRC[1])->b.u32           = typeArray->count;
            return true;
        }

        if (typeInfo->kind == TypeInfoKind::Struct)
        {
            node->resultRegisterRC         = reserveRegisterRC(context);
            node->parent->resultRegisterRC = node->resultRegisterRC;
            auto inst                      = emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, node->resultRegisterRC);
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
            auto inst                      = emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, node->resultRegisterRC);
            SWAG_ASSERT(node->resolvedSymbolOverload->storageOffset != UINT32_MAX);
            inst->b.u32 = node->resolvedSymbolOverload->storageOffset;
            emitInstruction(context, ByteCodeOp::CopyVBtoRA32, node->resultRegisterRC[1], (uint32_t) node->resolvedSymbolOverload->computedValue.text.length());
            return true;
        }

        return internalError(context, "emitIdentifier, invalid constant type");
    }

    // Function parameter : it's a register on the stack
    if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->resultRegisterRC = reserveRegisterRC(context);
        if ((node->flags & AST_TAKE_ADDRESS) && typeInfo->kind != TypeInfoKind::Lambda && typeInfo->kind != TypeInfoKind::Array)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointerParam, node->resultRegisterRC);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC[0]);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = resolved->storageIndex;
            inst        = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC[1]);
            inst->b.u32 = resolved->storageOffset + 8;
            inst->c.u32 = resolved->storageIndex + 1;
        }
        else
        {
            SWAG_ASSERT(typeInfo->numRegisters() == 1);
            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC);
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
            typeInfo->kind == TypeInfoKind::TypeListTuple ||
            typeInfo->kind == TypeInfoKind::TypeListArray ||
            typeInfo->kind == TypeInfoKind::Struct)
        {
            ByteCodeInstruction* inst;
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::MakeBssSegPointer, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, node->resultRegisterRC);
            inst->b.u32 = resolved->storageOffset;
            if (node->flags & AST_TAKE_ADDRESS)
                inst->c.pointer = (uint8_t*) resolved;
        }
        else if ((node->flags & AST_TAKE_ADDRESS) && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            ByteCodeInstruction* inst;
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::MakeBssSegPointer, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, node->resultRegisterRC);
            inst->b.u32     = resolved->storageOffset;
            inst->c.pointer = (uint8_t*) resolved;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
            if (resolved->flags & OVERLOAD_VAR_BSS)
            {
                emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
                emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
            }
            else
            {
                emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
                emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Interface || typeInfo->isPointerTo(TypeInfoKind::Interface))
        {
            ByteCodeInstruction* inst;
            inst        = emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, node->resultRegisterRC);
            inst->b.u32 = resolved->storageOffset;
            inst->c.u32 = 0; // This is a read, so do not store size

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
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC);
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

        if (resolved->typeInfo->kind == TypeInfoKind::Reference)
        {
            if (node->flags & AST_TAKE_ADDRESS)
                emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
            else
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
                inst->b.u32 = resolved->storageOffset;
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Array ||
                 typeInfo->kind == TypeInfoKind::TypeListTuple ||
                 typeInfo->kind == TypeInfoKind::TypeListArray ||
                 typeInfo->kind == TypeInfoKind::Struct)
        {
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if ((node->flags & AST_TAKE_ADDRESS) && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC(context, node->resultRegisterRC, 2);
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[0])->b.u32 = resolved->storageOffset;
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[1])->b.u32 = resolved->storageOffset + 8;
        }
        else if (typeInfo->kind == TypeInfoKind::Interface)
        {
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u32 = resolved->storageOffset;
            if (node->flags & AST_FROM_UFCS)
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u32 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS)
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else
        {
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));

            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
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
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointer32, node->resultRegisterRC, 0, node->resultRegisterRC);
            inst->flags |= BCI_IMM_B;
            inst->b.u32 = node->resolvedSymbolOverload->storageOffset;
        }
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
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[i], resolved->registers[i]);

        SWAG_VERIFY(node->resultRegisterRC.size() > 0, internalError(context, format("emitIdentifier, cannot reference identifier '%s'", identifier->name.c_str()).c_str()));
        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    return internalError(context, "emitIdentifier");
}
