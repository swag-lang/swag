#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "CommandLine.h"
#include "SymTable.h"

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto typeInfo = TypeManager::concreteType(left->typeInfo);

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            emitInstruction(context, ByteCodeOp::CompareOpEqualBool, r0, r1, r2);
            return true;
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::CompareOpEqual8, r0, r1, r2);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::CompareOpEqual16, r0, r1, r2);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::F32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
            return true;
        case NativeTypeKind::String:
            if (right->typeInfo == g_TypeMgr.typeInfoNull)
                emitInstruction(context, ByteCodeOp::IsNullString, r0, r2);
            else
                emitInstruction(context, ByteCodeOp::CompareOpEqualString, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitCompareOpEqual, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer || typeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CompareOpEqualPointer, r0, r1, r2);
    }
    else
    {
        return internalError(context, "emitCompareOpEqual, invalid type");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOpLowerS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::CompareOpLowerS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOpLowerF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpLowerF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitCompareOpLower, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOpLowerPointer, r0, r1, r2);
    }
    else
    {
        return internalError(context, "emitCompareOpLower, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpGreater(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitCompareOpGreater, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOpGreaterPointer, r0, r1, r2);
    }
    else
    {
        return internalError(context, "emitCompareOpGreater, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOp(ByteCodeGenContext* context)
{
    AstNode* node = context->node;
    auto     r0   = node->childs[0]->resultRegisterRC;
    auto     r1   = node->childs[1]->resultRegisterRC;

    emitCast(context, node->childs[0], TypeManager::concreteType(node->childs[0]->typeInfo), node->childs[0]->castedTypeInfo);
    emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo);

    if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context));
        auto r2 = node->resultRegisterRC;

        switch (node->token.id)
        {
        case TokenId::SymLower:
            emitInstruction(context, ByteCodeOp::MinusToTrue, r2);
            break;
        case TokenId::SymGreater:
            emitInstruction(context, ByteCodeOp::PlusToTrue, r2);
            break;
        case TokenId::SymLowerEqual:
            emitInstruction(context, ByteCodeOp::MinusZeroToTrue, r2);
            break;
        case TokenId::SymGreaterEqual:
            emitInstruction(context, ByteCodeOp::PlusZeroToTrue, r2);
            break;
        }

        switch (node->token.id)
        {
        case TokenId::SymExclamEqual:
            emitInstruction(context, ByteCodeOp::NegBool, r2);
            break;
        }
    }
    else
    {
        auto r2                = reserveRegisterRC(context);
        node->resultRegisterRC = r2;

        switch (node->token.id)
        {
        case TokenId::SymEqualEqual:
            SWAG_CHECK(emitCompareOpEqual(context, r0, r1, r2));
            break;
        case TokenId::SymExclamEqual:
            SWAG_CHECK(emitCompareOpEqual(context, r0, r1, r2));
            emitInstruction(context, ByteCodeOp::NegBool, r2);
            break;
        case TokenId::SymLower:
            SWAG_CHECK(emitCompareOpLower(context, r0, r1, r2));
            break;
        case TokenId::SymGreater:
            SWAG_CHECK(emitCompareOpGreater(context, r0, r1, r2));
            break;
        case TokenId::SymLowerEqual:
            SWAG_CHECK(emitCompareOpGreater(context, r0, r1, r2));
            emitInstruction(context, ByteCodeOp::NegBool, r2);
            break;
        case TokenId::SymGreaterEqual:
            SWAG_CHECK(emitCompareOpLower(context, r0, r1, r2));
            emitInstruction(context, ByteCodeOp::NegBool, r2);
            break;
        default:
            return internalError(context, "emitCompareOpGreater, invalid token op");
        }

        freeRegisterRC(context, r0);
        freeRegisterRC(context, r1);
    }

    return true;
}

bool ByteCodeGenJob::emitIs(ByteCodeGenContext* context)
{
    AstNode* node  = context->node;
    AstNode* left  = node->childs[0];
    AstNode* right = node->childs[1];

	if (left->typeInfo->isNative(NativeTypeKind::Any))
	{
		node->resultRegisterRC = reserveRegisterRC(context);
		auto inst = emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, node->resultRegisterRC);
		inst->b.u32 = right->computedValue.reg.u32;
		emitInstruction(context, ByteCodeOp::CompareOpEqualPointer, node->resultRegisterRC, left->resultRegisterRC[1], node->resultRegisterRC);
		freeRegisterRC(context, left);
		freeRegisterRC(context, right);
	}
	else
	{
		return internalError(context, "emitIs, invalid type");
	}

    return true;
}