#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "SourceFile.h"
#include "SemanticJob.h"
#include "Module.h"
#include "Ast.h"

bool ByteCodeGenJob::emitCastToNativeAny(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo)
{
    auto node = context->node;

    if (fromTypeInfo == g_TypeMgr->typeInfoNull)
    {
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRC[1]);
        exprNode->semFlags |= AST_SEM_TYPE_IS_NULL;
        return true;
    }

    // Two registers. One for the pointer to the value, and one for the typeinfo.
    RegisterList r0;
    reserveLinearRegisterRC2(context, r0);
    SWAG_ASSERT(exprNode->resultRegisterRC.size() <= 2);

    // This is the value part.
    // Make a pointer to the value
    if ((fromTypeInfo->flags & TYPEINFO_RETURN_BY_COPY) || (exprNode->flags & AST_VALUE_IS_TYPEINFO))
    {
        emitInstruction(context, ByteCodeOp::CopyRBtoRA64, r0[0], exprNode->resultRegisterRC[0]);
    }
    else if (exprNode->resultRegisterRC.size() == 2)
    {
        // If inside a function, we copy the value to the stack, and address the stack as the any value.
        // That way, even if registers are changed, the memory layout remains correct.
        // In other words, CopyRBAddrToRA2 is satanic, but don't know how to do that in another way for now.
        if (exprNode->ownerFct)
        {
            emitInstruction(context, ByteCodeOp::SetAtStackPointer64, exprNode->extension->stackOffset, exprNode->resultRegisterRC[0]);
            emitInstruction(context, ByteCodeOp::SetAtStackPointer64, exprNode->extension->stackOffset + 8, exprNode->resultRegisterRC[1]);
            emitInstruction(context, ByteCodeOp::MakeStackPointer, r0[0], exprNode->extension->stackOffset);
        }
        else
        {
            // Be sure registers are contiguous, as we address the first of them
            SWAG_ASSERT(exprNode->resultRegisterRC[0] == exprNode->resultRegisterRC[1] - 1);
            emitInstruction(context, ByteCodeOp::CopyRBAddrToRA2, r0[0], exprNode->resultRegisterRC[0], exprNode->resultRegisterRC[1]);
        }
    }
    else
    {
        SWAG_ASSERT(exprNode->resultRegisterRC.size() == 1);
        emitInstruction(context, ByteCodeOp::CopyRBAddrToRA, r0[0], exprNode->resultRegisterRC[0]);
    }

    if (!exprNode->resultRegisterRC.cannotFree)
    {
        exprNode->ownerScope->owner->allocateExtension();
        for (int r = 0; r < exprNode->resultRegisterRC.size(); r++)
            exprNode->ownerScope->owner->extension->registersToRelease.push_back(exprNode->resultRegisterRC[r]);
    }

    // This is the type part.
    // :AnyTypeSegment
    SWAG_ASSERT(exprNode->extension);
    SWAG_ASSERT(exprNode->extension->anyTypeSegment);
    emitMakeSegPointer(context, exprNode->extension->anyTypeSegment, exprNode->extension->anyTypeOffset, r0[1]);

    exprNode->resultRegisterRC      = r0;
    context->node->resultRegisterRC = r0;
    return true;
}

