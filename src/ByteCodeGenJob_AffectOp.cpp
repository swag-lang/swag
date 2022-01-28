#include "pch.h"
#include "Ast.h"
#include "ByteCodeGenJob.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "TypeManager.h"
#include "ErrorIds.h"
#include "Diagnostic.h"

bool ByteCodeGenJob::emitCopyArray(ByteCodeGenContext* context, TypeInfo* typeInfo, RegisterList& dstReg, RegisterList& srcReg, AstNode* from)
{
    auto typeArray = CastTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
    if (typeArray->finalType->kind != TypeInfoKind::Struct)
    {
        emitMemCpy(context, dstReg, srcReg, typeArray->sizeOf);
        return true;
    }

    auto typeStruct = CastTypeInfo<TypeInfoStruct>(typeArray->finalType, TypeInfoKind::Struct);
    if (typeStruct->flags & TYPEINFO_STRUCT_NO_COPY)
    {
        return context->report(Hint::isType(typeArray), {from, Fmt(Err(Err0231), typeStruct->getDisplayNameC())});
    }

    if ((from->flags & AST_NO_LEFT_DROP) || (!typeStruct->opDrop && !typeStruct->opUserDropFct))
    {
        if (typeStruct->canRawCopy())
        {
            emitMemCpy(context, dstReg, srcReg, typeArray->sizeOf);
            return true;
        }
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
    TypeInfo* fromTypeInfo = from ? from->typeInfo : typeInfo;

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
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    if (typeInfo->isLambda())
    {
        SWAG_ASSERT(!fromTypeInfo->isClosure());
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    if (typeInfo->isClosure() && fromTypeInfo->isLambda())
    {
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = sizeof(void*);
        return true;
    }

    if (typeInfo->isClosure() && fromTypeInfo->isClosure())
    {
        SWAG_ASSERT(from);
        if (from->kind == AstNodeKind::MakePointerLambda)
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
            auto inst = emitInstruction(context, ByteCodeOp::SetAtPointer64, r0);
            inst->flags |= BCI_IMM_B;
            inst->b.u32 = 2 * sizeof(void*); // Offset to the capture storage (2 pointers from the start)
            inst->c.u32 = sizeof(void*);

            // Copy closure capture buffer
            emitInstruction(context, ByteCodeOp::Add64byVB64, r0)->b.u64 = 2 * sizeof(void*);
            emitMemCpy(context, r0, r1[1], SWAG_LIMIT_CLOSURE_SIZEOF - 2 * sizeof(void*));
        }
        else
        {
            emitMemCpy(context, r0, r1, typeInfo->sizeOf);
        }

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Interface)
    {
        if (fromTypeInfo == g_TypeMgr->typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
        }
        else
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        }

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        if (fromTypeInfo == g_TypeMgr->typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
        }
        else if (node->childs.size() > 1 && node->childs[1]->typeInfo->kind == TypeInfoKind::Array)
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);

            auto typeArray = CastTypeInfo<TypeInfoArray>(node->childs[1]->typeInfo, TypeInfoKind::Array);
            auto r2        = reserveRegisterRC(context);

            emitInstruction(context, ByteCodeOp::SetImmediate64, r2)->b.u64     = typeArray->count;
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r2)->c.u32 = 8;

            freeRegisterRC(context, r2);
        }
        else
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        }

        return true;
    }

    if (typeInfo->isNative(NativeTypeKind::String))
    {
        if (fromTypeInfo == g_TypeMgr->typeInfoNull)
        {
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0);
            emitInstruction(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
        }
        else
        {
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        }

        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectEqual, type not native");

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
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::SetAtPointer32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    case NativeTypeKind::Any:
    {
        auto r2 = reserveRegisterRC(context);
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
        emitInstruction(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        freeRegisterRC(context, r2);
        return true;
    }
    default:
        return context->internalError("emitAffectEqual, type not supported");
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
        case NativeTypeKind::Rune:
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
            return context->internalError("emitAffectPlusEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(leftTypeInfo), TypeInfoKind::Pointer);
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB64, r1)->b.u64 = sizeOf;
        emitInstruction(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
        return true;
    }

    return context->internalError("emitAffectPlusEqual, type invalid");
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
        case NativeTypeKind::Rune:
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
            return context->internalError("emitAffectMinusEqual, type not supported");
        }
    }
    else if (leftTypeInfo->kind == TypeInfoKind::Pointer)
    {
        auto typePtr = CastTypeInfo<TypeInfoPointer>(TypeManager::concreteType(leftTypeInfo), TypeInfoKind::Pointer);
        auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            emitInstruction(context, ByteCodeOp::Mul64byVB64, r1)->b.u64 = sizeOf;
        emitInstruction(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
        return true;
    }

    return context->internalError("emitAffectMinusEqual, type invalid");
}

