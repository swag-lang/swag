#include "pch.h"
#include "AstNode.h"
#include "ByteCodeGenJob.h"
#include "TypeManager.h"
#include "ByteCodeOp.h"
#include "ByteCode.h"
#include "SourceFile.h"
#include "Module.h"

bool ByteCodeGenJob::emitCastToNativeAny(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* fromTypeInfo)
{
    if (!exprNode->concreteTypeInfo)
        return true;

    RegisterList r0;
    reserveRegisterRC(context, r0, 2);

    auto numRegs = exprNode->resultRegisterRC.size();
    SWAG_ASSERT(numRegs <= 2);

    // Make a pointer to the value
    if (fromTypeInfo->kind == TypeInfoKind::Native)
    {
        // Be sure registers are contiguous, as we address the first of them
        SWAG_ASSERT(exprNode->resultRegisterRC.size() != 2 || exprNode->resultRegisterRC[0] == exprNode->resultRegisterRC[1] - 1);
        emitInstruction(context, ByteCodeOp::CopyRARBAddr, r0[0], exprNode->resultRegisterRC[0]);
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Struct)
    {
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[0], exprNode->resultRegisterRC);
    }
    else
    {
        return internalError(context, "emitCastToNativeAny, invalid type", exprNode);
    }

    // Get concrete typeinfo from constant segment
    SWAG_ASSERT(exprNode->concreteTypeInfoStorage != UINT32_MAX);
    emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, r0[1])->b.u64 = exprNode->concreteTypeInfoStorage;

    exprNode->additionalRegisterRC = exprNode->resultRegisterRC;
    exprNode->resultRegisterRC     = r0;
    return true;
}

bool ByteCodeGenJob::emitCastToNativeBool(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind == TypeInfoKind::Pointer || typeInfo->kind == TypeInfoKind::Lambda)
    {
        emitInstruction(context, ByteCodeOp::CastBool64, exprNode->resultRegisterRC);
        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
        break;
    case NativeTypeKind::U8:
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastBool8, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastBool16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastBool32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::CastBool64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeBool, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
    case NativeTypeKind::U16:
    case NativeTypeKind::U32:
    case NativeTypeKind::Char:
    case NativeTypeKind::U64:
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeU8, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeS16, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x0000FFFF;
        break;
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeU64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
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
    case NativeTypeKind::Char:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x00000000'FFFFFFFF;
        break;
    case NativeTypeKind::U64:
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64S64, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeU64, invalid source type");
        break;
    }

    return true;
}
bool ByteCodeGenJob::emitCastToNativeS8(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
    case NativeTypeKind::U16:
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
    case NativeTypeKind::S8:
    case NativeTypeKind::S16:
    case NativeTypeKind::S32:
    case NativeTypeKind::S64:
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S8, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeS8, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeS16(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S16, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeS16, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeS32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::Bool:
    case NativeTypeKind::U8:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x0000FFFF;
        break;
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32S32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeS32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeS64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x0000000'000000FF;
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x0000000'0000FFFF;
        break;
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::ClearMaskU64, exprNode->resultRegisterRC)->b.u64 = 0x0000000'FFFFFFFF;
        break;
    case NativeTypeKind::U64:
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32S64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
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
        internalError(context, "emitCastToNativeS64, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeF32(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x000000FF;
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x0000FFFF;
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::CastU64F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64F32, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        break;
    case NativeTypeKind::F64:
        emitInstruction(context, ByteCodeOp::CastF64F32, exprNode->resultRegisterRC);
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeF32, invalid source type");
        break;
    }

    return true;
}

