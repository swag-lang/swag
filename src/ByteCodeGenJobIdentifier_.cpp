#include "pch.h"
#include "ByteCodeGenJob.h"
#include "SymTable.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "Ast.h"
#include "Module.h"
#include "Diagnostic.h"

bool ByteCodeGenJob::emitIdentifierRef(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    node->resultRegisterRC = node->childs.back()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitGetErr(ByteCodeGenContext* context)
{
    auto node = context->node;
    reserveRegisterRC(context, node->resultRegisterRC, 2);
    emitInstruction(context, ByteCodeOp::IntrinsicGetErr, node->resultRegisterRC[0], node->resultRegisterRC[1]);
    return true;
}

bool ByteCodeGenJob::emitTryThrowExit(ByteCodeGenContext* context)
{
    auto node = CastAst<AstTryCatch>(context->node, AstNodeKind::Try, AstNodeKind::Throw);

    // Leave the current scope
    SWAG_CHECK(emitLeaveScope(context, node->ownerScope));
    if (context->result != ContextResult::Done)
        return true;

    // Set default value
    auto returnType = TypeManager::concreteType(node->ownerFct->returnType->typeInfo, CONCRETE_ALIAS);
    if (!returnType->isNative(NativeTypeKind::Void))
    {
        if (returnType->kind == TypeInfoKind::Struct)
        {
            if (!(node->doneFlags & AST_DONE_TRY_2))
            {
                reserveRegisterRC(context, node->regInit, 1);
                emitInstruction(context, ByteCodeOp::CopyRRtoRC, node->regInit);
                node->doneFlags |= AST_DONE_TRY_2;
            }

            TypeInfoPointer pt;
            pt.pointedType = returnType;
            SWAG_CHECK(emitInit(context, &pt, node->regInit, 1, nullptr, nullptr));
            if (context->result != ContextResult::Done)
                return true;

            freeRegisterRC(context, node->regInit);
        }
        else if (returnType->kind == TypeInfoKind::Array)
        {
            auto typeArr = CastTypeInfo<TypeInfoArray>(returnType, TypeInfoKind::Array);
            if (typeArr->finalType->kind != TypeInfoKind::Struct)
            {
                auto r0 = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::CopyRRtoRC, r0);
                emitInstruction(context, ByteCodeOp::SetZeroAtPointerX, r0)->b.u64 = typeArr->sizeOf;
                freeRegisterRC(context, r0);
            }
            else
            {
                if (!(node->doneFlags & AST_DONE_TRY_2))
                {
                    reserveRegisterRC(context, node->regInit, 1);
                    emitInstruction(context, ByteCodeOp::CopyRRtoRC, node->regInit);
                    node->doneFlags |= AST_DONE_TRY_2;
                }

                TypeInfoPointer pt;
                pt.pointedType = typeArr->finalType;
                SWAG_CHECK(emitInit(context, &pt, node->regInit, typeArr->totalCount, nullptr, nullptr));
                if (context->result != ContextResult::Done)
                    return true;

                freeRegisterRC(context, node->regInit);
            }
        }
        else if (returnType->flags & TYPEINFO_RETURN_BY_COPY)
        {
            internalError(context, "emitTry, unsupported return type");
        }
        else if (returnType->numRegisters() == 1)
        {
            auto r0 = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::ClearRA, r0);
            emitInstruction(context, ByteCodeOp::CopyRCtoRR, r0);
            freeRegisterRC(context, r0);
        }
        else if (returnType->numRegisters() == 2)
        {
            auto r0 = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::ClearRA, r0);
            emitInstruction(context, ByteCodeOp::CopyRCtoRR2, r0, r0);
            freeRegisterRC(context, r0);
        }
        else
        {
            internalError(context, "emitTry, unsupported return type");
        }
    }

    // Return from function
    if (node->ownerInline)
    {
        node->seekJump = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::Jump);
        node->ownerInline->returnList.push_back(node);
    }
    else
    {
        if (node->ownerFct->stackSize)
            emitInstruction(context, ByteCodeOp::IncSP)->a.s32 = node->ownerFct->stackSize;
        emitInstruction(context, ByteCodeOp::Ret);
    }

    return true;
}