bool ByteCodeGenJob::emitAffectMulEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectMulEqual, type not native");

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
    case NativeTypeKind::Rune:
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
        return context->internalError("emitAffectMulEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectAndEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectAndEqual, type not native");

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
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpAndEqS64, r0, r1);
        return true;
    default:
        return context->internalError("emitAffectAndEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectOrEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectOrEqual, type not native");

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
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpOrEqS64, r0, r1);
        return true;
    default:
        return context->internalError("emitAffectOrEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectXorEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectXorEqual, type not native");

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqU8, r0, r1);
        return true;
    case NativeTypeKind::S16:
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqU16, r0, r1);
        return true;
    case NativeTypeKind::S32:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqU32, r0, r1);
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpXorEqU64, r0, r1);
        return true;
    default:
        return context->internalError("emitAffectXorEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectShiftLeftEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectShiftLeftEqual, type not native");

    emitSafetyLeftShiftEq(context, r0, r1, typeInfo);

    auto     opNode     = CastAst<AstOp>(context->node, AstNodeKind::AffectOp);
    uint16_t shiftFlags = 0;
    if (opNode->specFlags & AST_SPEC_OP_SMALL)
        shiftFlags |= BCI_SHIFT_SMALL;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU8, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU16, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU32, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU64, r0, r1)->flags |= shiftFlags;
        return true;

    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU8, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU16, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU32, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::Int:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpShiftLeftEqU64, r0, r1)->flags |= shiftFlags;
        return true;
    default:
        return context->internalError("emitAffectShiftLeftEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectShiftRightEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectShiftRightEqual, type not native");

    emitSafetyRightShiftEq(context, r0, r1, typeInfo);

    auto     opNode     = CastAst<AstOp>(context->node, AstNodeKind::AffectOp);
    uint16_t shiftFlags = 0;
    if (opNode->specFlags & AST_SPEC_OP_SMALL)
        shiftFlags |= BCI_SHIFT_SMALL;

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS8, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS16, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS32, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqS64, r0, r1)->flags |= shiftFlags;
        return true;

    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU8, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU16, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU32, r0, r1)->flags |= shiftFlags;
        return true;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::AffectOpShiftRightEqU64, r0, r1)->flags |= shiftFlags;
        return true;
    default:
        return context->internalError("emitAffectShiftRightEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectPercentEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectPercentEqual, type not native");

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
    case NativeTypeKind::Rune:
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
        return context->internalError("emitAffectPercentEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    AstNode* node     = context->node;
    auto     typeInfo = TypeManager::concreteType(node->childs.front()->typeInfo);
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitAffectDivEqual, type not native");

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
    case NativeTypeKind::Rune:
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
        return context->internalError("emitAffectDivEqual, type not supported");
    }
}

bool ByteCodeGenJob::emitAffect(ByteCodeGenContext* context)
{
    auto     job       = context->job;
    AstNode* node      = context->node;
    AstNode* leftNode  = context->node->childs[0];
    AstNode* rightNode = context->node->childs[1];

    if (!(node->doneFlags & AST_DONE_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->childs[1], TypeManager::concreteType(node->childs[1]->typeInfo), node->childs[1]->castedTypeInfo));
        if (context->result == ContextResult::Pending)
            return true;
        node->doneFlags |= AST_DONE_CAST1;
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
        SWAG_CHECK(emitAffectXorEqual(context, r0, r1));
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
        return context->internalError("emitAffect, invalid token op");
    }

    freeRegisterRC(context, r0);
    freeRegisterRC(context, r1);

    return true;
}