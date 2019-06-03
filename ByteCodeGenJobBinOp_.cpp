#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"

bool ByteCodeGenJob::emitBinaryOpPlus(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BinOpPlusS32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BinOpPlusS64);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BinOpPlusU32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BinOpPlusU64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpPlusF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpPlusF64);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOpMinus(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BinOpMinusS32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BinOpMinusS64);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BinOpMinusU32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BinOpMinusU64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpMinusF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpMinusF64);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOpMul(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BinOpMulS32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BinOpMulS64);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BinOpMulU32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BinOpMulU64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpMulF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpMulF64);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOpDiv(ByteCodeGenContext* context)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpDivF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpDivF64);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    switch (node->token.id)
    {
    case TokenId::SymPlus:
        SWAG_CHECK(emitBinaryOpPlus(context));
        return true;
    case TokenId::SymMinus:
        SWAG_CHECK(emitBinaryOpMinus(context));
        return true;
    case TokenId::SymAsterisk:
        SWAG_CHECK(emitBinaryOpMul(context));
        return true;
    case TokenId::SymSlash:
        SWAG_CHECK(emitBinaryOpDiv(context));
        return true;
    default:
        return internalError(context);
    }
}
