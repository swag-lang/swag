#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "Module.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "SymTable.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitAffectEqual(ByteCodeGenContext* context, RegisterList& r0, RegisterList& r1, TypeInfo* forcedTypeInfo, AstNode* from)
{
    AstNode*  node         = context->node;
    auto      typeInfo     = forcedTypeInfo ? forcedTypeInfo : node->childs.front()->typeInfo;
    TypeInfo* fromTypeInfo = from ? from->typeInfo : nullptr;

    if (typeInfo->kind == TypeInfoKind::Reference)
    {
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    typeInfo = TypeManager::concreteReferenceType(typeInfo);

    if (typeInfo->kind == TypeInfoKind::Struct)
    {
        waitStructGenerated(context, CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Struct));
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(emitStructCopyMoveCall(context, r0, r1, typeInfo, from));
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Array || typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        emitInstruction(context, ByteCodeOp::MemCpyVC32, r0, r1)->c.u32 = typeInfo->sizeOf;
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Interface)
    {
        if (fromTypeInfo && fromTypeInfo == g_TypeMgr.typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0, 8);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::MemCpyVC32, r0, r1)->c.u32 = 2 * sizeof(void*);
        }

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        if (fromTypeInfo && fromTypeInfo == g_TypeMgr.typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0, 8);
        }
        else if (node->childs.size() > 1 && node->childs[1]->typeInfo->kind == TypeInfoKind::Array)
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);

            auto typeArray = CastTypeInfo<TypeInfoArray>(node->childs[1]->typeInfo, TypeInfoKind::Array);
            auto r2        = reserveRegisterRC(context);

            emitInstruction(context, ByteCodeOp::CopyVBtoRA64, r2)->b.u64 = typeArray->count;
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r2, 8);

            freeRegisterRC(context, r2);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1], 8);
        }

        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::SetAtPointer8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::SetAtPointer16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::F32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::SetAtPointer32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    case NativeTypeKind::String:
        if (fromTypeInfo && fromTypeInfo == g_TypeMgr.typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0, 8);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1], 8);
        }
        return true;
    case NativeTypeKind::Any:
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1], 8);
        return true;
    default:
        return internalError(context, "emitAffectEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectPlusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node         = context->node;
    auto     leftTypeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (leftTypeInfo->kind == TypeInfoKind::Native)
    {
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS8, r0, r1);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS16, r0, r1);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqF32, r0, r1);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqF64, r0, r1);
            return true;
        default:
            return internalError(context, "emitAffectPlusEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(leftTypeInfo), TypeInfoKind::Pointer);
        int  sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB32, r1)->b.u32 = sizeOf;
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqPointer, r0, r1);
        return true;
    }

    return internalError(context, "emitAffectPlusEqual, type invalid");
}

bool ByteCodeGenJob::emitAffectMinusEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node         = context->node;
    auto     leftTypeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (leftTypeInfo->kind == TypeInfoKind::Native)
    {
        switch (leftTypeInfo->nativeType)
        {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS8, r0, r1);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS16, r0, r1);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
            return true;
        case NativeTypeKind::F32:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqF32, r0, r1);
            return true;
        case NativeTypeKind::F64:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqF64, r0, r1);
            return true;
        default:
            return internalError(context, "emitAffectMinusEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(leftTypeInfo), TypeInfoKind::Pointer);
        int  sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB32, r1)->b.u32 = sizeOf;
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqPointer, r0, r1);
        return true;
    }

    return internalError(context, "emitAffectMinusEqual, type invalid");
}

