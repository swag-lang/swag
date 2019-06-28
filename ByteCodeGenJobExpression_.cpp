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

bool ByteCodeGenJob::emitPointerRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);
    int  sizeOf = node->typeInfo->sizeOf;
    emitInstruction(context, ByteCodeOp::DeRefPointer, node->array->resultRegisterRC);
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRCxS32, node->access->resultRegisterRC)->b.s32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitArrayRef(ByteCodeGenContext* context)
{
    auto node   = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);
    int  sizeOf = node->typeInfo->sizeOf;
    emitInstruction(context, ByteCodeOp::BoundCheck, node->array->resultRegisterRC[1], node->access->resultRegisterRC);
    if (sizeOf > 1)
        emitInstruction(context, ByteCodeOp::MulRCxS32, node->access->resultRegisterRC)->b.s32 = sizeOf;
    emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC, node->access->resultRegisterRC);
    node->resultRegisterRC = node->array->resultRegisterRC;
    return true;
}

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto node = CastAst<AstPointerDeRef>(context->node, AstNodeKind::PointerDeRef);

    // Dereference of a string constant
    if (node->array->typeInfo->kind == TypeInfoKind::Native && node->array->typeInfo->nativeType == NativeType::String)
    {
        emitInstruction(context, ByteCodeOp::BoundCheck, node->array->resultRegisterRC[1], node->access->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::IncPointer, node->array->resultRegisterRC[0], node->access->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::DeRef8, node->array->resultRegisterRC[0]);
    }

    // Dereference of a pointer
    else if (node->array->typeInfo->kind == TypeInfoKind::Pointer || node->array->typeInfo->kind == TypeInfoKind::Array)
    {
        int sizeOf;
        if (node->array->typeInfo->kind == TypeInfoKind::Pointer)
        {
            auto typeInfo = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Pointer);
            sizeOf        = typeInfo->sizeOfPointedBy();
        }
        else
        {
            auto typeInfo = CastTypeInfo<TypeInfoArray>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Array);
            sizeOf        = typeInfo->pointedType->sizeOf;
        }

        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::MulRCxS32, node->access->resultRegisterRC)->b.s32 = sizeOf;
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
                return internalError(context, "emitArrayAccess, type not supported");
                break;
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

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    auto node     = context->node;
    auto typeInfo = TypeManager::concreteType(node->typeInfo);

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitLiteral, type not native");

    auto r0                = reserveRegisterRC(context);
    node->resultRegisterRC = r0;
    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.b = node->computedValue.reg.b;
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.u8 = node->computedValue.reg.u8;
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.u16 = node->computedValue.reg.u16;
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.u32 = node->computedValue.reg.u32;
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::CopyRCxVa64, 0, r0)->a.u64 = node->computedValue.reg.u64;
        return true;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.s8 = node->computedValue.reg.s8;
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.s16 = node->computedValue.reg.s16;
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.s32 = node->computedValue.reg.s32;
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CopyRCxVa64, 0, r0)->a.s64 = node->computedValue.reg.s64;
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.f32 = node->computedValue.reg.f32;
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::CopyRCxVa64, 0, r0)->a.f64 = node->computedValue.reg.f64;
        return true;
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r0)->a.u32 = node->computedValue.reg.u32;
        return true;
    case NativeType::String:
    {
        auto r1    = reserveRegisterRC(context);
        auto index = context->sourceFile->module->reserveDataSegmentString(node->computedValue.text);
        node->resultRegisterRC += r1;
        emitInstruction(context, ByteCodeOp::CopyRCxVaStr, index, r0, r1);
        return true;
    }

    default:
        return internalError(context, "emitLiteral, type not supported");
    }
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
