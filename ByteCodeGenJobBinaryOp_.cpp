#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeOp.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitBinaryOpPlus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpPlus, type not native");

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
        return internalError(context, "emitBinaryOpPlus, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpMinus(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpMinus, type not native");

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
        return internalError(context, "emitBinaryOpMinus, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpMul(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpMul, type not native");

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
        return internalError(context, "emitBinaryOpMul, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOpDiv(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBinaryOpDiv, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::BinOpDivF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::BinOpDivF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBinaryOpDiv, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskAnd(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBitmaskAnd, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BitmaskAndS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BitmaskAndS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BitmaskAndU32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BitmaskAndU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBitmaskAnd, type not supported");
    }
}

bool ByteCodeGenJob::emitBitmaskOr(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitBitmaskOr, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::BitmaskOrS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::BitmaskOrS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::BitmaskOrU32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::BitmaskOrU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitBitmaskOr, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftLeft(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitShiftLeft, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::ShiftLeftS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::ShiftLeftS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::ShiftLeftU32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::ShiftLeftU64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitShiftLeft, type not supported");
    }
}

bool ByteCodeGenJob::emitShiftRight(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitShiftRight, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::ShiftRightS32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::ShiftRightS64, r0, r1, r2);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::ShiftRightS32, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::ShiftRightS64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitShiftRight, type not supported");
    }
}

bool ByteCodeGenJob::emitBinaryOp(ByteCodeGenContext* context)
{
    AstNode* node   = context->node;
    auto     module = context->sourceFile->module;

    auto r0 = node->childs[0]->resultRegisterRC;
    auto r1 = node->childs[1]->resultRegisterRC;
    auto r2 = module->reserveRegisterRC();
    module->freeRegisterRC(r0);
    module->freeRegisterRC(r1);
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
    case TokenId::SymAmpersandAmpersand:
        emitInstruction(context, ByteCodeOp::BinOpAnd, r0, r1, r2);
        return true;
    case TokenId::SymVerticalVertical:
        emitInstruction(context, ByteCodeOp::BinOpOr, r0, r1, r2);
        return true;
    case TokenId::SymVertical:
        SWAG_CHECK(emitBitmaskOr(context, r0, r1, r2));
        return true;
    case TokenId::SymAmpersand:
        SWAG_CHECK(emitBitmaskAnd(context, r0, r1, r2));
        return true;
    case TokenId::SymLowerLower:
        SWAG_CHECK(emitShiftLeft(context, r0, r1, r2));
        return true;
    case TokenId::SymGreaterGreater:
        SWAG_CHECK(emitShiftRight(context, r0, r1, r2));
        return true;
    default:
        return internalError(context, "emitBinaryOp, invalid token op");
    }
}

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCompareOpEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
        emitInstruction(context, ByteCodeOp::CompareOpEqualBool, r0, r1, r2);
        return true;
    case NativeType::S32:
    case NativeType::U32:
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::CompareOpEqual32, r0, r1, r2);
        return true;
    case NativeType::S64:
    case NativeType::U64:
    case NativeType::F32:
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitCompareOpEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCompareOpLower, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CompareOpLowerS32, r0, r1, r2);
        return true;
    case NativeType::U32:
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CompareOpLowerS64, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CompareOpLowerF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::CompareOpLowerF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitCompareOpLower, type not supported");
    }
}

bool ByteCodeGenJob::emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCompareOpGreater, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::CompareOpGreaterS32, r0, r1, r2);
        return true;
    case NativeType::U32:
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, r0, r1, r2);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::CompareOpGreaterS64, r0, r1, r2);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, r0, r1, r2);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, r0, r1, r2);
        return true;
    default:
        return internalError(context, "emitCompareOpGreater, type not supported");
    }
}

bool ByteCodeGenJob::emitCompareOp(ByteCodeGenContext* context)
{
    AstNode* node   = context->node;
    auto     module = context->sourceFile->module;

    auto r0 = node->childs[0]->resultRegisterRC;
    auto r1 = node->childs[1]->resultRegisterRC;
    auto r2 = module->reserveRegisterRC();
    module->freeRegisterRC(r0);
    module->freeRegisterRC(r1);
    node->resultRegisterRC = r2;

    switch (node->token.id)
    {
    case TokenId::SymEqualEqual:
        SWAG_CHECK(emitCompareOpEqual(context, r0, r1, r2));
        return true;
    case TokenId::SymExclamEqual:
        SWAG_CHECK(emitCompareOpEqual(context, r0, r1, r2));
        emitInstruction(context, ByteCodeOp::NegBool, r2);
        return true;
    case TokenId::SymLower:
        SWAG_CHECK(emitCompareOpLower(context, r0, r1, r2));
        return true;
    case TokenId::SymGreater:
        SWAG_CHECK(emitCompareOpGreater(context, r0, r1, r2));
        return true;
    case TokenId::SymLowerEqual:
        SWAG_CHECK(emitCompareOpGreater(context, r0, r1, r2));
        emitInstruction(context, ByteCodeOp::NegBool, r2);
        return true;
    case TokenId::SymGreaterEqual:
        SWAG_CHECK(emitCompareOpLower(context, r0, r1, r2));
        emitInstruction(context, ByteCodeOp::NegBool, r2);
        return true;
    default:
        return internalError(context, "emitCompareOpGreater, invalid token op");
    }
}
