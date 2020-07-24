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

bool ByteCodeGenJob::emitCompareTypeInfos(ByteCodeGenContext* context, uint32_t r0, uint32_t r1, uint32_t r2)
{
    emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
    emitInstruction(context, ByteCodeOp::JumpIfTrue, r2);
    auto instBig = context->bc->numInstructions;

    emitInstruction(context, ByteCodeOp::JumpIfZero64, r0);
    auto instBig1 = context->bc->numInstructions;

    emitInstruction(context, ByteCodeOp::JumpIfZero64, r1);
    auto instBig2 = context->bc->numInstructions;

    // Typeinfo pointers are not equal. Need to do the full check with the names
    // (names are supposed to be the same even across all modules)
    RegisterList tmpReg;
    reserveRegisterRC(context, tmpReg, 4);
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, tmpReg[0], r0);
    emitInstruction(context, ByteCodeOp::DeRefStringSlice, tmpReg[0], tmpReg[1]);
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, tmpReg[2], r1);
    emitInstruction(context, ByteCodeOp::DeRefStringSlice, tmpReg[2], tmpReg[3]);

    // Compare name lengths : if not true, exit the test
    emitInstruction(context, ByteCodeOp::CompareOpEqual64, tmpReg[1], tmpReg[3], r2);
    emitInstruction(context, ByteCodeOp::JumpIfNotTrue, r2);
    auto instLength = context->bc->numInstructions;

    // Compare names
    emitInstruction(context, ByteCodeOp::CompareOpEqualString, tmpReg[0], tmpReg[2], tmpReg[1]);
    emitInstruction(context, ByteCodeOp::CopyRBtoRA, r2, tmpReg[1]);

    // Jump here when comparison is done
    context->bc->out[instLength - 1].b.s32 = context->bc->numInstructions - instLength;

    freeRegisterRC(context, tmpReg);
    context->bc->out[instBig - 1].b.s32  = context->bc->numInstructions - instBig;
    context->bc->out[instBig1 - 1].b.s32 = context->bc->numInstructions - instBig1;
    context->bc->out[instBig2 - 1].b.s32 = context->bc->numInstructions - instBig2;

    return true;
}

bool ByteCodeGenJob::emitCompareOpEqual(ByteCodeGenContext* context, AstNode* left, AstNode* right, RegisterList& r0, RegisterList& r1, RegisterList& r2)
{
    auto typeInfo = TypeManager::concreteType(left->typeInfo);

    if (typeInfo->kind == TypeInfoKind::Native)
    {
        switch (typeInfo->nativeType)
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
        case NativeTypeKind::U64:
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
            return true;
        case NativeTypeKind::String:
            // First compare string sizes
            emitInstruction(context, ByteCodeOp::CompareOpEqual32, r0[1], r1[1], r2);
            emitInstruction(context, ByteCodeOp::JumpIfNotTrue, r2)->b.s32 = 2;
            // Then compare strings
            emitInstruction(context, ByteCodeOp::CopyRBtoRA, r2, r0[1]);
            emitInstruction(context, ByteCodeOp::CompareOpEqualString, r0, r1, r2);
            return true;
        default:
            return internalError(context, "emitCompareOpEqual, type not supported");
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        // Special case for typeinfos, as this is not safe to just compare pointers.
        // The same typeinfo can be different if defined in two different modules, so we need
        // to make a compare by name too
        typeInfo->computeScopedName();
        if (typeInfo->flags & TYPEINFO_TYPEINFO_PTR)
        {
            SWAG_CHECK(emitCompareTypeInfos(context, r0[0], r1[0], r2[0]));
        }

        // Simple pointer compare
        else
        {
            emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
        }
    }
    else if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CompareOpEqual64, r0, r1, r2);
    }
    else if (typeInfo->kind == TypeInfoKind::Interface)
    {
        emitInstruction(context, ByteCodeOp::CompareOpEqualInterface, r0, r1, r2);
    }
    else if (typeInfo->kind == TypeInfoKind::Slice)
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
    auto     r0   = node->childs[0]->resultRegisterRC;
    auto     r1   = node->childs[1]->resultRegisterRC;

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

    if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
    {
        SWAG_CHECK(emitUserOp(context));
        if (context->result == ContextResult::Pending)
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
        RegisterList r2;
        reserveRegisterRC(context, r2, 1);
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
        auto inst              = emitInstruction(context, ByteCodeOp::MakeConstantSegPointer, node->resultRegisterRC);
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