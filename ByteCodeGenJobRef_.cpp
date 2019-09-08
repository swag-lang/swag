#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "SymTable.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC, node->array->resultRegisterRC);
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC         = node->array->resultRegisterRC;
    node->parent->resultRegisterRC = node->resultRegisterRC;

    freeRegisterRC(context, node->access->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitArrayRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    if (g_CommandLine.debugBoundCheck)
    {
        auto typeInfo = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);
        auto r0       = reserveRegisterRC(context);

        emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u32 = typeInfo->count - 1;
        emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, r0);

        freeRegisterRC(context, r0);
    }

    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitSliceRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    node->array->resultRegisterRC += reserveRegisterRC(context);
    auto inst   = emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
    inst->c.u32 = sizeof(void*);
    emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC[0], node->array->resultRegisterRC[0]);

    if (g_CommandLine.debugBoundCheck)
        emitInstruction(context, ByteCodeOp::BoundCheckReg, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);

    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;

    freeRegisterRC(context, node->access->resultRegisterRC);
    return true;
}

bool ByteCodeGenJob::emitStructDeRef(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto typeInfo = node->typeInfo;

    if (typeInfo->kind == TypeInfoKind::Struct || typeInfo->kind == TypeInfoKind::Array)
    {
        return true;
    }

    typeInfo = TypeManager::concreteType(typeInfo);
    if (typeInfo->isNative(NativeTypeKind::String))
    {
        node->resultRegisterRC += reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
		node->resultRegisterRC += reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->sizeOf)
        {
        case 1:
            emitInstruction(context, ByteCodeOp::DeRef8, node->resultRegisterRC);
            break;
        case 2:
            emitInstruction(context, ByteCodeOp::DeRef16, node->resultRegisterRC);
            break;
        case 4:
            emitInstruction(context, ByteCodeOp::DeRef32, node->resultRegisterRC);
            break;
        case 8:
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC);
            break;
        default:
            return internalError(context, "emitStructDeRef, native, size not supported");
        }

        return true;
    }

    return internalError(context, "emitStructDeRef, invalid type");
}

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);

    // Dereference of a string constant
    if (node->array->typeInfo->isNative(NativeTypeKind::String))
    {
        if (g_CommandLine.debugBoundCheck)
            emitInstruction(context, ByteCodeOp::BoundCheckString, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC);
        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access->resultRegisterRC);
    }

    // Dereference of a slice
    else if (node->array->typeInfo->kind == TypeInfoKind::Slice)
    {
        auto typeInfo = CastTypeInfo<TypeInfoSlice>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Slice);
        int  sizeOf   = typeInfo->pointedType->sizeOf;

        if (g_CommandLine.debugBoundCheck)
        {
            emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);
        }

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstantInt0())
        {
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (!(node->flags & AST_TAKE_ADDRESS))
        {
            switch (sizeOf)
            {
            case 1:
                emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC);
                break;
            case 2:
                emitInstruction(context, ByteCodeOp::DeRef16, node->array->resultRegisterRC);
                break;
            case 4:
                emitInstruction(context, ByteCodeOp::DeRef32, node->array->resultRegisterRC);
                break;
            case 8:
                emitInstruction(context, ByteCodeOp::DeRef64, node->array->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitPointerDeRef, slice, size not supported");
            }
        }

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access->resultRegisterRC);
    }

    // Dereference of a pointer
    else if (node->array->typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfo = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Pointer);
        int  sizeOf   = typeInfo->sizeOfPointedBy();

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstantInt0())
        {
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfo->pointedType->isNative(NativeTypeKind::String))
        {
            node->array->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
        }
        else if (!(node->flags & AST_TAKE_ADDRESS))
        {
            switch (sizeOf)
            {
            case 1:
                emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC);
                break;
            case 2:
                emitInstruction(context, ByteCodeOp::DeRef16, node->array->resultRegisterRC);
                break;
            case 4:
                emitInstruction(context, ByteCodeOp::DeRef32, node->array->resultRegisterRC);
                break;
            case 8:
                emitInstruction(context, ByteCodeOp::DeRef64, node->array->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitPointerDeRef, pointer, size not supported");
            }
        }

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access->resultRegisterRC);
    }

    // Dereference of an array
    else if (node->array->typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeInfo = CastTypeInfo<TypeInfoArray>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Array);
        int  sizeOf   = typeInfo->pointedType->sizeOf;
        if (g_CommandLine.debugBoundCheck)
        {
            auto r0 = reserveRegisterRC(context);

            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u32 = typeInfo->count - 1;
            emitInstruction(context, ByteCodeOp::BoundCheck, node->access->resultRegisterRC, r0);

            freeRegisterRC(context, r0);
        }

        // Increment pointer (if increment is not 0)
        if (!node->access->isConstantInt0())
        {
            if (sizeOf > 1)
                emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfo->pointedType->isNative(NativeTypeKind::String))
        {
            node->array->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
        }
        else if (!(node->flags & AST_TAKE_ADDRESS) && typeInfo->pointedType->kind != TypeInfoKind::Array)
        {
            switch (sizeOf)
            {
            case 1:
                emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC);
                break;
            case 2:
                emitInstruction(context, ByteCodeOp::DeRef16, node->array->resultRegisterRC);
                break;
            case 4:
                emitInstruction(context, ByteCodeOp::DeRef32, node->array->resultRegisterRC);
                break;
            case 8:
                emitInstruction(context, ByteCodeOp::DeRef64, node->array->resultRegisterRC);
                break;
            default:
                return internalError(context, "emitPointerDeRef, array, size not supported");
            }
        }

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, node->access->resultRegisterRC);
    }

    // Dereference a variadic parameter
    else if (node->array->typeInfo->kind == TypeInfoKind::Variadic)
    {
        RegisterList r0;

        reserveRegisterRC(context, r0, 2);
        emitInstruction(context, ByteCodeOp::CopyRARB, r0, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef64, r0);
        emitInstruction(context, ByteCodeOp::ShiftRightU64VB, r0)->b.u32 = 32;
        emitInstruction(context, ByteCodeOp::MulRAVB, r0)->b.u32         = sizeof(Register);
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, r0[0], r0[1]);
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeof(Register);

        // This will deref the offset of the variadic argument
        emitInstruction(context, ByteCodeOp::IncPointer, r0[1], node->access->resultRegisterRC, r0[1]);
        emitInstruction(context, ByteCodeOp::DeRef32, r0[1]);
        emitInstruction(context, ByteCodeOp::IncRA64, r0[1]);
        emitInstruction(context, ByteCodeOp::MulRAVB, r0[1])->b.u32 = sizeof(Register);

        // Point to the argument
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, r0[1], node->array->resultRegisterRC);

        node->resultRegisterRC = node->array->resultRegisterRC;
        freeRegisterRC(context, r0);
        freeRegisterRC(context, node->access->resultRegisterRC);
    }

    // Dereference a struct
    else if (node->array->typeInfo->kind == TypeInfoKind::Struct)
    {
        if (node->resolvedSymbolName && node->resolvedSymbolName->kind == SymbolKind::Function)
        {
            AstNode allParams;
            allParams.reset();
            allParams.childs = node->structFlatParams;
            SWAG_CHECK(emitUserOp(context, &allParams));
        }
    }
    else
    {
        return internalError(context, "emitPointerDeRef, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitMakeLambda(ByteCodeGenContext* context)
{
    auto node       = context->node;
    auto sourceFile = context->sourceFile;
    auto front      = node->childs.front();
    auto funcNode   = CastAst<AstFuncDecl>(front->resolvedSymbolOverload->node, AstNodeKind::FuncDecl);

    // Need to generate bytecode, if not already done or running
    {
        scoped_lock lk(funcNode->mutex);

        // Need to wait for function full semantic resolve
        if (!(funcNode->flags & AST_FULL_RESOLVE))
        {
            funcNode->dependentJobs.push_back(context->job);
            context->result = ByteCodeResult::Pending;
            return true;
        }

        if (!(funcNode->flags & AST_BYTECODE_GENERATED))
        {
            context->job->dependentNodes.push_back(funcNode);
            if (!funcNode->byteCodeJob)
            {
                funcNode->byteCodeJob               = g_Pool_byteCodeGenJob.alloc();
                funcNode->byteCodeJob->sourceFile   = sourceFile;
                funcNode->byteCodeJob->originalNode = funcNode;
                funcNode->byteCodeJob->nodes.push_back(funcNode);
                setupBC(sourceFile->module, funcNode);
                g_ThreadMgr.addJob(funcNode->byteCodeJob);
            }
        }
    }

    freeRegisterRC(context, front->resultRegisterRC);
    node->resultRegisterRC = reserveRegisterRC(context);
    auto inst              = emitInstruction(context, ByteCodeOp::MakeLambda, node->resultRegisterRC);
    inst->b.pointer        = (uint8_t*) funcNode->bc;
    return true;
}

bool ByteCodeGenJob::emitMakePointer(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    return true;
}