bool ByteCodeGenJob::emitAffectMulEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectMulEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS64, r0, r1);
        return true;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqF32, r0, r1);
        return true;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqF64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectMulEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectAndEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectAndEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectAndEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectOrEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectOrEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectXOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectXOrEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpXOrEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpXOrEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpXOrEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::AffectOpXOrEqS64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectXOrEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectShiftLeftEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectShiftLeftEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqS64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectShiftLeftEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectShiftRightEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectShiftRightEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS8, r0, r1);
        return true;
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU8, r0, r1);
        return true;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS16, r0, r1);
        return true;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU16, r0, r1);
        return true;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS32, r0, r1);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU32, r0, r1);
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS64, r0, r1);
        return true;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectShiftRightEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectPercentEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectPercentEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitSafetyDivZero(context, r1, 8);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqS8, r0, r1);
        return true;
    case NativeTypeKind::U8:
        emitSafetyDivZero(context, r1, 8);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqU8, r0, r1);
        return true;
    case NativeTypeKind::S16:
        emitSafetyDivZero(context, r1, 16);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqS16, r0, r1);
        return true;
    case NativeTypeKind::U16:
        emitSafetyDivZero(context, r1, 16);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqU16, r0, r1);
        return true;
    case NativeTypeKind::S32:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqS32, r0, r1);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqU32, r0, r1);
        return true;
    case NativeTypeKind::S64:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqS64, r0, r1);
        return true;
    case NativeTypeKind::U64:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqU64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectPercentEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectDivEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitSafetyDivZero(context, r1, 8);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
        emitSafetyDivZero(context, r1, 16);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqS64, r0, r1);
        return true;
    case NativeTypeKind::U8:
        emitSafetyDivZero(context, r1, 8);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqU8, r0, r1);
        return true;
    case NativeTypeKind::U16:
        emitSafetyDivZero(context, r1, 16);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqU16, r0, r1);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqU32, r0, r1);
        return true;
    case NativeTypeKind::U64:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqU64, r0, r1);
        return true;
    case NativeTypeKind::F32:
        emitSafetyDivZero(context, r1, 32);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqF32, r0, r1);
        return true;
    case NativeTypeKind::F64:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::AffectOpDivEqF64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectDivEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffect(ByteCodeGenContext* context)
{
    auto     job       = context->job;
    AstNode* node      = context->node;
    AstNode* leftNode  = context->node->childs[0];
    AstNode* rightNode = context->node->childs[1];

    if (!(node->flags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->flags |= AST_DONE_CAST1;
    }

    if (leftNode->typeInfo->isNative(NativeTypeKind::Any))
    {
        leftNode->typeInfo = rightNode->typeInfo;
    }

    if (node->resolvedUserOpSymbolName && node->resolvedUserOpSymbolName->kind == SymbolKind::Function)
    {
        if (leftNode->kind == AstNodeKind::IdentifierRef && leftNode->childs.back()->kind == AstNodeKind::ArrayPointerIndex)
        {
            auto arrayNode = CastAst<AstArrayPointerIndex>(leftNode->childs.back(), AstNodeKind::ArrayPointerIndex);
            if (!job->allParamsTmp)
                job->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
            job->allParamsTmp->childs = arrayNode->structFlatParams;
            SWAG_CHECK(emitUserOp(context, job->allParamsTmp));
            if (context->result == ContextResult::Pending)
                return true;
        }
        else
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result == ContextResult::Pending)
                return true;
        }

        return true;
    }

    auto r0 = node->childs[0]->resultRegisterRC;
    auto r1 = node->childs[1]->resultRegisterRC;

    switch (node->token.id)
    {
    case TokenId::SymEqual:
        SWAG_CHECK(emitAffectEqual(context, r0, r1, nullptr, rightNode));
        if (context->result == ContextResult::Pending)
            return true;
        break;
    case TokenId::SymPlusEqual:
        SWAG_CHECK(emitAffectPlusEqual(context, r0, r1));
        break;
    case TokenId::SymMinusEqual:
        SWAG_CHECK(emitAffectMinusEqual(context, r0, r1));
        break;
    case TokenId::SymAsteriskEqual:
        SWAG_CHECK(emitAffectMulEqual(context, r0, r1));
        break;
    case TokenId::SymSlashEqual:
        SWAG_CHECK(emitAffectDivEqual(context, r0, r1));
        break;
    case TokenId::SymAmpersandEqual:
        SWAG_CHECK(emitAffectAndEqual(context, r0, r1));
        break;
    case TokenId::SymVerticalEqual:
        SWAG_CHECK(emitAffectOrEqual(context, r0, r1));
        break;
    case TokenId::SymCircumflexEqual:
        SWAG_CHECK(emitAffectXOrEqual(context, r0, r1));
        break;
    case TokenId::SymLowerLowerEqual:
        SWAG_CHECK(emitAffectShiftLeftEqual(context, r0, r1));
        break;
    case TokenId::SymGreaterGreaterEqual:
        SWAG_CHECK(emitAffectShiftRightEqual(context, r0, r1));
        break;
    case TokenId::SymPercentEqual:
        SWAG_CHECK(emitAffectPercentEqual(context, r0, r1));
        break;
    default:
        return internalError(context, "emitAffect, invalid token op");
    }

    freeRegisterRC(context, r0);
    freeRegisterRC(context, r1);

    return true;
}