#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Diagnostic.h"
#include "Report/ErrorIds.h"
#include "Report/Report.h"
#include "Semantic/Semantic.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"

bool ByteCodeGen::emitCopyArray(ByteCodeGenContext* context, TypeInfo* typeInfo, const RegisterList& dstReg, const RegisterList& srcReg, AstNode* from)
{
    const auto typeArray = castTypeInfo<TypeInfoArray>(typeInfo, TypeInfoKind::Array);
    const auto finalType = TypeManager::concreteType(typeArray->finalType);
    if (!finalType->isStruct())
    {
        const auto fromType = TypeManager::concreteType(from->typeInfo);
        if (fromType->isArray() || fromType->isListArray())
        {
            emitMemCpy(context, dstReg, srcReg, typeArray->sizeOf);
            return true;
        }

        if (!finalType->sizeOf)
            return true;

        RegisterList r0;
        reserveRegisterRC(context, r0, 2);

        auto inst   = EMIT_INST3(context, ByteCodeOp::IncPointer64, dstReg, 0, r0[0]);
        inst->b.u64 = static_cast<uint64_t>(typeArray->totalCount) * finalType->sizeOf;
        inst->addFlag(BCI_IMM_B);
        const auto seekJump = context->bc->numInstructions;

        switch (fromType->sizeOf)
        {
            case 1:
                EMIT_INST2(context, ByteCodeOp::SetAtPointer8, dstReg, srcReg);
                break;
            case 2:
                EMIT_INST2(context, ByteCodeOp::SetAtPointer16, dstReg, srcReg);
                break;
            case 4:
                EMIT_INST2(context, ByteCodeOp::SetAtPointer32, dstReg, srcReg);
                break;
            case 8:
                EMIT_INST2(context, ByteCodeOp::SetAtPointer64, dstReg, srcReg);
                break;
            case 16:
                EMIT_INST2(context, ByteCodeOp::SetAtPointer64, dstReg, srcReg[0]);
                EMIT_INST3(context, ByteCodeOp::SetAtPointer64, dstReg, srcReg[1], sizeof(void*));
                break;
            default:
                Report::internalError(from, "unsupported array initialization value type");
                return false;
        }

        inst        = EMIT_INST3(context, ByteCodeOp::IncPointer64, dstReg, 0, dstReg);
        inst->b.u64 = fromType->sizeOf;
        inst->addFlag(BCI_IMM_B);

        EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, dstReg, r0[0], r0[1]);
        inst        = EMIT_INST1(context, ByteCodeOp::JumpIfFalse, r0[1]);
        inst->b.s32 = static_cast<int32_t>(seekJump - context->bc->numInstructions);

        freeRegisterRC(context, r0);
        return true;
    }

    const auto typeStruct = castTypeInfo<TypeInfoStruct>(finalType, TypeInfoKind::Struct);
    if (typeStruct->hasFlag(TYPEINFO_STRUCT_NO_COPY))
    {
        Diagnostic err{from, formErr(Err0090, typeStruct->getDisplayNameC()), Diagnostic::isType(typeArray)};
        err.addNote(context->node->token, toNte(Nte0070));
        return context->report(err);
    }

    if (from->hasAstFlag(AST_NO_LEFT_DROP) || (!typeStruct->opDrop && !typeStruct->opUserDropFct))
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

    if (typeStruct->sizeOf)
    {
        // Need to loop on every element of the array to initialize them
        RegisterList r0;
        reserveRegisterRC(context, r0, 2);

        auto inst   = EMIT_INST3(context, ByteCodeOp::IncPointer64, dstReg, 0, r0[0]);
        inst->b.u64 = static_cast<uint64_t>(typeArray->totalCount) * finalType->sizeOf;
        inst->addFlag(BCI_IMM_B);
        const auto seekJump = context->bc->numInstructions;

        SWAG_CHECK(emitCopyStruct(context, dstReg, srcReg, typeStruct, from));

        inst        = EMIT_INST3(context, ByteCodeOp::IncPointer64, dstReg, 0, dstReg);
        inst->b.u64 = typeStruct->sizeOf;
        inst->addFlag(BCI_IMM_B);
        inst        = EMIT_INST3(context, ByteCodeOp::IncPointer64, srcReg, 0, srcReg);
        inst->b.u64 = typeStruct->sizeOf;
        inst->addFlag(BCI_IMM_B);

        EMIT_INST3(context, ByteCodeOp::CompareOpEqual64, dstReg, r0[0], r0[1]);
        inst        = EMIT_INST1(context, ByteCodeOp::JumpIfFalse, r0[1]);
        inst->b.s32 = static_cast<int32_t>(seekJump - context->bc->numInstructions);

        freeRegisterRC(context, r0);
    }

    return true;
}

