#include "pch.h"
#include "Global.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeInfo.h"
#include "SourceFile.h"
#include "Module.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"

bool ByteCodeGenJob::emitAffectEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::Bool:
    case NativeType::S8:
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOp8, r0, r1);
        return true;
    case NativeType::S16:
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOp16, r0, r1);
        return true;
    case NativeType::S32:
    case NativeType::U32:
    case NativeType::F32:
    case NativeType::Char:
        emitInstruction(context, ByteCodeOp::AffectOp32, r0, r1);
        return true;
    case NativeType::S64:
    case NativeType::U64:
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::AffectOp64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectPlusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectPlusEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqS8, r0, r1);
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqU8, r0, r1);
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqS16, r0, r1);
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqU16, r0, r1);
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqS32, r0, r1);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqU32, r0, r1);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqU64, r0, r1);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqF32, r0, r1);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqF64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectMinusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectMinusEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqS8, r0, r1);
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqU8, r0, r1);
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqS16, r0, r1);
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqU16, r0, r1);
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqS32, r0, r1);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqU32, r0, r1);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqU64, r0, r1);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqF32, r0, r1);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqF64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectMinusEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectMulEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectMulEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS8, r0, r1);
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU8, r0, r1);
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS16, r0, r1);
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU16, r0, r1);
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS32, r0, r1);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU32, r0, r1);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS64, r0, r1);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU64, r0, r1);
        return true;
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqF32, r0, r1);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqF64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectMulEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectAndEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectAndEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS8, r0, r1);
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqU8, r0, r1);
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS16, r0, r1);
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqU16, r0, r1);
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS32, r0, r1);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqU32, r0, r1);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS64, r0, r1);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqU64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectAndEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectOrEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::S8:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS8, r0, r1);
        return true;
    case NativeType::U8:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqU8, r0, r1);
        return true;
    case NativeType::S16:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS16, r0, r1);
        return true;
    case NativeType::U16:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqU16, r0, r1);
        return true;
    case NativeType::S32:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS32, r0, r1);
        return true;
    case NativeType::U32:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqU32, r0, r1);
        return true;
    case NativeType::S64:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS64, r0, r1);
        return true;
    case NativeType::U64:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqU64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectOrEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs[0]->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectDivEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeType::F32:
        emitInstruction(context, ByteCodeOp::AffectOpDivEqF32, r0, r1);
        return true;
    case NativeType::F64:
        emitInstruction(context, ByteCodeOp::AffectOpDivEqF64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectDivEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffect(ByteCodeGenContext* context)
{
    AstNode* node   = context->node;
    auto     module = context->sourceFile->module;

    auto r0 = node->childs[0]->resultRegisterRC;
    auto r1 = node->childs[1]->resultRegisterRC;
    module->freeRegisterRC(r0);
    module->freeRegisterRC(r1);

    emitCast(context, node->childs[1]->castedTypeInfo, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo));
    switch (node->token.id)
    {
    case TokenId::SymEqual:
        SWAG_CHECK(emitAffectEqual(context, r0, r1));
        return true;
    case TokenId::SymPlusEqual:
        SWAG_CHECK(emitAffectPlusEqual(context, r0, r1));
        return true;
    case TokenId::SymMinusEqual:
        SWAG_CHECK(emitAffectMinusEqual(context, r0, r1));
        return true;
    case TokenId::SymAsteriskEqual:
        SWAG_CHECK(emitAffectMulEqual(context, r0, r1));
        return true;
    case TokenId::SymSlashEqual:
        SWAG_CHECK(emitAffectDivEqual(context, r0, r1));
        return true;
    case TokenId::SymAmpersandEqual:
        SWAG_CHECK(emitAffectAndEqual(context, r0, r1));
        return true;
    case TokenId::SymVerticalEqual:
        SWAG_CHECK(emitAffectOrEqual(context, r0, r1));
        return true;
    default:
        return internalError(context, "emitAffect, invalid token op");
    }
}