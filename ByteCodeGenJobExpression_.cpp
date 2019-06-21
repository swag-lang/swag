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

bool ByteCodeGenJob::emitPointerDeRef(ByteCodeGenContext* context)
{
    auto node     = CastAst<AstPointerDeref>(context->node, AstNodeKind::PointerDeref);
    auto typeInfo = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(node->array->typeInfo), TypeInfoKind::Pointer);
    int  sizeOf   = typeInfo->sizeOfPointedBy();

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

    auto r1 = node->resultRegisterRC = reserveRegisterRC(context);
    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.b = node->computedValue.reg.b;
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.u8 = node->computedValue.reg.u8;
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.u16 = node->computedValue.reg.u16;
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.u32 = node->computedValue.reg.u32;
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::CopyRCxVa64, 0, r1)->a.u64 = node->computedValue.reg.u64;
        return true;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.s8 = node->computedValue.reg.s8;
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.s16 = node->computedValue.reg.s16;
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.s32 = node->computedValue.reg.s32;
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CopyRCxVa64, 0, r1)->a.s64 = node->computedValue.reg.s64;
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.f32 = node->computedValue.reg.f32;
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::CopyRCxVa64, 0, r1)->a.f64 = node->computedValue.reg.f64;
        return true;
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::CopyRCxVa32, 0, r1)->a.u32 = node->computedValue.reg.u32;
        return true;
    case NativeType::String:
    {
        auto index = context->sourceFile->module->reserveDataSegmentString(node->computedValue.text);
        emitInstruction(context, ByteCodeOp::CopyRCxVaStr, index, r1);
        return true;
    }

    default:
        return internalError(context, "emitLiteral, type not supported");
    }
}