bool ByteCodeGen::emitAffectEqual(ByteCodeGenContext* context, const RegisterList& r0, const RegisterList& r1, TypeInfo* forcedTypeInfo, AstNode* from)
{
    const AstNode*  node         = context->node;
    auto            typeInfo     = forcedTypeInfo ? forcedTypeInfo : node->firstChild()->typeInfo;
    const TypeInfo* fromTypeInfo = from ? from->typeInfo : typeInfo;

    typeInfo = TypeManager::concreteType(typeInfo);
    if (node->childCount() && node->firstChild()->hasSemFlag(SEMFLAG_FROM_REF))
        typeInfo = TypeManager::concretePtrRefType(typeInfo);

    if (typeInfo->isStruct())
    {
        Semantic::waitStructGenerated(context->baseJob, typeInfo);
        YIELD();
        SWAG_CHECK(emitCopyStruct(context, r0, r1, typeInfo, from));
        return true;
    }

    if (typeInfo->isArray())
    {
        Semantic::waitStructGenerated(context->baseJob, typeInfo);
        YIELD();
        SWAG_CHECK(emitCopyArray(context, typeInfo, r0, r1, from));
        return true;
    }

    if (typeInfo->isListTuple() || typeInfo->isListArray())
    {
        emitMemCpy(context, r0, r1, typeInfo->sizeOf);
        return true;
    }

    if (typeInfo->isPointer())
    {
        EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    if (typeInfo->isLambda())
    {
        SWAG_ASSERT(!fromTypeInfo->isClosure());
        EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1);
        return true;
    }

    const auto aliasFrom = TypeManager::concreteType(fromTypeInfo);
    if (typeInfo->isClosure() && aliasFrom->isLambda())
    {
        EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1);
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = sizeof(void*);
        return true;
    }

    if (typeInfo->isClosure() && aliasFrom->isPointerNull())
    {
        EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0);
        return true;
    }

    if (typeInfo->isClosure() && aliasFrom->isClosure())
    {
        // Because we copy the full capture block, but only capture fields are initialized
        SWAG_ASSERT(from);

        // @ConvertToClosure
        if (from->is(AstNodeKind::MakePointerLambda))
        {
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1);
            const auto inst = EMIT_INST1(context, ByteCodeOp::SetAtPointer64, r0);
            inst->addFlag(BCI_IMM_B);
            inst->b.u32 = 1; // <> 0 for closure, 0 for lambda
            inst->c.u32 = sizeof(void*);

            // Copy closure capture buffer
            const auto nodeCapture = castAst<AstMakePointer>(from, AstNodeKind::MakePointerLambda);
            SWAG_ASSERT(nodeCapture->lambda->captureParameters);
            const auto typeBlock = castTypeInfo<TypeInfoStruct>(nodeCapture->lastChild()->typeInfo, TypeInfoKind::Struct);
            if (!typeBlock->fields.empty())
            {
                EMIT_INST1(context, ByteCodeOp::Add64byVB64, r0)->b.u64 = 2 * sizeof(void*);
                emitMemCpy(context, r0, r1[1], typeBlock->sizeOf);
            }
        }
        else
        {
            emitMemCpy(context, r0, r1, typeInfo->sizeOf);
        }

        return true;
    }

    if (typeInfo->isInterface())
    {
        if (fromTypeInfo->isPointerNull())
        {
            EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0);
            EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
        }
        else
        {
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        }

        return true;
    }

    if (typeInfo->isSlice())
    {
        if (fromTypeInfo->isPointerNull())
        {
            EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0);
            EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
        }
        else if (node->childCount() > 1 && node->secondChild()->typeInfo->isArray())
        {
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1);

            const auto typeArray = castTypeInfo<TypeInfoArray>(node->secondChild()->typeInfo, TypeInfoKind::Array);
            const auto r2        = reserveRegisterRC(context);

            EMIT_INST1(context, ByteCodeOp::SetImmediate64, r2)->b.u64     = typeArray->count;
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r2)->c.u32 = 8;

            freeRegisterRC(context, r2);
        }
        else
        {
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        }

        return true;
    }

    if (typeInfo->isString())
    {
        if (fromTypeInfo->isPointerNull())
        {
            EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0);
            EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
        }
        else
        {
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
        }

        return true;
    }

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitAffectEqual, type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
            EMIT_INST2(context, ByteCodeOp::SetAtPointer8, r0, r1);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::SetAtPointer16, r0, r1);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::F32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::SetAtPointer32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1);
            return true;
        case NativeTypeKind::Any:
        {
            if (fromTypeInfo->isPointerNull())
            {
                EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0);
                EMIT_INST1(context, ByteCodeOp::SetZeroAtPointer64, r0)->b.u32 = 8;
            }
            else
            {
                EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[0]);
                EMIT_INST2(context, ByteCodeOp::SetAtPointer64, r0, r1[1])->c.u32 = 8;
            }

            return true;
        }
        default:
            return Report::internalError(context->node, "emitAffectEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectPlusEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (leftTypeInfo->isNative())
    {
        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqS8, r0, r1);
                return true;
            case NativeTypeKind::U8:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqU8, r0, r1);
                return true;
            case NativeTypeKind::S16:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqS16, r0, r1);
                return true;
            case NativeTypeKind::U16:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqU16, r0, r1);
                return true;
            case NativeTypeKind::S32:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqS32, r0, r1);
                return true;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqU32, r0, r1);
                return true;
            case NativeTypeKind::S64:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
                return true;
            case NativeTypeKind::U64:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqU64, r0, r1);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqF32_Safe, r0, r1);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqF64_Safe, r0, r1);
                return true;
            default:
                return Report::internalError(context->node, "emitAffectPlusEqual, type not supported");
        }
    }

    if (leftTypeInfo->isPointer())
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(TypeManager::concreteType(leftTypeInfo), TypeInfoKind::Pointer);
        const auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, r1)->b.u64 = sizeOf;
        EMIT_INST2(context, ByteCodeOp::AffectOpPlusEqS64, r0, r1);
        return true;
    }

    return Report::internalError(context->node, "emitAffectPlusEqual, type invalid");
}