bool ByteCodeGenJob::emitCastToInterface(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto node = context->node;
    if (fromTypeInfo == g_TypeMgr->typeInfoNull)
    {
        node->semFlags |= AST_SEM_FROM_NULL;
        node->resultRegisterRC = exprNode->resultRegisterRC;
        return true;
    }

    TypeInfoStruct* fromTypeStruct = nullptr;
    bool            fromPointer    = false;
    if (fromTypeInfo->kind == TypeInfoKind::Struct)
    {
        fromTypeStruct = CastTypeInfo<TypeInfoStruct>(fromTypeInfo, TypeInfoKind::Struct);
    }
    else if (fromTypeInfo->isPointerTo(TypeInfoKind::Struct))
    {
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromTypeInfo, TypeInfoKind::Pointer);
        fromTypeStruct       = CastTypeInfo<TypeInfoStruct>(fromTypePointer->pointedType, TypeInfoKind::Struct);
        fromPointer          = true;
    }
    else
    {
        return context->internalError("emitCastToInterface, invalid type");
    }

    SWAG_ASSERT(fromTypeStruct->cptRemainingInterfaces == 0);
    SWAG_ASSERT(exprNode->extension);
    SWAG_ASSERT(exprNode->extension->castItf);

    transformResultToLinear2(context, exprNode);

    // Need to make the pointer on the data
    if (exprNode->extension->castOffset)
    {
        auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, exprNode->resultRegisterRC, 0, exprNode->resultRegisterRC);
        SWAG_ASSERT(exprNode->extension && exprNode->extension->castOffset != UINT32_MAX);
        inst->b.u64 = exprNode->extension->castOffset;
        inst->flags |= BCI_IMM_B;
    }

    // :ItfIsConstantSeg
    emitMakeSegPointer(context, &node->sourceFile->module->constantSegment, exprNode->extension->castItf->offset, exprNode->resultRegisterRC[1]);
    node->resultRegisterRC = exprNode->resultRegisterRC;

    return true;
}

bool ByteCodeGenJob::emitCastToNativeBool(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    auto r0 = reserveRegisterRC(context);

    if (typeInfo->isClosure())
    {
        emitInstruction(context, ByteCodeOp::DeRef64, r0, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastBool64, r0, r0);
    }
    else if (typeInfo->kind == TypeInfoKind::Pointer || typeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRC);
    }
    else if (typeInfo->kind == TypeInfoKind::Interface || typeInfo->kind == TypeInfoKind::Slice)
    {
        emitInstruction(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRC[1]);
    }
    else
    {
        if (typeInfo->kind != TypeInfoKind::Native)
            return context->internalError("emitCast, expression type not native", exprNode);

        switch (typeInfo->nativeType)
        {
        case NativeTypeKind::Bool:
            break;
        case NativeTypeKind::U8:
        case NativeTypeKind::S8:
            emitInstruction(context, ByteCodeOp::CastBool8, r0, exprNode->resultRegisterRC);
            break;
        case NativeTypeKind::U16:
        case NativeTypeKind::S16:
            emitInstruction(context, ByteCodeOp::CastBool16, r0, exprNode->resultRegisterRC);
            break;
        case NativeTypeKind::Rune:
        case NativeTypeKind::U32:
        case NativeTypeKind::S32:
            emitInstruction(context, ByteCodeOp::CastBool32, r0, exprNode->resultRegisterRC);
            break;
        case NativeTypeKind::S64:
        case NativeTypeKind::U64:
        case NativeTypeKind::Int:
        case NativeTypeKind::UInt:
            emitInstruction(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRC);
            break;
        case NativeTypeKind::String:
            emitInstruction(context, ByteCodeOp::CastBool64, r0, exprNode->resultRegisterRC);
            break;
        default:
            context->node = exprNode;
            context->internalError("emitCastToNativeBool, invalid source type");
            break;
        }
    }

    freeRegisterRC(context, exprNode->resultRegisterRC);
    exprNode->resultRegisterRC = r0;
    return true;
}

