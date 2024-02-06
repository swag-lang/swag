#include "pch.h"
#include "AstFlags.h"
#include "ByteCodeGen.h"
#include "ByteCodeGenContext.h"
#include "ByteCodeOp.h"
#include "Report.h"
#include "TypeManager.h"

bool ByteCodeGen::emitUnaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t rt, uint32_t r0)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);
    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitUnaryOpMinus, type not native");

    if (mustEmitSafety(context, SAFETY_OVERFLOW))
        emitSafetyNeg(context, r0, typeInfoExpr);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
        EMIT_INST2(context, ByteCodeOp::NegS32, rt, r0);
        return true;
    case NativeTypeKind::S64:
        EMIT_INST2(context, ByteCodeOp::NegS64, rt, r0);
        return true;
    case NativeTypeKind::F32:
        EMIT_INST2(context, ByteCodeOp::NegF32, rt, r0);
        return true;
    case NativeTypeKind::F64:
        EMIT_INST2(context, ByteCodeOp::NegF64, rt, r0);
        return true;
    default:
        return Report::internalError(context->node, "emitUnaryOpMinus, type not supported");
    }
}

bool ByteCodeGen::emitUnaryOpInvert(const ByteCodeGenContext* context, const TypeInfo* typeInfoExpr, uint32_t rt, uint32_t r0)
{
    const auto typeInfo = TypeManager::concreteType(typeInfoExpr);
    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitUnaryOpInvert, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        EMIT_INST2(context, ByteCodeOp::InvertU8, rt, r0);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        EMIT_INST2(context, ByteCodeOp::InvertU16, rt, r0);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
        EMIT_INST2(context, ByteCodeOp::InvertU32, rt, r0);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        EMIT_INST2(context, ByteCodeOp::InvertU64, rt, r0);
        return true;
    default:
        return Report::internalError(context->node, "emitUnaryOpInvert, type not supported");
    }
}

bool ByteCodeGen::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode*   node  = context->node;
    const auto front = node->childs[0];

    node->resultRegisterRc = front->resultRegisterRc;
    ensureCanBeChangedRC(context, node->resultRegisterRc);

    if (!(node->semFlags & SEMFLAG_EMIT_OP))
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            SWAG_CHECK(emitUserOp(context));
            YIELD();
            node->semFlags |= SEMFLAG_EMIT_OP;
        }
        else
        {
            const auto typeInfoExpr = front->castedTypeInfo ? front->castedTypeInfo : front->typeInfo;

            switch (node->tokenId)
            {
            case TokenId::SymExclam:
            {
                SWAG_CHECK(emitCast(context, node, front->typeInfo, front->castedTypeInfo));
                SWAG_ASSERT(context->result == ContextResult::Done);
                const auto rt = reserveRegisterRC(context);
                EMIT_INST2(context, ByteCodeOp::NegBool, rt, node->resultRegisterRc);
                freeRegisterRC(context, node->resultRegisterRc);
                node->resultRegisterRc = rt;
                break;
            }

            case TokenId::SymMinus:
                SWAG_CHECK(emitUnaryOpMinus(context, typeInfoExpr, node->resultRegisterRc, node->resultRegisterRc));
                break;
            case TokenId::SymTilde:
                SWAG_CHECK(emitUnaryOpInvert(context, typeInfoExpr, node->resultRegisterRc, node->resultRegisterRc));
                break;

            default:
                return Report::internalError(context->node, "emitUnaryOp, invalid token op");
            }

            node->semFlags |= SEMFLAG_EMIT_OP;
        }
    }

    if (!(node->semFlags & SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
        YIELD();
        node->semFlags |= SEMFLAG_CAST1;
    }

    return true;
}
