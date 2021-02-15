#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "TypeManager.h"

bool ByteCodeGenJob::emitCopyArray(ByteCodeGenContext* context, TypeInfo* typeInfo, RegisterList& dstReg, RegisterList& srcReg, AstNode* from)
{
    auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
    auto finalType = typeArray->finalType;

    // Copy of relative types
    if (finalType->flags & TYPEINFO_RELATIVE)
    {
        RegisterList rloop   = reserveRegisterRC(context);
        RegisterList toReg   = reserveRegisterRC(context);
        RegisterList fromReg = reserveRegisterRC(context);
        RegisterList rtmp    = reserveRegisterRC(context);

        emitInstruction(context, ByteCodeOp::CopyRBtoRA, toReg, dstReg);
        emitInstruction(context, ByteCodeOp::CopyRBtoRA, fromReg, srcReg);

        auto inst     = emitInstruction(context, ByteCodeOp::SetImmediate64, rloop);
        inst->b.u64   = typeArray->totalCount;
        auto seekJump = context->bc->numInstructions;

        emitInstruction(context, ByteCodeOp::CopyRBtoRA, rtmp, fromReg);
        SWAG_CHECK(emitUnwrapRelativePointer(context, rtmp, finalType->relative));
        SWAG_CHECK(emitWrapRelativePointer(context, toReg, rtmp, finalType->relative, finalType));

        inst        = emitInstruction(context, ByteCodeOp::IncPointer64, toReg, 0, toReg);
        inst->b.u64 = finalType->sizeOf;
        inst->flags |= BCI_IMM_B;
        inst        = emitInstruction(context, ByteCodeOp::IncPointer64, fromReg, 0, fromReg);
        inst->b.u64 = finalType->sizeOf;
        inst->flags |= BCI_IMM_B;

        emitInstruction(context, ByteCodeOp::DecrementRA64, rloop);
        emitInstruction(context, ByteCodeOp::JumpIfNotZero64, rloop)->b.s32 = seekJump - context->bc->numInstructions - 1;

        freeRegisterRC(context, rtmp);
        freeRegisterRC(context, rloop);
        freeRegisterRC(context, toReg);
        freeRegisterRC(context, fromReg);
        return true;
    }

    if (typeArray->finalType->kind != TypeInfoKind::Struct)
    {
        emitMemCpy(context, dstReg, srcReg, typeArray->sizeOf);
        return true;
    }

    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct);
    if (typeStruct->canRawCopy())
    {
        emitMemCpy(context, dstReg, srcReg, typeArray->sizeOf);
        return true;
    }

    if (typeArray->totalCount == 1)
    {
        SWAG_CHECK(emitCopyStruct(context, dstReg, srcReg, typeStruct, from));
        return true;
    }

    // Need to loop on every element of the array in order to initialize them
    RegisterList r0 = reserveRegisterRC(context);

    auto inst     = emitInstruction(context, ByteCodeOp::SetImmediate64, r0);
    inst->b.u64   = typeArray->totalCount;
    auto seekJump = context->bc->numInstructions;

    SWAG_CHECK(emitCopyStruct(context, dstReg, srcReg, typeStruct, from));

    inst        = emitInstruction(context, ByteCodeOp::IncPointer64, dstReg, 0, dstReg);
    inst->b.u64 = typeStruct->sizeOf;
    inst->flags |= BCI_IMM_B;
    inst        = emitInstruction(context, ByteCodeOp::IncPointer64, srcReg, 0, srcReg);
    inst->b.u64 = typeStruct->sizeOf;
    inst->flags |= BCI_IMM_B;

    emitInstruction(context, ByteCodeOp::DecrementRA64, r0);
    emitInstruction(context, ByteCodeOp::JumpIfNotZero64, r0)->b.s32 = seekJump - context->bc->numInstructions - 1;

    freeRegisterRC(context, r0);
    return true;
}

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
        context->job->waitStructGenerated(typeInfo);
        if (context->result == ContextResult::Pending)
            return true;
        SWAG_CHECK(emitCopyStruct(context, r0, r1, typeInfo, from));
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        context->job->waitStructGenerated(typeInfo);
        if (context->result == ContextResult::Pending)
            return true;

        // This is only valid because we cannot affect array by hand. It must comes from a function, with a var declaration,
        // so the variable on the left has not been initialized
        from->flags |= AST_NO_LEFT_DROP;
        SWAG_CHECK(emitCopyArray(context, typeInfo, r0, r1, from));
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::TypeListTuple || typeInfo->kind == TypeInfoKind::TypeListArray)
    {
        emitMemCpy(context, r0, r1, typeInfo->sizeOf);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        if (typeInfo->flags & TYPEINFO_RELATIVE)
            SWAG_CHECK(emitWrapRelativePointer(context, r0, r1, typeInfo->relative, fromTypeInfo));
        else
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
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64OffVB32, r0, 8);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::IncPointer64, r0, 8, r1[0])->flags |= BCI_IMM_B;
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r1[0], r1[1]);
        }

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        if (fromTypeInfo && fromTypeInfo == g_TypeMgr.typeInfoNull)
        {
            if (typeInfo->relative)
                emitWrapRelativePointer(context, r0, r1[0], typeInfo->relative, fromTypeInfo);
            else
                emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64OffVB32, r0, 8);
        }
        else if (node->childs.size() > 1 && node->childs[1]->typeInfo->kind == TypeInfoKind::Array)
        {
            if (typeInfo->relative)
                emitWrapRelativePointer(context, r0, r1[0], typeInfo->relative, fromTypeInfo);
            else
                emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);

            auto typeArray = CastTypeInfo<TypeInfoArray>(node->childs[1]->typeInfo, TypeInfoKind::Array);
            auto r2        = reserveRegisterRC(context);

            emitInstruction(context, ByteCodeOp::SetImmediate64, r2)->b.u64 = typeArray->count;
            emitInstruction(context, ByteCodeOp::IncPointer64, r0, 8, r1[0])->flags |= BCI_IMM_B;
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r1[0], r2);

            freeRegisterRC(context, r2);
        }
        else
        {
            if (typeInfo->relative)
                emitWrapRelativePointer(context, r0, r1[0], typeInfo->relative, fromTypeInfo);
            else
                emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::IncPointer64, r0, 8, r1[0])->flags |= BCI_IMM_B;
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r1[0], r1[1]);
        }

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::TypeSet)
    {
        auto r2 = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
        emitInstruction(context, ByteCodeOp::IncPointer64, r0, 8, r2)->flags |= BCI_IMM_B;
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r2, r1[1]);
        freeRegisterRC(context, r2);
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
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    case NativeTypeKind::String:
        if (fromTypeInfo && fromTypeInfo == g_TypeMgr.typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64OffVB32, r0, 8);
        }
        else
        {
            auto r2 = reserveRegisterRC(context);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::IncPointer64, r0, 8, r2)->flags |= BCI_IMM_B;
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r2, r1[1]);
            freeRegisterRC(context, r2);
        }
        return true;
    case NativeTypeKind::Any:
    {
        auto r2 = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
        emitInstruction(context, ByteCodeOp::IncPointer64, r0, 8, r2)->flags |= BCI_IMM_B;
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r2, r1[1]);
        freeRegisterRC(context, r2);
        return true;
    }
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
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS8, r0, r1);
            return true;
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqU8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS16, r0, r1);
            return true;
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqU16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS32, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqU32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqU64, r0, r1);
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
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB64, r1)->b.u64 = sizeOf;

        if (leftTypeInfo->flags & TYPEINFO_RELATIVE)
        {
            switch (leftTypeInfo->sizeOf)
            {
            case 1:
                emitInstruction(context, ByteCodeOp::AffectOpPlusEqS8, r0, r1);
                break;
            case 2:
                emitInstruction(context, ByteCodeOp::AffectOpPlusEqS16, r0, r1);
                break;
            case 4:
                emitInstruction(context, ByteCodeOp::AffectOpPlusEqS32, r0, r1);
                break;
            case 8:
                emitInstruction(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
                break;
            }
        }
        else
        {
            emitInstruction(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
        }

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
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS8, r0, r1);
            return true;
        case NativeTypeKind::U8:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqU8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS16, r0, r1);
            return true;
        case NativeTypeKind::U16:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqU16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS32, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Char:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqU32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::Int:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
            return true;
        case NativeTypeKind::U64:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqU64, r0, r1);
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
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB64, r1)->b.u64 = sizeOf;

        if (leftTypeInfo->flags & TYPEINFO_RELATIVE)
        {
            switch (leftTypeInfo->sizeOf)
            {
            case 1:
                emitInstruction(context, ByteCodeOp::AffectOpMinusEqS8, r0, r1);
                break;
            case 2:
                emitInstruction(context, ByteCodeOp::AffectOpMinusEqS16, r0, r1);
                break;
            case 4:
                emitInstruction(context, ByteCodeOp::AffectOpMinusEqS32, r0, r1);
                break;
            case 8:
                emitInstruction(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
                break;
            }
        }
        else
        {
            emitInstruction(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
        }

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
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS8, r0, r1);
        return true;
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU8, r0, r1);
        return true;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS16, r0, r1);
        return true;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU16, r0, r1);
        return true;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS32, r0, r1);
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqS64, r0, r1);
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpMulEqU64, r0, r1);
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
    case NativeTypeKind::Bool:
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
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
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
    case NativeTypeKind::Bool:
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
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectOrEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectXorEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectXorEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqS8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqS16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqS64, r0, r1);
        return true;
    default:
        return internalError(context, "emitAffectXorEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectShiftLeftEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitAffectShiftLeftEqual, type not native");

    emitSafetyLeftShiftEq(context, r0, r1, typeInfo);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU64, r0, r1);
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

    emitSafetyRightShiftEq(context, r0, r1, typeInfo);
    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
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
    case NativeTypeKind::Int:
        emitSafetyDivZero(context, r1, 64);
        emitInstruction(context, ByteCodeOp::AffectOpModuloEqS64, r0, r1);
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
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
    case NativeTypeKind::Int:
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
    case NativeTypeKind::UInt:
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

    // User special function
    if (node->hasSpecialFuncCall())
    {
        if (node->doneFlags & AST_DONE_FLAT_PARAMS)
        {
            auto arrayNode = CastAst<AstArrayPointerIndex>(leftNode->childs.back(), AstNodeKind::ArrayPointerIndex);
            if (!job->allParamsTmp)
                job->allParamsTmp = Ast::newFuncCallParams(node->sourceFile, nullptr);
            job->allParamsTmp->childs = arrayNode->structFlatParams;
            SWAG_CHECK(emitUserOp(context, job->allParamsTmp));
            if (context->result != ContextResult::Done)
                return true;
        }
        else
        {
            SWAG_CHECK(emitUserOp(context));
            if (context->result != ContextResult::Done)
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
    case TokenId::SymPlusPercentEqual:
        SWAG_CHECK(emitAffectPlusEqual(context, r0, r1));
        break;
    case TokenId::SymMinusEqual:
    case TokenId::SymMinusPercentEqual:
        SWAG_CHECK(emitAffectMinusEqual(context, r0, r1));
        break;
    case TokenId::SymAsteriskEqual:
    case TokenId::SymAsteriskPercentEqual:
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
        SWAG_CHECK(emitAffectXorEqual(context, r0, r1));
        break;
    case TokenId::SymLowerLowerEqual:
    case TokenId::SymLowerLowerPercentEqual:
        SWAG_CHECK(emitAffectShiftLeftEqual(context, r0, r1));
        break;
    case TokenId::SymGreaterGreaterEqual:
    case TokenId::SymGreaterGreaterPercentEqual:
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