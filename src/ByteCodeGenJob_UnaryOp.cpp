#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "SourceFile.h"

bool ByteCodeGenJob::emitUnaryOpMinus(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError( "emitUnaryOpMinus, type not native");

    emitSafetyNeg(context, r0, typeInfoExpr);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::NegS32, r0);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::NegS64, r0);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::NegF32, r0);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::NegF64, r0);
        return true;
    default:
        return context->internalError( "emitUnaryOpMinus, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOpInvert(ByteCodeGenContext* context, TypeInfo* typeInfoExpr, uint32_t r0)
{
    auto typeInfo = TypeManager::concreteReferenceType(typeInfoExpr);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError( "emitUnaryOpInvert, type not native");

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
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::InvertU64, r0);
        return true;
    default:
        return context->internalError( "emitUnaryOpInvert, type not supported");
    }
}

bool ByteCodeGenJob::emitUnaryOp(ByteCodeGenContext* context)
{
    AstNode* node          = context->node;
    auto     front         = node->childs[0];
    node->resultRegisterRC = front->resultRegisterRC;

    if (!(node->doneFlags & AST_DONE_EMIT_OP))
    {
        // User special function
        if (node->hasSpecialFuncCall())
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result != ContextResult::Done)
                return true;
            node->doneFlags |= AST_DONE_EMIT_OP;
        }
        else
        {
            auto typeInfoExpr = node->castedTypeInfo ? node->castedTypeInfo : node->typeInfo;

            switch (node->token.id)
            {
            case TokenId::SymExclam:
            {
                SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
                node->doneFlags |= AST_DONE_CAST2;
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
                return context->internalError( "emitUnaryOp, invalid token op");
            }

            node->doneFlags |= AST_DONE_EMIT_OP;
        }
    }

    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
    }

    return true;
}