bool ByteCodeGenJob::emitThrow(ByteCodeGenContext* context)
{
    auto node = CastAst<AstTryCatch>(context->node, AstNodeKind::Throw);
    auto expr = node->childs.back();

    if (!(node->doneFlags & AST_DONE_TRY_1))
    {
        emitInstruction(context, ByteCodeOp::IntrinsicSetErr, expr->resultRegisterRC[0], expr->resultRegisterRC[1]);
        node->doneFlags |= AST_DONE_TRY_1;
    }

    SWAG_CHECK(emitTryThrowExit(context));
    if (context->result != ContextResult::Done)
        return true;

    return true;
}

bool ByteCodeGenJob::emitTry(ByteCodeGenContext* context)
{
    auto node              = CastAst<AstTryCatch>(context->node, AstNodeKind::Try);
    node->resultRegisterRC = node->childs.front()->childs.back()->resultRegisterRC;

    if (!(node->doneFlags & AST_DONE_TRY_1))
    {
        RegisterList r0;
        reserveRegisterRC(context, r0, 2);
        emitInstruction(context, ByteCodeOp::IntrinsicGetErr, r0[0], r0[1]);
        node->seekInsideJump = context->bc->numInstructions;
        emitInstruction(context, ByteCodeOp::JumpIfZero64, r0[1]);
        freeRegisterRC(context, r0);
        node->doneFlags |= AST_DONE_TRY_1;
    }

    SWAG_CHECK(emitTryThrowExit(context));
    if (context->result != ContextResult::Done)
        return true;

    context->bc->out[node->seekInsideJump].b.s32 = context->bc->numInstructions - node->seekInsideJump - 1;
    return true;
}

