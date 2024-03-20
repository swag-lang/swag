#include "pch.h"
#include "Backend/ByteCode/ByteCode.h"
#include "Backend/ByteCode/Gen/ByteCodeGen.h"
#include "Backend/ByteCode/Gen/ByteCodeGenContext.h"
#include "Report/Report.h"
#include "Semantic/Scope.h"
#include "Semantic/Type/TypeManager.h"
#include "Syntax/Ast.h"
#include "Syntax/AstFlags.h"
#include "Wmf/Module.h"

bool ByteCodeGen::emitCastToNativeAny(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* fromTypeInfo)
{
    const auto node = context->node;

    if (fromTypeInfo->isPointerNull())
    {
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        EMIT_INST1(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRc[1]);
        exprNode->addSemFlag(SEMFLAG_TYPE_IS_NULL);
        return true;
    }

    // Two registers. One for the pointer to the value, and one for the typeinfo.
    RegisterList r0;
    reserveLinearRegisterRC2(context, r0);
    SWAG_ASSERT(exprNode->resultRegisterRc.size() <= 2);

    // This is the value part.
    // Make a pointer to the value
    if (fromTypeInfo->isStruct() ||
        fromTypeInfo->isArray() ||
        fromTypeInfo->isListArray() ||
        fromTypeInfo->isListTuple() ||
        exprNode->isConstantGenTypeInfo())
    {
        EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0[0], exprNode->resultRegisterRc[0]);
    }
    else if (exprNode->resultRegisterRc.size() == 2)
    {
        // If inside a function, we copy the value to the stack, and address the stack as the any value.
        // That way, even if registers are changed, the memory layout remains correct.
        if (exprNode->ownerFct)
        {
            EMIT_INST2(context, ByteCodeOp::SetAtStackPointer64, exprNode->extMisc()->stackOffset, exprNode->resultRegisterRc[0]);
            EMIT_INST2(context, ByteCodeOp::SetAtStackPointer64, exprNode->extMisc()->stackOffset + 8, exprNode->resultRegisterRc[1]);
            EMIT_INST2(context, ByteCodeOp::MakeStackPointer, r0[0], exprNode->extMisc()->stackOffset);
        }
        else
        {
            return Report::internalError(exprNode, "cannot convert type to 'any'");
        }
    }
    else
    {
        SWAG_ASSERT(exprNode->resultRegisterRc.size() == 1);
        EMIT_INST2(context, ByteCodeOp::CopyRBAddrToRA, r0[0], exprNode->resultRegisterRc[0]);
    }

    if (!exprNode->resultRegisterRc.cannotFree)
    {
        exprNode->ownerScope->owner->allocateExtension(ExtensionKind::Misc);
        for (uint32_t r = 0; r < exprNode->resultRegisterRc.size(); r++)
            exprNode->ownerScope->owner->extMisc()->registersToRelease.push_back(exprNode->resultRegisterRc[r]);
    }

    // This is the type part.
    // :AnyTypeSegment
    SWAG_ASSERT(exprNode->hasExtraPointer(ExtraPointerKind::AnyTypeSegment));
    const auto anyTypeSegment = exprNode->extraPointer<DataSegment>(ExtraPointerKind::AnyTypeSegment);
    emitMakeSegPointer(context, anyTypeSegment, exprNode->extMisc()->anyTypeOffset, r0[1]);

    exprNode->resultRegisterRc      = r0;
    context->node->resultRegisterRc = r0;
    return true;
}