bool ByteCodeGen::emitAffectMinusEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (leftTypeInfo->isNative())
    {
        switch (leftTypeInfo->nativeType)
        {
            case NativeTypeKind::S8:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqS8, r0, r1);
                return true;
            case NativeTypeKind::U8:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqU8, r0, r1);
                return true;
            case NativeTypeKind::S16:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqS16, r0, r1);
                return true;
            case NativeTypeKind::U16:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqU16, r0, r1);
                return true;
            case NativeTypeKind::S32:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqS32, r0, r1);
                return true;
            case NativeTypeKind::U32:
            case NativeTypeKind::Rune:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqU32, r0, r1);
                return true;
            case NativeTypeKind::S64:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
                return true;
            case NativeTypeKind::U64:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqU64, r0, r1);
                return true;
            case NativeTypeKind::F32:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqF32_Safe, r0, r1);
                return true;
            case NativeTypeKind::F64:
                EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqF64_Safe, r0, r1);
                return true;
            default:
                return Report::internalError(context->node, "emitAffectMinusEqual, type not supported");
        }
    }

    if (leftTypeInfo->isPointer())
    {
        const auto typePtr = castTypeInfo<TypeInfoPointer>(TypeManager::concreteType(leftTypeInfo), TypeInfoKind::Pointer);
        const auto sizeOf  = typePtr->pointedType->sizeOf;
        if (sizeOf > 1)
            EMIT_INST1(context, ByteCodeOp::Mul64byVB64, r1)->b.u64 = sizeOf;
        EMIT_INST2(context, ByteCodeOp::AffectOpMinusEqS64, r0, r1);
        return true;
    }

    return Report::internalError(context->node, "emitAffectMinusEqual, type invalid");
}