bool ByteCodeGenJob::emitCastToNativeF64(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo)
{
    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, expression type not native", exprNode);

    switch (typeInfo->nativeType)
    {
    case NativeTypeKind::U8:
    case NativeTypeKind::Bool:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x000000FF;
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U16:
        emitInstruction(context, ByteCodeOp::ClearMaskU32, exprNode->resultRegisterRC)->b.u32 = 0x0000FFFF;
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::Char:
    case NativeTypeKind::U32:
        emitInstruction(context, ByteCodeOp::CastU32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::U64:
        emitInstruction(context, ByteCodeOp::CastU64F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S8:
        emitInstruction(context, ByteCodeOp::CastS8S16, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S16:
        emitInstruction(context, ByteCodeOp::CastS16S32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S32:
        emitInstruction(context, ByteCodeOp::CastS32F32, exprNode->resultRegisterRC);
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::S64:
        emitInstruction(context, ByteCodeOp::CastS64F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F32:
        emitInstruction(context, ByteCodeOp::CastF32F64, exprNode->resultRegisterRC);
        break;
    case NativeTypeKind::F64:
        break;
    default:
        context->node = exprNode;
        internalError(context, "emitCastToNativeF64, invalid source type");
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

    if (fromTypeInfo == g_TypeMgr.typeInfoNull)
    {
        freeRegisterRC(context, exprNode);
        reserveLinearRegisterRC(context, exprNode->resultRegisterRC, 2);
        emitInstruction(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRC[0]);
        emitInstruction(context, ByteCodeOp::ClearRA, exprNode->resultRegisterRC[1]);
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::Slice)
    {
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::Array)
    {
        auto         typeArray = CastTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        RegisterList r0;
        reserveLinearRegisterRC(context, r0, 2);
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[0], exprNode->resultRegisterRC[0]);
        emitInstruction(context, ByteCodeOp::CopyRAVB64, r0[1])->b.u32 = typeArray->count;
        freeRegisterRC(context, node);
        exprNode->resultRegisterRC = r0;
        return true;
    }

    if (fromTypeInfo->kind == TypeInfoKind::TypeList)
    {
        auto typeList = CastTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeList);
        SWAG_ASSERT(typeList->childs.size() == 2);
        SWAG_ASSERT(typeList->childs[0]->kind == TypeInfoKind::Pointer || typeList->childs[0]->kind == TypeInfoKind::Array);
        SWAG_ASSERT(typeList->childs[1]->kind == TypeInfoKind::Native);

        RegisterList r0;
        reserveLinearRegisterRC(context, r0, 2);
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[0], exprNode->resultRegisterRC[0]);
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[1], exprNode->resultRegisterRC[1]);
        freeRegisterRC(context, node);
        exprNode->resultRegisterRC = r0;
        return true;
    }

    internalError(context, "emitCastToNativeString, invalid type");
    return false;
}

bool ByteCodeGenJob::emitCastToInterface(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto node      = context->node;
    auto toTypeItf = CastTypeInfo<TypeInfoStruct>(typeInfo, TypeInfoKind::Interface);

    if (fromTypeInfo == g_TypeMgr.typeInfoNull)
    {
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Struct)
    {
        auto fromTypeStruct = CastTypeInfo<TypeInfoStruct>(fromTypeInfo, TypeInfoKind::Struct);
        auto itf            = fromTypeStruct->hasInterface(toTypeItf);
        SWAG_ASSERT(itf);

        RegisterList r0;
        reserveLinearRegisterRC(context, r0, 2);

        // We use registers as a storage for the interface, in order to have just one register in the end
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[0], exprNode->resultRegisterRC);
        SWAG_ASSERT(itf->offset != UINT32_MAX);
        emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, r0[1])->b.u64 = itf->offset;
        emitInstruction(context, ByteCodeOp::CopyRARBAddr, exprNode->resultRegisterRC, r0[0]);

        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->isPointerTo(TypeInfoKind::Struct))
    {
        auto fromTypePointer = CastTypeInfo<TypeInfoPointer>(fromTypeInfo, TypeInfoKind::Pointer);
        auto fromTypeStruct  = CastTypeInfo<TypeInfoStruct>(fromTypePointer->pointedType, TypeInfoKind::Struct);
        auto itf             = fromTypeStruct->hasInterface(toTypeItf);
        SWAG_ASSERT(itf);

        RegisterList r0;
        reserveLinearRegisterRC(context, r0, 2);

        // We use registers as a storage for the interface, in order to have just one register in the end
        emitInstruction(context, ByteCodeOp::CopyRARB, r0[0], exprNode->resultRegisterRC);
        SWAG_ASSERT(itf->offset != UINT32_MAX);
        emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, r0[1])->b.u64 = itf->offset;
        emitInstruction(context, ByteCodeOp::CopyRARBAddr, exprNode->resultRegisterRC, r0[0]);

        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else
    {
        return internalError(context, "emitCastToInterface, invalid type");
    }

    return true;
}

bool ByteCodeGenJob::emitCastToSlice(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    auto node        = context->node;
    auto toTypeSlice = CastTypeInfo<TypeInfoSlice>(typeInfo, TypeInfoKind::Slice);

    if (fromTypeInfo == g_TypeMgr.typeInfoNull)
    {
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Slice)
    {
        auto fromTypeSlice     = CastTypeInfo<TypeInfoSlice>(fromTypeInfo, TypeInfoKind::Slice);
        int  diff              = fromTypeSlice->pointedType->sizeOf / toTypeSlice->pointedType->sizeOf;
        auto inst              = emitInstruction(context, ByteCodeOp::MulRAVB, exprNode->resultRegisterRC[1]);
        inst->b.u32            = diff;
        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::TypeList)
    {
        if (!(exprNode->flags & AST_SLICE_INIT_EXPRESSION))
        {
            auto fromTypeList = CastTypeInfo<TypeInfoList>(fromTypeInfo, TypeInfoKind::TypeList);
            int  diff         = fromTypeList->childs.front()->sizeOf / toTypeSlice->pointedType->sizeOf;
            auto inst         = emitInstruction(context, ByteCodeOp::MulRAVB, exprNode->resultRegisterRC[1]);
            inst->b.u32       = diff;
        }

        node->resultRegisterRC = exprNode->resultRegisterRC;
    }
    else if (fromTypeInfo->kind == TypeInfoKind::Array)
    {
        auto fromTypeArray     = CastTypeInfo<TypeInfoArray>(fromTypeInfo, TypeInfoKind::Array);
        node->resultRegisterRC = exprNode->resultRegisterRC;
        node->resultRegisterRC += reserveRegisterRC(context);
        auto inst   = emitInstruction(context, ByteCodeOp::CopyRAVB32, node->resultRegisterRC[1]);
        inst->b.u32 = fromTypeArray->count;
        transformResultToLinear2(context, node);
    }
    else
    {
        return internalError(context, "emitCastToSlice, invalid expression type", exprNode);
    }

    return true;
}

