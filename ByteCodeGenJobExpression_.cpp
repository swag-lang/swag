#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "CommandLine.h"
#include "SemanticJob.h"
#include "Global.h"
#include "SymTable.h"
#include "ThreadManager.h"

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::ArrayPointerIndex);
    int  sizeOf = node->typeInfo->sizeOf;

    emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC, node->array->resultRegisterRC);

    if (!g_CommandLine.optimizeByteCode || sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC         = node->array->resultRegisterRC;
    node->parent->resultRegisterRC = node->resultRegisterRC;
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

        context->sourceFile->module->freeRegisterRC(r0);
    }

    if (!g_CommandLine.optimizeByteCode || sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
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

    if (!g_CommandLine.optimizeByteCode || sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitStructDeRef(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto typeInfo = node->typeInfo;

    if (typeInfo->isNative(NativeType::String))
    {
        node->resultRegisterRC += reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::DeRefString, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::DeRefPointer, node->resultRegisterRC, node->resultRegisterRC);
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
    if (node->array->typeInfo->isNative(NativeType::String))
    {
        if (g_CommandLine.debugBoundCheck)
        {
            emitInstruction(context, ByteCodeOp::BoundCheckString, node->access->resultRegisterRC, node->array->resultRegisterRC[1]);
        }

        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC);
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
        if (!g_CommandLine.optimizeByteCode || !node->access->isConstantInt0())
        {
            if (!g_CommandLine.optimizeByteCode || sizeOf > 1)
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

        node->parent->resultRegisterRC = node->resultRegisterRC;
    }

    // Dereference of a pointer
    else if (node->array->typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfo = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Pointer);
        int  sizeOf   = typeInfo->sizeOfPointedBy();

        // Increment pointer (if increment is not 0)
        if (!g_CommandLine.optimizeByteCode || !node->access->isConstantInt0())
        {
            if (!g_CommandLine.optimizeByteCode || sizeOf > 1)
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
                return internalError(context, "emitPointerDeRef, pointer, size not supported");
            }
        }

        node->parent->resultRegisterRC = node->resultRegisterRC;
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

            context->sourceFile->module->freeRegisterRC(r0);
        }

        // Increment pointer (if increment is not 0)
        if (!g_CommandLine.optimizeByteCode || !node->access->isConstantInt0())
        {
            if (!g_CommandLine.optimizeByteCode || sizeOf > 1)
                emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.u32 = sizeOf;
            emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC, node->array->resultRegisterRC);
        }

        if (typeInfo->pointedType->isNative(NativeType::String))
        {
            node->array->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::DeRefString, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
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

        if (node->flags & AST_TAKE_ADDRESS)
            node->parent->typeInfo = typeInfo->pointedType;
        node->parent->resultRegisterRC = node->resultRegisterRC;
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

        freeRegisterRC(context, r0);
    }
    else
    {
        return internalError(context, "emitPointerDeRef, type not supported");
    }

    node->resultRegisterRC = node->array->resultRegisterRC;
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

bool ByteCodeGenJob::emitExpressionListBefore(ByteCodeGenContext* context)
{
    auto node = context->node;

    // Do not generate bytecode for childs in case of a constant expression, because
    // the full content of the expression is in the constant segment
    if (node->flags & AST_CONST_EXPR)
    {
        node->flags |= AST_NO_BYTECODE_CHILDS;
    }

    return true;
}

