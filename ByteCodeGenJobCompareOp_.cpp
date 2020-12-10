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
#include "Runtime.h"

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto leftTypeInfo  = TypeManager::concreteReferenceType(left->typeInfo);
    auto rightTypeInfo = TypeManager::concreteReferenceType(right->typeInfo);

    if (leftTypeInfo->kind == TypeInfoKind::Native)
    {
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
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
        case NativeTypeKind::Int:
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
            return true;
        case NativeTypeKind::String:
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r2, r1[1]);
            emitInstruction(context, ByteCodeOp::IntrinsicStrCmp, r0[0], r0[1], r1[0], r2);
            return true;
        default:
            return internalError(context, "emitCompareOpEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        // Special case for typeinfos, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        if (leftTypeInfo->isPointerToTypeInfo() || rightTypeInfo->isPointerToTypeInfo())
        {
            auto rflags = reserveRegisterRC(context);
            auto inst   = emitInstruction(context, ByteCodeOp::SetImmediate32, rflags);
            inst->b.u32 = Runtime::COMPARE_STRICT;
            inst        = emitInstruction(context, ByteCodeOp::IntrinsicTypeCmp, r0, r1, rflags, r2);
            freeRegisterRC(context, rflags);
        }

        // Simple pointer compare
        else
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Interface)
    {
        // Just compare pointers. This is enough for now, as we can only compare an interface to 'null'
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Slice)
    {
        // Just compare pointers. This is enough for now, as we can only compare a slice to 'null'
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0[1], r1[1], r2);
    }
    else
    {
        return internalError(context, "emitCompareOpEqual, invalid type");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto node  = context->node;
    auto left  = node->childs.front();
    auto right = node->childs.back();
    SWAG_CHECK(emitCompareOpEqual(context, left, right, r0, r1, r2));
    return true;
}

bool ByteCodeGenJob::emitCompareOp3Way(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CompareOp3WayS32, r0, r1, r2);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::CompareOp3WayU32, r0, r1, r2);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOp3WayS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CompareOp3WayU64, r0, r1, r2);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::CompareOp3WayF32, r0, r1, r2);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOp3WayF64, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitCompareOp3Way, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::CompareOp3WayU64, r0, r1, r2);
    }
    else
    {
        return internalError(context, "emitCompareOp3Way, type not native");
    }

    return true;
}

bool ByteCodeGenJob::emitCompareOpLower(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteReferenceType(node->childs[0]->typeInfo);
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
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOpLowerS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
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
        emitInstruction(context, ByteCodeOp::CompareOpLowerU64, r0, r1, r2);
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
    auto     typeInfo = TypeManager::concreteReferenceType(node->childs[0]->typeInfo);
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
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::CompareOpGreaterS64, r0, r1, r2);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
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
        emitInstruction(context, ByteCodeOp::CompareOpGreaterU64, r0, r1, r2);
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
    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[0], TypeManager::concreteType(node->childs[0]->typeInfo), node->childs[0]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
    }

    if (!(node->doneFlags & AST_DONE_CAST2))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST2;
    }

    if (node->resolvedUserOpSymbolOverload && node->resolvedUserOpSymbolOverload->symbol->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result != ContextResult::Done)
            return true;

        auto r2 = node->resultRegisterRC;
        switch (node->token.id)
        {
        case TokenId::SymLower:
            emitInstruction(context, ByteCodeOp::LowerZeroToTrue, r2);
            break;
        case TokenId::SymGreater:
            emitInstruction(context, ByteCodeOp::GreaterZeroToTrue, r2);
            break;
        case TokenId::SymLowerEqual:
            emitInstruction(context, ByteCodeOp::LowerEqZeroToTrue, r2);
            break;
        case TokenId::SymGreaterEqual:
            emitInstruction(context, ByteCodeOp::GreaterEqZeroToTrue, r2);
            break;
        case TokenId::SymExclamEqual:
            emitInstruction(context, ByteCodeOp::NegBool, r2);
            break;
        }
    }
    else
    {
        auto& r0 = node->childs[0]->resultRegisterRC;
        auto& r1 = node->childs[1]->resultRegisterRC;

        RegisterList r2        = reserveRegisterRC(context);
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
        case TokenId::SymLowerEqualGreater:
            SWAG_CHECK(emitCompareOp3Way(context, r0, r1, r2));
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
        auto inst              = emitInstruction(context, ByteCodeOp::MakeTypeSegPointer, node->resultRegisterRC);
        inst->b.u32            = right->computedValue.reg.u32;
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, node->resultRegisterRC, left->resultRegisterRC[1], node->resultRegisterRC);
        freeRegisterRC(context, left);
        freeRegisterRC(context, right);
    }
    else
    {
        return internalError(context, "emitIs, invalid type");
    }

    return true;
}