bool ByteCodeGen::emitCastToInterface(const ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* /*typeInfo*/, TypeInfo* fromTypeInfo)
{
    const auto node = context->node;
    if (fromTypeInfo->isPointerNull())
    {
        node->addSemFlag(SEMFLAG_FROM_NULL);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        return true;
    }

    SWAG_IF_ASSERT(const TypeInfoStruct* fromTypeStruct = nullptr);
    if (fromTypeInfo->isStruct())
    {
        SWAG_IF_ASSERT(fromTypeStruct = castTypeInfo<TypeInfoStruct>(fromTypeInfo, TypeInfoKind::Struct));
    }
    else if (fromTypeInfo->isPointerTo(TypeInfoKind::Struct))
    {
        SWAG_IF_ASSERT(const auto fromTypePointer = castTypeInfo<TypeInfoPointer>(fromTypeInfo, TypeInfoKind::Pointer));
        SWAG_IF_ASSERT(fromTypeStruct = castTypeInfo<TypeInfoStruct>(fromTypePointer->pointedType, TypeInfoKind::Struct));
    }
    else
    {
        return Report::internalError(context->node, "emitCastToInterface, invalid type");
    }

    SWAG_ASSERT(fromTypeStruct->cptRemainingInterfaces == 0);
    SWAG_ASSERT(exprNode->hasExtraPointer(ExtraPointerKind::CastItf));

    transformResultToLinear2(context, exprNode->resultRegisterRc);

    // Need to make the pointer on the data
    if (exprNode->extMisc()->castOffset)
    {
        const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, exprNode->resultRegisterRc, 0, exprNode->resultRegisterRc);
        inst->b.u64     = exprNode->extMisc()->castOffset;
        inst->addFlag(BCI_IMM_B);
    }

    // :ItfIsConstantSeg
    SWAG_ASSERT(exprNode->hasExtraPointer(ExtraPointerKind::CastItf));
    const auto castItf = exprNode->extraPointer<TypeInfoParam>(ExtraPointerKind::CastItf);
    emitMakeSegPointer(context, &node->token.sourceFile->module->constantSegment, castItf->offset, exprNode->resultRegisterRc[1]);
    node->resultRegisterRc = exprNode->resultRegisterRc;

    return true;
}

bool ByteCodeGen::emitCastToNativeBool(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    const auto r0 = reserveRegisterRC(context);

    if (typeInfo->isClosure())
    {
        EMIT_INST2(context, ByteCodeOp::DeRef64, r0, exprNode->resultRegisterRc);
        EMIT_INST2(context, ByteCodeOp::CastBool64, r0, r0);
    }
    else if (typeInfo->isPointer() || typeInfo->isLambdaClosure())
    {
        EMIT_INST2(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRc);
    }
    else if (typeInfo->isInterface() || typeInfo->isSlice())
    {
        EMIT_INST2(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRc[1]);
    }
    else
    {
        if (!typeInfo->isNative())
            return Report::internalError(exprNode, "emitCast, expression type not native");

        switch (typeInfo->nativeType)
        {
            case NativeTypeKind::Bool:
                EMIT_INST2(context, ByteCodeOp::CopyRBtoRA64, r0, exprNode->resultRegisterRc);
                break;
            case NativeTypeKind::U8:
            case NativeTypeKind::S8:
                EMIT_INST2(context, ByteCodeOp::CastBool8, r0, exprNode->resultRegisterRc);
                break;
            case NativeTypeKind::U16:
            case NativeTypeKind::S16:
                EMIT_INST2(context, ByteCodeOp::CastBool16, r0, exprNode->resultRegisterRc);
                break;
            case NativeTypeKind::Rune:
            case NativeTypeKind::U32:
            case NativeTypeKind::S32:
            case NativeTypeKind::F32:
                EMIT_INST2(context, ByteCodeOp::CastBool32, r0, exprNode->resultRegisterRc);
                break;
            case NativeTypeKind::S64:
            case NativeTypeKind::U64:
            case NativeTypeKind::F64:
            case NativeTypeKind::String:
                EMIT_INST2(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRc);
                break;
            default:
                Report::internalError(exprNode, "emitCastToNativeBool, invalid source type");
                break;
        }
    }

    freeRegisterRC(context, exprNode->resultRegisterRc);
    exprNode->resultRegisterRc = r0;
    return true;
}

