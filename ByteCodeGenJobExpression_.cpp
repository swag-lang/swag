#include "pch.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"
#include "Ast.h"
#include "CommandLine.h"
#include "SemanticJob.h"
#include "Global.h"

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);
    int  sizeOf = node->typeInfo->sizeOf;
    emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC);
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.s32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitArrayRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);
    int  sizeOf = node->typeInfo->sizeOf;

    if (g_CommandLine.debugBoundCheck)
    {
        auto typeInfo = CastTypeInfo<TypeInfoArray>(node->array->typeInfo, TypeInfoKind::Array);

        emitInstruction(context, ByteCodeOp::BoundCheckV, node->access->resultRegisterRC)->b.u32 = typeInfo->size - 1;
    }

    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.s32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);

    // Dereference of a string constant
    if (node->array->typeInfo->isNative(NativeType::String))
    {
        if (g_CommandLine.debugBoundCheck)
            emitInstruction(context, ByteCodeOp::BoundCheck, node->array->resultRegisterRC[1], node->access->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC[0], node->access->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC[0]);
    }

    // Dereference of a pointer
    else if (node->array->typeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typeInfo = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Pointer);
        int  sizeOf   = typeInfo->sizeOfPointedBy();

        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.s32 = sizeOf;
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC);
        if (!(node->flags & AST_LEFT_EXPRESSION))
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
                return internalError(context, "emitPointerDeRef, type not supported");
            }
        }
    }

    // Dereference of an array
    else if (node->array->typeInfo->kind == TypeInfoKind::Array)
    {
        auto typeInfo = CastTypeInfo<TypeInfoArray>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Array);
        int  sizeOf   = typeInfo->pointedType->sizeOf;
        if (g_CommandLine.debugBoundCheck)
            emitInstruction(context, ByteCodeOp::BoundCheckV, node->access->resultRegisterRC)->b.u32 = typeInfo->size - 1;

        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::MulRAVB, node->access->resultRegisterRC)->b.s32 = sizeOf;
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC);

        if (typeInfo->pointedType->isNative(NativeType::String))
        {
            node->array->resultRegisterRC += context->sourceFile->module->reserveRegisterRC(context->bc);
            emitInstruction(context, ByteCodeOp::DeRefString, node->array->resultRegisterRC[0], node->array->resultRegisterRC[1]);
        }
        else if (!(node->flags & AST_LEFT_EXPRESSION) && typeInfo->pointedType->kind != TypeInfoKind::Array)
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
                return internalError(context, "emitPointerDeRef, type not supported");
            }
        }
    }

    node->resultRegisterRC = node->array->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitMakePointer(ByteCodeGenContext* context)
{
    auto node              = context->node;
    node->resultRegisterRC = node->childs.front()->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitExpressionList(ByteCodeGenContext* context)
{
    auto node   = context->node;
    auto module = context->sourceFile->module;

    if (!(node->flags & AST_CONST_EXPR))
        return internalError(context, "emitExpressionList, expression not constant");

    // Reserve space in constant segment, and copy all
    uint32_t storageOffset = module->reserveConstantSegment(node->typeInfo->sizeOf);
    module->mutexConstantSeg.lock();
    auto offset = storageOffset;
    auto result = SemanticJob::collectLiterals(context->sourceFile, offset, node, SegmentBuffer::Constant);
    module->mutexConstantSeg.unlock();
    SWAG_CHECK(result);

    // Emit a reference to the buffer
    node->resultRegisterRC = reserveRegisterRC(context);
    auto inst              = emitInstruction(context, ByteCodeOp::RARefFromConstantSeg, node->resultRegisterRC);
    inst->b.u32            = storageOffset;
    return true;
}

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
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
            emitInstruction(context, ByteCodeOp::CopyRAVBStr, r0, r1)->c.u32 = index;
            return true;
        }
        default:
            return internalError(context, "emitLiteral, type not supported");
        }
    }
    else if (typeInfo == g_TypeMgr.typeInfoNull)
    {
        emitInstruction(context, ByteCodeOp::ClearRA, r0);
    }
    else
    {
        return internalError(context, "emitLiteral, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCountProperty(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstProperty>(context->node, AstNodeKind::IntrinsicProp);
    auto typeInfo = TypeManager::concreteType(node->expression->typeInfo);

    if (typeInfo->isNative(NativeType::String))
    {
        node->resultRegisterRC = node->expression->resultRegisterRC[1];
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

    if (typeInfo->isNative(NativeType::String))
    {
        node->resultRegisterRC = node->expression->resultRegisterRC[0];
    }
    else
    {
        return internalError(context, "emitCountData, type not supported");
    }

    return true;
}
