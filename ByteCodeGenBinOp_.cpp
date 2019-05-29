#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGen.h"

bool ByteCodeGen::emitBinaryOpPlus(ByteCodeGenContext* context, AstNode* node)
{
    auto typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, node);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeNodeId::BinOpPlusS32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeNodeId::BinOpPlusS64);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeNodeId::BinOpPlusU32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeNodeId::BinOpPlusU64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeNodeId::BinOpPlusF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeNodeId::BinOpPlusF64);
        return true;
    default:
        return internalError(context, node);
    }
}

bool ByteCodeGen::emitBinaryOpMinus(ByteCodeGenContext* context, AstNode* node)
{
    auto typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, node);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeNodeId::BinOpMinusS32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeNodeId::BinOpMinusS64);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeNodeId::BinOpMinusU32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeNodeId::BinOpMinusU64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeNodeId::BinOpMinusF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeNodeId::BinOpMinusF64);
        return true;
    default:
        return internalError(context, node);
    }
}

bool ByteCodeGen::emitBinaryOpMul(ByteCodeGenContext* context, AstNode* node)
{
    auto typeInfo = node->typeInfo;
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, node);

    switch (typeInfo->nativeType)
    {
    case NativeType::S32:
        emitInstruction(context, ByteCodeNodeId::BinOpMulS32);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeNodeId::BinOpMulS64);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeNodeId::BinOpMulU32);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeNodeId::BinOpMulU64);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeNodeId::BinOpMulF32);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeNodeId::BinOpMulF64);
        return true;
    default:
        return internalError(context, node);
    }
}

bool ByteCodeGen::emitBinaryOp(ByteCodeGenContext* context, AstNode* node)
{
    SWAG_CHECK(emitRawNode(context, node->childs[0]));
    SWAG_CHECK(emitRawNode(context, node->childs[1]));

    switch (node->token.id)
    {
    case TokenId::SymPlus:
        SWAG_CHECK(emitBinaryOpPlus(context, node));
        return true;
    case TokenId::SymMinus:
        SWAG_CHECK(emitBinaryOpMinus(context, node));
        return true;
    case TokenId::SymAsterisk:
        SWAG_CHECK(emitBinaryOpMul(context, node));
        return true;
    default:
        return internalError(context, node);
    }
}
