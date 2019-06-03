#include "pch.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        emitInstruction(context, ByteCodeOp::PushBool)->r0.b = node->computedValue.reg.b;
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::PushU8)->r0.u8 = node->computedValue.reg.u8;
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::PushU16)->r0.u16 = node->computedValue.reg.u16;
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::PushU32)->r0.u32 = node->computedValue.reg.u32;
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::PushU64)->r0.u64 = node->computedValue.reg.u64;
        return true;
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::PushS8)->r0.s8 = node->computedValue.reg.s8;
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::PushS16)->r0.s16 = node->computedValue.reg.s16;
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::PushS32)->r0.s32= node->computedValue.reg.s32;
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::PushS64)->r0.s64 = node->computedValue.reg.s64;
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::PushF32)->r0.f64 = node->computedValue.reg.f64;
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::PushF64)->r0.f64 = node->computedValue.reg.f64;
        return true;
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::PushU32)->r0.u32 = node->computedValue.reg.u32;
        return true;
    case NativeType::String:
        context->bc->strBuffer.push_back(node->computedValue.text);
        emitInstruction(context, ByteCodeOp::PushString)->r0.u32 = (uint32_t) context->bc->strBuffer.size() - 1;
        return true;

    default:
        return internalError(context);
    }
}
