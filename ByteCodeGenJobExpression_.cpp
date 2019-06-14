#include "pch.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "Module.h"
#include "SourceFile.h"
#include "ByteCodeOp.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);

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
        context->bc->strBuffer.push_back(node->computedValue.text);
        emitInstruction(context, ByteCodeOp::CopyRCxVaStr, 0, r1)->a.u32 = (uint32_t) context->bc->strBuffer.size() - 1;
        return true;

    default:
        return internalError(context, "emitLiteral, type not supported");
    }
}
