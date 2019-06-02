#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCode.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeNodeId.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitLiteral(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    auto&    out      = context->bc->out;

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        emitInstruction(context, ByteCodeNodeId::PushBool);
        out.addBool(node->computedValue.reg.b);
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeNodeId::PushU8);
        out.addU8(node->computedValue.reg.u8);
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeNodeId::PushU16);
        out.addU16(node->computedValue.reg.u16);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeNodeId::PushU32);
        out.addU32(node->computedValue.reg.u32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeNodeId::PushU64);
        out.addU64(node->computedValue.reg.u64);
        return true;
    case NativeType::S8:
        emitInstruction(context, ByteCodeNodeId::PushS8);
        out.addS8(node->computedValue.reg.s8);
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeNodeId::PushS16);
        out.addS16(node->computedValue.reg.s16);
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeNodeId::PushS32);
        out.addS32(node->computedValue.reg.s32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeNodeId::PushS64);
        out.addS64(node->computedValue.reg.s64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeNodeId::PushF32);
        out.addF32((float) node->computedValue.reg.f64);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeNodeId::PushF64);
        out.addF64(node->computedValue.reg.f64);
        return true;

    default:
        return internalError(context);
    }
}
