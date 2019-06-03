#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"

bool ByteCodeGenJob::emitBinaryOpPlus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BinOpPlusS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BinOpPlusS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BinOpPlusU32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BinOpPlusU64, r0, r1, r2);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpPlusF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpPlusF64, r0, r1, r2);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOpMinus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BinOpMinusS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BinOpMinusS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BinOpMinusU32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BinOpMinusU64, r0, r1, r2);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpMinusF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpMinusF64, r0, r1, r2);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOpMul(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BinOpMulS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BinOpMulS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BinOpMulU32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BinOpMulU64, r0, r1, r2);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpMulF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpMulF64, r0, r1, r2);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOpDiv(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context);

    switch (typeInfo->nativeType)
    {
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpDivF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpDivF64, r0, r1, r2);
        return true;
    default:
        return internalError(context);
    }
}

bool ByteCodeGenJob::emitBinaryOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;

    auto r0 = node->childs[0]->resultRegisterRC;
    auto r1 = node->childs[1]->resultRegisterRC;
    auto r2 = context->sourceFile->module->reserveRegisterRC();
    context->sourceFile->module->freeRegisterRC(r0);
    context->sourceFile->module->freeRegisterRC(r1);
    node->resultRegisterRC = r2;

    switch (node->token.id)
    {
    case TokenId::SymPlus:
        SWAG_CHECK(emitBinaryOpPlus(context, r0, r1, r2));
        return true;
    case TokenId::SymMinus:
        SWAG_CHECK(emitBinaryOpMinus(context, r0, r1, r2));
        return true;
    case TokenId::SymAsterisk:
        SWAG_CHECK(emitBinaryOpMul(context, r0, r1, r2));
        return true;
    case TokenId::SymSlash:
        SWAG_CHECK(emitBinaryOpDiv(context, r0, r1, r2));
        return true;
    default:
        return internalError(context);
    }
}