bool ByteCodeGen::emitAffectMulEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectMulEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqS8, r0, r1);
            return true;
        case NativeTypeKind::U8:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqU8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqS16, r0, r1);
            return true;
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqU16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqS32, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqU32, r0, r1);
            return true;
        case NativeTypeKind::S64:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqS64, r0, r1);
            return true;
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqU64, r0, r1);
            return true;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqF32_Safe, r0, r1);
            return true;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::AffectOpMulEqF64_Safe, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectMulEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectAndEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectAndEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST2(context, ByteCodeOp::AffectOpAndEq8, r0, r1);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::AffectOpAndEq16, r0, r1);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::AffectOpAndEq32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::AffectOpAndEq64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectAndEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectOrEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectOrEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST2(context, ByteCodeOp::AffectOpOrEq8, r0, r1);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::AffectOpOrEq16, r0, r1);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::AffectOpOrEq32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::AffectOpOrEq64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectOrEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectXorEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectXorEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST2(context, ByteCodeOp::AffectOpXorEq8, r0, r1);
            return true;
        case NativeTypeKind::S16:
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::AffectOpXorEq16, r0, r1);
            return true;
        case NativeTypeKind::S32:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::AffectOpXorEq32, r0, r1);
            return true;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::AffectOpXorEq64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectXorEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectShiftLeftEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectShiftLeftEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqS8, r0, r1);
            return true;
        case NativeTypeKind::U8:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqU8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqS16, r0, r1);
            return true;
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqU16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqS32, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqU32, r0, r1);
            return true;
        case NativeTypeKind::S64:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqS64, r0, r1);
            return true;
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftLeftEqU64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectShiftLeftEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectShiftRightEqual(const ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectShiftRightEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqS8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqS16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqS32, r0, r1);
            return true;
        case NativeTypeKind::S64:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqS64, r0, r1);
            return true;

        case NativeTypeKind::U8:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqU8, r0, r1);
            return true;
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqU16, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqU32, r0, r1);
            return true;
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::AffectOpShiftRightEqU64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectShiftRightEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectPercentEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectPercentEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            emitSafetyDivZero(context, r1, 8);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqS8, r0, r1);
            return true;
        case NativeTypeKind::U8:
            emitSafetyDivZero(context, r1, 8);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqU8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            emitSafetyDivZero(context, r1, 16);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqS16, r0, r1);
            return true;
        case NativeTypeKind::U16:
            emitSafetyDivZero(context, r1, 16);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqU16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            emitSafetyDivZero(context, r1, 32);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqS32, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitSafetyDivZero(context, r1, 32);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqU32, r0, r1);
            return true;
        case NativeTypeKind::S64:
            emitSafetyDivZero(context, r1, 64);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqS64, r0, r1);
            return true;
        case NativeTypeKind::U64:
            emitSafetyDivZero(context, r1, 64);
            EMIT_INST2(context, ByteCodeOp::AffectOpModuloEqU64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectPercentEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffectDivEqual(ByteCodeGenContext* context, uint32_t r0, uint32_t r1)
{
    const AstNode* node = context->node;

    const auto front        = node->firstChild();
    auto       leftTypeInfo = TypeManager::concreteType(front->typeInfo);
    if (front->hasSemFlag(SEMFLAG_FROM_REF))
        leftTypeInfo = TypeManager::concretePtrRefType(leftTypeInfo);

    if (!leftTypeInfo->isNative())
        return Report::internalError(context->node, "emitAffectDivEqual, type not native");

    switch (leftTypeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            emitSafetyDivZero(context, r1, 8);
            emitSafetyDivOverflow(context, r0, r1, 8, true);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqS8, r0, r1);
            return true;
        case NativeTypeKind::S16:
            emitSafetyDivZero(context, r1, 16);
            emitSafetyDivOverflow(context, r0, r1, 16, true);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqS16, r0, r1);
            return true;
        case NativeTypeKind::S32:
            emitSafetyDivZero(context, r1, 32);
            emitSafetyDivOverflow(context, r0, r1, 32, true);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqS32, r0, r1);
            return true;
        case NativeTypeKind::S64:
            emitSafetyDivZero(context, r1, 64);
            emitSafetyDivOverflow(context, r0, r1, 64, true);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqS64, r0, r1);
            return true;
        case NativeTypeKind::U8:
            emitSafetyDivZero(context, r1, 8);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqU8, r0, r1);
            return true;
        case NativeTypeKind::U16:
            emitSafetyDivZero(context, r1, 16);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqU16, r0, r1);
            return true;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            emitSafetyDivZero(context, r1, 32);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqU32, r0, r1);
            return true;
        case NativeTypeKind::U64:
            emitSafetyDivZero(context, r1, 64);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqU64, r0, r1);
            return true;
        case NativeTypeKind::F32:
            emitSafetyDivZero(context, r1, 32);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqF32, r0, r1);
            return true;
        case NativeTypeKind::F64:
            emitSafetyDivZero(context, r1, 64);
            EMIT_INST2(context, ByteCodeOp::AffectOpDivEqF64, r0, r1);
            return true;
        default:
            return Report::internalError(context->node, "emitAffectDivEqual, type not supported");
    }
}