bool ByteCodeGenJob::emitCastToNativeU8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeU8, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u64 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeS16, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u64 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u64 = 0x0000FFFF;
        break;
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Pointer)
        return true;

    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x00000000'000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x00000000'0000FFFF;
        break;
    case NativeTypeKind::U32:
    case NativeTypeKind::Rune:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x00000000'FFFFFFFF;
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeU64, invalid source type");
        break;
    }

    return true;
}
bool ByteCodeGenJob::emitCastToNativeS8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
    case NativeTypeKind::U16:
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeS8, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeS16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u64 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeS16, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeS32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u64 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u64 = 0x0000FFFF;
        break;
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeS64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x0000000'000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x0000000'0000FFFF;
        break;
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x0000000'FFFFFFFF;
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeS64, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeF32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::CastU8F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::CastU16F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::CastU64F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::CastS64F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeF32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeF64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::CastU8F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::CastU16F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::Rune:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::CastU32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::UInt:
        emitInstruction(context, ByteCodeOp::CastU64F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::Int:
        emitInstruction(context, ByteCodeOp::CastS64F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        break;
    default:
        context->node = exprNode;
        context->internalError("emitCastToNativeF64, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeString(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo)
{
    auto node = context->node;

    if (fromTypeInfo->isNative(NativeTypeKind::String))
    {
        return true;
    }

    if (fromTypeInfo == g_TypeMgr->typeInfoNull)
    {
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRC[0]);
        emitInstruction(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRC[1]);
        exprNode->semFlags |= AST_SEM_TYPE_IS_NULL;
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::Slice)
    {
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::Array)
    {
        auto typeArray = CastTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC                                                                     = exprNode->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::SetImmediate64, exprNode->resultRegisterRC[1])->b.u64 = typeArray->count;
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::TypeListTuple)
    {
#ifdef SWAG_HAS_ASSERT
        auto typeList = CastTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeListTuple);
        SWAG_ASSERT(typeList->subTypes.size() == 2);
        SWAG_ASSERT(typeList->subTypes[0]->typeInfo->kind == TypeInfoKind::Pointer || typeList->subTypes[0]->typeInfo->kind == TypeInfoKind::Array);
        SWAG_ASSERT(typeList->subTypes[1]->typeInfo->kind == TypeInfoKind::Native);
#endif
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        return true;
    }

    context->internalError("emitCastToNativeString, invalid type");
    return false;
}

bool ByteCodeGenJob::emitCastToSlice(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto node        = context->node;
    auto toTypeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

    if (fromTypeInfo->kind == TypeInfoKind::TypedVariadic)
    {
        auto typeVariadic = CastTypeInfo<TypeInfoVariadic>(fromTypeInfo, TypeInfoKind::TypedVariadic);
        fromTypeInfo      = TypeManager::concreteReferenceType(typeVariadic->rawType);
    }

    if (fromTypeInfo == g_TypeMgr->typeInfoNull || fromTypeInfo->isNative(NativeTypeKind::String))
    {
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice = CastTypeInfo<TypeInfoSlice>(fromTypeInfo, TypeInfoKind::Slice);
        auto diff          = fromTypeSlice->pointedType->sizeOf / toTypeSlice->pointedType->sizeOf;
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);
        auto inst              = emitInstruction(context, ByteCodeOp::Mul64byVB64, exprNode->resultRegisterRC[1]);
        inst->b.u64            = diff;
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::TypeListTuple)
    {
        auto fromTypeList = CastTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeListTuple);
        auto diff         = fromTypeList->subTypes.front()->typeInfo->sizeOf / toTypeSlice->pointedType->sizeOf;
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);
        auto inst              = emitInstruction(context, ByteCodeOp::Mul64byVB64, exprNode->resultRegisterRC[1]);
        inst->b.u64            = diff;
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::TypeListArray)
    {
        auto fromTypeList = CastTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeListArray);
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        auto inst              = emitInstruction(context, ByteCodeOp::SetImmediate64, node->resultRegisterRC[1]);
        inst->b.u64            = fromTypeList->subTypes.count;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Array)
    {
        auto fromTypeArray = CastTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        auto inst              = emitInstruction(context, ByteCodeOp::SetImmediate64, node->resultRegisterRC[1]);
        inst->b.u64            = fromTypeArray->count;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Pointer)
    {
        transformResultToLinear2(context, exprNode);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        emitInstruction(context, ByteCodeOp::DeRefStringSlice, node->resultRegisterRC[0], node->resultRegisterRC[1]);
    }
    else
    {
        return context->internalError("emitCastToSlice, invalid expression type", exprNode);
    }

    return true;
}

