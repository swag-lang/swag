#include "pch.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "Report.h"

bool ByteCodeGenJob::emitUnaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);
    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitUnaryOpMinus, type not native");

    if (mustEmitSafety(context, SAFETY_OVERFLOW))
        emitSafetyNeg(context, r0, typeInfoExpr);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::NegS32, r0);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::NegS64, r0);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::NegF32, r0);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::NegF64, r0);
        return true;
    default:
        return Report::internalError(context->node, "emitUnaryOpMinus, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOpInvert(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0)
{
    auto typeInfo = TypeManager::concreteType(typeInfoExpr);
    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitUnaryOpInvert, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::InvertU8, r0);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::InvertU16, r0);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::InvertU32, r0);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::InvertU64, r0);
        return true;
    default:
        return Report::internalError(context->node, "emitUnaryOpInvert, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    auto     front         = node->childs[0];
    node->resultRegisterRC = front->resultRegisterRC;

    if (!(node->doneFlags & DONEFLAG_EMIT_OP))
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result != ContextResult::Done)
                return true;
            node->doneFlags |= DONEFLAG_EMIT_OP;
        }
        else
        {
            auto typeInfoExpr = front->castedTypeInfo ? front->castedTypeInfo : front->typeInfo;

            switch (node->tokenId)
            {
            case TokenId::SymExclam:
            {
                SWAG_CHECK(emitCast(context, node, front->typeInfo, front->castedTypeInfo));
                SWAG_ASSERT(context->result == ContextResult::Done);
                auto rt = reserveRegisterRC(context);
                emitInstruction(context, ByteCodeOp::NegBool, rt, node->resultRegisterRC);
                freeRegisterRC(context, node->resultRegisterRC);
                node->resultRegisterRC = rt;
                break;
            }

            case TokenId::SymMinus:
                ensureCanBeChangedRC(context, node->resultRegisterRC);
                SWAG_CHECK(emitUnaryOpMinus(context, typeInfoExpr, node->resultRegisterRC));
                break;

            case TokenId::SymTilde:
                ensureCanBeChangedRC(context, node->resultRegisterRC);
                SWAG_CHECK(emitUnaryOpInvert(context, typeInfoExpr, node->resultRegisterRC));
                break;

            default:
                return Report::internalError(context->node, "emitUnaryOp, invalid token op");
            }

            node->doneFlags |= DONEFLAG_EMIT_OP;
        }
    }

    if (!(node->doneFlags & DONEFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
        node->doneFlags |= DONEFLAG_CAST1;
    }

    return true;
}