bool ByteCodeGen::emitAffect(ByteCodeGenContext* context)
{
    AstNode* node      = context->node;
    AstNode* leftNode  = context->node->firstChild();
    AstNode* rightNode = context->node->secondChild();

    if (!node->hasSemFlag(SEMFLAG_CAST1))
    {
        SWAG_CHECK(emitCast(context, node->secondChild(), TypeManager::concreteType(node->secondChild()->typeInfo), node->secondChild()->typeInfoCast));
        YIELD();
        node->addSemFlag(SEMFLAG_CAST1);
    }

    if (leftNode->typeInfo->isAny())
    {
        leftNode->typeInfo = rightNode->typeInfo;
    }

    // User special function
    if (node->hasSpecialFuncCall())
    {
        if (node->hasSemFlag(SEMFLAG_FLAT_PARAMS))
        {
            const auto arrayNode = castAst<AstArrayPointerIndex>(leftNode->lastChild(), AstNodeKind::ArrayPointerIndex);
            context->allocateTempCallParams();
            context->allParamsTmp->children = arrayNode->structFlatParams;
            SWAG_CHECK(emitUserOp(context, context->allParamsTmp));
            YIELD();
        }
        else
        {
            SWAG_CHECK(emitUserOp(context));
            YIELD();
        }

        return true;
    }

    auto r0 = node->firstChild()->resultRegisterRc;
    auto r1 = node->secondChild()->resultRegisterRc;

    switch (node->token.id)
    {
        case TokenId::SymEqual:
            SWAG_CHECK(emitAffectEqual(context, r0, r1, nullptr, rightNode));
            YIELD();
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
            return Report::internalError(context->node, "emitAffect, invalid token op");
    }

    freeRegisterRC(context, r0);
    freeRegisterRC(context, r1);

    return true;
}