bool ByteCodeGenJob::emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo, bool isExplicit)
{
    if (fromTypeInfo == nullptr)
        return true;
    SWAG_ASSERT(typeInfo);

    auto job     = context->job;
    typeInfo     = TypeManager::concreteType(typeInfo, CONCRETE_ENUM | CONCRETE_FORCEALIAS);
    fromTypeInfo = TypeManager::concreteReferenceType(fromTypeInfo, CONCRETE_ENUM | CONCRETE_FUNC | CONCRETE_FORCEALIAS);

    // opCast
    if (exprNode->semFlags & AST_SEM_USER_CAST)
    {
        SWAG_ASSERT(exprNode->extension && exprNode->extension->resolvedUserOpSymbolOverload);

        if (!(exprNode->doneFlags & AST_DONE_FLAT_PARAMS))
        {
            exprNode->doneFlags |= AST_DONE_FLAT_PARAMS;
            if (!job->allParamsTmp)
                job->allParamsTmp = Ast::newFuncCallParams(exprNode->sourceFile, nullptr);
            job->allParamsTmp->childs.clear();
            job->allParamsTmp->childs.push_back(exprNode);
            job->allParamsTmp->allocateExtension();
            job->allParamsTmp->extension->resolvedUserOpSymbolOverload = exprNode->extension->resolvedUserOpSymbolOverload;
            job->allParamsTmp->inheritOwners(exprNode);
        }

        if (!isExplicit)
            exprNode->typeInfo = exprNode->castedTypeInfo;

        SWAG_CHECK(emitUserOp(context, nullptr, job->allParamsTmp));
        if (context->result != ContextResult::Done)
            return true;

        // If it has been inlined, then the inline block contains the register we need
        auto back = job->allParamsTmp->childs.back();
        if (back->kind == AstNodeKind::Inline)
            exprNode->resultRegisterRC = back->resultRegisterRC;

        if (context->node->resultRegisterRC.size())
            exprNode->resultRegisterRC = context->node->resultRegisterRC;
        else
            context->node->resultRegisterRC = exprNode->resultRegisterRC;

        return true;
    }

    // Cast from any to something real
    auto node = context->node;
    if (fromTypeInfo->isNative(NativeTypeKind::Any))
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);

        // Check that the type is correct
        auto anyNode = exprNode;
        if (!anyNode->extension || !anyNode->extension->anyTypeSegment)
        {
            SWAG_ASSERT(anyNode->childs.size());
            anyNode = anyNode->childs.front();
        }

        emitSafetyCastAny(context, anyNode, isExplicit);

        // Dereference the any content, except for a reference, where we want to keep the pointer
        // (pointer that comes from the data is already in the correct register)
        if (typeInfo->kind != TypeInfoKind::Reference)
        {
            SWAG_CHECK(emitTypeDeRef(context, exprNode->resultRegisterRC, typeInfo));
        }

        if (typeInfo->numRegisters() == 1)
        {
            truncRegisterRC(context, exprNode->resultRegisterRC, 1);
            node->resultRegisterRC = exprNode->resultRegisterRC;
        }
        else
        {
            SWAG_ASSERT(exprNode->resultRegisterRC.size() == typeInfo->numRegisters());
            node->resultRegisterRC = exprNode->resultRegisterRC;
        }

        exprNode->typeInfo       = typeInfo;
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    // Cast with to a pointer with an offset
    // When casting from one struct to another, with a 'using' on a field
    if (exprNode->semFlags & AST_SEM_USING)
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);

        truncRegisterRC(context, exprNode->resultRegisterRC, 1);
        node->resultRegisterRC   = exprNode->resultRegisterRC;
        exprNode->castedTypeInfo = nullptr;

        if (exprNode->extension && exprNode->extension->castOffset)
        {
            auto inst = emitInstruction(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
            SWAG_ASSERT(exprNode->extension && exprNode->extension->castOffset != 0xFFFFFFFF);
            inst->b.u64 = exprNode->extension->castOffset;
            inst->flags |= BCI_IMM_B;
        }

        // The field is a pointer : need to dereference it
        if (exprNode->semFlags & AST_SEM_DEREF_USING)
            emitInstruction(context, ByteCodeOp::DeRef64, node->resultRegisterRC, node->resultRegisterRC);

        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Lambda)
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);
        truncRegisterRC(context, exprNode->resultRegisterRC, 1);
        node->resultRegisterRC   = exprNode->resultRegisterRC;
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Array)
    {
        if (fromTypeInfo->kind == TypeInfoKind::Pointer)
        {
            truncRegisterRC(context, exprNode->resultRegisterRC, 1);
            node->resultRegisterRC   = exprNode->resultRegisterRC;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }

        if (fromTypeInfo->kind == TypeInfoKind::Array)
        {
            truncRegisterRC(context, exprNode->resultRegisterRC, 1);
            node->resultRegisterRC   = exprNode->resultRegisterRC;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }
    }

    if (typeInfo->kind == TypeInfoKind::Pointer || typeInfo->kind == TypeInfoKind::Reference)
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);

        if (fromTypeInfo->kind == TypeInfoKind::Struct)
        {
            truncRegisterRC(context, exprNode->resultRegisterRC, 1);
            node->resultRegisterRC   = exprNode->resultRegisterRC;
            exprNode->castedTypeInfo = nullptr;

            if (exprNode->extension && exprNode->extension->castOffset)
            {
                auto inst   = emitInstruction(context, ByteCodeOp::IncPointer64, node->resultRegisterRC, 0, node->resultRegisterRC);
                inst->b.u64 = exprNode->extension->castOffset;
                inst->flags |= BCI_IMM_B;
            }

            return true;
        }

        if (fromTypeInfo->kind == TypeInfoKind::Array ||
            fromTypeInfo->kind == TypeInfoKind::Pointer ||
            fromTypeInfo->kind == TypeInfoKind::Struct ||
            fromTypeInfo->kind == TypeInfoKind::Interface ||
            fromTypeInfo->kind == TypeInfoKind::Slice ||
            fromTypeInfo->kind == TypeInfoKind::Reference ||
            fromTypeInfo->kind == TypeInfoKind::Lambda ||
            fromTypeInfo->numRegisters() == 1)
        {
            truncRegisterRC(context, exprNode->resultRegisterRC, 1);
            node->resultRegisterRC   = exprNode->resultRegisterRC;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }

        if (fromTypeInfo->isNative(NativeTypeKind::String) ||
            fromTypeInfo->isNative(NativeTypeKind::Any))
        {
            truncRegisterRC(context, exprNode->resultRegisterRC, 1);
            node->resultRegisterRC   = exprNode->resultRegisterRC;
            exprNode->castedTypeInfo = nullptr;
            return true;
        }
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);
        SWAG_CHECK(emitCastToSlice(context, exprNode, typeInfo, fromTypeInfo));
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Interface)
    {
        ensureCanBeChangedRC(context, exprNode->resultRegisterRC);
        SWAG_CHECK(emitCastToInterface(context, exprNode, typeInfo, fromTypeInfo));
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return context->internalError("emitCast, cast type not native");

    emitSafetyCast(context, typeInfo, fromTypeInfo, exprNode);
    ensureCanBeChangedRC(context, exprNode->resultRegisterRC);

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
    case NativeTypeKind::Int:
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
    case NativeTypeKind::UInt:
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
        return context->internalError("emitCast, invalid cast type");
    }

    node->resultRegisterRC   = exprNode->resultRegisterRC;
    exprNode->typeInfo       = typeInfo;
    exprNode->castedTypeInfo = nullptr;
    return true;
}

bool ByteCodeGenJob::emitExplicitCast(ByteCodeGenContext* context)
{
    auto node         = CastAst<AstCast>(context->node, AstNodeKind::Cast);
    auto typeInfo     = node->toCastTypeInfo;
    auto exprNode     = node->childs[1];
    auto fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);

    // First we cast with the user requested type. This is important to keep it, to
    // properly deref an 'any' for example
    SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo, true));
    if (context->result != ContextResult::Done)
        return true;

    // Then we cast again if necessary to the requested final type that can have been
    // changed (type promotion for example)
    if (node->toCastTypeInfo != node->typeInfo && node->toCastTypeInfo != node->castedTypeInfo)
    {
        SWAG_CHECK(emitCast(context, node, node->typeInfo, node->toCastTypeInfo));
        if (context->result != ContextResult::Done)
            return true;
    }

    return true;
}

bool ByteCodeGenJob::emitExplicitAutoCast(ByteCodeGenContext* context)
{
    auto node         = CastAst<AstCast>(context->node, AstNodeKind::AutoCast);
    auto typeInfo     = TypeManager::concreteType(node->typeInfo);
    auto exprNode     = node->childs[0];
    auto fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);
    SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo));
    node->castedTypeInfo = typeInfo;
    return true;
}