bool ByteCodeGenJob::sameStackFrame(ByteCodeGenContext* context, SymbolOverload* overload)
{
    if (!context->node->isSameStackFrame(overload))
        return context->report({context->node, context->node->token, format("cannot reference variable '%s' because it's in another stack frame", overload->symbol->name.c_str())});
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
    SWAG_VERIFY(typeInfo->kind != TypeInfoKind::Generic, internalError(context, "emitIdentifier, type is generic"));

    // If this is a retval, then just copy the return pointer register to a computing register
    if (resolved->flags & OVERLOAD_RETVAL)
    {
        RegisterList r0 = reserveRegisterRC(context);
        emitRetValRef(context, r0);
        identifier->resultRegisterRC                = r0;
        identifier->identifierRef->resultRegisterRC = identifier->resultRegisterRC;
        identifier->parent->resultRegisterRC        = node->resultRegisterRC;
        return true;
    }

    // Will be done in the variable declaration
    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        if (node->flags & AST_IN_TYPE_VAR_DECLARATION)
            return true;
    }

    // Function parameter : it's a register on the stack
    if (resolved->flags & OVERLOAD_VAR_FUNC_PARAM)
    {
        node->resultRegisterRC = reserveRegisterRC(context);

        // Get a parameter from a #selectif block... this is special
        if (node->isSelectIfParam(resolved))
        {
            ByteCodeInstruction* inst;
            if (typeInfo->numRegisters() == 2)
            {
                reserveLinearRegisterRC2(context, node->resultRegisterRC);
                inst = emitInstruction(context, ByteCodeOp::GetFromStackParam64SI, node->resultRegisterRC[0], node->resultRegisterRC[1]);
            }
            else
            {
                inst = emitInstruction(context, ByteCodeOp::GetFromStackParam64SI, node->resultRegisterRC[0]);
            }

            auto typeFunc   = CastTypeInfo<TypeInfoFuncAttr>(resolved->node->ownerFct->typeInfo, TypeInfoKind::FuncAttr);
            inst->c.u64     = typeFunc->registerIdxToParamIdx(resolved->storageIndex);
            inst->d.pointer = (uint8_t*) resolved;

            identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
            node->parent->resultRegisterRC              = node->resultRegisterRC;
            return true;
        }

        SWAG_CHECK(sameStackFrame(context, resolved));
        if ((node->forceTakeAddress()) && typeInfo->kind != TypeInfoKind::Lambda && typeInfo->kind != TypeInfoKind::Array)
        {
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
            else
            {
                auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointerParam, node->resultRegisterRC);
                inst->b.u64 = resolved->storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
        {
            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
            inst->c.u64 = resolved->storageIndex;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = 8;
            else // Get the structure pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
        {
            // Get the ITable pointer
            if (node->flags & AST_FROM_UFCS)
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->storageOffset + 8;
                inst->c.u64 = resolved->storageIndex + 1;
            }
            // Get the structure pointer
            else
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC);
                inst->b.u64 = resolved->storageOffset;
                inst->c.u64 = resolved->storageIndex;
            }
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC[0]);
            inst->b.u64 = resolved->storageOffset;
            inst->c.u64 = resolved->storageIndex;
            inst        = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC[1]);
            inst->b.u64 = resolved->storageOffset + 8;
            inst->c.u64 = resolved->storageIndex + 1;
        }
        else if (typeInfo->kind == TypeInfoKind::Pointer && typeInfo->flags & TYPEINFO_RELATIVE)
        {
            return internalError(context, "function arguments relative pointers are not supported yet");
        }
        else
        {
            SWAG_ASSERT(typeInfo->numRegisters() == 1);
            auto inst   = emitInstruction(context, ByteCodeOp::GetFromStackParam64, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
            inst->c.u64 = resolved->storageIndex;
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
            inst->b.u64 = resolved->storageOffset;
            if (node->forceTakeAddress())
                inst->c.pointer = (uint8_t*) resolved;
        }
        else if ((node->forceTakeAddress()) && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            ByteCodeInstruction* inst;
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::MakeBssSegPointer, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, node->resultRegisterRC);
            inst->b.u64     = resolved->storageOffset;
            inst->c.pointer = (uint8_t*) resolved;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
        {
            ByteCodeInstruction* inst;
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the struct pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
        {
            ByteCodeInstruction* inst;
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::MakeBssSegPointer, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::MakeMutableSegPointer, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the struct pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            if (resolved->flags & OVERLOAD_VAR_BSS)
            {
                emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC[0])->b.u64 = resolved->storageOffset;
                emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC[1])->b.u64 = resolved->storageOffset + 8;
            }
            else
            {
                emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC[0])->b.u64 = resolved->storageOffset;
                emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC[1])->b.u64 = resolved->storageOffset + 8;
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Pointer && typeInfo->flags & TYPEINFO_RELATIVE)
        {
            return internalError(context, "global variables relative pointers are not supported yet");
        }
        else
        {
            ByteCodeInstruction* inst;
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            if (resolved->flags & OVERLOAD_VAR_BSS)
                inst = emitInstruction(context, ByteCodeOp::GetFromBssSeg64, node->resultRegisterRC);
            else
                inst = emitInstruction(context, ByteCodeOp::GetFromMutableSeg64, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
        }

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Variable from the stack
    if (resolved->flags & OVERLOAD_VAR_LOCAL)
    {
        SWAG_CHECK(sameStackFrame(context, resolved));
        node->resultRegisterRC = reserveRegisterRC(context);

        if (resolved->typeInfo->kind == TypeInfoKind::Reference)
        {
            if (node->forceTakeAddress())
                emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u64 = resolved->storageOffset;
            else
            {
                auto inst   = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
                inst->b.u64 = resolved->storageOffset;
            }
        }
        else if (typeInfo->kind == TypeInfoKind::Array ||
                 typeInfo->kind == TypeInfoKind::TypeListTuple ||
                 typeInfo->kind == TypeInfoKind::TypeListArray ||
                 typeInfo->kind == TypeInfoKind::Struct)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
        }
        else if ((node->forceTakeAddress()) && (!typeInfo->isNative(NativeTypeKind::String) || node->parent->kind != AstNodeKind::ArrayPointerIndex))
        {
            auto inst   = emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC);
            inst->b.u64 = resolved->storageOffset;
            if (node->parent->flags & AST_ARRAY_POINTER_REF)
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->isPointerTo(TypeInfoKind::Interface) && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
        {
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC)->b.u64 = resolved->storageOffset;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the structure pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Interface && (node->flags & (AST_FROM_UFCS | AST_TO_UFCS)))
        {
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u64 = resolved->storageOffset;
            if (node->flags & AST_FROM_UFCS) // Get the ITable pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC)->c.u64 = sizeof(void*);
            else if (node->flags & AST_TO_UFCS) // Get the structure pointer
                emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        }
        else if (typeInfo->kind == TypeInfoKind::Pointer && typeInfo->flags & TYPEINFO_RELATIVE)
        {
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u64 = resolved->storageOffset;
            SWAG_CHECK(emitUnwrapRelativePointer(context, node->resultRegisterRC, typeInfo->relative));
        }
        else if (typeInfo->kind == TypeInfoKind::Slice && typeInfo->flags & TYPEINFO_RELATIVE)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::MakeStackPointer, node->resultRegisterRC)->b.u64 = resolved->storageOffset;
            SWAG_CHECK(emitUnwrapRelativePointer(context, node->resultRegisterRC[0], typeInfo->relative));
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[1])->b.u64 = resolved->storageOffset + 8;
        }
        else if (typeInfo->numRegisters() == 2)
        {
            reserveLinearRegisterRC2(context, node->resultRegisterRC);
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[0])->b.u64 = resolved->storageOffset;
            emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC[1])->b.u64 = resolved->storageOffset + 8;
        }
        else
        {
            SWAG_ASSERT(typeInfo->sizeOf <= sizeof(uint64_t));
            ByteCodeInstruction* inst = nullptr;
            switch (typeInfo->sizeOf)
            {
            case 1:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack8, node->resultRegisterRC);
                break;
            case 2:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack16, node->resultRegisterRC);
                break;
            case 4:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack32, node->resultRegisterRC);
                break;
            case 8:
                inst = emitInstruction(context, ByteCodeOp::GetFromStack64, node->resultRegisterRC);
                break;
            }

            inst->b.u64 = resolved->storageOffset;
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
        SWAG_VERIFY(node->resultRegisterRC.size() > 0, internalError(context, format("emitIdentifier, cannot reference identifier '%s'", identifier->token.text.c_str()).c_str()));

        emitSafetyNullPointer(context, node->resultRegisterRC);
        if (node->resolvedSymbolOverload->storageOffset > 0)
        {
            auto inst   = emitInstruction(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
            inst->b.u64 = node->resolvedSymbolOverload->storageOffset;
            inst->flags |= BCI_IMM_B;
        }

        if (!(node->forceTakeAddress()))
            emitStructDeRef(context);
        else if (node->parent->flags & AST_ARRAY_POINTER_REF)
            emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    // Reference to an inline parameter : the registers are directly stored in the overload symbol
    if (resolved->flags & OVERLOAD_VAR_INLINE)
    {
        SWAG_CHECK(sameStackFrame(context, resolved));

        // We need to copy register, and not use it directly, because the register can be changed by
        // some code after (like when dereferencing something)
        SWAG_VERIFY(resolved->registers.size() > 0, internalError(context, format("emitIdentifier, identifier not generated '%s'", identifier->token.text.c_str()).c_str()));

        reserveRegisterRC(context, node->resultRegisterRC, resolved->registers.size());
        for (int i = 0; i < node->resultRegisterRC.size(); i++)
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, node->resultRegisterRC[i], resolved->registers[i]);

        identifier->identifierRef->resultRegisterRC = node->resultRegisterRC;
        node->parent->resultRegisterRC              = node->resultRegisterRC;
        return true;
    }

    return internalError(context, "emitIdentifier");
}