bool ByteCodeGenJob::emitExpressionList(ByteCodeGenContext* context)
{
    auto node = context->node;

    // This is a special expression list which initialize the pointer and the count of a slice
    if (node->flags & AST_SLICE_INIT_EXPRESSION)
    {
        node->resultRegisterRC = node->childs.front()->resultRegisterRC;
        node->resultRegisterRC += node->childs.back()->resultRegisterRC;
        return true;
    }

    auto module      = context->sourceFile->module;
    auto job         = context->job;
    bool isConstExpr = node->flags & AST_CONST_EXPR;

    // Reserve space in constant segment, except if expression is not constexpr, because in that case,
    // each affectation will be done one by one
    auto     typeList      = CastTypeInfo<TypeInfoList>(node->typeInfo, TypeInfoKind::TypeList);
    uint32_t storageOffset = isConstExpr ? module->reserveConstantSegment(typeList->sizeOf) : 0;
    job->collectChilds.clear();
    module->mutexConstantSeg.lock();
    auto offset = storageOffset;
    auto result = SemanticJob::collectLiterals(context->sourceFile, offset, node, isConstExpr ? nullptr : &job->collectChilds, isConstExpr ? SegmentBuffer::Constant : SegmentBuffer::None);
    module->mutexConstantSeg.unlock();
    SWAG_CHECK(result);

    reserveRegisterRC(context, node->resultRegisterRC, 2);

    if (!isConstExpr)
    {
        // Must be called only on the real node !
        auto listNode = CastAst<AstExpressionList>(context->node, AstNodeKind::ExpressionList);

        // Emit one affectation per child
        int      offsetIdx = listNode->storageOffset;
        uint32_t oneOffset = typeList->childs.front()->sizeOf;
        for (auto child : job->collectChilds)
        {
            emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC)->b.u32 = offsetIdx;
            emitAffectEqual(context, node->resultRegisterRC, child->resultRegisterRC);
            offsetIdx += oneOffset;
            module->freeRegisterRC(child->resultRegisterRC);
        }

        // Reference to the stack, and store the number of element in a register
        emitInstruction(context, ByteCodeOp::RARefFromStack, node->resultRegisterRC[0])->b.u32 = listNode->storageOffset;
        emitInstruction(context, ByteCodeOp::CopyRAVB32, node->resultRegisterRC[1])->b.u32     = (uint32_t) listNode->childs.size();
    }
    else
    {
        // Emit a reference to the buffer
        auto inst   = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        inst->c.u64 = ((uint64_t) storageOffset << 32) | (uint32_t) typeList->childs.size();
    }

    return true;
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    return emitLiteral(context, nullptr);
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context, TypeInfo* toType)
{
    auto node     = context->node;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    auto r0                = reserveRegisterRC(context);
    node->resultRegisterRC = r0;

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeType::Bool:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.b = node->computedValue.reg.b;
            return true;
        case NativeType::U8:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u8 = node->computedValue.reg.u8;
            return true;
        case NativeType::U16:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u16 = node->computedValue.reg.u16;
            return true;
        case NativeType::U32:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u32 = node->computedValue.reg.u32;
            return true;
        case NativeType::U64:
            emitInstruction(context, ByteCodeOp::CopyRAVB64, r0)->b.u64 = node->computedValue.reg.u64;
            return true;
        case NativeType::S8:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.s8 = node->computedValue.reg.s8;
            return true;
        case NativeType::S16:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.s16 = node->computedValue.reg.s16;
            return true;
        case NativeType::S32:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.s32 = node->computedValue.reg.s32;
            return true;
        case NativeType::S64:
            emitInstruction(context, ByteCodeOp::CopyRAVB64, r0)->b.s64 = node->computedValue.reg.s64;
            return true;
        case NativeType::F32:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.f32 = node->computedValue.reg.f32;
            return true;
        case NativeType::F64:
            emitInstruction(context, ByteCodeOp::CopyRAVB64, r0)->b.f64 = node->computedValue.reg.f64;
            return true;
        case NativeType::Char:
            emitInstruction(context, ByteCodeOp::CopyRAVB32, r0)->b.u32 = node->computedValue.reg.u32;
            return true;
        case NativeType::String:
        {
            auto r1    = reserveRegisterRC(context);
            auto index = context->sourceFile->module->reserveString(node->computedValue.text);
            node->resultRegisterRC += r1;
            emitInstruction(context, ByteCodeOp::CopyRARBStr, r0, r1)->c.u32 = index;
            return true;
        }
        default:
            return internalError(context, "emitLiteral, type not supported");
        }
    }
    else if (typeInfo == g_TypeMgr.typeInfoNull)
    {
        emitInstruction(context, ByteCodeOp::ClearRA, r0);
        if (toType && (toType->kind == TypeInfoKind::Slice || toType->isNative(NativeType::String)))
        {
            node->resultRegisterRC += reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::ClearRA, node->resultRegisterRC[1]);
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        auto     typeArray     = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
        uint32_t storageOffset = node->computedValue.reg.u32;
        node->resultRegisterRC += reserveRegisterRC(context);
        auto inst   = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC[0], node->resultRegisterRC[1]);
        inst->c.u64 = ((uint64_t) storageOffset << 32) | (uint32_t) typeArray->count;
    }
    else
    {
        return internalError(context, "emitLiteral, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCountProperty(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto expr     = node->childs.front();
    auto typeInfo = TypeManager::concreteType(expr->typeInfo);

    if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
    {
        node->resultRegisterRC = expr->resultRegisterRC[1];
    }
    else if (typeInfo->kind == TypeInfoKind::Variadic)
    {
        node->resultRegisterRC = expr->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::DeRef32, node->resultRegisterRC);
    }
    else
    {
        return internalError(context, "emitCountProperty, type not supported");
    }

    return true;
}

bool ByteCodeGenJob::emitDataProperty(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto typeInfo = TypeManager::concreteType(node->expression->typeInfo);

    if (typeInfo->isNative(NativeType::String) || typeInfo->kind == TypeInfoKind::Slice)
    {
        node->resultRegisterRC = node->expression->resultRegisterRC[0];
    }
    else if (typeInfo->kind == TypeInfoKind::Array)
    {
        node->resultRegisterRC = node->expression->resultRegisterRC[0];
    }
    else
    {
        return internalError(context, "emitDataProperty, type not supported");
    }

    return true;
}