bool ByteCodeGen::emitCastToNativeU8(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U16:
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
        case NativeTypeKind::U64:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeU8, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeU16(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRc)->b.u64 = 0x000000FF;
            break;
        case NativeTypeKind::U16:
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeS16, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeU32(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRc)->b.u64 = 0x000000FF;
            break;
        case NativeTypeKind::U16:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRc)->b.u64 = 0x0000FFFF;
            break;
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeS32, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeU64(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (typeInfo->isPointer())
        return true;

    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::CastS16S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRc)->b.u64 = 0x00000000'000000FF;
            break;
        case NativeTypeKind::U16:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRc)->b.u64 = 0x00000000'0000FFFF;
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRc)->b.u64 = 0x00000000'FFFFFFFF;
            break;
        case NativeTypeKind::U64:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            EMIT_INST2(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeU64, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeS8(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
        case NativeTypeKind::U16:
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
        case NativeTypeKind::S8:
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeS8, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeS16(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRc)->b.u64 = 0x000000FF;
            break;
        case NativeTypeKind::U16:
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeS16, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeS32(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
        case NativeTypeKind::U8:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRc)->b.u64 = 0x000000FF;
            break;
        case NativeTypeKind::U16:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRc)->b.u64 = 0x0000FFFF;
            break;
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
        case NativeTypeKind::U64:
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S32:
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeS32, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeS64(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRc)->b.u64 = 0x0000000'000000FF;
            break;
        case NativeTypeKind::U16:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRc)->b.u64 = 0x0000000'0000FFFF;
            break;
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
            EMIT_INST1(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRc)->b.u64 = 0x0000000'FFFFFFFF;
            break;
        case NativeTypeKind::U64:
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::CastS16S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S64:
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            EMIT_INST2(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeS64, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeF32(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST2(context, ByteCodeOp::CastU8F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::CastU16F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
            EMIT_INST2(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::CastU64F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::CastS16F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S64:
            EMIT_INST2(context, ByteCodeOp::CastS64F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F32:
            break;
        case NativeTypeKind::F64:
            EMIT_INST2(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeF32, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeF64(const ByteCodeGenContext* context, AstNode* exprNode, const TypeInfo* typeInfo)
{
    if (!typeInfo->isNative())
        return Report::internalError(exprNode, "emitCast, expression type not native");

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::U8:
        case NativeTypeKind::Bool:
            EMIT_INST2(context, ByteCodeOp::CastU8F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::U16:
            EMIT_INST2(context, ByteCodeOp::CastU16F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
            EMIT_INST2(context, ByteCodeOp::CastU32F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::U64:
            EMIT_INST2(context, ByteCodeOp::CastU64F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S8:
            EMIT_INST2(context, ByteCodeOp::CastS8F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S16:
            EMIT_INST2(context, ByteCodeOp::CastS16F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S32:
            EMIT_INST2(context, ByteCodeOp::CastS32F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::S64:
            EMIT_INST2(context, ByteCodeOp::CastS64F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F32:
            EMIT_INST2(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRc, exprNode->resultRegisterRc);
            break;
        case NativeTypeKind::F64:
            break;
        default:
            Report::internalError(exprNode, "emitCastToNativeF64, invalid source type");
            break;
    }

    return true;
}

bool ByteCodeGen::emitCastToNativeString(const ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo)
{
    const auto node = context->node;

    if (fromTypeInfo->isString())
    {
        return true;
    }

    if (fromTypeInfo->isPointerNull())
    {
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        EMIT_INST1(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRc[0]);
        EMIT_INST1(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRc[1]);
        exprNode->addSemFlag(SEMFLAG_TYPE_IS_NULL);
        return true;
    }

    if (fromTypeInfo->isSlice())
    {
        return true;
    }

    if (fromTypeInfo->isArray())
    {
        const auto typeArray = castTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        const auto inst        = EMIT_INST1(context, ByteCodeOp::SetImmediate64, exprNode->resultRegisterRc[1]);
        inst->b.u64            = typeArray->count;
        return true;
    }

    if (fromTypeInfo->isListTuple())
    {
#ifdef SWAG_HAS_ASSERT
        const auto typeList = castTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeListTuple);
        SWAG_ASSERT(typeList->subTypes.size() == 2);
        SWAG_ASSERT(typeList->subTypes[0]->typeInfo->isPointer() || typeList->subTypes[0]->typeInfo->isArray());
        SWAG_ASSERT(typeList->subTypes[1]->typeInfo->isNative());
#endif
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        return true;
    }

    Report::internalError(context->node, "emitCastToNativeString, invalid type");
    return false;
}

bool ByteCodeGen::emitCastToSlice(const ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    const auto node        = context->node;
    const auto toTypeSlice = castTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

    if (fromTypeInfo->isTypedVariadic())
    {
        const auto typeVariadic = castTypeInfo<TypeInfoVariadic>(fromTypeInfo, TypeInfoKind::TypedVariadic);
        fromTypeInfo            = TypeManager::concreteType(typeVariadic->rawType);
    }

    if (fromTypeInfo->isPointerNull() || fromTypeInfo->isString())
    {
        node->resultRegisterRc = exprNode->resultRegisterRc;
    }
    else if (fromTypeInfo->isSlice())
    {
        const auto fromTypeSlice = castTypeInfo<TypeInfoSlice>(fromTypeInfo, TypeInfoKind::Slice);

        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);
        if (fromTypeSlice->pointedType->sizeOf > toTypeSlice->pointedType->sizeOf)
        {
            const auto diff = fromTypeSlice->pointedType->sizeOf / toTypeSlice->pointedType->sizeOf;
            const auto inst = EMIT_INST1(context, ByteCodeOp::Mul64byVB64, exprNode->resultRegisterRc[1]);
            inst->b.u64     = diff;
        }
        else if (fromTypeSlice->pointedType->sizeOf < toTypeSlice->pointedType->sizeOf)
        {
            const auto diff = toTypeSlice->pointedType->sizeOf / fromTypeSlice->pointedType->sizeOf;
            const auto inst = EMIT_INST1(context, ByteCodeOp::Div64byVB64, exprNode->resultRegisterRc[1]);
            inst->b.u64     = diff;
        }

        node->resultRegisterRc = exprNode->resultRegisterRc;
    }
    else if (fromTypeInfo->isListTuple())
    {
        const auto fromTypeList = castTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeListTuple);
        const auto diff         = fromTypeList->subTypes.front()->typeInfo->sizeOf / toTypeSlice->pointedType->sizeOf;
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);
        const auto inst        = EMIT_INST1(context, ByteCodeOp::Mul64byVB64, exprNode->resultRegisterRc[1]);
        inst->b.u64            = diff;
        node->resultRegisterRc = exprNode->resultRegisterRc;
    }
    else if (fromTypeInfo->isListArray())
    {
        const auto fromTypeList = castTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeListArray);
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        const auto inst        = EMIT_INST1(context, ByteCodeOp::SetImmediate64, node->resultRegisterRc[1]);
        inst->b.u64            = fromTypeList->subTypes.count;
    }
    else if (fromTypeInfo->isArray())
    {
        const auto fromTypeArray = castTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        const auto inst        = EMIT_INST1(context, ByteCodeOp::SetImmediate64, node->resultRegisterRc[1]);
        inst->b.u64            = fromTypeArray->sizeOf / toTypeSlice->pointedType->sizeOf;
    }
    else if (fromTypeInfo->isPointer())
    {
        transformResultToLinear2(context, exprNode->resultRegisterRc);
        node->resultRegisterRc = exprNode->resultRegisterRc;
        EMIT_INST2(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRc[0], node->resultRegisterRc[1]);
    }
    else
    {
        return Report::internalError(exprNode, "emitCastToSlice, invalid expression type");
    }

    return true;
}

bool ByteCodeGen::emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, uint32_t /*emitCastFlags*/)
{
    if (fromTypeInfo == nullptr)
        return true;
    SWAG_ASSERT(typeInfo);

    typeInfo     = TypeManager::concreteType(typeInfo, CONCRETE_ENUM | CONCRETE_FORCE_ALIAS);
    fromTypeInfo = TypeManager::concreteType(fromTypeInfo, CONCRETE_ENUM | CONCRETE_FUNC | CONCRETE_FORCE_ALIAS);

    // opCast
    if (exprNode->hasSemFlag(SEMFLAG_USER_CAST))
    {
        SWAG_ASSERT(exprNode->hasExtraPointer(ExtraPointerKind::UserOp));

        if (!exprNode->hasSemFlag(SEMFLAG_FLAT_PARAMS))
        {
            exprNode->addSemFlag(SEMFLAG_FLAT_PARAMS);
            context->allocateTempCallParams();
            context->allParamsTmp->children.push_back(exprNode);
            context->allParamsTmp->addExtraPointer(ExtraPointerKind::UserOp, exprNode->extraPointer<SymbolOverload>(ExtraPointerKind::UserOp));
            context->allParamsTmp->inheritOwners(exprNode);
            context->allParamsTmp->inheritTokenLocation(exprNode->token);
            context->allParamsTmp->semFlags = 0;
            context->allParamsTmp->removeAstFlag(AST_INLINED);
        }

        SWAG_CHECK(emitUserOp(context, nullptr, context->allParamsTmp));
        YIELD();

        // If it has been inlined, then the inline block contains the register we need
        const auto back = context->allParamsTmp->lastChild();
        if (back->is(AstNodeKind::Inline))
        {
            exprNode->resultRegisterRc = back->resultRegisterRc;
            exprNode->allocateExtension(ExtensionKind::Owner);
            exprNode->extOwner()->nodesToFree.push_back(back);
        }

        if (context->node->resultRegisterRc.size())
            exprNode->resultRegisterRc = context->node->resultRegisterRc;
        else
            context->node->resultRegisterRc = exprNode->resultRegisterRc;

        return true;
    }

    // Cast from any to something real
    const auto node = context->node;
    if (fromTypeInfo->isAny())
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);

        // Check that the type is correct
        auto anyNode = exprNode;
        if (!anyNode->hasExtraPointer(ExtraPointerKind::AnyTypeSegment))
        {
            SWAG_ASSERT(!anyNode->children.empty());
            anyNode = anyNode->firstChild();
        }

        emitSafetyCastAny(context, anyNode, typeInfo);

        // Dereference the any content, except for a reference, where we want to keep the pointer
        // (pointer that comes from the data is already in the correct register)
        if (!typeInfo->isPointerRef())
        {
            SWAG_CHECK(emitTypeDeRef(context, exprNode->resultRegisterRc, typeInfo));
        }

        if (typeInfo->numRegisters() == 1)
        {
            truncRegisterRC(context, exprNode->resultRegisterRc, 1);
            node->resultRegisterRc = exprNode->resultRegisterRc;
        }
        else
        {
            SWAG_ASSERT(exprNode->resultRegisterRc.size() == typeInfo->numRegisters());
            node->resultRegisterRc = exprNode->resultRegisterRc;
        }

        exprNode->typeInfo       = typeInfo;
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    // Cast to a pointer with an offset
    // When casting from one struct to another, with a 'using' on a field
    if (exprNode->hasSemFlag(SEMFLAG_USING))
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);

        truncRegisterRC(context, exprNode->resultRegisterRc, 1);
        node->resultRegisterRc   = exprNode->resultRegisterRc;
        exprNode->castedTypeInfo = nullptr;

        if (exprNode->hasExtMisc() && exprNode->extMisc()->castOffset)
        {
            const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->resultRegisterRc, 0, node->resultRegisterRc);
            inst->b.u64     = exprNode->extMisc()->castOffset;
            inst->addFlag(BCI_IMM_B);
        }

        // The field is a pointer : need to dereference it
        if (exprNode->hasSemFlag(SEMFLAG_DEREF_USING))
            EMIT_INST2(context, ByteCodeOp::DeRef64, node->resultRegisterRc, node->resultRegisterRc);

        return true;
    }

    if (typeInfo->isLambdaClosure())
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);
        truncRegisterRC(context, exprNode->resultRegisterRc, 1);
        node->resultRegisterRc   = exprNode->resultRegisterRc;
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->isArray())
    {
        if (fromTypeInfo->isPointer())
        {
            truncRegisterRC(context, exprNode->resultRegisterRc, 1);
            node->resultRegisterRc   = exprNode->resultRegisterRc;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }

        if (fromTypeInfo->isArray())
        {
            truncRegisterRC(context, exprNode->resultRegisterRc, 1);
            node->resultRegisterRc   = exprNode->resultRegisterRc;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }
    }

    if (typeInfo->isPointer())
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);

        if (fromTypeInfo->isStruct())
        {
            truncRegisterRC(context, exprNode->resultRegisterRc, 1);
            node->resultRegisterRc   = exprNode->resultRegisterRc;
            exprNode->castedTypeInfo = nullptr;

            if (exprNode->hasExtMisc() && exprNode->extMisc()->castOffset)
            {
                const auto inst = EMIT_INST3(context, ByteCodeOp::IncPointer64, node->resultRegisterRc, 0, node->resultRegisterRc);
                inst->b.u64     = exprNode->extMisc()->castOffset;
                inst->addFlag(BCI_IMM_B);
            }

            return true;
        }

        if (fromTypeInfo->isArray() ||
            fromTypeInfo->isPointer() ||
            fromTypeInfo->isStruct() ||
            fromTypeInfo->isInterface() ||
            fromTypeInfo->isSlice() ||
            fromTypeInfo->isLambdaClosure() ||
            fromTypeInfo->numRegisters() == 1)
        {
            truncRegisterRC(context, exprNode->resultRegisterRc, 1);
            node->resultRegisterRc   = exprNode->resultRegisterRc;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }

        if (fromTypeInfo->isString() ||
            fromTypeInfo->isAny())
        {
            truncRegisterRC(context, exprNode->resultRegisterRc, 1);
            node->resultRegisterRc   = exprNode->resultRegisterRc;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }
    }

    if (typeInfo->isSlice())
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);
        SWAG_CHECK(emitCastToSlice(context, exprNode, typeInfo, fromTypeInfo));
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->isInterface())
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRc);
        SWAG_CHECK(emitCastToInterface(context, exprNode, typeInfo, fromTypeInfo));
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    // Pass through
    if (typeInfo->isStruct())
    {
        node->resultRegisterRc   = exprNode->resultRegisterRc;
        exprNode->typeInfo       = typeInfo;
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (!typeInfo->isNative())
        return Report::internalError(context->node, "emitCast, cast type not native");

    emitSafetyCastOverflow(context, typeInfo, fromTypeInfo, exprNode);
    ensureCanBeChangedRC(context, exprNode->resultRegisterRc);

    switch (typeInfo->nativeType)
    {
        case NativeTypeKind::Bool:
            SWAG_CHECK(emitCastToNativeBool(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::S8:
            SWAG_CHECK(emitCastToNativeS8(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::S16:
            SWAG_CHECK(emitCastToNativeS16(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::S32:
            SWAG_CHECK(emitCastToNativeS32(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::S64:
            SWAG_CHECK(emitCastToNativeS64(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::U8:
            SWAG_CHECK(emitCastToNativeU8(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::U16:
            SWAG_CHECK(emitCastToNativeU16(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::U32:
        case NativeTypeKind::Rune:
            SWAG_CHECK(emitCastToNativeU32(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::U64:
            SWAG_CHECK(emitCastToNativeU64(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::F32:
            SWAG_CHECK(emitCastToNativeF32(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::F64:
            SWAG_CHECK(emitCastToNativeF64(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::String:
            SWAG_CHECK(emitCastToNativeString(context, exprNode, fromTypeInfo));
            break;
        case NativeTypeKind::Any:
            SWAG_CHECK(emitCastToNativeAny(context, exprNode, fromTypeInfo));
            break;
        default:
            return Report::internalError(context->node, "emitCast, invalid cast type");
    }

    node->resultRegisterRc   = exprNode->resultRegisterRc;
    exprNode->typeInfo       = typeInfo;
    exprNode->castedTypeInfo = nullptr;
    return true;
}

bool ByteCodeGen::emitExplicitCast(ByteCodeGenContext* context)
{
    const auto node         = castAst<AstCast>(context->node, AstNodeKind::Cast);
    const auto typeInfo     = node->toCastTypeInfo;
    const auto exprNode     = node->secondChild();
    const auto fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

    // First we cast with the user requested type. This is important to keep it, to
    // properly deref an 'any' for example
    SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo, EMIT_CAST_FLAG_EXPLICIT));
    YIELD();

    // Then we cast again if necessary to the requested final type that can have been
    // changed (type promotion for example)
    if (node->toCastTypeInfo != node->typeInfo && node->toCastTypeInfo != node->castedTypeInfo)
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->toCastTypeInfo));
        YIELD();
    }

    return true;
}

bool ByteCodeGen::emitExplicitAutoCast(ByteCodeGenContext* context)
{
    const auto node     = castAst<AstCast>(context->node, AstNodeKind::AutoCast);
    const auto exprNode = node->firstChild();

    // Will be done by parent in case of a func call param
    if (node->parent->is(AstNodeKind::FuncCallParam))
    {
        node->resultRegisterRc   = exprNode->resultRegisterRc;
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    const auto typeInfo     = TypeManager::concreteType(node->typeInfo);
    const auto fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);
    SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo, EMIT_CAST_FLAG_AUTO));
    YIELD();
    node->castedTypeInfo = nullptr;

    return true;
}