bool ByteCodeGenJob::emitCast(ByteCodeGenContext* context, AstNode* exprNode, TypeInfo* typeInfo, TypeInfo* fromTypeInfo)
{
    if (fromTypeInfo == nullptr)
        return true;
    SWAG_ASSERT(typeInfo);
    typeInfo = TypeManager::concreteType(typeInfo, CONCRETE_ENUM | CONCRETE_ALIAS);

    // opCast
    if (exprNode->flags & AST_USER_CAST)
    {
        SWAG_ASSERT(exprNode->resolvedUserOpSymbolOverload);
        SWAG_CHECK(emitUserOp(context, nullptr, exprNode));
        if (context->result == ContextResult::Pending)
            return true;
        return true;
    }

    // Cast from any to something real
    auto node = context->node;
    if (fromTypeInfo->isNative(NativeTypeKind::Any))
    {
        auto r0 = reserveRegisterRC(context);
        if (context->sourceFile->module->buildParameters.target.debugAnyCastCheck || g_CommandLine.debug)
        {
            auto inst = emitInstruction(context, ByteCodeOp::RAAddrFromConstantSeg, r0);
            SWAG_ASSERT(exprNode->concreteTypeInfoStorage != UINT32_MAX);
            inst->b.u32 = exprNode->concreteTypeInfoStorage;
            emitInstruction(context, ByteCodeOp::CompareOpEqualPointer, r0, exprNode->resultRegisterRC[1], r0);
            inst            = emitInstruction(context, ByteCodeOp::IntrinsicAssert, r0);
            inst->c.pointer = (uint8_t*) "invalid cast from 'any'";
            inherhitLocation(inst, exprNode);
        }

        SWAG_CHECK(emitTypeDeRef(context, exprNode->resultRegisterRC, typeInfo));
        node->resultRegisterRC   = exprNode->resultRegisterRC;
        exprNode->typeInfo       = typeInfo;
        exprNode->castedTypeInfo = nullptr;
        freeRegisterRC(context, r0);
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Pointer)
    {
        if (fromTypeInfo->kind == TypeInfoKind::Array ||
            fromTypeInfo->kind == TypeInfoKind::Pointer ||
            fromTypeInfo->kind == TypeInfoKind::Struct ||
            fromTypeInfo->kind == TypeInfoKind::Interface ||
            fromTypeInfo->kind == TypeInfoKind::Slice ||
            fromTypeInfo->isNative(NativeTypeKind::String))
        {
            node->resultRegisterRC   = exprNode->resultRegisterRC[0];
            exprNode->castedTypeInfo = nullptr;
            return true;
        }
    }

    if (typeInfo->kind == TypeInfoKind::Slice)
    {
        SWAG_CHECK(emitCastToSlice(context, exprNode, typeInfo, fromTypeInfo));
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->kind == TypeInfoKind::Interface)
    {
        SWAG_CHECK(emitCastToInterface(context, exprNode, typeInfo, fromTypeInfo));
        exprNode->castedTypeInfo = nullptr;
        return true;
    }

    if (typeInfo->kind != TypeInfoKind::Native)
        return internalError(context, "emitCast, cast type not native");

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
    case NativeTypeKind::Char:
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
        return internalError(context, "emitCast, invalid cast type");
    }

    node->resultRegisterRC   = exprNode->resultRegisterRC;
    exprNode->typeInfo       = typeInfo;
    exprNode->castedTypeInfo = nullptr;
    return true;
}

bool ByteCodeGenJob::emitExplicitCast(ByteCodeGenContext* context)
{
    AstNode* node         = context->node;
    auto     typeInfo     = node->typeInfo;
    auto     exprNode     = node->childs[1];
    auto     fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);
    SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo));
    return true;
}

bool ByteCodeGenJob::emitExplicitAutoCast(ByteCodeGenContext* context)
{
    AstNode* node         = context->node;
    auto     typeInfo     = TypeManager::concreteType(node->castedTypeInfo ? node->castedTypeInfo : node->typeInfo);
    auto     exprNode     = node->childs[0];
    auto     fromTypeInfo = TypeManager::concreteType(exprNode->typeInfo);
    SWAG_CHECK(emitCast(context, exprNode, typeInfo, fromTypeInfo));
    node->castedTypeInfo = typeInfo;
    return true;
}