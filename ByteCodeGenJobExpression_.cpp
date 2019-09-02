#include "pch.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "SemanticJob.h"

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
            freeRegisterRC(context, child->resultRegisterRC);
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
